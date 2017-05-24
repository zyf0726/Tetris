#pragma once
#ifndef SINGLEFILE
#include "shared.h"
#endif

struct options {
    int feature_enabled[N_FEATURES];
    int enabled_f_indices[N_FEATURES];
    int n_features_enabled;
    int n_weights_enabled;

    int verbose;
    int population_size;
    int tournament_group_size;
    int max_n_generations;
    int crossover_points;
    int elitism;
    int no_log;
    int no_change_duration;
    int reset_volume;
    int n_trials;
    int print_board;
    int n_piece_lookahead;
    int randomization_range;
    int mutation_range;

    float feature_enable_rate;
    float mutation_rate;
    float crossover_rate;
    float tournament_group_random_selection;

    selection sel;

    const char* log_directory;

};

INLINE void enable_feature (int feature_i, struct options * opt);

