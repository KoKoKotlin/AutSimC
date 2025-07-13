#include <stdio.h>
#include <stdlib.h>

#include <graphviz.h>
#include <aut.h>
#include <loader.h>


int main() {
	const string path = "./examples/enfa.aut";

	loader_result_t res = { 0 };
	loader_result_type_t res_type = load(path, &res);

	sv_t view = aut_to_dot(&res.aut);
	
	if (sv_write_to_file(&view, "__temp.dot") != 0) {
		fprintf(stderr, "Could not write dot representation to file!\n");
		exit(-1);
	}

	int dot_res = system("dot -Tpng __temp.dot -o __test_output.png");
	system("rm -rf __temp.dot __test_output.png");
	if (dot_res != 0) {
		fprintf(stderr, "Error while trying to convert the dot file to png!\n");
		exit(-1);
	}
	
	return 0;
}
