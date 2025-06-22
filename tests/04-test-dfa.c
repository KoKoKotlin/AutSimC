#include <utils.h>
#include <aut.h>

int main() {
	string state_names_temp[] = {
		"a",
		"b",
		"F"
	};
	ARRAY_TO_SIZED(state_names_temp, 3, string, state_names);
	size_t initial_states_temp[] = { 0 };
	ARRAY_TO_SIZED(initial_states_temp, 1, size_t, initial_states);
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
	hashtable_t transitions = aut_create_transitions(pairs, syms);
	string alphabet = "ab";
	
	aut_t aut = aut_new(DFA, state_names, final_states, initial_states, transitions, alphabet); 

	aut_debug_print(&aut);

	string test_inputs[] = {
		"aaa",
		"bbb",
		"aabb",
		"ba",
		"aba"
	};

	bool expected_results[] = { true, true, true, false, false };
	
	for (size_t i = 0; i < 5; i++) {
		string curr = test_inputs[i];
		if (aut_accepts(&aut, curr) != expected_results[i]) {
			fprintf(stderr, "DFA failed on input %s! Expected: %d, Actual: %d.", curr, aut_accepts(&aut, curr), expected_results[i]);
			exit(-1);
		}
	}
}
