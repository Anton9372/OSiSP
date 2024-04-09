#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define MAX_LEN_RECORD 80
#define COUNT_RECORDS 10

typedef struct record {
    char name[MAX_LEN_RECORD];
    char address[MAX_LEN_RECORD];
    uint8_t semester;
} record;

int main() {
    FILE *file = fopen("file.bin", "wb"); // Открыть файл для записи в бинарном формате

    if (file == NULL) {
        perror("Ошибка открытия файла");
        return EXIT_FAILURE;
    }

    record records[COUNT_RECORDS];

    // Заполнение структур данными
    for (int i = 0; i < COUNT_RECORDS; ++i) {
        sprintf(records[i].name, "Name %d", i);
        sprintf(records[i].address, "Address %d", i);
        records[i].semester = i % 5 + 1; // Просто для примера, чтобы значения были разные
    }

    // Запись структур в файл
    fwrite(records, sizeof(record), COUNT_RECORDS, file);

    fclose(file); // Закрыть файл

    printf("Данные успешно записаны в файл.\n");

    return EXIT_SUCCESS;
}
