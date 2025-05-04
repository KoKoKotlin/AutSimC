#ifndef _SV_H
#define _SV_V

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "da.h"


// the size field is missing such that DA_APPEND cannot be used with this struct
typedef struct {
	size_t count;
	char* items;
} sv_t;

// Mutable string builder
typedef struct {
	size_t count;
	size_t size;
	char* items;
} sb_t;

void sb_append(sb_t*, char);
void sb_extend(sb_t*, const char* );
sv_t sb_build(const sb_t*);
sb_t sb_from_cstr(const char*);
void sb_free(sb_t*);
sv_t sv_slice(const sv_t*, size_t, size_t);
sv_t sv_from_cstr(const char*);
void sv_print(const sv_t*);
void sv_free(sv_t*);

#endif
