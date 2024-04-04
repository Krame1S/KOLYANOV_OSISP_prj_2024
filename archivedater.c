#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <file_extension>\n", argv[0]);
        return 1;
    }

    char *file_extension = argv[1];
    char command[256];

    //Распаковка архива
    sprintf(command, "tar -xvf archive.tar.gz");
    system(command);

    // Изменение дат модификации файлов указанного типа
    // Найти файл с максимальной датой модификации
    sprintf(command, "ls -lt *%s | head -n 2 | tail -n 1 > max_mod_file.txt", file_extension);
    system(command);

    // Получить дату модификации этого файла
    sprintf(command, "stat -c %y $(cat max_mod_file.txt) > max_mod_date.txt");
    system(command);

    // Установить эту дату модификации для всех файлов этого типа
    sprintf(command, "touch -d \"$(cat max_mod_date.txt)\" *%s", file_extension);
    system(command);

    //system("rm max_mod_file.txt max_mod_date.txt");

    // Создание нового архива с измененными файлами
    sprintf(command, "tar -czvf updated_archive.tar.gz *");
    system(command);

    return 0;
}

