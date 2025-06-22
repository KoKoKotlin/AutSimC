#include "utils.h"

size_t hashfunc_u32(u32 x) {
	x ^= x >> 16;
	x *= 0x85ebca6b;
	x ^= x >> 13;
	x *= 0xc2b2ae35;
	x ^= x >> 16;
	return x;
}

bool string_contains(char c, const string syms) {
	for (size_t i = 0; i < strlen(syms); ++i) {
		if (c == syms[i]) return true;
	}

	return false;
}

bool sarray_contains(void* obj, sarray_t* arr, size_t item_size, comperator_t comperator) {
	for (size_t i = 0; i < arr->size; i++) {
		void* obj2 = (void*)((uint8_t*)arr->items + i * item_size);
		if (comperator(obj, obj2)) return true;
	}
	return false;
}

int sarray_index_of(void* obj, sarray_t* arr, size_t item_size, comperator_t comperator) {
	for (size_t i = 0; i < arr->size; i++) {
		void* obj2 = (void*)((uint8_t*)arr->items + i * item_size);
		if (comperator(obj, obj2)) return (int)i;
	}
	return -1;
}

void list_resize(list_t* list, size_t item_size) {
	if (list->capacity > list->count) return;

	list->capacity *= 2;
	list->items = realloc(list->items, list->capacity * item_size);
}
