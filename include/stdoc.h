#ifndef STDOC_H
#define STDOC_H

/*
 * stdoc.h - Primary public entry point for the stdoc library
 *
 * This header acts as the main aggregation layer for the stdoc API.
 * Users should include only this file to gain access to all
 * exposed functionality provided by the library.
 *
 * Design notes:
 * - Keeps user-facing includes minimal and stable
 * - Hides internal structure and file organization
 * - Simplifies integration and portability
 *
 * Any additional modules intended for public use should be
 * included here.
 */

#include "stdoc/io/stdoc_version.h"
#include "stdoc/io/stdoc_printf.h"

#endif /* STDOC_H */
