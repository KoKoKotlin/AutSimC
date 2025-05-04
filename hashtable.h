#ifndef _HASHTABLE_H
#define _HASHTABLE_H

#include <inttypes.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include "utils.h"

#define HASHTABLE_T_ARENA_SIZE 65536

typedef size_t (*hashfunc_t)(u32); 

typedef struct {
	u32 key;
	char value;
} hashtable_entry_t;

// a hashtable can have at most HASHTABLE_T_ARENA_SIZE entries
typedef struct {
	size_t entry_count;
	hashtable_entry_t** items;
	hashfunc_t hashfunc;
	hashtable_entry_t* _entry_arena;
	size_t _current_loc;
} hashtable_t;

size_t hashfunc_uint32(void*);
hashtable_t ht_new(size_t, hashfunc_t);
void ht_insert(hashtable_t*, u32, char);
char* ht_get(const hashtable_t*, u32);
void ht_destroy(hashtable_t*);

#endif
