#ifndef TABLE_H
#define TABLE_H

#include <stdio.h>
#include <stdlib.h>

typedef unsigned int KeyType;
typedef unsigned int RelType;
typedef unsigned int InfoType;

typedef enum {
    OK = 0,
    ERR_MEMORY = -1,
    ERR_VER_NOT_FOUND = -2,
    ERR_KEY_NOT_FOUND = -3,
    ERR_FILE = -4,
    ERR_TABLE_NOT_EXISTS = -5
} ErrorCode;

typedef struct Node {
    RelType release;
    InfoType* info;
    struct Node* next;
} Node;

typedef struct KeySpace {
    KeyType key;
    Node* node;
    struct KeySpace* link;
} KeySpace;

typedef struct Table {
    KeySpace* ks;
} Table;

Table* create_table();
ErrorCode insert(Table*, KeyType, InfoType);
ErrorCode delete_by_key(Table*, KeyType);
ErrorCode delete_by_version(Table*, KeyType, RelType);
ErrorCode search_all_versions(Table*, Table**, KeyType);
ErrorCode search_specific_version(Table*, Table**, KeyType, RelType);
void print_table(Table*);
ErrorCode import_from_file(Table*, const char*);
void free_table(Table*);
#endif
