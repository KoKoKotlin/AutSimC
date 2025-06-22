#ifndef __LOADER_H_
#define __LOADER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "sv.h"
#include "utils.h"
#include "dfa.h"

typedef enum {
	IDENT,
	NUMBER,
	OP_S,
	OP_T,
	OP_DFA,
	OP_NFA,
	OP_ENFA,
	OPENING_PAREN,
	CLOSING_PAREN,
	COMMA,
	TOK_EOF,
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
	dfa_t dfa;
	loader_error_t error;
} loader_result_t;

typedef enum {
	DFA,
	NFA,
	ENFA,
	LOADER_ERROR,
	LOADER_RESULT_COUNT,
} loader_result_type_t;

CREATE_SIZED_ARRAY(token_type_t);
typedef sarray_token_type_t_t sarray_token_type_t;

loader_result_type_t load(const string, loader_result_t*);
char* loader_result_to_string(loader_result_type_t);

#endif
