#ifndef _DA_H
#define _DA_H

#define DA_APPEND(da, item) do {					\
		if ((da)->count == (da)->size) DA_EXTEND((da), (da)->size * 2);	\
		(da)->items[(da)->count++] = item;			\
} while(0)

#define DA_EXTEND(da, ncount) do {						\
		(da)->items = realloc((da)->items, (ncount) * sizeof((da)->items)/sizeof((da)->items[0])); \
		(da)->size = ncount;					\
} while(0)

#define DA_INIT(da, ncount, nsize) do {				\
		(da)->items = malloc((nsize) * (ncount));	\
		(da)->size = (ncount);				\
		(da)->count = 0;				\
} while(0)

#endif
