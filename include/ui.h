#ifndef HANDLERS_H
#define HANDLERS_H

#include "table.h"

#define CONTINUE 1
#define EXIT 0

void display_menu();
int menu_choice(int, Table*);
const char* error_message(ErrorCode);
void case_insert(Table*);
void case_delete(Table*);
void case_delete_concrete(Table*);
void case_search(Table*);
void case_search_concrete(Table*);
void case_import_table(Table*);

#endif
