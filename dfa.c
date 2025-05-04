#include "dfa.h"

u32 calc_state_key(u32 idx_state1, u32 idx_state2) {
	assert(idx_state1 < 65536 && idx_state2 < 65536 && "DFA CALC STATE KEY idx out of bounds");
	return idx_state1 << 16 | idx_state2; 
}

hashtable_t dfa_create_transisitions(sarray_pair_u32_t pairs, char* values) {
	assert(pairs.size == strlen(values) && "DFA CREATE TRANSITION both arrays have to have same size!");

	hashtable_t ht = ht_new(pairs.size, &hashfunc_u32);
	for (size_t i = 0; i < pairs.size; ++i) {
		u32 key = calc_state_key(pairs.items[i].v1, pairs.items[i].v2);
		ht_insert(&ht, key, values[i]);
	}
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

char* dfa_get_transition(const dfa_t* dfa, size_t idx_state1, size_t idx_state2) {
	assert(idx_state1 < 65536 && idx_state2 < 65536 && "DFA CALC STATE KEY idx out of bounds");
	return ht_get(&dfa->transitions, calc_state_key(idx_state1, idx_state2));
}

bool dfa_check_valid(const dfa_t* dfa) {
	bool* trans_for_char = malloc(strlen(dfa->alphabet) * sizeof(bool));
	for (size_t i = 0; i < dfa->state_count; ++i) {
		memset(trans_for_char, 0, strlen(dfa->alphabet));

		for (size_t j = 0; j < dfa->state_count; ++j) {
			char* c1 = dfa_get_transition(dfa, i, j);
			if (c1 != NULL) {
				char* c2 = strchr(dfa->alphabet, *c1);
				if (!c2) continue;
				trans_for_char[(size_t)(c2 - dfa->alphabet)] = true;
			}
		}

		for (size_t j = 0; j < strlen(dfa->alphabet); ++j) {
			if (!trans_for_char[j]) return false;
		}
	}

	if (dfa->initial_state >= dfa->state_count) return false;

	for (size_t i = 0; i < dfa->final_states.size; ++i) {
		if (dfa->final_states.items[i] >= dfa->state_count) return false;
	}
	
	return true;
}

