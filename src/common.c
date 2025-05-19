#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "common.h"

char* concatStrings(size_t count, ...) {
    va_list args;

    va_start(args, count);
    size_t len = 0;
    for (size_t i = 0; i < count; i++) {
        len += strlen(va_arg(args, const char*));
    }
    va_end(args);

    va_start(args, count);
    char* result = calloc(len + 1, sizeof(char));
    for (size_t i = 0; i < count; i++) {
        strcat(result, va_arg(args, const char*));
    }
    va_end(args);

    return result;
}
