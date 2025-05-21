#ifndef __SV_H
#define __SV_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#include "da.h"
#include "utils.h"

#define SV_FMT "%.*s"
#define SV_ARG(sv) (int)((sv).count), (sv).items

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
void sb_extend(sb_t*, const char*);
sb_t sb_read_file(const string);
sv_t sb_build(const sb_t*);
sb_t sb_from_cstr(const char*);
void sb_free(sb_t*);
sv_t sv_slice(const sv_t*, size_t, size_t);
sv_t sv_next_line(sv_t*);
sv_t sv_chop(sv_t*, char, bool);
sv_t sv_drop(sv_t*, size_t);
bool sv_cmp_str(const sv_t* view, const string other);
bool sv_cmp_sv(const sv_t* view, const sv_t* other);
sv_t sv_from_cstr(const char*);
char* sv_to_str(const sv_t* sv);
bool sv_tol(const sv_t* sv, int base, long* res);
void sv_print(const sv_t*);
void sv_free(sv_t*);

#endif
