#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <asm-generic/fcntl.h>
#include <stdarg.h>
#include <time.h>
#include <signal.h>

// Constants for buffer sizes
#define COMMAND_SIZE 256
#define MAX_MOD_DATE_SIZE 20
#define FILE_EXTENSION_SIZE 10

// Global variable for the log file
FILE *log_file;

// Log Levels
typedef enum {
    DEBUG,
    INFO,
    WARN,
    ERROR
} LogLevel;

// var for current_log_level
LogLevel current_log_level = INFO;

typedef enum {
    OPT_VERBOSE,
    OPT_QUIET,
    OPT_LOGLEVEL,
    OPT_OUTPUTDIR,
    OPT_HELP,
    OPT_VERSION,
    OPT_DRYRUN,
    OPT_UNKNOWN
} Option;

typedef struct {
    Option option;
    char *argument;
} ParsedOption;


// Function prototypes
void process_archive(const char *archive_path, const char *archive_type, const char *file_extension);
char* read_archive_type(const char *file_path);
void log_message(LogLevel level, const char *format, ...);
char* read_file_extension(int argc, char *argv[]);
void create_temp_dir();
void extract_archive_to_temp_dir(const char *archive_path, const char *archive_type);
void change_file_mod_dates_in_temp_dir(const char *file_extension);
void get_mod_date(const char *file_extension);
char* read_mod_date_from_file(const char *filename);
void set_mod_dates(const char *file_extension, const char *max_mod_date);
void create_updated_archive_from_temp_dir(const char *archive_path, const char *archive_type);
void delete_temp_files();
void remove_temp_dir();
void execute_command(const char *command);
void init_log_file();
void parse_command_line_arguments(int argc, char *argv[]);
ParsedOption parse_option(const char *arg);
void register_signal_handlers();
void signal_handler(int signal);
void register_cleanup_functions();
void close_log_file();

int main(int argc, char *argv[]) {
    // Initialize log file
    init_log_file();

    // Register signal handlers
    register_signal_handlers();

    // Register cleanup functions
    register_cleanup_functions();

        // Parse command-line arguments
    parse_command_line_arguments(argc, argv);

    // Read the file extension from command-line arguments
    char *file_extension = read_file_extension(argc, argv);

    // Extract and process the archive
    const char *archive_path = argv[2];
    const char *archive_type = read_archive_type(archive_path);
    process_archive(archive_path, archive_type, file_extension);

    // log success message
    log_message(INFO, "Success");

    return 0;
}

// Signal handler function
void signal_handler(int signal) {
    log_message(INFO, "Received signal %d, cleaning up and exiting...", signal);
    exit(0);
}

// Function to register signal handlers
void register_signal_handlers() {
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
}

// Function to initialize the log file
void close_log_file() {
    if (log_file != NULL) {
        fclose(log_file);
        log_file = NULL;
    }
}

// Function to register cleanup functions
void register_cleanup_functions() {
    atexit(delete_temp_files);
    atexit(remove_temp_dir);
    atexit(close_log_file);
}

// Function for parsing command-line arguments
ParsedOption parse_option(const char *arg) {
    ParsedOption result = {OPT_UNKNOWN, NULL};

    if (strcmp(arg, "-v") == 0) {
        result.option = OPT_VERBOSE;
    } else if (strcmp(arg, "-q") == 0) {
        result.option = OPT_QUIET;
    } else if (strcmp(arg, "-l") == 0) {
        result.option = OPT_LOGLEVEL;
    } else if (strcmp(arg, "-o") == 0) {
        result.option = OPT_OUTPUTDIR;
    } else if (strcmp(arg, "-h") == 0) {
        result.option = OPT_HELP;
    } else if (strcmp(arg, "--version") == 0) {
        result.option = OPT_VERSION;
    } else if (strcmp(arg, "-d") == 0) {
        result.option = OPT_DRYRUN;
    } else {
        result.option = OPT_UNKNOWN;
    }

    return result;
}


// Function for parsing command-line arguments
void parse_command_line_arguments(int argc, char *argv[]) {
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
                    exit(1);
                }
                break;
            case OPT_OUTPUTDIR:
                if (i + 1 < argc) {
                    i++;
                    if (chdir(argv[i]) != 0) {
                        perror("chdir() to specified output directory failed");
                        exit(1);
                    }
                    log_message(INFO, "Changed working directory to %s", argv[i]);
                } else {
                    log_message(ERROR, "Missing argument for -o");
                    exit(1);
                }
                break;
            case OPT_HELP:
                print_help_message();
                exit(0);
                break;
            case OPT_VERSION:
                printf("archivedater version 1.0\n");
                exit(0);
                break;
            case OPT_UNKNOWN:
                if(argv[i][0] == '-') {
                    log_message(ERROR, "\nInvalid option '%s'.\nTry './archivedater -h' for help.", argv[i]);
                    exit(1);
                }
                break;
        }
    }
}

// Function for printing help message
void print_help_message() {
    printf("NAME\n");
    printf("    archivedater - Corrects dates within archives, prioritizing the most relevant file types\n");
    printf("\n");
    printf("SYNOPSIS\n");
    printf("    archivedater [FILE_TYPE] [ARCHIVE_PATH] [OPTIONS]\n");
    printf("\n");
    printf("DESCRIPTION\n");
    printf("    This utility is designed to correct dates within archives, prioritizing the most relevant file types specified by the user. It aims to enhance the accuracy and consistency of date information across various archived files, providing a streamlined approach to archive management. By allowing users to select the types of files to be considered for date correction, this utility offers a flexible solution tailored to specific archive needs.\n");
    printf("\n");
    printf("OPTIONS\n");
    printf("    -h, --help\n");
    printf("        Display this help message and exit.\n");
    printf("    -v, --verbose\n");
    printf("        Enable verbose mode.\n");
    printf("    -q, --quiet\n");
    printf("        Enable quiet mode.\n");
    printf("    -l, --loglevel\n");
    printf("        Set log level (DEBUG, INFO, WARN, ERROR).\n");
    printf("    -o, --outputdir\n");
    printf("        Set output directory.\n");
    printf("    --version\n");
    printf("        Display version information and exit.\n");
    printf("    -d, --dryrun\n");
    printf("        Perform a dry run without making changes.\n");
    printf("\n");
    printf("EXIT STATUS\n");
    printf("    0 Successful execution.\n");
    printf("    1 An error occurred.\n");
    printf("\n");
    printf("SEE ALSO\n");
    printf("    man(1), man-pages(7)\n");
}

// Function for processing the archive
void process_archive(const char *archive_path, const char *archive_type, const char *file_extension) {
    create_temp_dir();
    extract_archive_to_temp_dir(archive_path, archive_type);
    change_file_mod_dates_in_temp_dir(file_extension);
    get_mod_date(file_extension);
    char *max_mod_date = read_mod_date_from_file("max_mod_date.txt");
    set_mod_dates(file_extension, max_mod_date);
    create_updated_archive_from_temp_dir(archive_path, archive_type);
}

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
    char timestamp[80];

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", timeinfo);

    // Log to console
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


// Function for reading the file extension from command-line arguments
char* read_archive_type(const char *file_path) {
    // Check for .tar.gz, .tar.bz2, or .zip extensions
    int path_length = strlen(file_path);
    if (path_length >= 7 && strcmp(file_path + path_length - 7, ".tar.gz") == 0) {
        return "tar.gz";
    } else if (path_length >= 8 && strcmp(file_path + path_length - 8, ".tar.bz2") == 0) {
        return "tar.bz2";
    } else if (path_length >= 4 && strcmp(file_path + path_length - 4, ".zip") == 0) {
        return "zip";
    } else {
        log_message(ERROR, "Unsupported archive type or no file extension found in '%s'", file_path);
        exit(1);
    }
}

// Function for reading the file extension from command-line arguments
char* read_file_extension(int argc, char *argv[]) {
    if (argc >= 2) {
        return argv[1];
    } else {
        log_message(ERROR, "Usage: program file_extension");
        exit(1);
    }
}

// Function for creating a temporary directory
void create_temp_dir() {
    char command[COMMAND_SIZE];
    snprintf(command, sizeof(command), "mkdir temp_dir");
    execute_command(command);
    log_message(INFO, "Created temporary directory");
}

// Function for extracting an archive to the temporary directory
void extract_archive_to_temp_dir(const char *archive_path, const char *archive_type) {
    char command[COMMAND_SIZE];
    if (strcmp(archive_type, "tar.gz") == 0) {
        snprintf(command, sizeof(command), "tar -xzvf %s -C temp_dir", archive_path);
    } else if (strcmp(archive_type, "tar.bz2") == 0) {
        snprintf(command, sizeof(command), "tar -xjvf %s -C temp_dir", archive_path);
    } else if (strcmp(archive_type, "zip") == 0) {
        snprintf(command, sizeof(command), "unzip %s -d temp_dir", archive_path);
    } else {
        log_message(ERROR, "Unsupported archive type: %s", archive_type);
        exit(1);
    }
    execute_command(command);
    log_message(INFO, "Extracted archive to temporary directory");
}

// Function for changing file modification dates in the temporary directory
void change_file_mod_dates_in_temp_dir(const char *file_extension) {
    char command[COMMAND_SIZE];
    snprintf(command, sizeof(command), "find temp_dir -type f -name '*%s' -printf '%%T@ %%p\\n' | sort -n | tail -n 1 | awk '{print $2}' > max_mod_file.txt", file_extension);
    execute_command(command);
    log_message(INFO, "Changed file modification dates in temporary directory");
}

// Function for getting the modification date of a file
void get_mod_date(const char *file_extension) {
    char command[COMMAND_SIZE];
    snprintf(command, sizeof(command), "stat -c %%y $(cat max_mod_file.txt) | awk '{print $1, $2}' > max_mod_date.txt");
    execute_command(command);
    log_message(INFO, "Got modification date of a file");
}

// Function for reading the modification date from a file
char* read_mod_date_from_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        log_message(ERROR, "Error opening file '%s': %s", filename, strerror(errno));
        exit(1);
    }

    char *mod_date = (char *)malloc(MAX_MOD_DATE_SIZE * sizeof(char));
    if (mod_date == NULL) {
        log_message(ERROR, "Error allocating memory");
        exit(1);
    }

    if (fgets(mod_date, MAX_MOD_DATE_SIZE, file) == NULL) {
        log_message(ERROR, "Error reading mod date from file '%s'", filename);
        exit(1);
    }

    fclose(file);
    return mod_date;
}

// Function for setting modification dates for all files of a certain type
void set_mod_dates(const char *file_extension, const char *max_mod_date) {
    char command[COMMAND_SIZE];
    snprintf(command, sizeof(command), "find temp_dir -name '*%s' -exec touch -d '%s' {} +", file_extension, max_mod_date);
    execute_command(command);
    log_message(INFO, "Set modification dates for all files of a certain type");
}

// Function for creating an updated archive from the temporary directory
void create_updated_archive_from_temp_dir(const char *archive_path, const char *archive_type) {
    char command[COMMAND_SIZE];
    if (strcmp(archive_type, "tar.gz") == 0) {
        snprintf(command, sizeof(command), "tar -czvf %s -C temp_dir .", archive_path);
    } else if (strcmp(archive_type, "tar.bz2") == 0) {
        snprintf(command, sizeof(command), "tar -cjvf %s -C temp_dir .", archive_path);
    } else if (strcmp(archive_type, "zip") == 0) {
        snprintf(command, sizeof(command), "cd temp_dir && zip -r %s .", archive_path);
    } else {
        log_message(ERROR, "Unsupported archive type: %s", archive_type);
        exit(1);
    }
    execute_command(command);
    log_message(INFO, "Created updated archive from temporary directory");
}

// Function for deleting temporary files
void delete_temp_files() {
    char command[COMMAND_SIZE];
    snprintf(command, sizeof(command), "rm -f max_mod_file.txt max_mod_date.txt");
    execute_command(command);
    log_message(INFO, "Deleted temporary files");
}

// Function for removing the temporary directory
void remove_temp_dir() {
    char command[COMMAND_SIZE];
    snprintf(command, sizeof(command), "rm -rf temp_dir");
    execute_command(command);
    log_message(INFO, "Removed temporary directory");
}

// Function for executing a command
void execute_command(const char *command) {
    pid_t pid = fork();
    if (pid == -1) {
        log_message(ERROR, "Error forking process");
        exit(1);
    } else if (pid == 0) {
        // Child process
        // Redirect stdout and stderr to /dev/null to suppress output
        int dev_null = open("/dev/null", O_WRONLY);
        dup2(dev_null, STDOUT_FILENO);
        dup2(dev_null, STDERR_FILENO);
        close(dev_null);

        execl("/bin/sh", "sh", "-c", command, (char *)NULL);
        log_message(ERROR, "Error executing command: %s", command);
        exit(1);
    } else {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            int exit_status = WEXITSTATUS(status);
            if (exit_status != 0) {
                log_message(ERROR, "Command '%s' failed with exit status %d", command, exit_status);
                exit(1);
            }
        } else {
            log_message(ERROR, "Command '%s' did not terminate normally", command);
            exit(1);
        }
    }
}

// Function to initialize the log file
void init_log_file() {
    log_file = fopen("program.log", "a");
    if (log_file == NULL) {
        fprintf(stderr, "Error opening log file: %s\n", strerror(errno));
        exit(1);
    }
}