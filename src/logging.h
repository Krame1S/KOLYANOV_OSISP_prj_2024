#ifndef LOGGING_H
#define LOGGING_H

#include <stdio.h>

typedef enum {
    DEBUG,
    INFO,
    WARN,
    ERROR
} LogLevel;

extern LogLevel current_log_level;

void log_message(LogLevel level, const char *format, ...);
void init_log_file();
void close_log_file();

#endif