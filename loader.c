#include "loader.h"

#define ERROR(fmt, ...) fprintf(stderr, "[ERROR] " fmt "\n", __VA_ARGS__)
#define TT_NAME(toktype) token_type_names[toktype]

#define EXPECT(var_name, count, ...) \
	if (!expect_token(&lexer, &var_name, count, __VA_ARGS__)) { \
		expect_error(&var_name, path, count, __VA_ARGS__);  \
		goto loader_error;					  \
	}


// TODO: still add sanity checks
// TODO: add string/char parsing/toktype
// TODO: add string as state name, char as transition sym

// (dfa|nfa|enfa) no_of_states no_of_transitions no_of_initial_states no_of_final_states (initial_state0, initial_state1, ...) (final_state0, final_state1, ...) alphabet
// (s state_name)*
// (t start_state_name dest_state_name symbol)*

static string loader_result_names[4] = {
	"DFA",
	"NFA",
	"ENFA",
	"LOADER_ERROR",
};

static string token_type_names[TOKEN_TYPE_COUNT] = {
	"IDENT",
	"NUMBER",
	"OP_S",
	"OP_T",
	"OP_DFA",
	"OP_NFA",
	"OP_ENFA",
	"OPENING_PAREN",
	"CLOSING_PAREN",
	"EOF",
};

lexer_t lexer_init(sv_t view) {
	lexer_t lexer = { 0 };
	lexer.view = view;
	lexer.buf = view.items;
	lexer.col = 1;
	lexer.row = 1;
	return lexer;
}

token_type_t get_token_type(sv_t* view) {

	if (sv_cmp_str(view, "dfa")) {
		return OP_DFA;
	} else if (sv_cmp_str(view, "nfa")) {
		return OP_NFA;
	} else if (sv_cmp_str(view, "enfa")) {
		return OP_ENFA;
	} else if (sv_cmp_str(view, "s")) {
		return OP_S;
	} else if (sv_cmp_str(view, "t")) {
		return OP_T;
	} else if (sv_cmp_str(view, "(")) {
		return OPENING_PAREN;
	} else if (sv_cmp_str(view, ")")) {
		return CLOSING_PAREN;
	} else if (sv_cmp_str(view, ",")) {
		return COMMA;
	}

	size_t res;
	if (sv_tol(view, 10, (long*)&res)) {
		return NUMBER;
	}

	return IDENT;
}

void trim_whitespace(lexer_t* lexer) {
	char* buf = lexer->view.items;
	size_t i = 0;
	while (i < lexer->view.count) {
		char next = buf[i];
		if (string_contains(next, "\t\r ")) {
			lexer->col += 1;
		} else if (next == '\n') {
			lexer->col = 1;
			lexer->row += 1;
		} else {
			break;
		}
		i++;
	}
	if (i > 0) sv_chopi(&lexer->view, i);
}

token_t next_token(lexer_t* lexer) {
	token_t tok = { 0 };
	
	trim_whitespace(lexer);

	if (lexer->view.count == 0) {
		tok.type = TOK_EOF;
		return tok;
	}

	char* start = lexer->view.items;
	for (size_t i = 0; ; ++i) {
		char next = start[i];
		if (next == '(' || next == ')' || next == ',') {
			tok.col = lexer->col;
			tok.row = lexer->row;
			tok.view = sv_chopi(&lexer->view, 1);
			tok.type = get_token_type(&tok.view);
			lexer->current += 1;
			lexer->col += 1;
			break;
		} else if (string_contains(next, " \n") || i == lexer->view.count) {
			tok.col = lexer->col;
			tok.row = lexer->row;
			tok.view = sv_chopi(&lexer->view, i);
			tok.type = get_token_type(&tok.view);
			lexer->current += i;
			lexer->col += i;
			break;
		}
	}	

	if (lexer->buf[lexer->current - 1] == '\n') {
		lexer->col = 1;
		lexer->row += 1;
	}

	return tok;
}

bool tok_type_equal(void* obj1, void* obj2) {
	token_type_t type1 = *(token_type_t*)obj1;
	token_type_t type2 = *(token_type_t*)obj2;

	return type1 == type2;
}

bool expect_token(lexer_t* lexer, token_t* res, int count, ...) {
	token_t token = next_token(lexer);
	*res = token;
	
	va_list args;
	va_start(args, count);

	for (size_t i = 0; i < count; i++) {
		token_type_t type = va_arg(args, token_type_t);
		if (type == token.type) {
			va_end(args);
			return true;
		}
	}

	va_end(args);
	return false;
}

void expect_error(const token_t* token, const string path, int count, ...) {
	fprintf(stderr, "[ERROR] %s:%zu:%zu: Expected ", path, token->row, token->col);
	va_list args;
	va_start(args, count);

	for (int i = 0; i < count; ++i) {
		token_type_t type = va_arg(args, token_type_t);
		fprintf(stderr, "%s%s", i>0? ", " : "", TT_NAME(type));
	}

	va_end(args);

	fprintf(stderr, ", but got %s("SV_FMT")\n", TT_NAME(token->type), SV_ARG(token->view));
}

void debug_print_token(const token_t* token) {
	printf("%zu:%zu:\t\t\""SV_FMT"\"\t\t%s\n", token->row, token->col, SV_ARG(token->view), token_type_names[token->type]);
}

char* loader_result_to_string(loader_result_type_t res) {
	if (res >= LOADER_RESULT_COUNT)
		assert(false && "UNREACHABLE!");       
	else
		return loader_result_names[res];
}

bool find_state_by_name(const sarray_string_t* state_names, const sv_t* name, size_t* res) {
	for (size_t i = 0; i < state_names->size; i++) {
		if (sv_cmp_str(name, state_names->items[i])) {
			*res = i;
			return true;
		}
	}
	return false;
}

// (dfa|nfa|enfa) no_of_states no_of_transitions (initial_state0, initial_state1, ...) (final_state0, final_state1, ...) alphabet
// (s state_name)*
// (t start_state_name dest_state_name symbol)*

loader_result_type_t load(const string path, loader_result_t* res) {
	sb_t file_contents_builder = sb_read_file(path);

 defer: {
		sb_free(&file_contents_builder);
	}
	
	if (file_contents_builder.items == NULL) return LOADER_ERROR;
	sv_t file_contents = sb_build(&file_contents_builder);

	lexer_t lexer = lexer_init(file_contents);
	token_t temp;

	token_t op_type;
	EXPECT(op_type, 3, OP_DFA, OP_NFA, OP_ENFA);
	loader_result_type_t res_type;

	switch (op_type.type) {
	case OP_DFA: res_type = DFA; break;
	case OP_NFA: res_type = NFA; break;
	case OP_ENFA: res_type = ENFA; break;
	default: UNREACHABLE();
	}

	token_t state_count_tok;
	EXPECT(state_count_tok, 1, NUMBER);
	size_t state_count;
	sv_tol(&state_count_tok.view, 10, (long*)&state_count);
	
	token_t transition_count_tok;
	EXPECT(transition_count_tok, 1, NUMBER);
	size_t transition_count;
	sv_tol(&transition_count_tok.view, 10, (long*)&transition_count);
	
	token_t initial_state_count_tok;
	EXPECT(initial_state_count_tok, 1, NUMBER);
	size_t initial_state_count;
	sv_tol(&initial_state_count_tok.view, 10, (long*)&initial_state_count);

	token_t final_state_count_tok;
	EXPECT(final_state_count_tok, 1, NUMBER);
	size_t final_state_count; 
	sv_tol(&final_state_count_tok.view, 10, (long*)&final_state_count);

	EXPECT(temp, 1, OPENING_PAREN);
	token_t initial_state_tok;
	EXPECT(initial_state_tok, 1, NUMBER);
	EXPECT(temp, 1, CLOSING_PAREN);
	
	EXPECT(temp, 1, OPENING_PAREN);
	res->dfa.final_states.size = final_state_count;
	res->dfa.final_states.items = calloc(final_state_count, sizeof(size_t));
	
	for (size_t i = 0; i < final_state_count; ++i) {
		token_t final_state_tok;
		EXPECT(final_state_tok, 1, NUMBER);
		size_t final_state;
		sv_tol(&final_state_tok.view, 10, (long*)&final_state);
		res->dfa.final_states.items[i] = final_state;

		if (i + 1 == final_state_count) {
			EXPECT(temp, 1, CLOSING_PAREN);
		} else {
			EXPECT(temp, 1, COMMA);
		}
	}

	token_t alphabet_tok;
	EXPECT(alphabet_tok, 1, IDENT);
	res->dfa.alphabet = sv_to_str(&alphabet_tok.view);

	res->dfa.state_names.size = state_count;
	res->dfa.state_names.items = calloc(state_count, sizeof(string));
	for (size_t i = 0; i < state_count; i++) {
		EXPECT(temp, 1, OP_S);

		token_t state_name_tok;
		EXPECT(state_name_tok, 1, IDENT);
		res->dfa.state_names.items[i] = sv_to_str(&state_name_tok.view);
	}

	res->dfa.transitions = ht_new(transition_count, &hashfunc_u32);
        PAIR_STRUCT(u32, u32) *pairs_temp = calloc(transition_count, sizeof(PAIR_STRUCT(u32, u32)));
	string syms = calloc(transition_count + 1, sizeof(char));

	for (size_t i = 0; i < transition_count; i++) {
		EXPECT(temp, 1, OP_T);
		
		token_t start_state_tok, end_state_tok;
		EXPECT(start_state_tok, 1, IDENT);
		EXPECT(end_state_tok, 1, IDENT);
		
		u32 start_state, end_state;
		if (!find_state_by_name(&res->dfa.state_names, &start_state_tok.view, (size_t*)&start_state)) goto loader_error;
		if (!find_state_by_name(&res->dfa.state_names, &end_state_tok.view, (size_t*)&end_state)) goto loader_error;

		token_t sym_tok;
		EXPECT(sym_tok, 1, IDENT);
		
		if (sym_tok.view.count != 1) goto loader_error;
		if (!string_contains(sym_tok.view.items[1], res->dfa.alphabet)) goto loader_error;
		
		pairs_temp[i] = PAIR(u32, u32, start_state, end_state);
		syms[i] = sym_tok.view.items[i];
	}
	ARRAY_TO_SIZED(pairs_temp, transition_count, pair_u32_u32_t, pairs);
	res->dfa.transitions = dfa_create_transitions(pairs, syms);
	
	return res_type;

 loader_error:
	return LOADER_ERROR;
}
