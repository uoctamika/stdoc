#ifndef STDOC_IO_H
#define STDOC_IO_H

/*
 * stdoc_version - print library/version information
 *
 * This function is expected to emit version metadata
 * for the stdoc runtime. Implementation is defined
 * elsewhere.
 */
void stdoc_version(void);

/*
 * stdoc_syscall_write - minimal syscall wrapper for write(2)
 * @fd:    file descriptor
 * @buf:   buffer to write
 * @count: number of bytes to write
 *
 * Provides a thin inline abstraction over the write syscall
 * across multiple architectures. This avoids libc dependency
 * and performs a direct kernel transition.
 *
 * Return:
 *   On success: number of bytes written
 *   On error:   negative error code (architecture dependent ABI)
 *
 * Notes:
 * - Clobbers and calling conventions follow each architecture ABI.
 * - No error translation is performed.
 * - Intended for low-level/runtime usage only.
 */
static long stdoc_syscall_write(
    __attribute__((unused)) int fd,
    __attribute__((unused)) const void *buf,
    __attribute__((unused)) unsigned long count)
{
    long ret;

#if defined(__x86_64__)
    /* x86_64 syscall convention:
     * rax = syscall number
     * rdi, rsi, rdx = arguments
     */
    __asm__ volatile (
        "mov $1, %%rax\n"  /* __NR_write */
        "syscall"
        : "=a"(ret)
        : "D"(fd), "S"(buf), "d"(count)
        : "rcx", "r11", "memory"
    );

#elif defined(__i386__)
    /* i386 syscall convention (int 0x80):
     * eax = syscall number
     * ebx, ecx, edx = arguments
     */
    __asm__ volatile (
        "mov $4, %%eax\n"  /* __NR_write */
        "int $0x80"
        : "=a"(ret)
        : "b"(fd), "c"(buf), "d"(count)
        : "memory"
    );

#elif defined(__aarch64__)
    /* AArch64 syscall convention:
     * x8 = syscall number
     * x0, x1, x2 = arguments
     */
    __asm__ volatile (
        "mov x8, #64\n"    /* __NR_write */
        "svc #0"
        : "=r"(ret)
        : "r"(0), "r"(1), "r"(2)
        : "x8", "memory"
    );

#elif defined(__arm__)
    /* ARM (32-bit) syscall convention:
     * r7 = syscall number
     * r0, r1, r2 = arguments
     */
    __asm__ volatile (
        "mov r7, #4\n"     /* __NR_write */
        "swi #0"
        : "=r"(ret)
        : "r"(0), "r"(1), "r"(2)
        : "r7", "memory"
    );

#else
# error "Architecture not supported for inline asm syscall"
#endif

    return ret;
}

/*
 * stdoc_printf - lightweight printf implementation
 * @format: format string (printf-style)
 * @...:    variadic arguments
 *
 * A minimal formatted output function intended to operate
 * without full libc support. Format string is validated by
 * the compiler via attribute annotations.
 *
 * Context:
 *   Suitable for freestanding environments, early runtime,
 *   or embedded systems.
 */
__attribute__((format(printf, 1, 2)))
void stdoc_printf(const char *format, ...);

#endif /* STDOC_IO_H */
