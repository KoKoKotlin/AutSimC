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
		void* obj2 = (void*)((u8*)arr->items + i * item_size);
		if (comperator(obj, obj2)) return true;
	}
	return false;
}

bool list_contains(void* obj, list_t* list, size_t item_size, comperator_t comperator) {
	for (size_t i = 0; i < list->count; i++) {
		void* obj2 = (void*)((u8*)list->items + i * item_size);
		if (comperator(obj, obj2)) return true;
	}
	return false;
}

int sarray_index_of(void* obj, sarray_t* arr, size_t item_size, comperator_t comperator) {
	for (size_t i = 0; i < arr->size; i++) {
		void* obj2 = (void*)((u8*)arr->items + i * item_size);
		if (comperator(obj, obj2)) return (int)i;
	}
	return -1;
}

void list_resize(list_t* list, size_t item_size) {
	if (list->capacity > list->count) return;

	list->capacity *= 2;
	list->items = realloc(list->items, list->capacity * item_size);
}


void list_extend(list_t* list, void* items, size_t item_count, size_t item_size) {
	while (list->capacity < list->count + item_count) list_resize(list, item_size);
	u8* bytes = (u8*)list->items;
	memcpy(bytes + list->count * item_size, items, item_size * item_count);
}