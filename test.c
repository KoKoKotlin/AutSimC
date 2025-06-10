#include "dfa.h"
#include "hashtable.h"
#include "utils.h"
#include "loader.h"

#include <stdlib.h>


void test_dfa() {
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
	
	dfa_t dfa = dfa_new(state_names, final_states, 0, transitions, alphabet); 

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

void test_loader() {
	const string path = "examples/dfa.aut";

	loader_result_t res = { 0 };
	loader_result_type_t res_type = load(path, &res);
	printf("Loaded: %s\n", loader_result_to_string(res_type));
	if (res_type == DFA) {
		dfa_debug_print(&res.dfa);
	}
	
	string test_inputs[] = {
		"ab",
		"ababab",
		"aaa",
		"bbb",
		"aabb",
		"ba",
		"aba"
	};
	
	for (size_t i = 0; i < 7; i++) {
		string curr = test_inputs[i];
		printf("%s: DFA accepts? %d\n", curr, dfa_accepts(&res.dfa, curr));
	}
}

int main() {
	// test_dfa();
	// printf("\n");
	test_loader();
	
	return 0;
}
