#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "options.h"
#include "logging.h"
#include "archive_processing.h"

// Function for parsing command-line arguments
void parse_command_line_arguments(int argc, char *argv[], char **working_dir, char **file_extension) {
    for (int i = 1; i < argc; i++) {
        ParsedOption option = parse_option(argv[i]);

        switch (option.option) {
            case OPT_VERBOSE:
                current_log_level = DEBUG;
                log_message(DEBUG, "Verbose mode enabled");
                break;
            case OPT_QUIET:
                current_log_level = ERROR;
                log_message(INFO, "Quiet mode enabled");
                break;
            case OPT_FILETYPE:
                if (i + 1 < argc) {
                    i++;
                    *file_extension = argv[i];
                } else {
                    log_message(ERROR, "Missing argument for -f");
                }
                log_message(INFO, "File type specified");
                break;
            case OPT_LOGLEVEL:
                if (i + 1 < argc) {
                    i++;
                    if (strcmp(argv[i], "DEBUG") == 0) {
                        current_log_level = DEBUG;
                    } else if (strcmp(argv[i], "INFO") == 0) {
                        current_log_level = INFO;
                    } else if (strcmp(argv[i], "WARN") == 0) {
                        current_log_level = WARN;
                    } else if (strcmp(argv[i], "ERROR") == 0) {
                        current_log_level = ERROR;
                    } else {
                        log_message(ERROR, "Invalid log level '%s'", argv[i]);
                        exit(1);
                    }
                } else {
                    log_message(ERROR, "Missing argument for -l");
                    exit_status = 1;
                    exit(1);
                }
                break;
            case OPT_OUTPUTDIR:
                if (i + 1 < argc) {
                    i++;
                    if (chdir(argv[i]) != 0) {
                        perror("chdir() to specified output directory failed");
                        exit_status = 1;
                        exit(1);
                    }
                    *working_dir = argv[i];
                    log_message(INFO, "Changed output directory to %s", argv[i]);
                    log_message(WARN, "The updated archive will be created in the specified output directory");
                } else {
                    log_message(ERROR, "Missing argument for -o");
                    exit_status = 1;
                    exit(1);
                }
                break;
            case OPT_HELP:
                print_help_message();
                exit_status = 2;
                exit(0);
                break;
            case OPT_VERSION:
                printf("archivedater version 1.0\n");
                exit_status = 2;
                exit(0);
                break;
            case OPT_UNKNOWN:
                if(argv[i][0] == '-') {
                    log_message(ERROR, "\nInvalid option '%s'.\nTry './archivedater -h' for help.", argv[i]);
                    exit_status = 1;
                    exit(1);
                }
                break;
        }
    }
}

// Function for parsing command-line arguments
ParsedOption parse_option(const char *arg) {
    ParsedOption result = {OPT_UNKNOWN, NULL};

    if (strcmp(arg, "-v") == 0 || strcmp(arg, "--verbose") == 0) {
        result.option = OPT_VERBOSE;
    } else if (strcmp(arg, "-q") == 0 || strcmp(arg, "--quiet") == 0) {
        result.option = OPT_QUIET;
    } else if (strcmp(arg, "-f") == 0 || strcmp(arg, "--filetype") == 0) {
        result.option = OPT_FILETYPE;
    } else if (strcmp(arg, "-l") == 0 || strcmp(arg, "--loglevel") == 0) {
        result.option = OPT_LOGLEVEL;
    } else if (strcmp(arg, "-o") == 0 || strcmp(arg, "--outputdir") == 0) {
        result.option = OPT_OUTPUTDIR;
    } else if (strcmp(arg, "-h") == 0 || strcmp(arg, "--help") == 0) {
        result.option = OPT_HELP;
    } else if (strcmp(arg, "--version") == 0) {
        result.option = OPT_VERSION;
    } else {
        result.option = OPT_UNKNOWN;
    }

    return result;
}

// Function for printing help message
void print_help_message() {
    printf("NAME\n");
    printf("    archivedater - corrects the dates in the archives by the last file modification date\n");
    printf("\n");
    printf("SYNOPSIS\n");
    printf("    archivedater [ARCHIVE_PATH] [OPTIONS]\n");
    printf("\n");
    printf("DESCRIPTION\n");
    printf("    This utility is designed to correct dates within archives, prioritizing the most relevant file types specified by the user. It aims to enhance the accuracy and consistency of date information across various archived files, providing a streamlined approach to archive management. By allowing users to select the types of files to be considered for date correction, this utility offers a flexible solution tailored to specific archive needs.\n");
    printf("\n");
    printf("OPTIONS (no need to specify [ARCHIVE_PATH])\n");
    printf("    -h, --help\n");
    printf("        Display this help message and exit.\n");
    printf("    --version\n");
    printf("        Display version information and exit.\n");
    printf("\n");
    printf("OPTIONS (need to specify [ARCHIVE_PATH])\n");
    printf("    -f, --filetype\n");
    printf("        Set file type to be processed.\n");
    printf("    -v, --verbose\n");
    printf("        Enable verbose mode.\n");
    printf("    -q, --quiet\n");
    printf("        Enable quiet mode.\n");
    printf("    -l, --loglevel\n");
    printf("        Set log level (DEBUG, INFO, WARN, ERROR).\n");
    printf("    -o, --outputdir\n");
    printf("        Set output directory.\n");
    printf("\n");
    printf("EXIT STATUS\n");
    printf("    0 Successful execution.\n");
    printf("    1 An error occurred.\n");
    printf("\n");
    printf("SEE ALSO\n");
    printf("    https://github.com/Krame1S/KOLYANOV_OSISP_prj_2024/blob/master/README.md\n");
}