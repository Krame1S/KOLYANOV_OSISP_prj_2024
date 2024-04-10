#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

// Константы для размеров буферов
#define COMMAND_SIZE 256
#define MAX_MOD_DATE_SIZE 20
#define FILE_EXTENSION_SIZE 10

// Функция для распаковки архива
void extract_archive() {
    char command[COMMAND_SIZE];
    snprintf(command, sizeof(command), "tar -xvf archive.tar.gz");
    if (system(command) != 0) {
        perror("Error extracting archive");
        exit(1);
    }
}

// Функция для изменения дат модификации файлов указанного типа
void change_file_mod_dates(const char *file_extension) {
    char command[COMMAND_SIZE];
    snprintf(command, sizeof(command), "ls -lt -t *%s | head -n 1 | awk '{print $9}' > max_mod_file.txt", file_extension);
    if (system(command) != 0) {
        perror("Error finding file with maximum modification date");
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

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <file_extension>\n", argv[0]);
        return 1;
    }

    char file_extension[FILE_EXTENSION_SIZE];
    strncpy(file_extension, argv[1], FILE_EXTENSION_SIZE - 1);
    file_extension[FILE_EXTENSION_SIZE - 1] = '\0'; // Ensure null-termination

    // Распаковка архива
    extract_archive();

    // Изменение дат модификации файлов указанного типа
    change_file_mod_dates(file_extension);

    // Получение даты модификации файла
    get_mod_date(file_extension);

    // Получение даты модификации в нужном формате (YYYY-MM-DD HH:MM:SS)
    FILE *dateFile = fopen("max_mod_date.txt", "r");
    if (dateFile == NULL) {
        perror("Error opening max_mod_date.txt");
        return 1;
    }

    char max_mod_date[MAX_MOD_DATE_SIZE];
    if (fgets(max_mod_date, sizeof(max_mod_date), dateFile) == NULL) {
        perror("Error reading date from max_mod_date.txt");
        fclose(dateFile);
        return 1;
    }

    fclose(dateFile);

    // Удаление символа новой строки из даты
    max_mod_date[strcspn(max_mod_date, "\n")] = '\0';

    // Установка даты модификации для всех файлов указанного типа
    set_mod_dates(file_extension, max_mod_date);

    // Создание нового архива с измененными файлами
    create_updated_archive();

    return 0;
}
