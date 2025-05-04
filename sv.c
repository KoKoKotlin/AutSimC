#include "sv.h"

void sb_append(sb_t* sb, char c) {
	DA_APPEND(sb, c);
}

void sb_extend(sb_t* sb, const char* str) {
	while (sb->size < sb->count + strlen(str)) DA_EXTEND(sb, sb->size + strlen(str));
	memcpy(sb->items + sb->count, str, strlen(str));
}

sv_t sb_build(const sb_t* sb) {
	sv_t view = { 0 };
	view.count = sb->count;
	memcpy(view.items, sb->items, sb->count * sizeof(char));
}

sv_t sv_slice(const sv_t* sv, size_t start, size_t end) {
	end = end - 1;
	assert(end >= start && "SV View end < start!");
	assert(start < sv->count && end < sv->count && 0 <= start && 0 <= end && "SV View out of bounds!");
	sv_t slice_view = { .count = end - start, .items = sv->items + start };
	return slice_view;
}

sb_t sb_from_cstr(const char* str) {
	sb_t builder = {0};
	builder.size = 0;
	builder.count = strlen(str);
	sb_extend(&builder, str);
	return builder;
}

sv_t sv_from_cstr(const char* str) {
	sv_t view = {0};
	view.count = strlen(str);
	view.items = malloc(view.count * sizeof(char));
	memcpy(view.items, str, strlen(str));
	return view;
}

void sv_print(const sv_t* sv) {
	printf("%.*s", sv->count, sv->items);
}

void sb_free(sb_t* sb) {
	free(sb->items);
}

void sv_free(sv_t* sv) {
	free(sv->items);
}
