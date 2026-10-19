#include "../include/table.h"
#include "../include/input.h"
#include <string.h>

Table* create_table() {
    Table* table = malloc(sizeof(Table));
    if (!table) {
        return NULL;
    }
    table -> ks = NULL;
    return table;
}

KeySpace* find_key_space(Table* table, KeyType key) {
    KeySpace* current = table -> ks;
    while (current) {
        if (current -> key == key) {
            return current;
        }
        current = current -> link;
    }
    return NULL;
}

ErrorCode insert(Table* table, KeyType key, InfoType info) {
    if (!table) {
        return ERR_TABLE_NOT_EXISTS;
    }
    KeySpace* ks = find_key_space(table, key);
    if (!ks) {
        KeySpace* new_ks = malloc(sizeof(KeySpace));
        if (!new_ks) {
            return ERR_MEMORY;
        }
        new_ks -> node = NULL;
        new_ks -> link = table -> ks;
        new_ks -> key = key;

        table -> ks = new_ks;
        ks = new_ks;
    }
    Node* new_node = malloc(sizeof(Node));
    if (new_node == NULL) {
        return ERR_MEMORY;
    }
    InfoType* info_ptr = malloc(sizeof(InfoType));
    if (info_ptr == NULL) {
        free(new_node);
        new_node = NULL;
        return ERR_MEMORY;
    }
    *info_ptr = info;
    new_node -> info = info_ptr;
    if (!ks -> node) {
        new_node -> release = 1;
    } else {
        RelType current_ver = ks -> node -> release;
        new_node -> release = current_ver + 1;
    }
    new_node -> next = ks -> node;
    ks -> node = new_node;
    return OK;
}

ErrorCode delete_by_key(Table* table, KeyType key) {
    if (!table) {
        return ERR_TABLE_NOT_EXISTS;
    }
    KeySpace* curr = table -> ks;
    KeySpace* prev = NULL;
    while (curr) {
        if (curr -> key == key) {
            if (prev != NULL) {
                prev -> link = curr -> link;
            } else {
                table -> ks = curr -> link;
            }
            Node* node = curr -> node;
            while (node) {
                Node* tmp = node;
                node = node -> next;
                free(tmp -> info);
                tmp -> info = NULL;
                free(tmp);
                tmp = NULL;
            }
            free(curr);
            curr = NULL;
            return OK;
        }
        prev = curr;
        curr = curr -> link;
    }
    return ERR_KEY_NOT_FOUND;
}

ErrorCode delete_by_version(Table* table, KeyType key, RelType release) {
    if (!table) {
        return ERR_TABLE_NOT_EXISTS;
    }
    KeySpace* ks = find_key_space(table, key);
    if (!ks) {
        return ERR_KEY_NOT_FOUND;
    }
    Node* curr = ks -> node;
    Node* prev = NULL;
    while (curr) {
        if (curr -> release == release) {
            if (prev != NULL) {
                prev -> next = curr -> next;
            } else {
                ks -> node = curr -> next;
            }
            free(curr -> info);
            curr -> info = NULL;
            free(curr);
            curr = NULL;
            if (ks -> node == NULL) {
                delete_by_key(table, key);
            }
            return OK;
        }
        prev = curr;
        curr = curr -> next;
    }
    return ERR_VER_NOT_FOUND;
}

ErrorCode search_all_versions(Table* table, Table** new_table, KeyType key) {
    KeySpace* ks = find_key_space(table, key);
    if (!ks) {
        return ERR_KEY_NOT_FOUND;
    }
    Table* result = create_table();
    if (!result) {
        return ERR_MEMORY;
    }
    KeySpace* new_ks = malloc(sizeof(KeySpace));
    if (!new_ks) {
        free_table(result);
        result = NULL;
        return ERR_MEMORY;
    }
    new_ks -> link = NULL;
    new_ks -> node = NULL;
    new_ks -> key = ks -> key;

    Node* src = ks -> node;
    while (src) {
        Node* new_node = malloc(sizeof(Node));
        if (new_node == NULL) {
            free_table(result);
            result = NULL;
            return ERR_MEMORY;
        }
        new_node -> info = malloc(sizeof(InfoType));
        if (new_node -> info == NULL) {
            free(new_node);
            new_node = NULL;
            free_table(result);
            result = NULL;
            return ERR_MEMORY;
        }
        new_node -> release = src -> release;
        *(new_node -> info) = *(src -> info);
        new_node -> next = new_ks -> node;
        new_ks -> node = new_node;
        src = src -> next;
    }
    result -> ks = new_ks;
    *new_table = result;
    return OK;
}

ErrorCode search_specific_version(Table* table, Table** new_table, KeyType key, RelType release) {
    KeySpace* ks = find_key_space(table, key);
    if (!ks) {
        return ERR_KEY_NOT_FOUND;
    }
    Node* curr = ks -> node;
    while (curr) {
        if (curr -> release == release) {
            Table* result = create_table();
            if (!result) {
                return ERR_MEMORY;
            }
            KeySpace* new_ks = malloc(sizeof(KeySpace));
            if (!new_ks) {
                free_table(result);
                result = NULL;
                return ERR_MEMORY;
            }
            new_ks -> node = malloc(sizeof(Node));
            if (new_ks -> node == NULL) {
                free(new_ks);
                new_ks = NULL;
                free_table(result);
                result = NULL;
                return ERR_MEMORY;
            }
            new_ks -> key = key;
            new_ks -> node -> info = malloc(sizeof(InfoType));
            if (new_ks -> node -> info == NULL) {
                free(new_ks -> node);
                new_ks -> node = NULL;
                free(new_ks);
                new_ks = NULL;
                free_table(result);
                result = NULL;
                return ERR_MEMORY;
            }
            new_ks -> node -> release = release;
            *(new_ks -> node -> info) = *(curr -> info);
            new_ks -> node -> next = NULL;
            new_ks -> link = NULL;
            result -> ks = new_ks;
            *new_table = result;
            return OK;
        }
        curr = curr -> next;
    }
    *new_table = NULL;
    return ERR_VER_NOT_FOUND;
}

void print_table(Table* table) {
    if (!table || !table -> ks) {
        printf("Таблица пуста\n");
        return;
    }
    KeySpace* ks = table -> ks;
    while (ks) {
        printf("Ключ: %u\n", ks -> key);
        Node* node = ks -> node;
        while (node) {
            printf("Версия: %u, Значение: %u\n", node -> release, *(node -> info));
            node = node -> next;
        }
        ks = ks -> link;
    }
}

ErrorCode import_from_file(Table* table, const char* filename) {
    if (!table || !filename) {
        return ERR_TABLE_NOT_EXISTS;
    }
    FILE* file = fopen(filename, "r");
    if (!file) {
        return ERR_FILE;
    }
    char* line = NULL;
    while ((line = my_getline(file, 0)) != NULL) {
        line[strcspn(line, "\n")] = 0;
        char* key = strtok(line, ",");
        char* value = strtok(NULL, ",");
        if (key == NULL || value == NULL) {
            free(line);
            line = NULL;
            continue;
        }
        char* endptr;
        int check_key = strtol(key, &endptr, 10);
        if (*endptr != '\0' || check_key <= 0) {
            free(line);
            line = NULL;
            continue;
        }
        endptr = NULL;
        int check_info = strtol(value, &endptr, 10);
        if (*endptr != '\0' || check_info <= 0) {
            free(line);
            line = NULL;
            continue;
        }
        insert(table, check_key, check_info);
        free(line);
        line = NULL;
    }
    fclose(file);
    return OK;
}

void free_table(Table* table) {
    if (!table) {
        return;
    }
    KeySpace* cur_ks = table -> ks;
    while (cur_ks) {
        KeySpace* next_ks = cur_ks -> link;
        Node* cur_node = cur_ks -> node;
        while (cur_node) {
            Node* next_node = cur_node -> next;
            free(cur_node -> info);
            cur_node -> info = NULL;
            free(cur_node);
            cur_node = next_node;
        }
        free(cur_ks);
        cur_ks = next_ks;
    }
    free(table);
}
