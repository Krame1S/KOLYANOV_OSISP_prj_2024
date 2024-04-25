#include <stdio.h>
#include <stdlib.h>

#include "archive_processing.h"
#include "logging.h"
#include "options.h"
#include "utils.h"

int main(int argc, char *argv[]) {
    char *working_dir = NULL;
    char *file_extension = "*";

    // Initialize log file
    init_log_file();

    // Register signal handlers
    register_signal_handlers();

    // Parse command-line arguments
    parse_command_line_arguments(argc, argv, &working_dir, &file_extension);

    // Extract and process the archive
    char *archive_path = argv[1];
    const char *archive_type = read_archive_type(archive_path);
    process_archive(archive_path, archive_type, file_extension, working_dir);

    delete_temp_files();
    remove_temp_dir();
    close_log_file();

    if(exit_status == 0)
        execution_successful_message();
    else
        execution_failed_message();

    return 0;
}