#pragma once
#include <stdbool.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>

/*
#define Char char
#define Short short
#define Int int
#define Long long
#define LongLong long long

#define make_opt(type)  \
typedef struct          \
{                       \
    type value;         \
    bool has_value;     \
}Optional##type

make_opt(Char);
make_opt(Short);
make_opt(Int);
make_opt(Long);
make_opt(LongLong);

#undef Char
#undef Short
#undef Int
#undef Long
#undef LongLong
#undef make_opt

*/

typedef struct
{
    uint8_t value;
    bool has_value;
} OptionalU8;

typedef struct
{
    int8_t value;
    bool has_value;
} OptionalI8;

typedef struct
{
    uint16_t value;
    bool has_value;
} OptionalU16;

typedef struct
{
    int16_t value;
    bool has_value;
} OptionalI16;

typedef struct
{
    uint32_t value;
    bool has_value;
} OptionalU32;

typedef struct
{
    int32_t value;
    bool has_value;
} OptionalI32;

typedef OptionalI32 OptionalInt;

typedef struct
{
    uint64_t value;
    bool has_value;
} OptionalU64;

typedef struct
{
    uint8_t value;
    bool has_value;
} OptionalI64;

typedef struct
{
    union
    {
        void* value;
        int32_t error;
    };
    bool has_value;
} OptionalPtr;


typedef struct IntStack IntStack;

IntStack* int_stack_create(size_t size);

int int_stack_push(IntStack* stack, int num);

int int_stack_pop(IntStack* stack);

int int_stack_dupe(IntStack* stack);

void int_stack_swap(IntStack* stack);

int int_stack_copy(IntStack* stack, size_t index);

int int_stack_push_string(IntStack* stack, const char* string);

void int_stack_destoy(IntStack* stack);

void int_stack_print(IntStack* stack);

typedef struct I64Stack I64Stack;

I64Stack* i64_stack_create(size_t size);

int64_t i64_stack_push(I64Stack* stack, int64_t num);

int64_t i64_stack_pop(I64Stack* stack);

int64_t i64_stack_dupe(I64Stack* stack);

void i64_stack_swap(I64Stack* stack);

int64_t i64_stack_copy(I64Stack* stack, size_t index);

int64_t i64_stack_push_string(I64Stack* stack, const char* string);

void i64_stack_destoy(I64Stack* stack);



typedef struct IntList IntList;

IntList* int_list_create(size_t init_size);

int int_list_append(IntList* list, int num);

OptionalInt int_list_get(IntList* list, size_t index);

void int_list_destroy(IntList* list);

void int_list_print(IntList* list);

typedef struct U8List U8List;

U8List* u8_list_create(size_t init_size);

int u8_list_append(U8List* list, uint8_t num);

OptionalU8 u8_list_get(U8List* list, size_t index);

size_t u8_list_size(U8List* list);

void u8_list_destroy(U8List* list);

char* u8_list_get_string(U8List* list);

typedef struct List List;

List* list_create(size_t size, size_t element_size);

int list_append(List* list, void* element);

OptionalPtr list_get(List* list, size_t index);

size_t list_size(List* list);

void list_destroy(List* list);

typedef struct IntLinkedlist IntLinkedlist;

IntLinkedlist* int_linkedlist_create();

size_t int_linkedlist_size(IntLinkedlist* list);

int int_linkedlist_append(IntLinkedlist* list, int num);

OptionalInt int_linkedlist_get(IntLinkedlist* list, size_t index);

size_t int_list_size(IntList* list);

int int_linkedlist_delete(IntLinkedlist* list, size_t index);

void int_linkedlist_destroy(IntLinkedlist* list);

void int_linkedlist_print(IntLinkedlist* list);

typedef struct IntHashMap IntHashMap;

typedef struct PtrHashMap PtrHashMap;

IntHashMap* int_hashmap_create();

bool int_hashmap_add(IntHashMap* hashmap, char* key, int value);

OptionalInt int_hashmap_get(IntHashMap* hashmap, char* key);

void int_hashmap_destroy(IntHashMap* hashmap);

PtrHashMap* ptr_hashmap_create();

bool ptr_hashmap_add(PtrHashMap* hashmap, char* key, void* ptr);

OptionalPtr ptr_hashmap_get(PtrHashMap* hashmap, char* key);
