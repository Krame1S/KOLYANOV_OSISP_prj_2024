#include <stdarg.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "logging.h"
#include "utils.h"
#include "archive_processing.h"

FILE *log_file = NULL;
LogLevel current_log_level = WARN;

// Logging function with timestamps and log levels
void log_message(LogLevel level, const char *format, ...) {
    if (level < current_log_level) {
        return;
    }

    va_list args;
    va_start(args, format);
    va_list args_copy;
    va_copy(args_copy, args);

    // Get current time
    time_t rawtime;
    struct tm *timeinfo;
    char timestamp[TIME_STAMP_SIZE];

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", timeinfo);

    // Log to console
    usleep(20000);
    printf("%s %s: ", timestamp, level == DEBUG ? "DEBUG" : level == INFO ? "INFO" : level == WARN ? "WARN" : "ERROR");
    vprintf(format, args);
    printf("\n");

    // Log to file
    if (log_file != NULL) {
        fprintf(log_file, "%s %s: ", timestamp, level == DEBUG ? "DEBUG" : level == INFO ? "INFO" : level == WARN ? "WARN" : "ERROR");
        
        if (vfprintf(log_file, format, args_copy) < 0) {
            fprintf(stderr, "Error writing to log file\n");
        }
        fprintf(log_file, "\n");
    }

    va_end(args);
    va_end(args_copy);
}

// Function to initialize the log file
void init_log_file() {
    log_file = fopen("program.log", "a");
    if (log_file == NULL) {
        fprintf(stderr, "Error opening log file: %s\n", strerror(errno));
        exit(1);
    }
}

// Function to initialize the log file
void close_log_file() {
    if (log_file != NULL) {
        fclose(log_file);
        log_file = NULL;
    }
}