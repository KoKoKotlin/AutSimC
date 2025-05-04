#include "hashtable.h"

hashtable_entry_t* ht_new_entry(hashtable_t* ht, u32 key, char value) {
	hashtable_entry_t* entry = ht->_entry_arena + ht->_current_loc;
	ht->_current_loc++;
	entry->key = key;
	entry->value = value;
	return entry;
}

size_t ht_index(const hashtable_t* ht, u32 key) {
	return ht->hashfunc(key) % ht->entry_count;
}

void ht_rehash(hashtable_t* ht) {
	ht->entry_count *= 2;
	printf("%d\n", ht->entry_count);
	assert(ht->entry_count < HASHTABLE_T_ARENA_SIZE && "Hashtable overflow");

	free(ht->items);
	ht->items = malloc(ht->entry_count * sizeof(hashtable_entry_t*));
	memset(ht->items, 0, ht->entry_count * sizeof(hashtable_entry_t*));

	bool collision = false;
	for (size_t i = 0; i < ht->_current_loc; ++i) {
		size_t index = ht_index(ht, ht->_entry_arena[i].key);
		if (ht->items[index] != 0) {
			collision = true;
			break;
		}
		ht->items[index] = ht->_entry_arena + i;
	}

	if (collision) ht_rehash(ht);
}

hashtable_t ht_new(size_t initial_size, hashfunc_t hashfunc) {
	hashtable_t ht = { 0 };
	ht.entry_count = initial_size;
	ht.items = calloc(initial_size, sizeof(hashtable_entry_t*));
	ht.hashfunc = hashfunc;
	ht._entry_arena = calloc(HASHTABLE_T_ARENA_SIZE, sizeof(hashtable_entry_t));
	ht._current_loc = 0;
	return ht;
}

void ht_insert(hashtable_t* ht, u32 key, char value) {
	size_t index = ht_index(ht, key);
	// check for hash collition and rehash if collision is detected
	while (ht->items[index] != 0) ht_rehash(ht);
	ht->items[index] = ht_new_entry(ht, key, value);
}

char* ht_get(const hashtable_t* ht, u32 key) {
	hashtable_entry_t* entry = ht->items[ht_index(ht, key)]; 
	if (entry) return &entry->value;
	else return NULL;
}

void ht_destroy(hashtable_t* ht) {
	free(ht->items);
	free(ht->_entry_arena);
}

void ht_debug_print(const hashtable_t* ht) {
	for (size_t i = 0; i < ht->entry_count; ++i) {
		printf("%d: %p\n", i, ht->items[i]);
	}
}
