#ifndef SINGLEFILE
#include <string.h>

#include "options.h"
#include "board.h"
#include "feature_functions.h"
#include "feature_helpers.h"
#endif
char full_cells_on_line[POSSIBLE_LINES];
feature features[N_FEATURES] = {
    {
        .name = "--f-max-height",

        .dynamic = false,
        .function = &f_max_height,
    }, {
        .name = "--f-n-holes",

        .dynamic = false,
        .function = &f_n_holes,
    },  {
        .name = "--f-landing-height",

        .dynamic = true,
        .function = &f_landing_height,
    }, {
        .name = "--f-cell-transitions",

        .dynamic = false,
        .function = &f_cell_transitions,
    }, {
        .name = "--f-deep-well-sum",

        .dynamic = false,
        .function = &f_deep_well_sum,
    }, {
        .name = "--f-height-differences",

        .dynamic = false,
        .function = &f_height_differences,
    }, {
        .name = "--f-mean-height",

        .dynamic = false,
        .function = &f_mean_height,
    }, {
        .name = "--f-v-max-height",

        .dynamic = true,
        .function = &f_v_max_height,
    }, {
        .name = "--f-v-n-holes",

                .dynamic = true,
        .function = &f_v_n_holes,
    }, {
        .name = "--f-v-height-differences",

                .dynamic = true,
        .function = &f_v_height_differences,
    }, {
        .name = "--f-v-mean-height",

                .dynamic = true,
        .function = &f_v_mean_height,
    }, {
        .name = "--f-removed-lines",

                .dynamic = true,
        .function = &f_removed_lines,
    }, {
        .name = "--f-weighted-blocks",

        .dynamic = false,
        .function = &f_weighted_blocks,
    }, {
        .name = "--f-well-sum",

        .dynamic = false,
        .function = &f_well_sum,
    }, {
        .name = "--f-n-blocks",

        .dynamic = false,
        .function = &f_n_blocks,
    }, {
        .name = "--f-eroded-piece-cells",

                .dynamic = true,
        .function = &f_eroded_piece_cells,
    }, {
        .name = "--f-row-transitions",

        .dynamic = false,
        .function = &f_row_transitions,
    }, {
        .name = "--f-column-transitions",

        .dynamic = false,
        .function = &f_column_transitions,
    }, {
        .name = "--f-cumulative-wells-dell",

        .dynamic = false,
        .function = &f_cumulative_wells_dell,
    }, {
        .name = "--f-cumulative-wells-fast",

        .dynamic = false,
        .function = &f_cumulative_wells_fast,
    }, {
        .name = "--f-min-height",

        .dynamic = false,
        .function = &f_min_height,
    }, {
        .name = "--f-max-minus-mean-height",

        .dynamic = false,
        .function = &f_max_minus_mean_height,
    }, {
        .name = "--f-mean-minus-min-height",

        .dynamic = false,
        .function = &f_mean_minus_min_height,
    }, {
        .name = "--f-mean-hole-depth",

        .dynamic = false,
        .function = &f_mean_hole_depth,
    }, {
        .name = "--f-max-height-difference",

        .dynamic = false,
        .function = &f_max_height_difference,
    }, {
        .name = "--f-n-adjacent-holes",

        .dynamic = false,
        .function = &f_n_adjacent_holes,
    }, {
        .name = "--f-max-well-depth",

        .dynamic = false,
        .function = &f_max_well_depth,
    }, {
        .name = "--f-hole-depths",

        .dynamic = false,
        .function = &f_hole_depths,
    }, {
        .name = "--f-n-rows-with-holes",

        .dynamic = false,
        .function = &f_n_rows_with_holes,
    }, {
        .name = "--f-diversity",

        .dynamic = false,
        .function = &f_diversity,
    }
};

int column_heigths[BOARD_WIDTH];
int features_cached[N_FEATURES];
float cached_feature_values[N_FEATURES];

void reset_feature_caches (const options * opt) {
    for (int i = 0; i < BOARD_WIDTH; i++) {
        column_heigths[i] = -1;
    }

    for (int i = 0; i < N_FEATURES; i++) {
        features_cached[i] = 0;
    }
}

void initialize_feature_helpers (const options * opt)
{

    for (int i = 0; i < POSSIBLE_LINES; i++) {
        int n_full_cells = 0;

        for (int x = 0; x < BOARD_WIDTH; x++) {
            if (i & cell_masks[x]) {
                n_full_cells++;
            }
        }

        full_cells_on_line[i] = n_full_cells;
    }

    reset_feature_caches(opt);
}

int feature_exists (const char * name) {
    for (int i = 0; i < N_FEATURES; i++) {
        if (strcmp(features[i].name, name) == 0) {
            return 1;
        }
    }

    return 0;
}

int feature_index (const char * name) {
    for (int i = 0; i < N_FEATURES; i++) {
        if (strcmp(features[i].name, name) == 0) {
            return i;
        }
    }

    exit(1);
}

float call_feature (int feature_i, board * new_board, board * old_board, t_last_placement * tlp) {
    if (features_cached[feature_i] == 0) {
        cached_feature_values[feature_i] = (features[feature_i].function) (new_board, old_board, tlp);
        features_cached[feature_i] = 1;
    }

    return cached_feature_values[feature_i];
}

int column_height (board * board, int column) {
    if (column_heigths[column] == -1) {
        column_heigths[column] = 0;

        for (int y = 0; y < BOARD_HEIGHT; y++) {
            if (board->lines[y] & cell_masks[column]) {
                column_heigths[column] = BOARD_HEIGHT - y;
                break;
            }
        }
    }

    return column_heigths[column];
}
