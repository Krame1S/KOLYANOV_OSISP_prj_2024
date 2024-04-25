#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

#include "archive_processing.h"
#include "logging.h"
#include "utils.h"

//global var for tracking exit status
int exit_status = 0;

// Function for processing the archive
void process_archive(char *archive_path, const char *archive_type, const char *file_extension, char *working_dir) {
    create_temp_dir();
    extract_archive_to_temp_dir(archive_path, archive_type);
    change_file_mod_dates_in_temp_dir(file_extension);
    get_mod_date();
    char *max_mod_date = read_mod_date_from_file("max_mod_date.txt");
    set_mod_dates(file_extension, max_mod_date);
    create_updated_archive_from_temp_dir(archive_path, archive_type, working_dir);
}

// Function for reading the file extension from command-line arguments
char* read_archive_type(const char *archive_path) {
    int path_length = strlen(archive_path);
    if (path_length >= 7 && strcmp(archive_path + path_length - 7, ".tar.gz") == 0) {
        return "tar.gz";
    } else if (path_length >= 8 && strcmp(archive_path + path_length - 8, ".tar.bz2") == 0) {
        return "tar.bz2";
    } else if (path_length >= 4 && strcmp(archive_path + path_length - 4, ".zip") == 0) {
        return "zip";
    } else {
        log_message(ERROR, "Unsupported archive type or archive path '%s'", archive_path);
        exit_status = 1;
        exit(1);
    }
}

// Function for creating a temporary directory
void create_temp_dir() {
    char command[COMMAND_SIZE];
    snprintf(command, sizeof(command), "mkdir temp_dir");
    execute_command(command);
    log_message(INFO, "Created temporary directory");
    temp_dir_created = 1;
}

// Function for extracting an archive to the temporary directory
void extract_archive_to_temp_dir(char *archive_path, const char *archive_type) {
    char command[COMMAND_SIZE];
    if (strcmp(archive_type, "tar.gz") == 0) {
        snprintf(command, sizeof(command), "tar -xzvf %s -C temp_dir", archive_path);
    } else if (strcmp(archive_type, "tar.bz2") == 0) {
        snprintf(command, sizeof(command), "tar -xjvf %s -C temp_dir", archive_path);
    } else if (strcmp(archive_type, "zip") == 0) {
        snprintf(command, sizeof(command), "unzip %s -d temp_dir", archive_path);
    } else {
        log_message(ERROR, "Unsupported archive type: %s", archive_type);
        exit_status = 1;
        exit(1);
    }
    execute_command(command);
    log_message(INFO, "Extracted archive to temporary directory");
}

// Function for changing file modification dates in the temporary directory
void change_file_mod_dates_in_temp_dir(const char *file_extension) {
    char command[COMMAND_SIZE];
    snprintf(command, sizeof(command), "find temp_dir -type f -name '*%s' -printf '%%T@ %%p\\n' | sort -n | tail -n 1 | awk '{print $2}' > max_mod_file.txt", file_extension);
    temp_files_created = 1;
    execute_command(command);
    log_message(INFO, "Changed file modification dates in temporary directory");
}

// Function for getting the modification date of a file
void get_mod_date() {
    char command[COMMAND_SIZE];
    snprintf(command, sizeof(command), "stat -c %%y $(cat max_mod_file.txt) | awk '{print $1, $2}' > max_mod_date.txt");
    temp_files_created = 1;
    execute_command(command);
    log_message(INFO, "Got modification date of a file");
}

// Function for reading the modification date from a file
char* read_mod_date_from_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        log_message(ERROR, "Error opening file '%s': %s", filename, strerror(errno));
        exit_status = 1;
        exit(1);
    }

    char *mod_date = (char *)malloc(MAX_MOD_DATE_SIZE * sizeof(char));
    if (mod_date == NULL) {
        log_message(ERROR, "Error allocating memory");
        exit_status = 1;
        exit(1);
    }

    if (fgets(mod_date, MAX_MOD_DATE_SIZE, file) == NULL) {
        log_message(ERROR, "Error reading mod date from file '%s'", filename);
        exit_status = 1;
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
    log_message(INFO, "Set modification dates for files");
}

// Function for creating an updated archive from the temporary directory
void create_updated_archive_from_temp_dir(char *archive_path, const char *archive_type, char *working_dir) {
    char command[COMMAND_SIZE];
    bool flag = 0;
    char *filename = extract_file_name(archive_path, &flag);
    if (flag == 0) {
        if(archive_path == working_dir)
            filename = "";
        archive_path = working_dir;
    } else {
        archive_path = "";
    }
    if (strcmp(archive_type, "tar.gz") == 0) {
        snprintf(command, sizeof(command), "tar -czvf %s%s -C temp_dir .", archive_path, filename);
    } else if (strcmp(archive_type, "tar.bz2") == 0) {
        snprintf(command, sizeof(command), "tar -cjvf %s%s -C temp_dir .", archive_path, filename);
    } else if (strcmp(archive_type, "zip") == 0) {
        snprintf(command, sizeof(command), "cd temp_dir && zip -r %s%s .", archive_path, filename);
    } else {
        log_message(ERROR, "Unsupported archive type: %s", archive_type);
        exit_status = 1;
        exit(1);
    }
    execute_command(command);
    log_message(INFO, "Created updated archive from temporary directory");
}

// Function for deleting temporary files
void delete_temp_files() {
    if (temp_files_created) {
        char command[COMMAND_SIZE];
        snprintf(command, sizeof(command), "rm -f max_mod_file.txt max_mod_date.txt");
        execute_command(command);
        log_message(INFO, "Deleted temporary files");
    }
}

// Function for removing the temporary directory
void remove_temp_dir() {
    if (temp_dir_created) {
        char command[COMMAND_SIZE];
        snprintf(command, sizeof(command), "rm -rf temp_dir");
        execute_command(command);
        log_message(INFO, "Removed temporary directory");
    }
}