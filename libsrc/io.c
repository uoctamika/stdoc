#include <stdoc/io.h>
#include <stdio.h>

void stdoc_version(void)
{
     printf("     standard library Uoc     \n");
     printf("Version: 0.0.1 \n");
}


void stdoc_printf(const char *format, ...) {
    va_list args;
    va_start(args, format);

    while (*format) {
        if (*format == '%') {
            format++;
            switch (*format) {
                case 'c': {
                    char c = (char)va_arg(args, int);
                    stdoc_syscall_write(1, &c, 1);
                    break;
                }
                case 's': {
                    char *s = va_arg(args, char *);
                    if (!s) s = "(null)";
                    unsigned long len = 0;
                    while (s[len]) len++;
                    stdoc_syscall_write(1, s, len);
                    break;
                }
                case 'd': {
                    int n = va_arg(args, int);
                    char buf[12];
                    int i = 0;
                    if (n == 0) {
                        buf[i++] = '0';
                    } else if (n < 0) {
                        stdoc_syscall_write(1, "-", 1);
                        n = -n;
                    }
                    while (n > 0) {
                        buf[i++] = (n % 10) + '0';
                        n /= 10;
                    }
                    while (i > 0) {
                        char out = buf[--i];
                        stdoc_syscall_write(1, &out, 1);
                    }
                    break;
                }
                case 'x': {
                    unsigned int x = va_arg(args, unsigned int);
                    char buf[16];
                    int i = 0;
                    if (x == 0) buf[i++] = '0';
                    while (x > 0) {
                        int rem = x % 16;
                        buf[i++] = (rem < 10) ? (rem + '0') : (rem - 10 + 'a');
                        x /= 16;
                    }
                    while (i > 0) {
                        char out = buf[--i];
                        stdoc_syscall_write(1, &out, 1);
                    }
                    break;
                }
                case '%':
                    stdoc_syscall_write(1, "%", 1);
                    break;
            }
        } else {
            stdoc_syscall_write(1, format, 1);
        }
        format++;
    }
    va_end(args);
}
