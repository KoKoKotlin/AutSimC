#include "aut.h"

u32 get_key(u32 start_idx, char sym) {
	return start_idx << 8 | (u32)sym;
}

hashtable_t aut_create_transitions(sarray_pair_u32_t pairs, string transition_symbols) {
	assert(pairs.size == strlen(transition_symbols) && "AUT CREATE TRANSITION both arrays have to have same size!");

	hashtable_t transitions = ht_new(32, &hashfunc_u32);
	for (size_t i = 0; i < pairs.size; ++i) {
		PAIR_STRUCT(u32, u32) pair = pairs.items[i];
		u32 start_idx = pair.v1;
		u32 end_idx = pair.v2;
		char transition_sym = transition_symbols[i];

		u32 key = get_key(start_idx, transition_sym);
		ht_insert(&transitions, key, end_idx);
	}

	return transitions;
}

aut_t aut_new(aut_type_t type, sarray_string_t state_names, sarray_size_t final_states, sarray_size_t initial_states, hashtable_t transitions, char* alphabet) {
	aut_t aut = {
		.type = type,
		.state_names = state_names,
		.final_states = final_states,
		.initial_states = initial_states,
		.transitions = transitions,
		.alphabet = alphabet
	};
	return aut; 
}

u32* aut_read_single(const aut_t* aut, size_t curr_state_idx, char c) {
	assert(strchr(aut->alphabet, c) != NULL && "The given char is not in the auts alphabet!");
	return ht_get(&aut->transitions, get_key(curr_state_idx, c));
}

bool aut_is_final_state(const aut_t* aut, u32 state_idx) {
	for (size_t i = 0; i < aut->final_states.size; ++i) {
		if (aut->final_states.items[i] == state_idx) return true;
	}

	return false;
}

bool aut_accepts(const aut_t* aut, string input) {
	if (aut->type == DFA) {
	u32 current_state = aut->initial_states.items[0];
	for (size_t i = 0; i < strlen(input); ++i) {
		char next = input[i];
		u32* next_state = aut_read_single(aut, current_state, next);
		if (next_state == NULL) return false; // symbol not in alphabet
		current_state = *next_state;
	}

	return aut_is_final_state(aut, current_state);
	} else {
		TODO("accept not implemented for NFA and ENFA!");
	}
}

bool aut_check_valid(const aut_t* aut) {
	if (aut->type == DFA) {
	    for (size_t i = 0; i < aut->state_names.size; ++i) {
		    for(char* c = aut->alphabet; *c != '\0'; c++) {
			    if (aut_read_single(aut, i, *c) == NULL) return false;
		    }
	    }

	    if (aut->initial_states.size != 1) return false;
	    if (aut->initial_states.items[0] >= aut->state_names.size) return false;

	    for (size_t i = 0; i < aut->final_states.size; ++i) {
		    if (aut->final_states.items[i] >= aut->state_names.size) return false;
	    }

	    return true;
	} else {
		TODO("check_valid not implemented for NFA and ENFA!");
	}
}

void aut_debug_print(const aut_t* aut) {
	printf("Type of automaton: %s\n", AUTTYPE_NAME(aut->type));
	printf("Number of states: %zu\n", aut->state_names.size);
	printf("Number of inital states: %zu\n", aut->initial_states.size);
	for (size_t i = 0; i < aut->initial_states.size; ++i) {
		printf("\t%zu ", aut->initial_states.items[i]);
	}
	printf("\n");
	printf("Number of final states: %zu\n", aut->final_states.size);
	for (size_t i = 0; i < aut->final_states.size; ++i) {
		printf("\t%zu ", aut->final_states.items[i]);
	}
	printf("\n");
	printf("Alphabet: %s\n", aut->alphabet);
	
	for (u32 i = 0; i < aut->state_names.size; ++i) {
		for(char* c = aut->alphabet; *c != '\0'; c++) {
			u32* next_state_ptr = aut_read_single(aut, i, *c);
			u32 next_state = *next_state_ptr;
			printf("  - Transition: %s(%u) & %c -> %s(%u)\n", aut->state_names.items[i], i, *c, aut->state_names.items[next_state], next_state);
		}
	}
}
