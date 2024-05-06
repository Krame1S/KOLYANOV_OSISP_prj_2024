#ifndef ARCHIVE_PROCESSING_H
#define ARCHIVE_PROCESSING_H

void process_archive(char *archive_path, const char *archive_type, const char *file_extension, char *working_dir);
char* read_archive_type(const char *archive_path);
void create_temp_dir();
void extract_archive_to_temp_dir(char *archive_path, const char *archive_type);
void change_file_mod_dates_in_temp_dir(const char *file_extension);
void get_mod_date();
char* read_mod_date_from_file(const char *filename);
void set_mod_dates(const char *file_extension, const char *max_mod_date);
void create_updated_archive_from_temp_dir(char *archive_path, const char *archive_type, char *working_dir);
void delete_temp_files();
void remove_temp_dir();

#endif