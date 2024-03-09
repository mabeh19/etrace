#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <threads.h>

#include "etrace.h"

static thread_local struct error {
    size_t code;
    char stack[ETRACE_STACK_SIZE];
} errorStack;
static thread_local char *errorStackPtr;


static void clear(void)
{
    memset(&errorStack, 0, sizeof errorStack);
    errorStackPtr = errorStack.stack;
}

void etrace_report(size_t code, const char *error, ...)
{
    if (!errorStackPtr) errorStackPtr = &errorStack.stack[0];
    if (errorStackPtr >= errorStack.stack + ETRACE_STACK_SIZE) return;

    va_list va;
    va_start(va, error);
    if (errorStack.code == 0) errorStack.code = code;
    errorStackPtr += 1 + vsnprintf(errorStackPtr, (errorStack.stack + ETRACE_STACK_SIZE) - errorStackPtr, error, va);
    va_end(va);
}


const char *etrace_top(void)
{
    if (errorStackPtr == errorStack.stack) return NULL;

    const char *p = errorStackPtr - 1;
    while (*--p && p > errorStack.stack);

    return p;
}


const char *etrace_pop(void)
{ 
    if (errorStackPtr == errorStack.stack) return NULL;

    errorStackPtr--;
    while (*--errorStackPtr && errorStackPtr > errorStack.stack);
    const char *p = errorStackPtr + 1;

    if (errorStackPtr > errorStack.stack) errorStackPtr++;
    else p--;

    return p;
}


void etrace_fdump(FILE *out)
{
    if (errorStack.code) {
        char *msg = errorStack.stack;
        fprintf(out, "[%zu] ", errorStack.code);
        while (*msg) {
            fputs(msg, out);
            msg += strlen(msg) + 1;
        }
    }
}


void etrace_sdump(char *buf, size_t size)
{
    if (errorStack.code) {
        char *msg = errorStack.stack;
        size_t s = snprintf(buf, size, "[%zu] ", errorStack.code);
        size -= s;
        buf += s;
        while (*msg) {
            s = strlen(strncpy(buf, msg, size));
            size -= s;
            buf += s;
            msg += s + 1;
        }
    }
}


void etrace_fflush(FILE *out)
{
    etrace_fdump(out);
    clear();
}


void etrace_sflush(char *buf, size_t size)
{
    etrace_sdump(buf, size);
    clear();
}

