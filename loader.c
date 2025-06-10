#include "loader.h"

#define ERROR(fmt, ...) fprintf(stderr, "[ERROR] " fmt "\n", __VA_ARGS__)
#define TT_NAME(toktype) token_type_names[toktype]

// TODO: add checking of input and proper errors with loc and reason

// (dfa|nfa|enfa) no_of_states no_of_transitions (initial_state0, initial_state1, ...) (final_state0, final_state1, ...) alphabet
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
		if (next == '(' || next == ')') {
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

loader_result_type_t load(const string path, loader_result_t* res) {
	sb_t file_contents_builder = sb_read_file(path);
	if (file_contents_builder.items == NULL) return LOADER_ERROR;
	sv_t file_contents = sb_build(&file_contents_builder);
	sb_free(&file_contents_builder);

	lexer_t lexer = lexer_init(file_contents);
	token_t temp;

	token_t op_type;
	if (!expect_token(&lexer, &op_type, 3, OP_DFA, OP_NFA, OP_ENFA)) {
		expect_error(&op_type, path, 3, OP_DFA, OP_NFA, OP_ENFA);
		goto loader_error;
	}

	token_t state_count;
	if (!expect_token(&lexer, &state_count, 1, NUMBER)) {
		expect_error(&state_count, path, 1, NUMBER);
		goto loader_error;
	}
	
	token_t transition_count;
	if (!expect_token(&lexer, &transition_count, 1, NUMBER)) {
		expect_error(&transition_count, path, 1, NUMBER);
		goto loader_error;
	}

	if (!expect_token(&lexer, &temp, 1, OPENING_PAREN)) {
		expect_error(&temp, path, 1, OPENING_PAREN);
		goto loader_error;
	}
	// buf = sv_chopc(&line, ' ', false);
	// if (!sv_tol(&buf, 10, (long*)&res->dfa.initial_state)) goto loader_error;
	// buf = sv_chopc(&line, ' ', false);
	// size_t final_state_count;
	// if (!sv_tol(&buf, 10, (long*)&final_state_count)) goto loader_error;

	// res->dfa.final_states.size = final_state_count;
	// res->dfa.final_states.items = calloc(final_state_count, sizeof(size_t));
	
	// // TOOD: make sanity checks that the data is available
	// buf = sv_chopc(&line, ')', false);
	// sv_drop(&buf, 1);
	// if (!sv_tol(&buf, 10, (long*)&res->dfa.initial_state)) goto loader_error;
	// sv_drop(&line, 1);
	
	// buf = sv_chopc(&line, ')', false);
	// sv_drop(&buf, 1);
	// sv_t final_state_buf;
	// for (size_t i = 0; i < final_state_count; i++) {
		// if (i + 1 == final_state_count) {
			// final_state_buf = buf;
		// } else {
			// final_state_buf = sv_chopc(&buf, ',', false);
		// }

		// size_t final_state_idx;
		// if (!sv_tol(&final_state_buf, 10, (long*)&final_state_idx)) goto loader_error;
		// res->dfa.final_states.items[i] = final_state_idx;

		// if (*buf.items == ' ') sv_drop(&buf, 1);
	// }
	// line.items = line.items + 1;

	// buf = sv_chopc(&line, '\n', false);
	// res->dfa.alphabet = sv_to_str(&buf);

	// res->dfa.state_names.size = state_count;
	// res->dfa.state_names.items = calloc(state_count, sizeof(string));
	// for (size_t i = 0; i < state_count; i++) {
		// buf = sv_next_line(&file_contents);
		// sv_t prefix = sv_chopc(&buf, ' ', false);
		
		// if (!sv_cmp_str(&prefix, "s")) goto loader_error;
		// buf = sv_chopc(&buf, '\n', false);
		// res->dfa.state_names.items[i] = sv_to_str(&buf);
	// }

	// res->dfa.transitions = ht_new(transition_count, &hashfunc_u32);
        // PAIR_STRUCT(u32, u32) *pairs_temp = calloc(transition_count, sizeof(PAIR_STRUCT(u32, u32)));
	// string syms = calloc(transition_count + 1, sizeof(char));

	// for (size_t i = 0; i < transition_count; i++) {
		// buf = sv_next_line(&file_contents);
		// sv_t prefix = sv_chopc(&buf, ' ', false);

		// if (!sv_cmp_str(&prefix, "t")) goto loader_error;

		// sv_t state_name_1 = sv_chopc(&buf, ' ', false); 
		// size_t state1;
		// if (!find_state_by_name(&res->dfa.state_names, &state_name_1, &state1)) goto loader_error;
		
		// sv_t state_name_2 = sv_chopc(&buf, ' ', false); 
		// size_t state2;
		// if (!find_state_by_name(&res->dfa.state_names, &state_name_2, &state2)) goto loader_error;

		// char sym = *buf.items;
		// if (sym == '\n') goto loader_error;

		// //printf("%d %d %c\n", state1, state2, sym);
		// pairs_temp[i] = PAIR(u32, u32, state1, state2);
		// syms[i] = sym;
	// }
	// ARRAY_TO_SIZED(pairs_temp, transition_count, pair_u32_u32_t, pairs);
	// res->dfa.transitions = dfa_create_transitions(pairs, syms);
	
	// return res_type;

 loader_error:
	return LOADER_ERROR;
}
