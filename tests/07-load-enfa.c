#include <stdio.h>
#include <loader.h>

int main() {
	const string path = "examples/enfa.aut";

	loader_result_t res = { 0 };
	loader_result_type_t res_type = load(path, &res);
	if (res_type != SUCCESS) {
		fprintf(stderr, "Loader failed to load example %s!\n", path);
		exit(-1);
	}

	if (res.aut.type != ENFA) {
		fprintf(stderr, "Loader loaded wrong type of automaton! Expected: %s, Actual: %s.\n", AUTTYPE_NAME(DFA), AUTTYPE_NAME(res.aut.type));
		exit(-1);
	}

	aut_debug_print(&res.aut);
	string test_inputs[] = {
		"abbb",
		"aaa",
		"bbb",
		"aabb",
		"ba",
		"aba"
	};

	bool expected_results[] = { true, true, true, true, false, false };
	bool success = true;
	for (size_t i = 0; i < 6; i++) {
		string curr = test_inputs[i];
		if (aut_accepts(&res.aut, curr) != expected_results[i]) {
			fprintf(stderr, "ENFA failed on input %s! Expected: %d, Actual: %d.\n", curr, expected_results[i], aut_accepts(&res.aut, curr));
			success = false;
		}
	}

	if (!success) exit(-1);
	return 0;
}
