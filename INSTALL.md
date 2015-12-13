How to build and install beluga
===============================

This package provides no automated way to install; `beluga` is not a big
compiler and manual installation is not quite difficult.

I tested this installation process on my [Gentoo Linux](https://www.gentoo.org)
(x86-64) machine. Installing on systems other than Linux probably requires
non-trivial changes on the package; for example, headers to replace a part of
the standard headers may vary.

In this document, the term `beluga`, depending on the context, refers to the
compiler implementation or the whole package including a preprocessor and a
driver.


#### Installation directory

Before installation, you need to determine directories into which built
executables, supporting libraries and headers are placed. In this document, it
is assumed that:

- executables are placed in `/usr/local/bin/`
- supporting libraries in `/usr/local/lib32/bcc/`
- headers in `/usr/local/lib32/bcc/include/` and
- intermediate files in `/tmp/`.

As indicated by the paths, this means system-wide installation. Local
installation is also possible by simply changing those paths to your local ones
(always use _absolute_ paths; e.g., `/home/username/var/bin/` instead of
`~/user/var/bin/` or `./var/bin/`).

This installation configuration has to be encoded into the following files:

- `bcc/host/sc.h` for a preprocessor (`sea-canary`)
- `bcc/host/beluga.h` for a compiler (`beluga`)
- `bcc/host/as.h` for an assembler (`as`) and
- `bcc/host/ld.h` for a linker (`ld`).

If you find a directory under `bcc/` to contain configuration to meet your
needs, simply change the simbolic link `bcc/host` to point to that directory.

`sc.h` for my system looks like:

    "/usr/local/bin/sc",
    "-U__GNUC__",
    /* "-D__STRICT_ANSI__", */
    "-D_POSIX_SOURCE",
    "-D__i386__",
    "-D__unix__",
    "-D__linux__",
    "-D__gnuc_va_list=va_list",
    "$1",
    "-I/usr/local/lib32/bcc/include",
    "-I/usr/local/lib32/bcc/gcc/include",
    "-I/usr/local/include",
    "-I/usr/local/lib32/bcc/gcc/include-fixed",
    "-I/usr/include",
    "-v",
    "-o", "$3",
    "$2",

The first line invokes `sea-canaey` installed in `/usr/local/bin/` and the rest
specifies options to it. `sc.h` is processed by `#include` and thus you can
make use of C comments to exclude unnecessary lines. `"$1"`, `"$2"` and `"$3"`
have special meanings and will be replaced by user-provided options, an input
file name and an output file name respectively. For example, when you run the
compiler driver `bcc` with these options:

    -I/path/to/headers -o foo.o -c foo.c

options to the preprocessor (`-I/path/to/headers` in this example) substitute
for `$1`, `foo.c` does for `$2`. `$3` is replaced by a generated temporary name
to pass the result to the compiler.

Note that options for system header paths (starting with `-I` above) follow
`$1` to let `sea-canary` inspect user-provided paths first (if any) to locate
headers.

This shows an example of `beluga.h`:

    "/usr/local/bin/beluga",
    "--target=x86-linux",
    "-v",
    "$1",
    "-o", "$3",
    "$2",

You rarely need to change this probably except for the installation directory
of `beluga`.

`beluga` takes advantage of an assembler and a linker from the target system
and you has to ensure the driver be able to access them by giving proper paths
in `as.h` and `ld.h`.

`as.h` is as simple as `beluga.h`:

    "/usr/bin/as",
    "--32",
    "$1",
    "-o", "$3",
    "$2",

The `--32` option is to force the assembler to accept x86 assembly code on a
x86-64 system; `beluga` is a 32-bit compiler while my system is 64-bit.

On the other hand, `ld.h` looks complicated:

    "/usr/bin/ld",
    "-m", "elf_i386",
    "-dynamic-linker", "/lib/ld-linux.so.2",
    "/usr/lib32/crt1.o",
    "/usr/lib32/crti.o",
    "/usr/local/lib32/bcc/gcc/32/crtbegin.o",
    "-L/usr/loca/lib32/bcc/gcc/32",
    "-L/usr/lib32",
    "-L/lib32",
    "-L/usr/x86_64-pc-linux-gnu/lib",
    "-L/usr/lib",
    "-lc",
    "/usr/local/lib32/bcc/gcc/32/crtend.o",
    "/usr/lib32/crtn.o",
    "-o", "$3",
    "$1",
    "$2",
    "/usr/local/lib32/bcc/xfloat.o",

In the linking phase, a set of
[start-up code](https://en.wikipedia.org/wiki/Crt0) and supporting libraries
are linked to build an executable, which explains why there are many options
given to `ld`. The last one, `xfloat.o` is a support object file for
compiler-provided `float.h`.

Search paths for system headers, necessary start-up files and paths to system
libraries can be inspected by running an existing compiler (for example, `gcc`)
with an option to display programs invoked as in:

    gcc -v hello.c


#### Configuration macros

When building `beluga`, it is necessary to define several macros properly to
select optional features and to pass environmental information.

The following macros are used by [`ocelot`](http://code.woong.org/ocelot/) that
`beluga` depends on:

- `MEM_MAXALIGN`: specifies the maximum alignment factor of storage returned by
  `malloc()`. 4 or 8 is a good choice on most systems.

The macros used in common include:

- `HAVE_COLOR`: makes `beluga` and `sea-canary` generate diagnostics in color;
- `HAVE_ICONV`: makes `beluga` and `sea-canary` take advantage of
  [libiconv](https://www.gnu.org/software/libiconv/) to process character
  encodings other than [ASCII](https://en.wikipedia.org/wiki/ASCII) from input
  files, multibyte and wide characters/strings; and
- `SHOW_WARNCODE`: makes `beluga` and `sea-canary` display options to control
  warnings when issueing them.

Macros for the preprocessor(`sea-canary`) are:

- `HAVE_REALPATH`: makes `sea-canary` use
  [`realpath()`](http://man7.org/linux/man-pages/man3/realpath.3.html) for
  include optimization;
- `DIR_SEPARATOR`: a character to separate directories in paths. The
  default is `/` (no double quotes necessary). No need to change on Unix-like
  machines;
- `PATH_SEPARATOR`: a character to separate paths in `SYSTEM_HEADER_DIR`; and
- `SYSTEM_HEADER_DIR`: paths that `sea-canary` searches for system headers.
  This must be a C string, e.g., `"/usr/include:/usr/local/include"` (note
  double quotes).

Besides `SYSTEM_HEADER_DIR`, there are two other ways to set search paths. One
is, as already explained, giving `-I` options to `sc.h`, and the other is using
environmental variables `CPATH` and `C_INCLUDE_PATH`.

`sea-canary` searches for headers directories in the order:
- user-provided paths from the `-I` options,
- paths from the environmental variable `CPATH`,
- paths from the environmental variable `C_INCLUDE_PATH` and
- paths from the macro `SYSTEM_HEADER_DIR`.

Lastly, this macro is for the driver(`bcc`):

- `TMP_DIR`: driver's temporary directory. This macro must end with a directory
  separator. The default is `"/tmp/"` (note the double quotes).

_When passing a C string with the `-D` option, do not forget to escape double
quotes with backslashes; for instance, `-DTMP_DIR=\"var/tmp/\"`._


#### Building `beluga`

A usual setting to build `beluga` on a Unix-like machine is to run `make` on
the project root as follows:

    CFLAGS="-DMEM_MAXALIGN=4 -DHAVE_COLOR -DHAVE_ICONV -DSHOW_WARNCODE -DHAVE_REALPATH" make all

If you are on a x86-64 machine, it is necessary to add `-m32` to both `CFLAGS`
and `LDFLAGS`:

    CFLAGS="-DMEM_MAXALIGN=4 -DHAVE_COLOR -DHAVE_ICONV -DSHOW_WARNCODE -DHAVE_REALPATH -m32" LDFLAGS="-m32" make all

Successful build of `beluga` generates three executables, `bcc`, `beluga` and
`sc` in the `build/` directory.


#### Copying files

The generated executables, `bcc`, `beluga` and `bcc` have to be copied into the
directory you decided to use. Assuming you are on the project root,

    cp build/{bcc,beluga,sc} /usr/local/bin/

will do that. (Of course, ensure you have proper permission, e.g., by letting
`sudo` run the command.)

Copy also headers to override existing ones and a support object:

    mkdir -p /usr/local/lib32/bcc
    cp -r include /usr/local/lib32/bcc/
    cp xfloat.o /usr/local/lib32/bcc/

`beluga` utilizes libraries from the target system as much as possible and
needs to refer to existing libraries and headers for them. In order to avoid
hard-coding a path to existing resources, it is useful to create a symbolic
link to them, which `/usr/local/lib32/bcc/gcc` is for. On my system, for
instance:

    ln -s /usr/lib/gcc/x86_64-pc-linux-gnu/4.9.3 /usr/local/lib32/bcc/gcc

This path to `gcc`'s resources is also obtained from `gcc -v`.

We have finished to install `beluga`. By compiling a small program that uses
standard headers,

    bcc -W -Wall hello.c

you can examine your installation.


#### Any troubles?

If you encounter any problem while installing `beluga`, let me know so that I
can help. The version of your distro and simple description of the problem
would be enough.