/**
 * @file symtable.h
 * @author Ondrej Kožányi (xkozan01)
 * @brief Header for symtable
 */

#ifndef SYMTABLE_H
#define SYMTABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "scanner.h"
#include "error.h"

#define MAX_HT_SIZE 20011   //Prime number for hash table size (20011 is the closest prime number to 20000)

typedef enum {
    TYPE_INT,
    TYPE_DOUBLE,
    TYPE_STRING,
    TYPE_UNDEFINED,
    TYPE_INT_NIL,
    TYPE_DOUBLE_NIL,
    TYPE_STRING_NIL,
    TYPE_BOOL,
    TYPE_NIL
} data_type_t;

typedef struct data {
    data_type_t item_type;
    bool can_be_nil;
    bool value_is_nil;
    //bool is_defined;
    bool is_modifiable;
    int param_count;
    char **param_names;
    char **params_id;
    data_type_t *param_types;
    int line;
    bool is_function;
    char *string_value;
    double numeric_value;
    bool blinded_sign;
    int param_count_current;
} data_t;

//one item in symtable
typedef struct symtable_item
{
    char *key;
    data_t *data;
} symtable_item_t;

typedef symtable_item_t *Symtable[MAX_HT_SIZE];

unsigned int SymtableHashFunction(char *key);   //hash function
void SymtableInit(Symtable *table);            //initialize symtable
void SymtableAddItem(Symtable *table, char *key, data_t *data);  //add item to symtable with key and data
symtable_item_t* SymtableSearch(Symtable *table, char *key);    //search for item with given key in symtable
void SymtableDeleteItem(Symtable *table, char *key);        //delete item with given key from symtable
void SymtableDeleteAll(Symtable *table);                //delete all items from symtable

#endif //SYMTABLE_H
