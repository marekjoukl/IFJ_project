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
    unsigned int counter = 0;
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

    item->key = key;
    item->data = data;

    unsigned int hash = SymtableHashFunction(key);
    if ((*table)[hash] == NULL) {
        (*table)[hash] = item;
    }
    else {
        while ((*table)[hash] != NULL) {
            if (counter >= MAX_HT_SIZE) {
                fprintf(stderr, "Error: symtable.c - hash table is full\n");
                exit(INTERNAL_ERROR);      // EXIT CODE 99 - failed to allocate memory
            }
            hash = (hash + 1) % MAX_HT_SIZE;
            counter++;
        }
        (*table)[hash] = item;
    }
}

symtable_item_t* SymtableSearch(Symtable *table, char *key) {
    unsigned int counter = 0;
    unsigned int hash = SymtableHashFunction(key);
    if ((*table)[hash] == NULL) {
        return NULL;
    }
    else {
        symtable_item_t *item = (*table)[hash];
        while (item != NULL) {
            if (counter >= MAX_HT_SIZE) {
                break;
            }
            if (strcmp(item->key, key) == 0 && item->data.blinded_sign == false) {
                return item;
            }
            hash = (hash + 1) % MAX_HT_SIZE;
            item = (*table)[hash];
            counter++;
        }
    }
    return NULL;
}

void SymtableDeleteItem(Symtable *table, char *key) {
    symtable_item_t *item = SymtableSearch(table, key);
    if (item == NULL) return;
    item->data.blinded_sign = true;
}

void SymtableDeleteAll(Symtable *table) {
    for (size_t i = 0; i < MAX_HT_SIZE; i++) {
        if ((*table)[i] != NULL) {
            free((*table)[i]->key);

            if ((*table)[i]->data.param_names != NULL) {
                for (int j = 0; j < (*table)[i]->data.param_count; j++) {
                    if ((*table)[i]->data.param_names[j] != NULL) {
                        free((*table)[i]->data.param_names[j]);
                    }
                }
                free((*table)[i]->data.param_names);
            }

            if ((*table)[i]->data.string_value != NULL) {
                for (int j = 0; j < (*table)[i]->data.param_count; j++) {
                    if ((*table)[i]->data.params_id[j] != NULL) {
                        free((*table)[i]->data.params_id[j]);
                    }
                }
                free((*table)[i]->data.params_id);
            }

            if ((*table)[i]->data.param_types != NULL) {
                free((*table)[i]->data.param_types);
            }

            free((*table)[i]);
            (*table)[i] = NULL;
        }
    }
}
