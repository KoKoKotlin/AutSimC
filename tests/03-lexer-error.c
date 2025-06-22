
#include <stdio.h>

#include <utils.h>
#include <sv.h>
#include <loader.h>

int main() {
	string input_str = "\"string\" 10 'c abc";
	sv_t input = sv_from_cstr(input_str);
	lexer_t lexer = lexer_init(input);

	token_t tok = next_token(&lexer);
	while(tok.type != TOK_EOF) {
		if (tok.type == TOK_ERROR) {
			exit(0);
		}
		tok = next_token(&lexer);
	}

	fprintf(stderr, "Lexer has not found the error!");
	sv_free(&input);
	return -1;
}
