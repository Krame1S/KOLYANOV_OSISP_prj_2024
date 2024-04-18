#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <asm-generic/fcntl.h>
#include <stdarg.h>
#include <time.h>

// Constants for buffer sizes
#define COMMAND_SIZE 256
#define MAX_MOD_DATE_SIZE 20
#define FILE_EXTENSION_SIZE 10

// Global variable for the log file
FILE *log_file;

// Function prototypes
void process_archive(const char *archive_path, const char *archive_type, const char *file_extension);
char* read_archive_type(const char *file_path);
void log_message(const char *level, const char *format, ...);
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



int main(int argc, char *argv[]) {
    // Initialize log file
    init_log_file();

    // Read the file extension from command-line arguments
    char *file_extension = read_file_extension(argc, argv);

    // Extract and process the archive
    const char *archive_path = argv[2];
    const char *archive_type = read_archive_type(archive_path);
    process_archive(archive_path, archive_type, file_extension);

    // Cleanup
    delete_temp_files();
    remove_temp_dir();

    // Close the log file
    fclose(log_file);

    printf("Your archive has been updated successfully.\n");

    return 0;
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

// Logging function
void log_message(const char *level, const char *format, ...) {
    va_list args;
    va_start(args, format);

    // Log to console
    usleep(500000);
    printf("%s: ", level);
    vprintf(format, args);
    printf("\n");

    // Log to file
    if (log_file != NULL) {
        fprintf(log_file, "%s: ", level);
        vfprintf(log_file, format, args);
        fprintf(log_file, "\n");
    }

    va_end(args);
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
        log_message("ERROR", "Unsupported archive type or no file extension found in '%s'", file_path);
        exit(1);
    }
}

// Function for reading the file extension from command-line arguments
char* read_file_extension(int argc, char *argv[]) {
    if (argc >= 2) {
        return argv[1];
    } else {
        log_message("ERROR", "Usage: program file_extension");
        exit(1);
    }
}

// Function for creating a temporary directory
void create_temp_dir() {
    char command[COMMAND_SIZE];
    snprintf(command, sizeof(command), "mkdir temp_dir");
    execute_command(command);
    log_message("INFO", "Created temporary directory");
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
        log_message("ERROR", "Unsupported archive type: %s", archive_type);
        exit(1);
    }
    execute_command(command);
    log_message("INFO", "Extracted archive to temporary directory");
}

// Function for changing file modification dates in the temporary directory
void change_file_mod_dates_in_temp_dir(const char *file_extension) {
    char command[COMMAND_SIZE];
    snprintf(command, sizeof(command), "find temp_dir -type f -name '*%s' -printf '%%T@ %%p\\n' | sort -n | tail -n 1 | awk '{print $2}' > max_mod_file.txt", file_extension);
    execute_command(command);
    log_message("INFO", "Changed file modification dates in temporary directory");
}

// Function for getting the modification date of a file
void get_mod_date(const char *file_extension) {
    char command[COMMAND_SIZE];
    snprintf(command, sizeof(command), "stat -c %%y $(cat max_mod_file.txt) | awk '{print $1, $2}' > max_mod_date.txt");
    execute_command(command);
    log_message("INFO", "Got modification date of a file");
}

// Function for reading the modification date from a file
char* read_mod_date_from_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        log_message("ERROR", "Error opening file '%s': %s", filename, strerror(errno));
        exit(1);
    }

    char *mod_date = (char *)malloc(MAX_MOD_DATE_SIZE * sizeof(char));
    if (mod_date == NULL) {
        log_message("ERROR", "Error allocating memory");
        exit(1);
    }

    if (fgets(mod_date, MAX_MOD_DATE_SIZE, file) == NULL) {
        log_message("ERROR", "Error reading mod date from file '%s'", filename);
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
    log_message("INFO", "Set modification dates for all files of a certain type");
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
        log_message("ERROR", "Unsupported archive type: %s", archive_type);
        exit(1);
    }
    execute_command(command);
    log_message("INFO", "Created updated archive from temporary directory");
}

// Function for deleting temporary files
void delete_temp_files() {
    char command[COMMAND_SIZE];
    snprintf(command, sizeof(command), "rm -f max_mod_file.txt max_mod_date.txt");
    execute_command(command);
    log_message("INFO", "Deleted temporary files");
}

// Function for removing the temporary directory
void remove_temp_dir() {
    char command[COMMAND_SIZE];
    snprintf(command, sizeof(command), "rm -rf temp_dir");
    execute_command(command);
    log_message("INFO", "Removed temporary directory");
}

// Function for executing a command
void execute_command(const char *command) {
    printf("Executing command: %s\n", command); // Debugging output
    pid_t pid = fork();
    if (pid == -1) {
        log_message("ERROR", "Error forking process");
        exit(1);
    } else if (pid == 0) {
        // Child process
        int dev_null = open("/dev/null", O_WRONLY);
        dup2(dev_null, STDOUT_FILENO);
        dup2(dev_null, STDERR_FILENO);
        close(dev_null);

        execl("/bin/sh", "sh", "-c", command, (char *)NULL);
        log_message("ERROR", "Error executing command: %s", command);
        exit(1);
    } else {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            int exit_status = WEXITSTATUS(status);
            if (exit_status != 0) {
                log_message("ERROR", "Command '%s' failed with exit status %d", command, exit_status);
                exit(1);
            }
        } else {
            log_message("ERROR", "Command '%s' did not terminate normally", command);
            exit(1);
        }
    }
}


// Function to initialize the log file
void init_log_file() {
    log_file = fopen("program.log", "w");
    if (log_file == NULL) {
        fprintf(stderr, "Error opening log file: %s\n", strerror(errno));
        exit(1);
    }
}