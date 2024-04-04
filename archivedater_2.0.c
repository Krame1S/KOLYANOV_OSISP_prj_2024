#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>
#include <unistd.h>
#include <archive.h> 
#include <archive_entry.h>

#define MAX_PATH 1024
#define MAX_EXT 32

void extract_archive(const char* archive_path, const char* output_dir) {
    struct archive* archive;
    struct archive_entry* entry;
    int r;

    archive = archive_read_new();
    archive_read_support_format_all(archive);
    archive_read_support_filter_all(archive);
    r = archive_read_open_filename(archive, archive_path, 10240);
    if (r != ARCHIVE_OK) {
        fprintf(stderr, "Ошибка при открытии архива: %s\n", archive_error_string(archive));
        return;
    }

    r = archive_read_next_header(archive, &entry);
    while (r == ARCHIVE_OK) {
        const char* entry_pathname = archive_entry_pathname(entry);
        char output_path[MAX_PATH];
        snprintf(output_path, MAX_PATH, "%s/%s", output_dir, entry_pathname);

        archive_entry_set_pathname(entry, output_path);
        r = archive_read_extract(archive, entry, 0);
        if (r != ARCHIVE_OK) {
            fprintf(stderr, "Ошибка при распаковке: %s\n", archive_error_string(archive));
        }
        r = archive_read_next_header(archive, &entry);
    }

    archive_read_close(archive);
    archive_read_free(archive);
}

void update_file_times(const char* directory, const char* extension, time_t max_time) {
}

time_t find_max_file_time(const char* directory, const char* extension) {
}

void create_archive(const char* archive_path, const char* input_dir) {
    struct archive* archive;
    struct archive_entry* entry;
    struct stat st;
    char path[MAX_PATH];

    archive = archive_write_new();
    archive_write_set_format_pax_restricted(archive); 
    archive_write_open_filename(archive, archive_path);

    DIR* dir = opendir(input_dir);
    if (dir == NULL) {
        fprintf(stderr, "Не удалось открыть директорию: %s\n", input_dir);
        return;
    }

    struct dirent* file;
    while ((file = readdir(dir)) != NULL) {
        if (strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0) {
            continue;
        }

        snprintf(path, MAX_PATH, "%s/%s", input_dir, file->d_name);
        if (stat(path, &st) != 0) {
            fprintf(stderr, "Ошибка при получении информации о файле: %s\n", path);
            continue;
        }

        entry = archive_entry_new();
        archive_entry_set_pathname(entry, path);
        archive_entry_set_size(entry, st.st_size);
        archive_entry_set_filetype(entry, S_ISDIR(st.st_mode) ? AE_IFDIR : AE_IFREG);
        archive_entry_set_perm(entry, st.st_mode);
        archive_write_header(archive, entry);

        if (S_ISREG(st.st_mode)) {
            FILE* file = fopen(path, "rb");
            if (file == NULL) {
                fprintf(stderr, "Не удалось открыть файл: %s\n", path);
                continue;
            }

            char buffer[8192];
            size_t bytes_read;
            while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
                archive_write_data(archive, buffer, bytes_read);
            }

            fclose(file);
        }

        archive_entry_free(entry);
    }

    closedir(dir);
    archive_write_close(archive);
    archive_write_free(archive);
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Использование: %s <путь_к_архиву> <расширение_файла> <формат_архива>\n", argv[0]);
        return 1;
    }

    const char* archive_path = argv[1];
    const char* file_extension = argv[2];
    const char* archive_format = argv[3]; 

    char output_dir[MAX_PATH];
    snprintf(output_dir, MAX_PATH, "extracted_%s", archive_path);
    mkdir(output_dir, 0755); 

    // Распаковка архива
    extract_archive(archive_path, output_dir);

    // Поиск максимальной даты модификации для файлов с заданным расширением
    time_t max_time = find_max_file_time(output_dir, file_extension);
    if (max_time == 0) {
        fprintf(stderr, "Не найдено файлов с расширением %s\n", file_extension);
        return 1;
    }

    // Установка максимальной даты модификации для всех файлов с заданным расширением
    update_file_times(output_dir, file_extension, max_time);

    // Создание нового архива с измененными файлами
    char new_archive_path[MAX_PATH];
    snprintf(new_archive_path, MAX_PATH, "updated_%s.%s", archive_path, archive_format);
    create_archive(new_archive_path, output_dir);

    return 0;
}
