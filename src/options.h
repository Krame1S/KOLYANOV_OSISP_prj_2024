#ifndef OPTIONS_H
#define OPTIONS_H

typedef enum {
    OPT_VERBOSE,
    OPT_QUIET,
    OPT_FILETYPE,
    OPT_LOGLEVEL,
    OPT_OUTPUTDIR,
    OPT_HELP,
    OPT_VERSION,
    OPT_UNKNOWN
} Option;

typedef struct {
    Option option;
    char *argument;
} ParsedOption;

void parse_command_line_arguments(int argc, char *argv[], char **working_dir, char **file_extension);
ParsedOption parse_option(const char *arg);
void print_help_message();

#endif