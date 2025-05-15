#include "dfa.h"

u32 get_key(u32 start_idx, char sym) {
	return start_idx << 8 | (u32)sym;
}

hashtable_t dfa_create_transitions(sarray_pair_u32_t pairs, string transition_symbols) {
	assert(pairs.size == strlen(transition_symbols) && "DFA CREATE TRANSITION both arrays have to have same size!");

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

dfa_t dfa_new(size_t state_count, sarray_string_t state_names, sarray_size_t final_states, size_t initial_state, hashtable_t transitions, char* alphabet) {
	dfa_t dfa = {
		.state_count = state_count,
		.state_names = state_names,
		.final_states = final_states,
		.initial_state = initial_state,
		.transitions = transitions,
		.alphabet = alphabet
	};
	return dfa; 
}

u32* dfa_read_single(const dfa_t* dfa, size_t curr_state_idx, char c) {
	assert(strchr(dfa->alphabet, c) != NULL && "The given char is not in the dfas alphabet!");
	return ht_get(&dfa->transitions, get_key(curr_state_idx, c));
}

bool dfa_is_final_state(const dfa_t* dfa, u32 state_idx) {
	for (size_t i = 0; i < dfa->final_states.size; ++i) {
		if (dfa->final_states.items[i] == state_idx) return true;
	}

	return false;
}

bool dfa_accepts(const dfa_t* dfa, string input) {
	u32 current_state = dfa->initial_state;
	for (size_t i = 0; i < strlen(input); ++i) {
		char next = input[i];
		u32* next_state = dfa_read_single(dfa, current_state, next);
		if (next_state == NULL) return false; // symbol not in alphabet
		current_state = *next_state;
	}

	return dfa_is_final_state(dfa, current_state);
}

bool dfa_check_valid(const dfa_t* dfa) {
	for (size_t i = 0; i < dfa->state_count; ++i) {
		for(char* c = dfa->alphabet; *c != '\0'; c++) {
			if (dfa_read_single(dfa, i, *c) == NULL) return false;
		}
	}

	if (dfa->initial_state >= dfa->state_count) return false;

	for (size_t i = 0; i < dfa->final_states.size; ++i) {
		if (dfa->final_states.items[i] >= dfa->state_count) return false;
	}
	
	return true;
}

void dfa_debug_print(const dfa_t* dfa) {
	for (u32 i = 0; i < dfa->state_count; ++i) {
		for(char* c = dfa->alphabet; *c != '\0'; c++) {
			u32* next_state_ptr = dfa_read_single(dfa, i, *c);
			u32 next_state = *next_state_ptr;
			printf("%s(%u) & %c -> %s(%u)\n", dfa->state_names.items[i], i, *c, dfa->state_names.items[next_state], next_state);
		}
	}
}
