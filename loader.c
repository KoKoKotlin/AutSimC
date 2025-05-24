#include "loader.h"

// TODO: add checking of input and proper errors with loc and reason

// (dfa|nfa|enfa) no_of_states no_of_transitions (initial_state0, initial_state1, ...) (final_state0, final_state1, ...) alphabet
// (s state_name)*
// (t start_state_name dest_state_name symbol)*

static char* loader_result_names[4] = {
	"DFA",
	"NFA",
	"ENFA",
	"LOADER_ERROR",
};

char* loader_result_to_string(loader_result_type_t res) {
	if (res >= LOADER_RESULT_COUNT)
		assert(false && "UNREACHABLE!");       
	else
		return loader_result_names[res];
}

bool find_state_by_name(const sarray_string_t* state_names, const sv_t* name, size_t* res) {
	for (size_t i = 0; i < state_names->size; i++) {
		if (sv_cmp_str(name, state_names->items[i])) {
			*res = i;
			return true;
		}
	}
	return false;
}

loader_result_type_t load(const string path, loader_result_t* res) {
	sb_t file_contents_builder = sb_read_file(path);
	if (file_contents_builder.items == NULL) return LOADER_ERROR;
	sv_t file_contents = sb_build(&file_contents_builder);
	sb_free(&file_contents_builder);

	char* buffer = file_contents.items;
	sv_t line = sv_next_line(&file_contents);

	sv_t type = sv_chop(&line, ' ', false);

	loader_result_type_t res_type;
	if (sv_cmp_str(&type, "dfa")) {
		res_type = DFA;
	} else if (sv_cmp_str(&type, "nfa")) {
		res_type = NFA;
	} else if (sv_cmp_str(&type, "enfa")) {
		res_type = ENFA;
	} else {
		printf("ERROR: Type "SV_FMT" unknown!\n", SV_ARG(type));
		goto loader_error;
	}
	
	sv_t buf;
	buf = sv_chop(&line, ' ', false);
	size_t state_count;
	if (!sv_tol(&buf, 10, (long*)&state_count)) goto loader_error;
	buf = sv_chop(&line, ' ', false);
	size_t transition_count;
	if (!sv_tol(&buf, 10, (long*)&transition_count)) goto loader_error;
	buf = sv_chop(&line, ' ', false);
	if (!sv_tol(&buf, 10, (long*)&res->dfa.initial_state)) goto loader_error;
	buf = sv_chop(&line, ' ', false);
	size_t final_state_count;
	if (!sv_tol(&buf, 10, (long*)&final_state_count)) goto loader_error;

	res->dfa.final_states.size = final_state_count;
	res->dfa.final_states.items = calloc(final_state_count, sizeof(size_t));
	
	// TOOD: make sanity checks that the data is available
	buf = sv_chop(&line, ')', false);
	sv_drop(&buf, 1);
	if (!sv_tol(&buf, 10, (long*)&res->dfa.initial_state)) goto loader_error;
	sv_drop(&line, 1);
	
	buf = sv_chop(&line, ')', false);
	sv_drop(&buf, 1);
	sv_t final_state_buf;
	for (size_t i = 0; i < final_state_count; i++) {
		if (i + 1 == final_state_count) {
			final_state_buf = buf;
		} else {
			final_state_buf = sv_chop(&buf, ',', false);
		}

		size_t final_state_idx;
		if (!sv_tol(&final_state_buf, 10, (long*)&final_state_idx)) goto loader_error;
		res->dfa.final_states.items[i] = final_state_idx;

		if (*buf.items == ' ') sv_drop(&buf, 1);
	}
	line.items = line.items + 1;

	buf = sv_chop(&line, '\n', false);
	res->dfa.alphabet = sv_to_str(&buf);

	res->dfa.state_names.size = state_count;
	res->dfa.state_names.items = calloc(state_count, sizeof(string));
	for (size_t i = 0; i < state_count; i++) {
		buf = sv_next_line(&file_contents);
		sv_t prefix = sv_chop(&buf, ' ', false);
		
		if (!sv_cmp_str(&prefix, "s")) goto loader_error;
		buf = sv_chop(&buf, '\n', false);
		res->dfa.state_names.items[i] = sv_to_str(&buf);
	}

	res->dfa.transitions = ht_new(transition_count, &hashfunc_u32);
        PAIR_STRUCT(u32, u32) *pairs_temp = calloc(transition_count, sizeof(PAIR_STRUCT(u32, u32)));
	string syms = calloc(transition_count + 1, sizeof(char));

	for (size_t i = 0; i < transition_count; i++) {
		buf = sv_next_line(&file_contents);
		sv_t prefix = sv_chop(&buf, ' ', false);

		if (!sv_cmp_str(&prefix, "t")) goto loader_error;

		sv_t state_name_1 = sv_chop(&buf, ' ', false); 
		size_t state1;
		if (!find_state_by_name(&res->dfa.state_names, &state_name_1, &state1)) goto loader_error;
		
		sv_t state_name_2 = sv_chop(&buf, ' ', false); 
		size_t state2;
		if (!find_state_by_name(&res->dfa.state_names, &state_name_2, &state2)) goto loader_error;

		char sym = *buf.items;
		if (sym == '\n') goto loader_error;

		//printf("%d %d %c\n", state1, state2, sym);
		pairs_temp[i] = PAIR(u32, u32, state1, state2);
		syms[i] = sym;
	}
	ARRAY_TO_SIZED(pairs_temp, transition_count, pair_u32_u32_t, pairs);
	res->dfa.transitions = dfa_create_transitions(pairs, syms);
	
	return res_type;

 loader_error:
	free(buffer);
	return LOADER_ERROR;
}
