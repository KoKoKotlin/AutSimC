#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdint.h>
#include <stddef.h>

typedef char* string;
typedef uint32_t u32;

#define PAIR_STRUCT(type1, type2) pair_##type1##_##type2##_t
#define CREATE_PAIR_STRUCT(type1, type2) typedef struct { type1 v1; type2 v2; } PAIR_STRUCT(type1, type2);
#define PAIR(type1, type2, v1_, v2_) ((PAIR_STRUCT(type1, type2)){ .v1 = v1_, .v2 = v2_ })

#define CREATE_SIZED_ARRAY(type) typedef struct { size_t size; type* items; } sarray_##type##_t
#define ARRAY_TO_SIZED(array, array_size, type, name)		\
	sarray_##type##_t name;					\
	(name).size = array_size;				\
	(name).items = malloc(array_size * sizeof(type));	\
	memcpy((name).items, (array), array_size * sizeof(type));


CREATE_PAIR_STRUCT(u32, u32);
CREATE_PAIR_STRUCT(u32, char);

CREATE_SIZED_ARRAY(size_t);
typedef sarray_size_t_t sarray_size_t;
CREATE_SIZED_ARRAY(string);
CREATE_SIZED_ARRAY(u32);
CREATE_SIZED_ARRAY(pair_u32_u32_t);
typedef sarray_pair_u32_u32_t_t sarray_pair_u32_t;

size_t hashfunc_u32(u32);

#endif

