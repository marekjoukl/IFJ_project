/**
 * @file symtable.c
 * @author
 * @brief Parser using recursive descent
 */

#include "symtable.h"

unsigned int SymtableHashFunction(char *key)
{
    unsigned int hash = 5381;
    int c;

    for(int i = 0; key[i] != '\0'; i++) {
        c = key[i];
        hash = ((hash << 5) + hash) ^ c; /* Use XOR operation */
    }

    return hash % (MAX_HT_SIZE-1);
}

void SymtableInit(Symtable *table)
{
    for (size_t i = 0; i < MAX_HT_SIZE; i++) {
        (*table)[i] = NULL;
    }
}

void SymtableAddItem(Symtable *table, char *key, data_t data) {
    symtable_item_t *item = SymtableSearch(table, key);
    if (item != NULL) {
        fprintf(stderr, "Error: symtable.c - redefinition of variable\n");
        exit(DEFINITION_ERROR);    // EXIT CODE 3 - redefinition of variable
    }

    item = malloc(sizeof(symtable_item_t));
    if (item == NULL) {
        fprintf(stderr, "Error: symtable.c - malloc failed\n");
        exit(INTERNAL_ERROR);      // EXIT CODE 99 - failed to allocate memory
    }

    item->key = malloc(sizeof(char) * (strlen(key) + 1));
    if (item->key == NULL) {
        fprintf(stderr, "Error: symtable.c - malloc failed\n");
        exit(INTERNAL_ERROR);      // EXIT CODE 99 - failed to allocate memory
    }

    strcpy(item->key, key);
    item->data = data;

    unsigned int hash = SymtableHashFunction(key);
    if (table->ht_table_t[hash] == NULL) {
        table->ht_table_t[hash] = item;
    }
    else {
        //TODO: resolve collisions
    }

    table->curr_size++;
    if (table->curr_size == MAX_HT_SIZE) {
        fprintf(stderr, "Error: symtable.c - table is full\n");
        exit(INTERNAL_ERROR);      // EXIT CODE 99 - failed to allocate memory
    }

    if (table->curr_size >= table->allocated_size) {
        if (table->allocated_size * 2 > MAX_HT_SIZE) {
            table->allocated_size = MAX_HT_SIZE;
        }
        else {
            table->allocated_size *= 2;
        }
        table->ht_table_t = realloc(table->ht_table_t, sizeof(symtable_item_t*) * (table->allocated_size));
        if (table->ht_table_t == NULL) {
            fprintf(stderr, "Error: symtable.c - realloc failed\n");
            exit(INTERNAL_ERROR);      // EXIT CODE 99 - failed to allocate memory
        }
    }
}

symtable_item_t* SymtableSearch(Symtable *table, char *key) {
    unsigned int hash = SymtableHashFunction(key);
    if ((*table)[hash] == NULL) {
        return NULL;
    }
    else {
        symtable_item_t *item = (*table)[hash];
        while (item != NULL) {
            if (strcmp(item->key, key) == 0) {
                return item;
            }
            hash = (hash + 1) % MAX_HT_SIZE;
            item = (*table)[hash];
        }
    }
    return NULL;
}

void SymtableDeleteItem(Symtable *table, char *key) {
    symtable_item_t *item = SymtableSearch(table, key);
    if (item == NULL) return;
    free(item->key);
    free(item->data.param_names);
    free(item->data.params_id);
    free(item->data.param_types);
    free(item);
    item = NULL;
    table->curr_size--;
    if (table->curr_size < (table->allocated_size / 3))
    {
        table->ht_table_t = realloc(table->ht_table_t, sizeof(symtable_item_t*) * (table->allocated_size / 2));
    }
}

void SymtableDeleteAll(Symtable *table) {
    for (size_t i = 0; i < table->allocated_size; i++) {
        if (table->ht_table_t[i] != NULL) {
            free(table->ht_table_t[i]->key);
            free(table->ht_table_t[i]->data.param_names);
            free(table->ht_table_t[i]->data.params_id);
            free(table->ht_table_t[i]->data.param_types);
            free(table->ht_table_t[i]);
            table->ht_table_t[i] = NULL;
        }
    }
    free(table->ht_table_t);
    table->curr_size = 0;
    table->allocated_size = 0;
}
