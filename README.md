# stdoc (Standard Oc)

stdoc is a general-purpose utility library for the C programming language. It is designed to provide safer, more consistent, and more ergonomic alternatives to common patterns in standard C, while preserving performance and low-level control.

The C language is powerful, but it comes with several long-standing issues:

- Unsafe standard library functions (e.g. buffer overflows, undefined behavior)

- Inconsistent APIs and design patterns

- Lack of modern abstractions

- Verbose and error-prone memory handling


stdoc aims to address these problems by offering a carefully designed set of utilities that improve safety and developer experience without hiding the nature of C.


### Installation

You need several Tools for compiling this project:

- GCC / Clang latest version
- Make lastest version

Clone the repository:

```bash
git clone https://github.com/uoctamika/stdoc.git
```

Then build this project:

```bash
cd stdoc && make BUILD=release && make install
```

Then you can compile your own projects with stdoc:

```bash
gcc main.c -lstdoc -o app
```


### Example

```c
#include <stdoc.h>

int main(void) {
    stdoc_version(); // check version

    stdoc_printf("Hello from stdoc!\n");

    return 0;
}
```

### Naming Convention

All public APIs use the `stdoc_` prefix to avoid conflicts.
example:

stdoc_version();
stdoc_printf();

### Contributing

Contributions are welcome for making this library better. You can request new features or give us ideas.

### Sources

Wikipedia: https://en.wikipedia.org/wiki/C_(programming_language)
<br>
Documentation C: https://devdocs.io/c/
<br>
Documentation C Man Pages: man 3 libc
<br>
Documentation C Man Pages Online: https://man7.org/linux/man-pages/index.html
<br>
ISO C Wiki: https://www.iso-9899.info/wiki/The_Standard
<br>
ISO C Books latest: https://www.iso.org/standard/82075.html
<br>
