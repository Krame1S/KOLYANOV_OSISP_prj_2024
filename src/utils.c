#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

#include "utils.h"
#include "logging.h"
#include "archive_processing.h"

int temp_dir_created = 0;
int temp_files_created = 0;

void exit_message() {
    if(exit_status == 0)
        printf("The archive has been updated successfully.\n");
    else if(exit_status == 1)
        printf("The archive has not been updated.\n");
}

// Function to initialize the program
void initialize_program() {
    init_log_file();
    log_message(INFO, "Program started");
    register_signal_handlers();
    register_cleanup_functions();
}

// Function for handling missing archive path
void handle_missing_archive_path(int argc, char *argv[], char *working_dir) {
    if (argc < 2) {
        fprintf(stderr, "Error: No archive path provided.\n");
        fprintf(stderr, "Usage: %s [archive_path] [options]\n", argv[0]);
        exit_status = 1;
        exit(1);
    }
}

// Function for extracting the file name from a path
char *extract_file_name(char *path, bool* flag) {
    char *last_slash = strrchr(path, '/');
    if (last_slash == NULL) {
        *flag = 1;
        return path; 
    } else {
        return last_slash + 1; 
    }
}

// Signal handler function
void signal_handler(int signal) {
    log_message(INFO, "Received signal %d, cleaning up and exiting...", signal);
    exit(exit_status);
}

// Function to register signal handlers
void register_signal_handlers() {
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
}

// Function to register cleanup functions
void register_cleanup_functions() {
    atexit(exit_message);
    atexit(delete_temp_files);
    atexit(remove_temp_dir);
    atexit(close_log_file);
}

// Function for executing a command
void execute_command(const char *command) {
    pid_t pid = fork();
    if (pid == -1) {
        log_message(ERROR, "Error forking process");
        exit_status = 1;
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
        exit_status = 1;
        exit(1);
    } else {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            int exit_status = WEXITSTATUS(status);
            if (exit_status != 0) {
                log_message(ERROR, "Command '%s' failed with exit status %d", command, exit_status);
                exit_status = 1;
                exit(1);
            }
        } else {
            log_message(ERROR, "Command '%s' did not terminate normally", command);
            exit_status = 1;
            exit(1);
        }
    }
}