#include <stdio.h>
#include <assert.h>

#include <loader.h>
#include <utils.h>
#include <sv.h>

int main() {
	string input_str = "\"string\" 'c' 10 abc '\\e'";
	sv_t input = sv_from_cstr(input_str);
	lexer_t lexer = lexer_init(input);

	token_type_t types[5] = { 0 };
	token_type_t expected_types[5] = { STRING, CHAR, NUMBER, IDENT, CHAR };
	token_t tok = next_token(&lexer);
	size_t i = 0;
	char eps;
	while(tok.type != TOK_EOF) {
		types[i++] = tok.type;
		eps = tok.view.items[0];
		sv_print(&tok.view);
		printf("\n");
		tok = next_token(&lexer);
	}

	char msg[100] = { 0 };
	for (i = 0; i < 4; i++) {
		if (types[i] != expected_types[i]) {
		    sprintf(msg, "Types did not match! %d, %s != %s!\n",
				i,
			    TT_NAME(expected_types[i]),
			    TT_NAME(types[i]));
		    fprintf(stderr, "%s\n", msg);
		    exit(-1);
	    }
	}

	if (eps != SYM_EPS) {
		sprintf(msg, "The escape sequence `\\e` was not parsed correctly. %c != %c\n", SYM_EPS, eps);
		fprintf(stderr, "%s\n", msg);
		exit(-1);
	}
	
	sv_free(&input);
	return 0;
}
