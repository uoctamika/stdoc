#include <stdarg.h>
#include <limits.h>
#include <stdoc/io/stdoc_printf.h>

/*
 * stdoc_printf implementation
 *
 * Strategy:
 * - Iterate over format string
 * - On '%' → parse next specifier
 * - Write output immediately via syscall
 *
 * Trade-offs:
 * - Simple & minimal
 * - No buffering → potentially inefficient
 */
int stdoc_printf(const char* format, ...)
{
    va_list args;
    int written = 0; /* total bytes written */

    va_start(args, format);

    while (*format)
    {
        if (*format == '%')
        {
            format++;

            switch (*format)
            {
                case 'c':
                {
                    /* char promoted to int in variadic args */
                    char c = (char)va_arg(args, int);
                    written += stdoc_syscall_write(1, &c, 1);
                    break;
                }

                case 's':
                {
                    const char* s = va_arg(args, const char*);

                    /* Handle NULL string safely */
                    if (!s) s = "(null)";

                    /* Compute string length manually */
                    unsigned long len = 0;
                    while (s[len]) len++;

                    written += stdoc_syscall_write(1, s, len);
                    break;
                }

                case 'd':
                {
                    int n = va_arg(args, int);

                    /*
                     * Handle INT_MIN separately:
                     * -INT_MIN overflows (two's complement)
                     */
                    if (n == INT_MIN) {
                        written += stdoc_syscall_write(1, "-2147483648", 11);
                        break;
                    }

                    char buf[12]; /* enough for -2147483648\0 */
                    int i = 0;

                    if (n == 0) {
                        buf[i++] = '0';
                    } else {
                        if (n < 0) {
                            written += stdoc_syscall_write(1, "-", 1);
                            n = -n;
                        }

                        /* Convert number to reversed string */
                        while (n > 0) {
                            buf[i++] = (n % 10) + '0';
                            n /= 10;
                        }
                    }

                    /* Output in correct order */
                    while (i > 0) {
                        char out = buf[--i];
                        written += stdoc_syscall_write(1, &out, 1);
                    }
                    break;
                }

                case 'x':
                {
                    unsigned int x = va_arg(args, unsigned int);

                    char buf[16];
                    int i = 0;

                    if (x == 0) buf[i++] = '0';

                    /* Convert to hex (lowercase) */
                    while (x > 0) {
                        int rem = x % 16;
                        buf[i++] = (rem < 10)
                            ? (rem + '0')
                            : (rem - 10 + 'a');
                        x /= 16;
                    }

                    /* Reverse output */
                    while (i > 0) {
                        char out = buf[--i];
                        written += stdoc_syscall_write(1, &out, 1);
                    }
                    break;
                }

                case '%':
                    /* Escaped percent */
                    written += stdoc_syscall_write(1, "%", 1);
                    break;

                default:
                    /*
                     * Unknown specifier:
                     * Currently ignored (silent fail)
                     * Alternative: print as literal "%x"
                     */
                    break;
            }
        }
        else
        {
            /* Normal character */
            written += stdoc_syscall_write(1, format, 1);
        }

        format++;
    }

    va_end(args);
    return written;
}
