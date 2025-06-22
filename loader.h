#ifndef __LOADER_H_
#define __LOADER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "sv.h"
#include "utils.h"
#include "aut.h"

#define ERROR(fmt, ...) fprintf(stderr, "[ERROR] " fmt "\n", __VA_ARGS__)
#define TT_NAME(toktype) token_type_names[toktype]
#define LRES_NAME(res) loader_result_names[res]

#define EXPECT(var_name, count, ...) \
	if (!expect_token(&lexer, &var_name, count, __VA_ARGS__)) { \
		expect_error(&var_name, path, count, __VA_ARGS__);  \
		goto loader_error;					  \
	}

typedef enum {
	IDENT,
	STRING,
	CHAR,
	NUMBER,
	OP_S,
	OP_T,
	OP_A,
	OP_F,
	OP_I,
	OP_DFA,
	OP_NFA,
	OP_ENFA,
	OPENING_PAREN,
	CLOSING_PAREN,
	OPENING_BRACE,
	CLOSING_BRACE,
	COMMA,
	TOK_EOF,
	TOK_ERROR,
	TOKEN_TYPE_COUNT,
} token_type_t;

typedef struct {
	string buf;
	sv_t view;
	size_t current;
	size_t row;
	size_t col;
} lexer_t;

typedef struct {
	sv_t view;
	token_type_t type;
	size_t row;
	size_t col;
} token_t;

typedef enum {
	IO_ERROR,
} loader_error_type_t;

typedef struct {
	loader_error_type_t errtype;
	size_t col;
	size_t row;
} loader_error_t;

typedef union {
	aut_t aut;
	loader_error_t error;
} loader_result_t;

typedef enum {
	SUCCESS,
	LOADER_ERROR,
	LOADER_RESULT_COUNT,
} loader_result_type_t;

static string loader_result_names[4] = {
	"DFA",
	"NFA",
	"ENFA",
	"LOADER_ERROR",
};

static string token_type_names[TOKEN_TYPE_COUNT] = {
	"IDENT",
	"STRING",
	"CHAR",
	"NUMBER",
	"OP_S",
	"OP_T",
	"OP_A",
	"OP_F",
	"OP_I",
	"OP_DFA",
	"OP_NFA",
	"OP_ENFA",
	"OPENING_PAREN",
	"CLOSING_PAREN",
	"OPENING_BRACE",
	"CLOSING_BRACE",
	"COMMA",
	"EOF",
	"ERROR",
};

CREATE_SIZED_ARRAY(token_type_t);
typedef sarray_token_type_t_t sarray_token_type_t;

loader_result_type_t load(const string, loader_result_t*);
char* loader_result_to_string(loader_result_type_t);
lexer_t lexer_init(sv_t view);
token_t error_tok(lexer_t* lexer, const string msg);
token_t next_token(lexer_t* lexer);
void debug_print_token(const token_t* token);
void expect_error(const token_t* token, const string path, int count, ...);
bool expect_token(lexer_t* lexer, token_t* res, int count, ...);
bool tok_type_equal(void* obj1, void* obj2);
token_type_t get_token_type(sv_t* view);

#endif
