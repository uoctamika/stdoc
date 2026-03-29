#include <stdio.h>
#include <stdoc/io.h>

/*
 * stdoc_version - Print library version information
 */
void
stdoc_version(void)
{
    printf("     standard library Uoc     \n");
    printf("Version: 0.0.1 \n");
}

/*
 * stdoc_printf - Minimal printf implementation
 * @format: format string
 *
 * Supported format specifiers:
 *   %c - character
 *   %s - string
 *   %d - signed decimal
 *   %x - hexadecimal (lowercase)
 *   %% - literal '%'
 *
 * This implementation is intentionally minimal and writes
 * directly via stdoc_syscall_write().
 *
 * Limitations:
 * - No width/precision support
 * - No floating point
 * - No buffering (inefficient for large output)
 */
void
stdoc_printf(const char* format, ...)
{
    va_list args;

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
                    /* default argument promotion: char -> int */
                    char c = (char)va_arg(args, int);
                    stdoc_syscall_write(1, &c, 1);
                    break;
                }

                case 's':
                {
                    char* s = va_arg(args, char*);

                    /* match libc behavior for NULL string */
                    if (!s)
                        s = "(null)";

                    /* compute string length manually */
                    unsigned long len = 0;
                    while (s[len])
                        len++;

                    stdoc_syscall_write(1, s, len);
                    break;
                }

                case 'd':
                {
                    int  n = va_arg(args, int);
                    char buf[12]; /* enough for 32-bit int */
                    int  i = 0;

                    /*
                     * Special-case zero to avoid empty output
                     */
                    if (n == 0)
                    {
                        buf[i++] = '0';
                    }
                    else
                    {
                        /*
                         * Handle negative numbers.
                         * NOTE: INT_MIN is technically undefined
                         * when negated (overflow).
                         */
                        if (n < 0)
                        {
                            stdoc_syscall_write(1, "-", 1);
                            n = -n;
                        }

                        /* convert number to string (reversed) */
                        while (n > 0)
                        {
                            buf[i++] = (n % 10) + '0';
                            n /= 10;
                        }
                    }

                    /* output in correct order */
                    while (i > 0)
                    {
                        char out = buf[--i];
                        stdoc_syscall_write(1, &out, 1);
                    }
                    break;
                }

                case 'x':
                {
                    unsigned int x = va_arg(args, unsigned int);
                    char         buf[16]; /* enough for 32-bit hex */
                    int          i = 0;

                    if (x == 0)
                        buf[i++] = '0';

                    /* convert to hex (reversed) */
                    while (x > 0)
                    {
                        int rem  = x % 16;
                        buf[i++] = (rem < 10) ? (rem + '0') : (rem - 10 + 'a');
                        x /= 16;
                    }

                    /* output in correct order */
                    while (i > 0)
                    {
                        char out = buf[--i];
                        stdoc_syscall_write(1, &out, 1);
                    }
                    break;
                }

                case '%':
                    stdoc_syscall_write(1, "%", 1);
                    break;

                default:
                    /*
                     * Unknown specifier: ignore silently
                     * (matches minimal printf behavior)
                     */
                    break;
            }
        }
        else
        {
            /* write literal character */
            stdoc_syscall_write(1, format, 1);
        }

        format++;
    }

    va_end(args);
}
