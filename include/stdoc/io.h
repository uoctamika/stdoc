#ifndef STDOC_IO_H
#define STDOC_IO_H

void stdoc_version(void);

/* --- System Call Wrapper (Internal) --- */
static long stdoc_syscall_write( __attribute__((unused)) int fd, __attribute__((unused))  const void *buf, __attribute__((unused))  unsigned long count) {
    long ret;
#if defined(__x86_64__)
    __asm__ volatile (
        "mov $1, %%rax\n"  // syscall number for write (x86_64)
        "syscall"
        : "=a"(ret)
        : "D"(fd), "S"(buf), "d"(count)
        : "rcx", "r11", "memory"
    );
#elif defined(__i386__)
    __asm__ volatile (
        "mov $4, %%eax\n"  // syscall number for write (x86 32-bit)
        "int $0x80"
        : "=a"(ret)
        : "b"(fd), "c"(buf), "d"(count)
        : "memory"
    );
#elif defined(__aarch64__)
    __asm__ volatile (
        "mov x8, #64\n"    // syscall number for write (ARM64)
        "svc #0"
        : "=r"(ret)
        : "r"(0), "r"(1), "r"(2) // x0=fd, x1=buf, x2=count
        : "x8", "memory"
    );
#elif defined(__arm__)
    __asm__ volatile (
        "mov r7, #4\n"     // syscall number for write (ARM 32-bit)
        "swi #0"
        : "=r"(ret)
        : "r"(0), "r"(1), "r"(2) // r0=fd, r1=buf, r2=count
        : "r7", "memory"
    );
#else
    #error "Architecture not supported for inline asm syscall"
#endif
    return ret;
}

/* --- function for printf --- */
__attribute__((format(printf, 1, 2)))
void stdoc_printf(const char *format, ...);


#endif
