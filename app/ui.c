#include <stdio.h>
#include "../include/table.h"
#include "../include/ui.h"
#include "../include/input.h"

int menu_choice(int choice, Table* table) {
    switch(choice) {
        case 1: 
            case_insert(table); 
            break;
        case 2: 
            case_delete(table); 
            break;
        case 3: 
            case_delete_concrete(table); 
            break;
        case 4: 
            case_search(table); 
            break;
        case 5: 
            case_search_concrete(table); 
            break;
        case 6: 
            printf("\n=== Текущее состояние таблицы ===\n");
            print_table(table);
            break;
        case 7: 
            case_import_table(table); 
            break;
        case 8:
            return EXIT;
        default: 
            printf("Ошибка: Такой команды нет!\n");
    }
    return CONTINUE;
}

void display_menu() {
    printf(
        "\n=== Топ 8 крутых команд в таблице ===\n"
        "(1) Добавить новый элемент\n"
        "(2) Удалить все версии по ключу\n"
        "(3) Удалить конкретную версию\n"
        "(4) Поиск всех версий\n"
        "(5) Поиск конкретной версии\n"
        "(6) Вывод содержимого таблицы\n"
        "(7) Импорт таблицы из файла\n"
        "(8) Выход\n"
        "Выберите действие (1-8): "
    );
}

const char* error_message(ErrorCode code) {
    switch(code) {
        case OK: 
            return "Успешно";
        case ERR_MEMORY: 
            return "Ошибка выделения памяти";
        case ERR_KEY_NOT_FOUND: 
            return "Ключ не найден";
        case ERR_VER_NOT_FOUND: 
            return "Версия не найдена";
        case ERR_FILE: 
            return "Ошибка работы с файлом";
        case ERR_TABLE_NOT_EXISTS: 
            return "Таблицы не существует";
        default: 
            return "Неизвестная ошибка";
    }
}

void case_insert(Table* table) {
    KeyType key;
    InfoType info;
    if (input(&key, "Введите ключ: ") == -1) {
        return;
    }
    if (input(&info, "Введите значение: ") == -1) {
        return;
    }
    ErrorCode err = insert(table, key, info);
    if (err == OK) {
        printf("Элемент успешно добавлен\n");
    } else {
        printf("%s\n", error_message(err));
    }
}

void case_delete(Table* table) {
    KeyType key;
    if (input(&key, "Введите ключ: ") == -1) {
        return;
    }
    ErrorCode err = delete_by_key(table, key);
    if (err == OK) {
        printf("Элементы удалены успешно\n");
    } else {
        printf("%s\n", error_message(err));
    }
}

void case_delete_concrete(Table* table) {
    KeyType key;
    RelType release;
    if (input(&key, "Введите ключ: ") == -1) {
        return;
    }
    if (input(&release, "Введите версию: ") == -1) {
        return;
    }
    
    ErrorCode err = delete_by_version(table, key, release);
    if (err == OK) {
        printf("Версия удалена успешно\n");
    } else {
        printf("%s\n", error_message(err));
    }
}

void case_search(Table* table) {
    KeyType key;
    if (input(&key, "Введите ключ: ") == -1) {
        return;
    }
    Table* result;
    ErrorCode err = search_all_versions(table, &result, key);
    if (err == OK) {
        print_table(result);
        free_table(result);
    } else {
        printf("%s\n", error_message(err));
    }
}

void case_search_concrete(Table* table) {
    KeyType key;
    RelType release;
    if (input(&key, "Введите ключ: ") == -1) {
        return;
    }
    if (input(&release, "Введите версию: ") == -1) {
        return;
    }
    Table* result;
    ErrorCode err = search_specific_version(table, &result, key, release);
    if (err == OK) {
        print_table(result);
        free_table(result);
    } else {
        printf("%s\n", error_message(err));
    }
}

void case_import_table(Table* table) {
    char* filename;
    scanf("%*c");
    printf("Введите имя файла: ");
    filename = my_getline(NULL, 1);
    ErrorCode err = import_from_file(table, filename);
    if (err == OK) {
        printf("Данные успешно импортированы\n");
    } else {
        printf("%s\n", error_message(err));
    }
    free(filename);
}
