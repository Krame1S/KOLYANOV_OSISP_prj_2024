#include <stdbool.h>
#ifndef UTILS_H
#define UTILS_H

#define COMMAND_SIZE 256
#define MAX_MOD_DATE_SIZE 20
#define FILE_EXTENSION_SIZE 10
#define TIME_STAMP_SIZE 80

extern int temp_dir_created;
extern int temp_files_created;

void initialize_program();
void handle_missing_archive_path(int argc, char *argv[], char *working_dir);
char *extract_file_name(char *path, bool* flag);
void signal_handler(int signal);
void register_signal_handlers();
void register_cleanup_functions();
void execute_command(const char *command);
void register_cleanup_functions();

#endif