#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

typedef char* string;
typedef uint32_t u32;
typedef unsigned char u8;

#define UNREACHABLE() assert("Unreachable" && 0)
#define TODO(s) assert("TODO: "s && 0)

#define PAIR_STRUCT(type1, type2) pair_##type1##_##type2##_t
#define CREATE_PAIR_STRUCT(type1, type2) typedef struct { type1 v1; type2 v2; } PAIR_STRUCT(type1, type2);
#define PAIR(type1, type2, v1_, v2_) ((PAIR_STRUCT(type1, type2)){ .v1 = v1_, .v2 = v2_ })

typedef struct {
	size_t size;
	void* items;
} sarray_t;
#define CREATE_SIZED_ARRAY(type) typedef struct { size_t size; type* items; } sarray_##type##_t;
#define ARRAY_TO_SIZED(array, array_size, type, name)		\
	sarray_##type##_t name;					\
	(name).size = array_size;				\
	(name).items = malloc(array_size * sizeof(type));	\
	memcpy((name).items, (array), array_size * sizeof(type));

typedef struct {
	size_t count;
	size_t capacity;
	void* items;
} list_t;
void list_resize(list_t* list, size_t item_size);
#define CREATE_LIST(type) typedef struct { size_t count; size_t capacity; type* items; } list_##type##_t;
#define INIT_LIST(list, type, init_cap) do {			\
		(list).count = 0;				\
		(list).capacity = (init_cap);			\
		(list).items = calloc(init_cap, sizeof(type));	\
	} while(0)
#define LIST_APPEND(list, type, item) do { \
		list_resize((list_t*)&list, sizeof(type)); \
		list.items[list.count++] = (item);	   \
	} while(0) 
#define FREE_CONTAINER(container) free((container).items);

CREATE_LIST(u32);

#define SARRAY(type, count, data_ptr) (sarray_t) { .size = (count), .items = (data_ptr) })

typedef bool(*comperator_t)(void*, void*);

CREATE_PAIR_STRUCT(u32, u32);
CREATE_PAIR_STRUCT(u32, char);

CREATE_SIZED_ARRAY(size_t);
typedef sarray_size_t_t sarray_size_t;
CREATE_SIZED_ARRAY(string);
CREATE_SIZED_ARRAY(u32);
CREATE_SIZED_ARRAY(pair_u32_u32_t);
typedef sarray_pair_u32_u32_t_t sarray_pair_u32_t;

#define SARRAY_CONTAINS(obj, _sarray, item_size, comp) \
	sarray_contains((void*)(obj), (sarray_t*)(_sarray), (item_size), (comp))
#define SARRAY_INDEX_OF(obj, _sarray, item_size, comp) \
	sarray_index_of((void*)(obj), (sarray_t*)(_sarray), (item_size), (comp))

size_t hashfunc_u32(u32);
bool string_contains(char c, const string syms);
bool sarray_contains(void*, sarray_t*, size_t, comperator_t);
int sarray_index_of(void* obj, sarray_t* arr, size_t item_size, comperator_t comperator);

#endif

