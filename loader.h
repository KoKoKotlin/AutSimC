#ifndef __LOADER_H_
#define __LOADER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sv.h"
#include "utils.h"
#include "dfa.h"

typedef union {
	dfa_t dfa;
} loader_result_t;

typedef enum {
	DFA,
	NFA,
	ENFA,
	LOADER_ERROR,
	LOADER_RESULT_COUNT,
} loader_result_type_t;

loader_result_type_t load(const string, loader_result_t*);
char* loader_result_to_string(loader_result_type_t);

#endif
