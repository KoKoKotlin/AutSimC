#include "dfa.h"
#include "hashtable.h"
#include "utils.h"

#include <stdlib.h>


int main() {
	const size_t state_count = 3;
	string state_names_temp[] = {
		"a",
		"b",
		"F"
	};
	ARRAY_TO_SIZED(state_names_temp, 3, string, state_names);
	size_t final_states_temp[] = { 0, 1 };
	ARRAY_TO_SIZED(final_states_temp, 2, size_t, final_states);
	PAIR_STRUCT(u32, u32) pairs_temp[] = {
		PAIR(u32, u32, 0, 0),
		PAIR(u32, u32, 0, 1),
		PAIR(u32, u32, 1, 1),
		PAIR(u32, u32, 1, 2),
		PAIR(u32, u32, 2, 2),
		PAIR(u32, u32, 2, 2),
	};
	ARRAY_TO_SIZED(pairs_temp, 6, pair_u32_u32_t, pairs);
	string syms = "abbaab";
	hashtable_t transitions = dfa_create_transitions(pairs, syms);
	string alphabet = "ab";
	
	dfa_t dfa = dfa_new(state_count, state_names, final_states, 0, transitions, alphabet); 

	dfa_debug_print(&dfa);

	string test_inputs[] = {
		"aaa",
		"bbb",
		"aabb",
		"ba",
		"aba"
	};
	
	for (size_t i = 0; i < 5; i++) {
		string curr = test_inputs[i];
		printf("%s: DFA accepts? %d\n", curr, dfa_accepts(&dfa, curr));
	}
}
