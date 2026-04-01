#ifndef STDOC_MACROS_H
#define STDOC_MACROS_H

/*
 * macros.h
 *
 * Centralized compiler abstraction layer for stdoc.
 *
 * This header defines reusable macros for:
 *   - Compiler attributes (GCC/Clang)
 *   - Branch prediction hints
 *   - Inline control
 *
 * Design goals:
 *   - Portable across compilers
 *   - Zero overhead when unsupported
 *   - Single source of truth for all attributes
 *
 * Usage:
 *   Include this header in any public/private header
 *   that requires compiler-specific features.
 */

#if defined(__GNUC__) || defined(__clang__)
#define STDOC_COMPILER_GNU 1
#else
#define STDOC_COMPILER_GNU 0
#endif

/*
 * STDOC_ATTR_UNUSED
 *
 * Marks a variable or parameter as intentionally unused.
 *
 * Useful to suppress warnings in:
 *   - cross-platform code
 *   - partially implemented functions
 */
#if STDOC_COMPILER_GNU
#define STDOC_ATTR_UNUSED __attribute__((unused))
#else
#define STDOC_ATTR_UNUSED
#endif

/*
 * STDOC_ATTR_PRINTF(fmt, arg)
 *
 * Enables compile-time format string checking.
 *
 * Parameters:
 *   fmt - index of format string (1-based)
 *   arg - index of first variadic argument
 *
 * Example:
 *   int my_printf(const char *fmt, ...)
 *       STDOC_ATTR_PRINTF(1, 2);
 */
#if STDOC_COMPILER_GNU
#define STDOC_ATTR_PRINTF(fmt, arg) __attribute__((format(printf, fmt, arg)))
#else
#define STDOC_ATTR_PRINTF(fmt, arg)
#endif

/*
 * STDOC_ATTR_NORETURN
 *
 * Indicates that a function never returns.
 *
 * Useful for:
 *   - fatal error handlers
 *   - exit wrappers
 */
#if STDOC_COMPILER_GNU
#define STDOC_ATTR_NORETURN __attribute__((noreturn))
#else
#define STDOC_ATTR_NORETURN
#endif

/*
 * STDOC_ATTR_ALWAYS_INLINE
 *
 * Forces the compiler to inline a function.
 *
 * Note:
 *   Should be used carefully; may increase code size.
 */
#if STDOC_COMPILER_GNU
#define STDOC_ATTR_ALWAYS_INLINE inline __attribute__((always_inline))
#else
#define STDOC_ATTR_ALWAYS_INLINE inline
#endif

/*
 * STDOC_LIKELY(x)
 * STDOC_UNLIKELY(x)
 *
 * Provide branch prediction hints to the compiler.
 *
 * Example:
 *   if (STDOC_UNLIKELY(ptr == NULL)) { ... }
 */
#if STDOC_COMPILER_GNU
#define STDOC_LIKELY(x) __builtin_expect(!!(x), 1)
#define STDOC_UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
#define STDOC_LIKELY(x) (x)
#define STDOC_UNLIKELY(x) (x)
#endif

/*
 * STDOC_INLINE
 *
 * Portable inline keyword abstraction.
 */
#ifndef STDOC_INLINE
#define STDOC_INLINE inline
#endif

/*
 * STDOC_STATIC_INLINE
 *
 * Common pattern for header-only helpers.
 */
#ifndef STDOC_STATIC_INLINE
#define STDOC_STATIC_INLINE static inline
#endif

/*
 * STDOC_ARRAY_SIZE(arr)
 *
 * Computes number of elements in a static array.
 *
 * Warning:
 *   Do NOT use on pointers.
 */
#define STDOC_ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

/*
 * STDOC_UNUSED(x)
 *
 * Explicitly mark a variable as unused.
 *
 * Alternative to attribute usage:
 *   STDOC_UNUSED(var);
 */
#define STDOC_UNUSED(x) ((void)(x))

#endif /* STDOC_MACROS_H */
