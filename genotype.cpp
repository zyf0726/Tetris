
#ifndef SINGLEFILE
#include <stdlib.h>
#include <string.h>

#include "genotype.h"
#include "options.h"
#include "random.h"
#include "feature_functions.h"
#include "feature_helpers.h"
#endif

void free_genotype (genotype* g) {
    free(g->feature_weights);
    free(g);
}

genotype* initialize_genotype (options* opt) {
    genotype* g = (genotype*)malloc(sizeof(genotype));

    g->feature_weights = (float*)malloc(sizeof(float) * opt->n_weights_enabled);

    return g;
}

genotype* copy_genotype (genotype* gt, options* opt) {
    genotype* copy = initialize_genotype(opt);

    memcpy(copy->feature_weights, gt->feature_weights, sizeof(float) * opt->n_weights_enabled);

    return copy;
}

void randomize_genotype (genotype* g, options* opt) {
    for (int i = 0; i < opt->n_weights_enabled; i++) {
        g->feature_weights[i] = l_rand(- opt->randomization_range / 2 - 1, opt->randomization_range / 2 + 1, opt);
    }

}



genotype* crossover_genotypes (genotype* g_1, genotype* g_2, options* opt) {
    genotype* g = initialize_genotype(opt);

    if (opt->crossover_points == opt->n_features_enabled) {
        // Uniform crossover

        int weight_i = 0;

        for (int i = 0; i < opt->n_features_enabled; i++) {
            genotype * selecting_from_genotype;

            if (b_rand(opt)) {
                selecting_from_genotype = g_1;
            } else {
                selecting_from_genotype = g_2;
            }


            for (int a = 0; a < features[opt->enabled_f_indices[i]].weights; a++, weight_i++) {
                g->feature_weights[weight_i] = selecting_from_genotype->feature_weights[weight_i];
            }
        }
    } else {
        // Generate N crossover points and proceed with the crossover

        int possible_points[opt->n_features_enabled - 1];
        int selected_points[opt->crossover_points];

        for (int i = 0; i < opt->n_features_enabled - 1; i++) {
            possible_points[i] = i;
        }

        int r;

        for (int i = 0; i < opt->crossover_points; i++) {
            r = i + l_rand(0, opt->n_features_enabled - 1 - i, opt);

            selected_points[i] = possible_points[r];
            possible_points[r] = possible_points[i];
        }

        sort(selected_points, selected_points+ opt->crossover_points);

        int selecting_from = b_rand(opt),
            at_point = 0,
            weight_i = 0;

        for (int i = 0; i < opt->n_features_enabled; i++) {
            genotype * selecting_from_genotype;

            if (selecting_from) {
                selecting_from_genotype = g_1;
            } else {
                selecting_from_genotype = g_2;
            }


            for (int a = 0; a < features[opt->enabled_f_indices[i]].weights; a++, weight_i++) {
                g->feature_weights[weight_i] = selecting_from_genotype->feature_weights[weight_i];
            }

            if (at_point < opt->crossover_points && i == selected_points[at_point]) {
                at_point++;
                selecting_from = !selecting_from;
            }
        }
    }

    return g;
}

int mutate_genotype (genotype* g, options* opt) {
    int was_mutated = 0,
        weight_i = 0,
        previous_value;

    for (int i = 0; i < opt->n_features_enabled; i++) {
        for (int a = 0; a < features[opt->enabled_f_indices[i]].weights; a++) {
            if (f_rand(opt) > opt->mutation_rate) {
                previous_value = g->feature_weights[weight_i];

                g->feature_weights[weight_i] += l_rand(- opt->mutation_range / 2 - 1, opt->mutation_range / 2 + 1, opt);

                if (g->feature_weights[weight_i] != previous_value ) {
                    was_mutated = 1;
                }
            }

            weight_i++;
        }
    }

    return was_mutated;
}