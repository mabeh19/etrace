#include <stdio.h>
#include <stddef.h>


#ifndef ETRACE
#define ETRACE


#define ETRACE_STACK_SIZE   ((size_t)1024)


void etrace_report(size_t code, const char *error, ...);
const char *etrace_top(void);
const char *etrace_pop(void);
void etrace_fdump(FILE *out);
void etrace_sdump(char *buf, size_t size);
void etrace_fflush(FILE *out);
void etrace_sflush(char *buf, size_t size);

#endif /* ETRACE */
