#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>

// Константы для размеров буферов
#define COMMAND_SIZE 256
#define MAX_MOD_DATE_SIZE 20
#define FILE_EXTENSION_SIZE 10

// Function prototypes
void create_temp_dir();
void extract_archive_to_temp_dir();
void create_updated_archive_from_temp_dir();
void extract_archive();
void change_file_mod_dates_in_temp_dir(const char *file_extension);
void get_mod_date(const char *file_extension);
void set_mod_dates(const char *file_extension, const char *max_mod_date);
void create_updated_archive();
void delete_temp_files();
void remove_temp_dir();
char* read_file_extension(int argc, char *argv[]);
char* read_mod_date_from_file(const char *filename);
void execute_command(const char *command);

// Функция для создания временной директории
void create_temp_dir() {
    char command[COMMAND_SIZE];
    snprintf(command, sizeof(command), "mkdir temp_dir");
    execute_command(command);
}

// Функция для распаковки архива в временную директорию
void extract_archive_to_temp_dir() {
    char command[COMMAND_SIZE];
    snprintf(command, sizeof(command), "tar -xvf archive.tar.gz -C temp_dir");
    execute_command(command);
}

// Функция для создания нового архива из временной директории
void create_updated_archive_from_temp_dir() {
    char command[COMMAND_SIZE];
    snprintf(command, sizeof(command), "tar -czvf updated_archive.tar.gz -C temp_dir .");
    execute_command(command);
}

// Функция для распаковки архива
void extract_archive() {
    char command[COMMAND_SIZE];
    snprintf(command, sizeof(command), "tar -xvf archive.tar.gz");
    execute_command(command);
}

// Функция для изменения дат модификации файлов указанного типа в temp_dir
void change_file_mod_dates_in_temp_dir(const char *file_extension) {
    char command[COMMAND_SIZE];
    snprintf(command, sizeof(command), "ls -lt -t temp_dir/*%s | head -n 1 | awk '{print $9}' > max_mod_file.txt", file_extension);
    execute_command(command);
}

// Функция для получения даты модификации файла
void get_mod_date(const char *file_extension) {
    char command[COMMAND_SIZE];
    snprintf(command, sizeof(command), "stat -c %%y $(cat max_mod_file.txt) | awk '{print $1, $2}' > max_mod_date.txt");
    execute_command(command);
}

// Функция для установки даты модификации для всех файлов указанного типа
void set_mod_dates(const char *file_extension, const char *max_mod_date) {
    char command[COMMAND_SIZE];
    snprintf(command, sizeof(command), "find . -name '*%s' -exec touch -d '%s' {} +", file_extension, max_mod_date);
    execute_command(command);
}

// Функция для создания нового архива с измененными файлами
void create_updated_archive() {
    char command[COMMAND_SIZE];
    snprintf(command, sizeof(command), "tar -czvf updated_archive.tar.gz *");
    execute_command(command);
}

// Функция для удаления временных файлов
void delete_temp_files() {
    char command[COMMAND_SIZE];
    snprintf(command, sizeof(command), "rm -f max_mod_file.txt max_mod_date.txt");
    execute_command(command);
}

// Функция для удаления временной директории
void remove_temp_dir() {
    char command[COMMAND_SIZE];
    snprintf(command, sizeof(command), "rm -rf temp_dir");
    execute_command(command);
}

// Общая функция для выполнения команды
void execute_command(const char *command) {
    pid_t pid = fork();
    if (pid == -1) {
        perror("Error forking process");
        exit(1);
    } else if (pid == 0) {
        // Дочерний процесс
        execl("/bin/sh", "sh", "-c", command, (char *)NULL);
        perror("Error executing command");
        exit(1);
    } else {
        // Родительский процесс
        int status;
        waitpid(pid, &status, 0);
        if (WEXITSTATUS(status) != 0) {
            perror("Error executing command");
            exit(1);
        }
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
