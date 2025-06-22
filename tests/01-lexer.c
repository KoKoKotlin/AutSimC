#include <stdio.h>
#include <assert.h>

#include <loader.h>
#include <utils.h>
#include <sv.h>

int main() {
	string input_str = "\"string\" 'c' 10 abc";
	sv_t input = sv_from_cstr(input_str);
	lexer_t lexer = lexer_init(input);

	token_type_t types[4] = { 0 };
	token_type_t expected_types[4] = { STRING, CHAR, NUMBER, IDENT };
	token_t tok = next_token(&lexer);
	size_t i = 0;
	while(tok.type != TOK_EOF) {
		types[i++] = tok.type;
		tok = next_token(&lexer);
	}

	char msg[100] = { 0 };
	for (i = 0; i < 4; i++) {
		if (types[i] != expected_types[i]) {
		    sprintf(msg, "Types did not match! %s != %s!\n",
			    TT_NAME(expected_types[i]),
			    TT_NAME(types[i]));
		    fprintf(stderr, "%s\n", msg);
		    exit(-1);
	    }
	}

	
	sv_free(&input);
	return 0;
}
