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
