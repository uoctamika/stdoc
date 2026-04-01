#ifndef STDOC_PRINTF_H
#define STDOC_PRINTF_H

#include <stddef.h>
#include <stdoc/utility/compiler/macros/macros.h>

/*
 * stdoc_printf.h
 *
 * Minimal printf-like interface for stdoc.
 *
 * This module provides:
 *   - A lightweight printf implementation
 *   - Optional libc-free syscall backend
 *
 * Design goals:
 *   - No mandatory libc dependency
 *   - Small footprint
 *   - Cross-architecture support
 *
 * Supported format specifiers:
 *   %c - character
 *   %s - string
 *   %d - signed decimal (int)
 *   %x - hexadecimal (lowercase)
 *   %% - literal '%'
 *
 * Limitations:
 *   - No width / precision / flags
 *   - No floating-point support
 *   - No buffering (direct write calls)
 */

/*
 * stdoc_syscall_write
 *
 * Thin wrapper over the OS write syscall.
 *
 * Behavior:
 *   - If STDOC_USE_LIBC is defined:
 *       delegates to libc write()
 *   - Otherwise:
 *       performs raw syscall via inline assembly
 *
 * Parameters:
 *   fd    - file descriptor (1 = stdout, 2 = stderr)
 *   buf   - pointer to data buffer
 *   count - number of bytes to write
 *
 * Returns:
 *   Number of bytes written, or negative error code.
 *
 * Notes:
 *   - ABI-specific implementation per architecture
 *   - Intended for low-level / freestanding environments
 */

#if !defined(STDOC_USE_LIBC)

STDOC_STATIC_INLINE long
stdoc_syscall_write(int fd, const void* buf, unsigned long count)
{
    long ret;

#if defined(__x86_64__)

    /*
     * x86_64 Linux syscall ABI:
     *   rax = syscall number (1 = write)
     *   rdi = fd
     *   rsi = buf
     *   rdx = count
     *
     * Clobbers:
     *   rcx, r11 (as per syscall convention)
     */
    __asm__ volatile(
        "mov $1, %%rax\n"
        "syscall"
        : "=a"(ret)
        : "D"(fd), "S"(buf), "d"(count)
        : "rcx", "r11", "memory");

#elif defined(__i386__)

    /*
     * i386 Linux syscall ABI:
     *   eax = syscall number (4 = write)
     *   ebx = fd
     *   ecx = buf
     *   edx = count
     */
    __asm__ volatile(
        "mov $4, %%eax\n"
        "int $0x80"
        : "=a"(ret)
        : "b"(fd), "c"(buf), "d"(count)
        : "memory");

#elif defined(__aarch64__)

    /*
     * AArch64 Linux syscall ABI:
     *   x8 = syscall number (64 = write)
     *   x0 = fd
     *   x1 = buf
     *   x2 = count
     *
     * Note:
     *   Generic register constraints are used; compiler assigns registers.
     *   For strict ABI control, explicit register binding may be preferred.
     */
    __asm__ volatile(
        "mov x8, #64\n"
        "svc #0"
        : "=r"(ret)
        : "r"(fd), "r"(buf), "r"(count)
        : "x8", "memory");

#elif defined(__arm__)

    /*
     * ARM (32-bit) Linux syscall ABI:
     *   r7 = syscall number (4 = write)
     *   r0 = fd
     *   r1 = buf
     *   r2 = count
     */
    __asm__ volatile(
        "mov r7, #4\n"
        "swi #0"
        : "=r"(ret)
        : "r"(fd), "r"(buf), "r"(count)
        : "r7", "memory");

#else
#error "stdoc_syscall_write: unsupported architecture"
#endif

    return ret;
}

#else

#include <unistd.h>

/* libc-backed implementation */
#define stdoc_syscall_write(fd, buf, count) write(fd, buf, count)

#endif /* STDOC_USE_LIBC */

/*
 * stdoc_printf
 *
 * Minimal printf-like function.
 *
 * Parameters:
 *   format - format string
 *   ...    - variadic arguments
 *
 * Returns:
 *   Total number of bytes written.
 *
 * Behavior:
 *   - Writes directly to stdout (fd = 1)
 *   - No buffering is performed
 *
 * Notes:
 *   - Format string is validated at compile-time when supported
 *     via STDOC_ATTR_PRINTF
 *   - Behavior is undefined for unsupported format specifiers
 */
STDOC_ATTR_PRINTF(1, 2)
int stdoc_printf(const char* format, ...);

#endif /* STDOC_PRINTF_H */
