#ifndef _DFA_H
#define _DFA_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "hashtable.h"
#include "utils.h"

typedef struct {
	size_t state_count;
	sarray_string_t state_names;
	sarray_size_t final_states;
	size_t initial_state;
	hashtable_t transitions;
	char* alphabet;
} dfa_t;

dfa_t dfa_new(size_t, sarray_string_t, sarray_size_t, size_t, hashtable_t, char*);
#endif
