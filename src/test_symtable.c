#include <assert.h>
#include "symtable.h"

void test_symtable_init(void) {
    // Test symtable initialization
    Symtable sym_table;
    SymtableInit(&sym_table);
    // Check if all elements are NULL after initialization
    for (size_t i = 0; i < MAX_HT_SIZE; i++) {
        assert(sym_table[i] == NULL);
    }
}

void test_symtable_add_item(void) {
    // Test adding items to symtable
    Symtable sym_table;
    SymtableInit(&sym_table);

    char* key1 = malloc(sizeof(char) * 10);
    strncpy(key1, "variable1", 10);
    data_t data1 = {.item_type = TYPE_INT, .can_be_nil = false, .value_is_nil = false, .is_defined = true, .param_count = 0, .param_names = NULL, .params_id = NULL, .param_types = NULL, .line = 0, .is_function = false, .string_value = NULL, .numeric_value = 0, .blinded_sign = false };
    SymtableAddItem(&sym_table, key1, data1);

    char* key2 = malloc(sizeof(char) * 10);
    strncpy(key2, "variable2", 10);
    data_t data2 = {.item_type = TYPE_DOUBLE, .can_be_nil = true, .value_is_nil = true, .is_defined = false, .param_count = 1, .param_names = NULL, .params_id = NULL, .param_types = NULL, .line = 0, .is_function = false, .string_value = NULL, .numeric_value = 0, .blinded_sign = false};
    SymtableAddItem(&sym_table, key2, data2);


    // Check if the items are correctly added to the symtable
    symtable_item_t *item1 = SymtableSearch(&sym_table, "variable1");
    assert(item1 != NULL);
    assert(strcmp(item1->key, "variable1") == 0);
    assert(item1->data.item_type == TYPE_INT);

    symtable_item_t *item2 = SymtableSearch(&sym_table, "variable2");
    assert(item2 != NULL);
    assert(strcmp(item2->key, "variable2") == 0);
    assert(item2->data.item_type == TYPE_DOUBLE);

    symtable_item_t *item3 = SymtableSearch(&sym_table, "variable3");    
    assert(item3 == NULL);

     //Cleanup
     SymtableDeleteAll(&sym_table);
}

void test_symtable_delete_item(void) {
    // Test deleting items from symtable
    Symtable sym_table;
    SymtableInit(&sym_table);

    data_t data = {.item_type = TYPE_INT, .can_be_nil = false, .value_is_nil = false, .is_defined = true, .param_count = 0, .param_names = NULL, .params_id = NULL, .param_types = NULL, .line = 0, .is_function = false, .string_value = NULL, .numeric_value = 0, .blinded_sign = false};
    char* key1 = malloc(sizeof(char) * 10);
    strncpy(key1, "variable1", 10);
    SymtableAddItem(&sym_table, key1, data);
    symtable_item_t *item1 = SymtableSearch(&sym_table, "variable1");
    assert(item1 != NULL);

    // Check if the item is correctly deleted from the symtable
    SymtableDeleteItem(&sym_table, "variable1");
    symtable_item_t *item2 = SymtableSearch(&sym_table, "variable1");
    assert(item2 == NULL);

    SymtableDeleteAll(&sym_table);
}

 void test_symtable_delete_all(void) {
     // Test deleting all items from symtable
     Symtable sym_table;
     SymtableInit(&sym_table);

     data_t data1 = {.item_type = TYPE_INT, .can_be_nil = false, .value_is_nil = false, .is_defined = true, .param_count = 0, .param_names = NULL, .params_id = NULL, .param_types = NULL, .line = 0, .is_function = false, .string_value = NULL, .numeric_value = 0, .blinded_sign = false};
     data_t data2 = {.item_type = TYPE_DOUBLE, .can_be_nil = true, .value_is_nil = true, .is_defined = false, .param_count = 1, .param_names = NULL, .params_id = NULL, .param_types = NULL, .line = 0, .is_function = false, .string_value = NULL, .numeric_value = 0, .blinded_sign = false};

     char* key1 = malloc(sizeof(char) * 10);
     strncpy(key1, "variable1", 10);
     char* key2 = malloc(sizeof(char) * 10);
     strncpy(key1, "variable2", 10);

     SymtableAddItem(&sym_table, key1, data1);
     SymtableAddItem(&sym_table, key2, data2);

     // Check if all items are correctly deleted from the symtable
     SymtableDeleteAll(&sym_table);
     for (size_t i = 0; i < MAX_HT_SIZE; i++) {
         assert(sym_table[i] == NULL);
     }
 }

int main(void) {
    test_symtable_init();
    test_symtable_add_item();
    test_symtable_delete_item();
    test_symtable_delete_all();

    printf("All symtable tests passed!\n");

    return 0;
}   

