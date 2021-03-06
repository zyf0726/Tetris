#pragma once
#ifndef SINGLEFILE
#include "board.h"
#include "options.h"

#endif
#define POSSIBLE_LINES 65536 // 2^16

extern char full_cells_on_line[POSSIBLE_LINES];

void reset_feature_caches (const options * opt);
void initialize_feature_helpers (const options * opt);
int feature_exists (const char * name);
int feature_index (const char * name);
float call_feature (int feature_i, struct board * new_board, struct board * old_board, struct t_last_placement * tlp);
int column_height (struct board * board, int column);

