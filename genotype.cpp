
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
    free(g);
}

genotype* initialize_genotype (const options* opt) {
    genotype* g = (genotype*)malloc(sizeof(genotype));
    return g;
}

genotype* copy_genotype (genotype* gt, const options* opt) {
    genotype* copy = initialize_genotype(opt);

    memcpy(copy->feature_weights, gt->feature_weights, sizeof(float) * N_FE);

    return copy;
}

void randomize_genotype (genotype* g, const options* opt) {
    for (int i = 0; i < N_FE; i++) {
        g->feature_weights[i] = glb_fwt[i] + l_rand(- opt->randomization_range / 20 - 1, opt->randomization_range / 20 + 1);
    }

}



void crossover_genotypes (const genotype* g_1, const genotype* g_2, genotype* g, const  options* opt) {

    if (opt->crossover_points == N_FE) {
        // Uniform crossover


        for (int i = 0; i < N_FE; i++) {
            const genotype * selecting_from_genotype;

            if (b_rand()) {
                selecting_from_genotype = g_1;
            } else {
                selecting_from_genotype = g_2;
            }


                g->feature_weights[i] = selecting_from_genotype->feature_weights[i];
        }
    } else {
        // Generate N crossover points and proceed with the crossover

        int possible_points[N_FE - 1];
        int selected_points[opt->crossover_points];

        for (int i = 0; i < N_FE - 1; i++) {
            possible_points[i] = i;
        }

        int r;

        for (int i = 0; i < opt->crossover_points; i++) {
            r = i + l_rand(0, N_FE - 1 - i);

            selected_points[i] = possible_points[r];
            possible_points[r] = possible_points[i];
        }

        sort(selected_points, selected_points+ opt->crossover_points);

        int selecting_from = b_rand(),
            at_point = 0;

        for (int i = 0; i < N_FE; i++) {
            const genotype * selecting_from_genotype;

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

}

void mutate_genotype (genotype* g, const options* opt)
{
    for (int i = 0; i < N_FE; i++)
        g->feature_weights[i] += (normal_distribution<double>(0, opt->mutation_stdev * fabs(g->feature_weights[i])))(RAND);
}
void genotype::write(FILE* f, const options* opt)
{
    for (int i = 0; i < N_FE; ++i)
        fprintf(f, "%.5f ", feature_weights[i]);
    fprintf(f, "\n");
}
