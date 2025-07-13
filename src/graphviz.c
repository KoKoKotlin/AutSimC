#include "graphviz.h"

sv_t aut_to_dot(const aut_t* aut) {
	sb_t sb = { 0 };

	sb_extend(&sb, "digraph {\nrankdir=LR;\n");

	sb_extend(&sb, "start [shape = point];");
	sb_extend(&sb, "node[shape = doublecircle];");
	for (size_t i = 0; i < aut->initial_states.size; ++i) {
		sb_extend(&sb, "start -> q");
		sb_appendu(&sb, (u64)aut->initial_states.items[i]);
		sb_extend(&sb, ";\n");
	}

	sb_extend(&sb, "node[shape = circle];\n");
	for (size_t i = 0; i < aut->transitions.size; ++i) {
		transition_t t = aut->transitions.items[i];
		size_t start_idx = t.start_state;
		size_t end_idx = t.end_state;
		
		sb_appendc(&sb, 'q');
		sb_appendu(&sb, (u64)start_idx);
		sb_extend(&sb, " -> ");
		sb_appendc(&sb, 'q');
		sb_appendu(&sb, (u64)end_idx);
		sb_extend(&sb, " [label=\"");
		if (t.transition_sym != SYM_EPS) {
			sb_appendc(&sb, t.transition_sym);
		} else {
			sb_extend(&sb, "&#x03B5;");
		}
		sb_extend(&sb, "\"];\n");
	}
		
	sb_extend(&sb, "}\n");
	
	sv_t view = sb_build(&sb);
	sb_free(&sb);

	return view;
}
