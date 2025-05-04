#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <ctype.h>

#include "sv.h"
#include "dfa.h"
#include "utils.h"

#define TODO(str) (assert(false && str))
#define INSERT_STR(dest, src) do {			\
		(dest) = malloc(strlen(src));		\
		memcpy((dest), (src), strlen(src));	\
} while (0)

typedef struct {
	bool check_errors;
	bool interacive;
	char* source_code_path;
	char* graphviz_out_path;
	char* png_out_path;
	char* regex;
	char* input_word;
} cmd_opts_t;

static cmd_opts_t opts;

void debug_print_opts(cmd_opts_t* opts) {
	printf("Check errors:\t\t%d\n", opts->check_errors);
	printf("Interactive:\t\t%d\n", opts->interacive);
	printf("Source code path:\t%s\n", opts->source_code_path);
	printf("Graphviz out path:\t%s\n", opts->graphviz_out_path);
	printf("PNG out path:\t\t%s\n", opts->png_out_path);
	printf("Regex:\t\t\t%s\n", opts->regex);
	printf("Input word:\t\t%s\n", opts->input_word);
}

void print_help() {
	printf(
	       "Automaton Interpreter in C by Yannik Höll (2025)\n"
	       "Command line switches:\n"
	       "\t-h: Display help\n"
	       "\t-c: Check the source for errors (only works with provided source files)\n"
	       "\t-d <path>: Write graph representation to dot file for graphviz [default: automaton.dot]\n"
	       "\t-i: Start program in interactive mode\n"
	       "\t-p <path>: Path of the source of the automaton\n"
	       "\t-o <path>: Save a image of the graph at the given path [default: automaton.png]\n"
	       "\t-r <regular expression>: Regular expression from which an automaton is build\n"
	       "\t-w: Input word for the automaton [required when no -i, -c, -d provided]\n"
	       "You have to provide a path to a source file or a regular expression such that an automaton can be loaded.\n"
	       "If neither is provided, the program will exit without further action.\n"
	       "If both are provided the program will load from file. The regex will then be not taken intoaccount.\n"
       );
}

void parse_cmd_args(int argc, char** argv) {
	int c;
	while ((c = getopt(argc, argv, "hcd:p:w:r:o:")) != -1) {
		switch (c) {
		// only display help
		case 'h':
			print_help();
			exit(0);
			break;
		// check the source for errors (when source is provided) 
		case 'c':
			opts.check_errors = true;
			break;
		// provided path for graphviz output file
		case 'd':
			INSERT_STR(opts.graphviz_out_path, optarg);
			break;
		// enable interactive mode 
		case 'i':
			opts.interacive = true;
			break;
		// provided path for automaton source
		case 'p':
			INSERT_STR(opts.source_code_path, optarg);
			break;
		// provided png output path
		case 'o':
			INSERT_STR(opts.png_out_path, optarg);
			break;
		// provided regular expression
		case 'r':
			INSERT_STR(opts.regex, optarg);
			break;
		// provided input word to check
		case 'w':
			INSERT_STR(opts.input_word, optarg);
			break;
		case '?':
			exit(-1);
		}
	}
	debug_print_opts(&opts);
}

int main(int argc, char** argv) {
	parse_cmd_args(argc, argv);
	return 0;
}
