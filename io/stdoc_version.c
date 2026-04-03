#include <stdoc/io/stdoc_printf.h>
#include <stdoc/io/stdoc_version.h>

/*
 * stdoc_version - Print library version information
 */
void stdoc_version(void)
{
    stdoc_printf("     standard library Uoc     \n");
    stdoc_printf("Version: %s\n", STDOC_VERSION_STRING);
    stdoc_printf("Major: %d, Minor: %d, Patch: %d\n",
               STDOC_VERSION_MAJOR, STDOC_VERSION_MINOR, STDOC_VERSION_PATCH);
    stdoc_printf("Build Hash: %s\n", STDOC_BUILD_HASH);
    stdoc_printf("Build Date: %s\n", STDOC_BUILD_DATE);
}
