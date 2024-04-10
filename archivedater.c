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
void extract_archive_to_temp_dir(const char *archive_path);
void create_updated_archive_from_temp_dir(const char *archive_path);
void extract_archive();
void change_file_mod_dates_in_temp_dir(const char *file_extension);
void get_mod_date(const char *file_extension);
void set_mod_dates(const char *file_extension, const char *max_mod_date);
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
void extract_archive_to_temp_dir(const char *archive_path) {
    char command[COMMAND_SIZE];
    snprintf(command, sizeof(command), "tar -xvf %s -C temp_dir", archive_path);
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

// Функция для создания нового архива из временной директории
void create_updated_archive_from_temp_dir(const char *archive_path) {
    char command[COMMAND_SIZE];
    snprintf(command, sizeof(command), "tar -czvf %s -C temp_dir .", archive_path);
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
        if (WIFEXITED(status)) {
            int exit_status = WEXITSTATUS(status);
            if (exit_status != 0) {
                fprintf(stderr, "Command '%s' failed with exit status %d\n", command, exit_status);
                exit(1);
            }
        } else {
            fprintf(stderr, "Command '%s' did not terminate normally\n", command);
            exit(1);
        }
    }
}

// Функция для чтения расширения файла из аргументов командной строки
char* read_file_extension(int argc, char *argv[]) {
    if (argc >= 2) {
        return argv[1];
    } else {
        fprintf(stderr, "Usage: program file_extension\n");
        exit(1);
    }
}

// Функция для чтения даты модификации из файла
char* read_mod_date_from_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening file '%s': %s\n", filename, strerror(errno));
        exit(1);
    }

    char *mod_date = (char *)malloc(MAX_MOD_DATE_SIZE * sizeof(char));
    if (mod_date == NULL) {
        fprintf(stderr, "Error allocating memory\n");
        exit(1);
    }

    if (fgets(mod_date, MAX_MOD_DATE_SIZE, file) == NULL) {
        fprintf(stderr, "Error reading mod date from file '%s'\n", filename);
        exit(1);
    }

    fclose(file);
    return mod_date;
}

int main(int argc, char *argv[]) {
    // Чтение расширения файла из аргументов командной строки
    char *file_extension = read_file_extension(argc, argv);

    // Создание временной директории
    create_temp_dir();

    // Распаковка архива во временную директорию
    const char *archive_path = argv[2];
    extract_archive_to_temp_dir(archive_path);

    // Изменение дат модификации файлов указанного типа во временной директории
    change_file_mod_dates_in_temp_dir(file_extension);

    // Получение даты модификации последнего измененного файла
    get_mod_date(file_extension);

    // Чтение даты модификации из файла
    char *max_mod_date = read_mod_date_from_file("max_mod_date.txt");

    // Установка дат модификации для всех файлов указанного типа
    set_mod_dates(file_extension, max_mod_date);

    // Создание обновленного архива из временной директории
    create_updated_archive_from_temp_dir(archive_path);

    // Удаление временных файлов
    delete_temp_files();

    // Удаление временной директории
    remove_temp_dir();

    printf("Updated archive created successfully.\n");

    return 0;
}