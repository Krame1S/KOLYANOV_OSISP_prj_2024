#include <stdio.h>
#include <stdlib.h>

#include "archive_processing.h"
#include "logging.h"
#include "options.h"
#include "utils.h"

int main(int argc, char *argv[]) {
    char* working_dir = argv[1];
    char *file_extension = "*";

    initialize_program();
    
    handle_missing_archive_path(argc, argv, working_dir);

    parse_command_line_arguments(argc, argv, &working_dir, &file_extension);

    // Extract and process the archive
    char *archive_path = argv[1];
    const char *archive_type = read_archive_type(archive_path);
    process_archive(archive_path, archive_type, file_extension, working_dir);

    return 0;
}