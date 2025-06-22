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
	view.items = calloc(view.count, sizeof(char));
	memcpy(view.items, sb->items, sb->count * sizeof(char));
	return view;
}

sv_t sv_slice(const sv_t* sv, size_t start, size_t end) {
	end = end - 1;
	assert(end >= start && "SV View end < start!");
	assert(start < sv->count && end < sv->count && "SV View out of bounds!");
	sv_t slice_view = { .count = end - start, .items = sv->items + start };
	return slice_view;
}

sv_t sv_next_line(sv_t* view) {
	return sv_chopc(view, '\n', false);
}

sv_t sv_chopc(sv_t* view, char c, bool include_delim) {
	char* occ = view->items + view->count;
	for (size_t i = 0; i < view->count; ++i) {
		if (view->items[i] == c) {
			occ = view->items + i;
			break;
		}
	}
	
	sv_t res_view = { .count=(occ - view->items + (include_delim ? 1 : 0)), .items=view->items};
	view->items = occ + 1;
	view->count -= res_view.count;

	return res_view;
}

sv_t sv_chops(sv_t* view, string syms, bool include_delim) {
	char* occ = view->items + view->count;
	for (size_t i = 0; i < view->count; ++i) {
		if (string_contains(view->items[i], syms)) {
			occ = view->items + i;
			break;
		}
	}
	
	sv_t res_view = { .count=(occ - view->items + (include_delim ? 1 : 0)), .items=view->items};
	view->items = occ + 1;
	view->count -= res_view.count;

	return res_view;
}

sv_t sv_chopi(sv_t* view, size_t amount) {
	amount = (amount < view->count) ? amount : view->count;
	sv_t res_view = { .count=amount, .items=view->items};
	view->items += amount;
	view->count -= amount;

	return res_view;
}

sv_t sv_drop(sv_t* view, size_t count) {
	if (count > view->count) count = view->count;
	
	sv_t res_view = { .count=count, .items=view->items};
	view->items += count;
	view->count -= count;

	return res_view;
}

bool sv_cmp_str(const sv_t* view, const string other) {
	if (view->count != strlen(other)) return false;

	for (size_t i = 0; i < view->count; ++i) {
		if (view->items[i] != other[i]) return false;
	}

	return true;
}

bool sv_cmp_sv(const sv_t* this, const sv_t* other) {
	if (this->count != other->count) return false;

	for (size_t i = 0; i < this->count; ++i) {
		if (this->items[i] != other->items[i]) return false;
	}

	return true;
}

sb_t sb_from_cstr(const char* str) {
	sb_t builder = {0};
	builder.size = 0;
	builder.count = strlen(str);
	sb_extend(&builder, str);
	return builder;
}

sb_t sb_read_file(const string path) {
	sb_t builder = { 0 };
	FILE* file = fopen(path, "r");

	if (file == NULL) return builder; 

	fseek(file, 0, SEEK_END);
	size_t flen = ftell(file);
	rewind(file);

	builder.count = flen;
	builder.size = flen;
	builder.items = calloc(flen, sizeof(char));

	if (fread(builder.items, sizeof(char), flen, file) != flen) {
		free(builder.items);
		builder.items = NULL;
		builder.count = 0;
		builder.size = 0;
	}
	
	fclose(file);
	return builder;
}

sv_t sv_from_cstr(const char* str) {
	sv_t view = {0};
	view.count = strlen(str);
	view.items = malloc(view.count * sizeof(char));
	memcpy(view.items, str, strlen(str));
	return view;
}

char* sv_to_str(const sv_t* sv) {
	char* str = calloc(sv->count + 1, sizeof(char));
	memcpy(str, sv->items, sv->count * sizeof(char));
	return str;
}

bool sv_tol(const sv_t* sv, int base, long* res) {
	char* str = sv_to_str(sv);
	char* endptr;
	*res = strtol(str, &endptr, base);
	bool success = *endptr == '\0';
	free(str);
	return success;
}

void sv_print(const sv_t* sv) {
	printf(SV_FMT, SV_ARG(*sv));
}

void sb_free(sb_t* sb) {
	free(sb->items);
}

void sv_free(sv_t* sv) {
	free(sv->items);
}
