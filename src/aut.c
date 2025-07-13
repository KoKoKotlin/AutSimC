#include "aut.h"

u32 get_key(u32 start_idx, char sym) {
	return start_idx << 8 | (u32)sym;
}

sarray_transition_t aut_create_transitions(sarray_pair_u32_t pairs, string transition_symbols) {
	assert(pairs.size == strlen(transition_symbols) && "AUT CREATE TRANSITION both arrays have to have same size!");

	sarray_transition_t transitions = { 0 };
	transitions.size = pairs.size;
	transitions.items = calloc(pairs.size, sizeof(transition_t));
	for (size_t i = 0; i < pairs.size; ++i) {
		PAIR_STRUCT(u32, u32) pair = pairs.items[i];
		u32 start_idx = pair.v1;
		u32 end_idx = pair.v2;
		char transition_sym = transition_symbols[i];
		transition_t *t = &transitions.items[i];

		t->start_state = start_idx;
		t->end_state = end_idx;
		t->transition_sym = transition_sym;
	}

	return transitions;
}

aut_t aut_new(aut_type_t type, sarray_string_t state_names, sarray_size_t final_states, sarray_size_t initial_states, sarray_transition_t transitions, char* alphabet) {
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

sarray_u32_t aut_read_single(const aut_t* aut, size_t curr_state_idx, char c) {
	assert(strchr(aut->alphabet, c) != NULL && "The given char is not in the auts alphabet!");
	list_u32_t end_states = { 0 };
	INIT_LIST(end_states, u32, 10);
	
	for (size_t i = 0; i < aut->transitions.size; ++i) {
		transition_t* t = &aut->transitions.items[i];

		if (t->start_state != curr_state_idx || t->transition_sym != c) continue;
		LIST_APPEND(end_states, size_t, t->end_state);
	}

	ARRAY_TO_SIZED(end_states.items, end_states.count, u32, end_state_array);
	FREE_CONTAINER(end_states);
	return end_state_array;
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
		    sarray_u32_t next_states = aut_read_single(aut, current_state, next);
		    if (next_states.size == 0) return false; // symbol not in alphabet
		    current_state = next_states.items[0]; // there can only be one next state and also there has to one
		    FREE_CONTAINER(next_states);
	    }

	    return aut_is_final_state(aut, current_state);
	} else {
		TODO("accept not implemented for NFA and ENFA!");
	}
}

bool check_eps_transisitions(const aut_t* aut) {
	for (size_t i = 0; i < aut->transitions.size; ++i) {
		if (aut->transitions.items[i].transition_sym == SYM_EPS) return true;
	}

	return false;
}

bool aut_check_valid(const aut_t* aut) {
	    if (aut->initial_states.size == 0) return false;
	    if (aut->final_states.size == 0) return false;
	    if (aut->initial_states.items[0] >= aut->state_names.size) return false;

	    for (size_t i = 0; i < aut->final_states.size; ++i) {
		    if (aut->final_states.items[i] >= aut->state_names.size) return false;
	    }

	if (aut->type == DFA) {
		for (size_t i = 0; i < aut->state_names.size; ++i) {
			for(char* c = aut->alphabet; *c != '\0'; c++) {
				sarray_u32_t next_state = aut_read_single(aut, i, *c);
				bool invalid = next_state.size == 0;
				FREE_CONTAINER(next_state);
				if (invalid) return false;
			}		    	   
		}
		if (aut->initial_states.size != 1) return false;
	} else if (aut->type == NFA) {
		if (check_eps_transisitions(aut)) return false;
	}

	return true;
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
	
	printf("Transitions:\n");
	for (size_t i = 0; i < aut->transitions.size; ++i) {
		transition_t transition = aut->transitions.items[i];
		size_t start_state = transition.start_state;
		size_t end_state = transition.end_state;
		string start_state_name = aut->state_names.items[start_state];
		string end_state_name = aut->state_names.items[end_state];
		printf("\t- %s(%zu) -- ", start_state_name, start_state);
		if (transition.transition_sym != SYM_EPS) printf(" %c ", transition.transition_sym);
		else printf("eps");
	    printf(" --> %s(%zu)\n", end_state_name, end_state);
	}
}
