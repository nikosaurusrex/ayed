#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

void
log_fatal(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    fprintf(stderr, "[Fatal] ");

    vfprintf(stderr, fmt, args);
    putc('\n', stderr);
    va_end(args);

    exit(1);
}

void
log_error(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    fprintf(stderr, "[Error] ");

    vfprintf(stderr, fmt, args);
    putc('\n', stderr);
    va_end(args);
}

void
log_dev(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    fprintf(stdout, "[Dev] ");

    vfprintf(stdout, fmt, args);
    putc('\n', stdout);
    va_end(args);
}

void
log_info(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    fprintf(stdout, "[Info] ");

    vfprintf(stdout, fmt, args);
    putc('\n', stdout);
    va_end(args);
}
