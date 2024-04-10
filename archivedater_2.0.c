#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

// Константы для размеров буферов
#define COMMAND_SIZE 256
#define MAX_MOD_DATE_SIZE 20
#define FILE_EXTENSION_SIZE 10

// Функция для создания временной директории
void create_temp_dir() {
    char command[COMMAND_SIZE];
    snprintf(command, sizeof(command), "mkdir temp_dir");
    if (system(command) != 0) {
        perror("Error creating temporary directory");
        exit(1);
    }
}

// Функция для распаковки архива в временную директорию
void extract_archive_to_temp_dir() {
    char command[COMMAND_SIZE];
    snprintf(command, sizeof(command), "tar -xvf archive.tar.gz -C temp_dir");
    if (system(command) != 0) {
        perror("Error extracting archive to temporary directory");
        exit(1);
    }
}

// Функция для создания нового архива из временной директории
void create_updated_archive_from_temp_dir() {
    char command[COMMAND_SIZE];
    snprintf(command, sizeof(command), "tar -czvf updated_archive.tar.gz -C temp_dir .");
    if (system(command) != 0) {
        perror("Error creating updated archive from temporary directory");
        exit(1);
    }
}

// Функция для распаковки архива
void extract_archive() {
    char command[COMMAND_SIZE];
    snprintf(command, sizeof(command), "tar -xvf archive.tar.gz");
    if (system(command) != 0) {
        perror("Error extracting archive");
        exit(1);
    }
}

// Функция для изменения дат модификации файлов указанного типа в temp_dir
void change_file_mod_dates_in_temp_dir(const char *file_extension) {
    char command[COMMAND_SIZE];
    snprintf(command, sizeof(command), "ls -lt -t temp_dir/*%s | head -n 1 | awk '{print $9}' > max_mod_file.txt", file_extension);
    if (system(command) != 0) {
        perror("Error finding file with maximum modification date in temporary directory");
        exit(1);
    }
}

// Функция для получения даты модификации файла
void get_mod_date(const char *file_extension) {
    char command[COMMAND_SIZE];
    snprintf(command, sizeof(command), "stat -c %%y $(cat max_mod_file.txt) | awk '{print $1, $2}' > max_mod_date.txt");
    if (system(command) != 0) {
        perror("Error getting maximum modification date");
        exit(1);
    }
}

// Функция для установки даты модификации для всех файлов указанного типа
void set_mod_dates(const char *file_extension, const char *max_mod_date) {
    char command[COMMAND_SIZE];
    snprintf(command, sizeof(command), "find . -name '*%s' -exec touch -d '%s' {} +", file_extension, max_mod_date);
    if (system(command) != 0) {
        perror("Error setting modification date for files");
        exit(1);
    }
}

// Функция для создания нового архива с измененными файлами
void create_updated_archive() {
    char command[COMMAND_SIZE];
    snprintf(command, sizeof(command), "tar -czvf updated_archive.tar.gz *");
    if (system(command) != 0) {
        perror("Error creating updated archive");
        exit(1);
    }
}

// Функция для удаления временных файлов
void delete_temp_files() {
    char command[COMMAND_SIZE];
    snprintf(command, sizeof(command), "rm -f max_mod_file.txt max_mod_date.txt");
    if (system(command) != 0) {
        perror("Error deleting temporary files");
        exit(1);
    }
}

// Функция для удаления временной директории
void remove_temp_dir() {
    char command[COMMAND_SIZE];
    snprintf(command, sizeof(command), "rm -rf temp_dir");
    if (system(command) != 0) {
        perror("Error removing temporary directory");
        exit(1);
    }
}

// Функция для чтения расширения файла из аргументов командной строки
char* read_file_extension(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <file_extension>\n", argv[0]);
        exit(1);
    }
    char *file_extension = malloc(FILE_EXTENSION_SIZE);
    if (file_extension == NULL) {
        perror("Error allocating memory for file extension");
        exit(1);
    }
    strncpy(file_extension, argv[1], FILE_EXTENSION_SIZE - 1);
    file_extension[FILE_EXTENSION_SIZE - 1] = '\0'; // Ensure null-termination
    return file_extension;
}

// Функция для чтения даты модификации из файла
char* read_mod_date_from_file(const char *filename) {
    FILE *dateFile = fopen(filename, "r");
    if (dateFile == NULL) {
        perror("Error opening file");
        exit(1);
    }

    char *max_mod_date = malloc(MAX_MOD_DATE_SIZE);
    if (max_mod_date == NULL) {
        perror("Error allocating memory for modification date");
        fclose(dateFile);
        exit(1);
    }
    if (fgets(max_mod_date, MAX_MOD_DATE_SIZE, dateFile) == NULL) {
        perror("Error reading date from file");
        fclose(dateFile);
        free(max_mod_date); // Free allocated memory before exiting
        exit(1);
    }

    fclose(dateFile);
    max_mod_date[strcspn(max_mod_date, "\n")] = '\0'; // Удаление символа новой строки
    return max_mod_date;
}

int main(int argc, char *argv[]) {
    char *file_extension = read_file_extension(argc, argv);

    // Создание временной директории
    create_temp_dir();

    // Распаковка архива в временную директорию
    extract_archive_to_temp_dir();

    // Изменение дат модификации файлов указанного типа в temp_dir
    change_file_mod_dates_in_temp_dir(file_extension);

    // Получение даты модификации файла
    get_mod_date(file_extension);

    // Получение даты модификации в нужном формате (YYYY-MM-DD HH:MM:SS)
    char *max_mod_date = read_mod_date_from_file("max_mod_date.txt");

    // Установка даты модификации для всех файлов указанного типа
    set_mod_dates(file_extension, max_mod_date);

    // Создание нового архива с измененными файлами из временной директории
    create_updated_archive_from_temp_dir();

    // Удаление временных файлов
    delete_temp_files();

    // Удаление временной директории
    remove_temp_dir();

    // Освобождение памяти
    free(file_extension);
    free(max_mod_date);

    return 0;
}
