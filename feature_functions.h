#pragma once
#ifndef SINGLEFILE
#include "shared.h"
#include "board.h"
#include "tetromino.h"
#endif
struct feature {
    const char * name;
    int weights, dynamic;
    float (* function) (struct board *, struct board *, t_last_placement *);
};

extern struct feature features[N_FEATURES];
int index_feature (const char * name);
char * feature_name (int feature_i);
float (* feature_function(int feature_i)) (board *, board *, t_last_placement *);

float f_max_height            (board * new_board, board * old_board, t_last_placement * tlp);
float f_n_holes               (board * new_board, board * old_board, t_last_placement * tlp);
float f_column_heights        (board * new_board, board * old_board, t_last_placement * tlp);
float f_column_difference     (board * new_board, board * old_board, t_last_placement * tlp);
float f_landing_height        (board * new_board, board * old_board, t_last_placement * tlp);
float f_cell_transitions      (board * new_board, board * old_board, t_last_placement * tlp);
float f_deep_well_sum         (board * new_board, board * old_board, t_last_placement * tlp);
float f_height_differences    (board * new_board, board * old_board, t_last_placement * tlp);
float f_mean_height           (board * new_board, board * old_board, t_last_placement * tlp);
float f_v_max_height          (board * new_board, board * old_board, t_last_placement * tlp);
float f_v_n_holes             (board * new_board, board * old_board, t_last_placement * tlp);
float f_v_height_differences  (board * new_board, board * old_board, t_last_placement * tlp);
float f_v_mean_height         (board * new_board, board * old_board, t_last_placement * tlp);
float f_removed_lines         (board * new_board, board * old_board, t_last_placement * tlp);
float f_weighted_blocks       (board * new_board, board * old_board, t_last_placement * tlp);
float f_well_sum              (board * new_board, board * old_board, t_last_placement * tlp);
float f_n_blocks              (board * new_board, board * old_board, t_last_placement * tlp);
float f_eroded_piece_cells    (board * new_board, board * old_board, t_last_placement * tlp);
float f_row_transitions       (board * new_board, board * old_board, t_last_placement * tlp);
float f_column_transitions    (board * new_board, board * old_board, t_last_placement * tlp);
float f_cumulative_wells_dell (board * new_board, board * old_board, t_last_placement * tlp);
float f_cumulative_wells_fast (board * new_board, board * old_board, t_last_placement * tlp);
float f_min_height            (board * new_board, board * old_board, t_last_placement * tlp);
float f_max_minus_mean_height (board * new_board, board * old_board, t_last_placement * tlp);
float f_mean_minus_min_height (board * new_board, board * old_board, t_last_placement * tlp);
float f_mean_hole_depth       (board * new_board, board * old_board, t_last_placement * tlp);
float f_max_height_difference (board * new_board, board * old_board, t_last_placement * tlp);
float f_n_adjacent_holes      (board * new_board, board * old_board, t_last_placement * tlp);
float f_max_well_depth        (board * new_board, board * old_board, t_last_placement * tlp);
float f_hole_depths           (board * new_board, board * old_board, t_last_placement * tlp);
float f_n_rows_with_holes     (board * new_board, board * old_board, t_last_placement * tlp);
float f_diversity             (board * new_board, board * old_board, t_last_placement * tlp);

