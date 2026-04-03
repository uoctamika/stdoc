#include <limits.h>
#include <stdarg.h>
#include <stdoc/io/stdoc_printf.h>

/**
 * struct printf_ctx - Internal state for the buffered printf implementation.
 * @buf: The local buffer to collect output.
 * @idx: Current position in the buffer.
 * @total: Total bytes processed (including those flushed).
 */
struct printf_ctx
{
    char* buf;
    int   idx;
    int   total;
};

/**
 * flush_ctx() - Writes the current buffer to stdout.
 * @ctx: The printf context.
 */
static void flush_ctx(struct printf_ctx* ctx)
{
    if (ctx->idx > 0)
    {
        stdoc_syscall_write(1, ctx->buf, (unsigned long)ctx->idx);
        ctx->total += ctx->idx;
        ctx->idx = 0;
    }
}

/**
 * put_char_ctx() - Adds a character to the buffer, flushing if full.
 * @ctx: The printf context.
 * @c:   The character to add.
 */
static void put_char_ctx(struct printf_ctx* ctx, char c)
{
    if (ctx->idx >= STDOC_PRINTF_BUF_SIZE)
        flush_ctx(ctx);
    ctx->buf[ctx->idx++] = c;
}

/**
 * put_str_ctx() - Adds a string to the buffer.
 * @ctx: The printf context.
 * @s:   The string to add.
 */
static void put_str_ctx(struct printf_ctx* ctx, const char* s)
{
    if (!s)
        s = "(null)";
    while (*s)
        put_char_ctx(ctx, *s++);
}

/**
 * put_uint_ctx() - Converts an unsigned number to a given base and adds to
 * buffer.
 * @ctx:       The printf context.
 * @n:         The number to convert.
 * @base:      The base (e.g., 10 or 16).
 * @uppercase: Whether to use uppercase letters for bases > 10.
 */
static void put_uint_ctx(struct printf_ctx* ctx, unsigned long n, int base,
                         int uppercase)
{
    char buf[64]; /* Large enough for 64-bit binary/octal/dec/hex */
    int  i = 0;

    if (n == 0)
    {
        put_char_ctx(ctx, '0');
        return;
    }

    /* Standard conversion: digits are generated in reverse order */
    while (n > 0)
    {
        int rem = (int)(n % (unsigned long)base);
        if (rem < 10)
            buf[i++] = (char)(rem + '0');
        else
            buf[i++] = (char)(rem - 10 + (uppercase ? 'A' : 'a'));
        n /= (unsigned long)base;
    }

    /* Output digits in correct order */
    while (i > 0)
        put_char_ctx(ctx, buf[--i]);
}

/**
 * put_int_ctx() - Converts a signed integer to decimal and adds to buffer.
 * @ctx: The printf context.
 * @n:   The signed integer.
 */
static void put_int_ctx(struct printf_ctx* ctx, long n)
{
    if (n < 0)
    {
        put_char_ctx(ctx, '-');
        /* Handle potential overflow of -n using unsigned cast */
        unsigned long un =
            (n == LONG_MIN) ? (unsigned long)LONG_MAX + 1 : (unsigned long)-n;
        put_uint_ctx(ctx, un, 10, 0);
    }
    else
    {
        put_uint_ctx(ctx, (unsigned long)n, 10, 0);
    }
}

/**
 * stdoc_printf() - Minimal buffered printf implementation.
 * @format: The format string.
 * @...:    Variadic arguments.
 *
 * This implementation uses a local stack buffer to minimize syscalls.
 * It flushes the buffer once at the end, or whenever it becomes full.
 */
int stdoc_printf(const char* format, ...)
{
    char              local_buf[STDOC_PRINTF_BUF_SIZE];
    struct printf_ctx ctx;
    va_list           args;

    ctx.buf   = local_buf;
    ctx.idx   = 0;
    ctx.total = 0;

    va_start(args, format);

    while (*format)
    {
        if (*format == '%' && *(format + 1))
        {
            format++; /* Skip '%' */

            switch (*format)
            {
                case 'c':
                    /* char is promoted to int in variadic arguments */
                    put_char_ctx(&ctx, (char)va_arg(args, int));
                    break;

                case 's':
                    put_str_ctx(&ctx, va_arg(args, const char*));
                    break;

                case 'd':
                case 'i':
                    put_int_ctx(&ctx, (long)va_arg(args, int));
                    break;

                case 'u':
                    put_uint_ctx(
                        &ctx, (unsigned long)va_arg(args, unsigned int), 10, 0);
                    break;

                case 'x':
                    put_uint_ctx(
                        &ctx, (unsigned long)va_arg(args, unsigned int), 16, 0);
                    break;

                case 'X':
                    put_uint_ctx(
                        &ctx, (unsigned long)va_arg(args, unsigned int), 16, 1);
                    break;

                case 'p':
                {
                    void* p = va_arg(args, void*);
                    if (!p)
                    {
                        put_str_ctx(&ctx, "(nil)");
                    }
                    else
                    {
                        put_str_ctx(&ctx, "0x");
                        put_uint_ctx(&ctx, (unsigned long)p, 16, 0);
                    }
                    break;
                }

                case '%':
                    put_char_ctx(&ctx, '%');
                    break;

                default:
                    /* Unknown specifier: print literally */
                    put_char_ctx(&ctx, '%');
                    put_char_ctx(&ctx, *format);
                    break;
            }
        }
        else
        {
            /* Literal character */
            put_char_ctx(&ctx, *format);
        }

        format++;
    }

    va_end(args);

    /* Final flush to ensure all data is written */
    flush_ctx(&ctx);

    /* Return total bytes processed, which matches bytes written if syscall
     * succeeded */
    return ctx.total;
}
