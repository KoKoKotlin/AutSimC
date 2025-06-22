#include <stdio.h>

#include <dfa.h>
#include <loader.h>

int main() {
	const string path = "examples/dfa.aut";

	loader_result_t res = { 0 };
	loader_result_type_t res_type = load(path, &res);
	if (res_type != DFA) {
		fprintf(stderr, "Loader has loaded wrong type! Expected: %s, Actual: %s.\n", LRES_NAME(DFA), LRES_NAME(res_type));
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

	bool expected_results[] = { true, true, false, false, false, false, true };
	for (size_t i = 0; i < 7; i++) {
		string curr = test_inputs[i];
		if (dfa_accepts(&res.dfa, curr) != expected_results[i]) {
			fprintf(stderr, "DFA failed on input %s! Expected: %d, Actual: %d.", curr, dfa_accepts(&res.dfa, curr), expected_results[i]);
			exit(-1);
		}
	}
	
	return 0;
}
