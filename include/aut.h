#ifndef _AUT_H
#define _AUT_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "hashtable.h"
#include "utils.h"

#define AUTTYPE_NAME(type) auttype_names[type]

typedef enum {
	DFA,
	NFA,
	ENFA,
	AUT_TYPE_COUNT,
} aut_type_t;

static string auttype_names[] = {
	"DFA",
	"NFA",
	"ENFA",
	"AUT_TYPE_COUNT",
};

typedef struct {
	aut_type_t type;
	sarray_string_t state_names;
	sarray_size_t final_states;
	sarray_size_t initial_states;
	hashtable_t transitions;
	char* alphabet;
} aut_t;

aut_t aut_new(aut_type_t, sarray_string_t, sarray_size_t, sarray_size_t, hashtable_t, char*);
hashtable_t aut_create_transitions(sarray_pair_u32_t, string);
u32* aut_read_single(const aut_t*, size_t, char);
bool aut_accepts(const aut_t*, string);
void aut_debug_print(const aut_t*);
#endif
