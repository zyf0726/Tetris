
#ifndef SINGLEFILE
#include <stdlib.h>
#include <string.h>

#include "genotype.h"
#include "options.h"
#include "random.h"
#include "feature_functions.h"
#include "feature_helpers.h"
#include "shared.h"
#endif

void free_genotype (genotype* g) {
    free(g->feature_weights);
    free(g);
}

genotype* initialize_genotype (const options* opt) {
    genotype* g = (genotype*)malloc(sizeof(genotype));

    g->feature_weights = (float*)malloc(sizeof(float) * opt->n_weights_enabled);
    return g;
}

genotype* copy_genotype (genotype* gt, const options* opt) {
    genotype* copy = initialize_genotype(opt);

    memcpy(copy->feature_weights, gt->feature_weights, sizeof(float) * opt->n_weights_enabled);

    return copy;
}

void randomize_genotype (genotype* g, const options* opt) {
    for (int i = 0; i < opt->n_weights_enabled; i++) {
        g->feature_weights[i] = glb_fwt[i] + l_rand(- opt->randomization_range / 20 - 1, opt->randomization_range / 20 + 1);
    }

}



genotype* crossover_genotypes (genotype* g_1, genotype* g_2, const  options* opt) {
    genotype* g = initialize_genotype(opt);

    if (opt->crossover_points == opt->n_features_enabled) {
        // Uniform crossover


        for (int i = 0; i < opt->n_features_enabled; i++) {
            genotype * selecting_from_genotype;

            if (b_rand()) {
                selecting_from_genotype = g_1;
            } else {
                selecting_from_genotype = g_2;
            }


                g->feature_weights[i] = selecting_from_genotype->feature_weights[i];
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
            r = i + l_rand(0, opt->n_features_enabled - 1 - i);

            selected_points[i] = possible_points[r];
            possible_points[r] = possible_points[i];
        }

        sort(selected_points, selected_points+ opt->crossover_points);

        int selecting_from = b_rand(),
            at_point = 0;

        for (int i = 0; i < opt->n_features_enabled; i++) {
            genotype * selecting_from_genotype;

            if (selecting_from) {
                selecting_from_genotype = g_1;
            } else {
                selecting_from_genotype = g_2;
            }
            g->feature_weights[i] = selecting_from_genotype->feature_weights[i];

            if (at_point < opt->crossover_points && i == selected_points[at_point]) {
                at_point++;
                selecting_from = !selecting_from;
            }
        }
    }

    return g;
}

void mutate_genotype (genotype* g, const options* opt)
{
    normal_distribution<double> nd(0, opt->mutation_stdev);
    for (int i = 0; i < opt->n_features_enabled; i++)
            g->feature_weights[i] += nd(RAND);
}
void genotype::write(FILE* f, const options* opt)
{
    for (int i = 0; i < opt->n_features_enabled; ++i)
        fprintf(f, "%.5f ", feature_weights[i]);
    fprintf(f, "\n");
}
