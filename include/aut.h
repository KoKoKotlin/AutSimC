#ifndef _AUT_H
#define _AUT_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "utils.h"

#define AUTTYPE_NAME(type) auttype_names[type]
#define SYM_EPS 127

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
	size_t start_state;
	size_t end_state;
	char transition_sym;
} transition_t;

CREATE_SIZED_ARRAY(transition_t);
typedef sarray_transition_t_t sarray_transition_t;
CREATE_LIST(transition_t);
typedef list_transition_t_t list_transition_t;

typedef struct {
	aut_type_t type;
	sarray_string_t state_names;
	sarray_size_t final_states;
	sarray_size_t initial_states;
	sarray_transition_t transitions;
	char* alphabet;
} aut_t;

aut_t aut_new(aut_type_t, sarray_string_t, sarray_size_t, sarray_size_t, sarray_transition_t, char*);
aut_t aut_to_nfa(const aut_t* aut);
aut_t aut_to_dfa(const aut_t* aut);
sarray_transition_t aut_create_transitions(sarray_pair_u32_t pairs, string transition_symbols);
sarray_size_t aut_read_single(const aut_t*, size_t, char);
bool aut_accepts(const aut_t*, string);
void aut_debug_print(const aut_t*);
#endif
