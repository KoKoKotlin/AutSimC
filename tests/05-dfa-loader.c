#include <stdio.h>

#include <aut.h>
#include <loader.h>

int main() {
	const string path = "examples/dfa.aut";

	loader_result_t res = { 0 };
	loader_result_type_t res_type = load(path, &res);
	if (res_type != SUCCESS) {
		fprintf(stderr, "Loader failed to load example %s! Expected: %s, Actual: %s.\n", path);
		exit(-1);
	}

	if (res.aut.type != DFA) {
		fprintf(stderr, "Loader loaded wrong type of automaton! Expected: %s, Actual: %s.\n", AUTTYPE_NAME(DFA), AUTTYPE_NAME(res.aut.type));
		exit(-1);
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

	aut_debug_print(&res.aut);
	bool expected_results[] = { true, true, false, false, false, false, true };
	for (size_t i = 0; i < 7; i++) {
		string curr = test_inputs[i];
		if (aut_accepts(&res.aut, curr) != expected_results[i]) {
			fprintf(stderr, "DFA failed on input %s! Expected: %d, Actual: %d.", curr, expected_results[i], aut_accepts(&res.aut, curr));
			exit(-1);
		}
	}
	
	return 0;
}
