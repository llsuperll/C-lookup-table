#include "../include/table.h"
#include <stdio.h>
#include <stdlib.h>
#include "../include/ui.h"

int main() {
    Table* table = create_table();
    if (!table) {
        printf("Ошибка: Не удалось инициализировать таблицу\n");
        return 1;
    }
    int choice = 0, status = CONTINUE;
    while (status == CONTINUE) {
        display_menu();
        int retval = scanf("%d", &choice);
        if (retval == EOF) {
            status = EXIT;
        } else if (retval != 1) {
            scanf("%*[^\n]%*c");
            printf("Ошибка: Такой команды нет!\n");
        } else {
            status = menu_choice(choice, table);
        }
    }
    free_table(table);
    return 0;
}
