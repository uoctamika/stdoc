#ifndef STDOC_VERSION_H
#define STDOC_VERSION_H

#define STDOC_VERSION_MAJOR 0
#define STDOC_VERSION_MINOR 1
#define STDOC_VERSION_PATCH 0
#define STDOC_VERSION_STRING "0.5.0"

/* These can be defined at compile time, e.g., -DSTDOC_BUILD_HASH="<git_hash>" */
#ifndef STDOC_BUILD_HASH
#define STDOC_BUILD_HASH "unknown"
#endif

#ifndef STDOC_BUILD_DATE
#define STDOC_BUILD_DATE __DATE__ " " __TIME__
#endif

/*
 * stdoc_version - print library/version information
 *
 * This function is expected to emit version metadata
 * for the stdoc runtime. Implementation is defined
 * elsewhere.
 */
void stdoc_version(void);

#endif /* STDOC_VERSION_H */
