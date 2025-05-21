#ifndef _DFA_H
#define _DFA_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "hashtable.h"
#include "utils.h"

typedef struct {
	sarray_string_t state_names;
	sarray_size_t final_states;
	size_t initial_state;
	hashtable_t transitions;
	char* alphabet;
} dfa_t;

dfa_t dfa_new(sarray_string_t, sarray_size_t, size_t, hashtable_t, char*);
hashtable_t dfa_create_transitions(sarray_pair_u32_t, string);
u32* dfa_read_single(const dfa_t*, size_t, char);
bool dfa_accepts(const dfa_t*, string);
void dfa_debug_print(const dfa_t*);
#endif
