#include <stdbool.h>
#ifndef UTILS_H
#define UTILS_H

#define COMMAND_SIZE 256
#define MAX_MOD_DATE_SIZE 20
#define FILE_EXTENSION_SIZE 10
#define TIME_STAMP_SIZE 80

extern int temp_dir_created;
extern int temp_files_created;

void execution_successful_message();
char *extract_file_name(char *path, bool* flag);
void signal_handler(int signal);
void register_signal_handlers();
void register_cleanup_functions();
void execute_command(const char *command);
void execution_failed_message();

#endif