#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <file_extension>\n", argv[0]);
        return 1;
    }

    char *file_extension = argv[1];
    char command[256];

    // Распаковка архива
    sprintf(command, "tar -xvf archive.tar.gz");
    if (system(command) != 0) {
        printf("Error extracting archive\n");
        return 1;
    }

    // Изменение дат модификации файлов указанного типа
    // Найти файл с максимальной датой модификации
    sprintf(command, "ls -lt -t *%s | head -n 1 | awk '{print $9}' > max_mod_file.txt", file_extension);
    if (system(command) != 0) {
        printf("Error finding file with maximum modification date\n");
        return 1;
    }

    // Получить дату модификации этого файла
    sprintf(command, "stat -c %%y $(cat max_mod_file.txt) | awk '{print $1, $2}' > max_mod_date.txt");
    if (system(command) != 0) {
        printf("Error getting maximum modification date\n");
        return 1;
    }

    // Получить дату модификации в нужном формате (YYYY-MM-DD HH:MM:SS)
    FILE *dateFile = fopen("max_mod_date.txt", "r");
    if (dateFile == NULL) {
        printf("Error reading max_mod_date.txt\n");
        return 1;
    }

    char max_mod_date[20];
    if (fgets(max_mod_date, sizeof(max_mod_date), dateFile) == NULL) {
        printf("Error reading date from max_mod_date.txt\n");
        fclose(dateFile);
        return 1;
    }

    fclose(dateFile);

    // Удалить символ новой строки из даты
    max_mod_date[strcspn(max_mod_date, "\n")] = '\0';

    // Установить эту дату модификации для всех файлов этого типа
    sprintf(command, "find . -name '*%s' -exec touch -d '%s' {} +", file_extension, max_mod_date);
    if (system(command) != 0) {
        printf("Error setting modification date for files\n");
        return 1;
    }

    // Создание нового архива с измененными файлами
    sprintf(command, "tar -czvf updated_archive.tar.gz *");
    if (system(command) != 0) {
        printf("Error creating updated archive\n");
        return 1;
    }

    return 0;
}