#pragma once
#ifndef SINGLEFILE
#include "shared.h"
#include "options.h"
#endif
struct genotype {
    float * feature_weights;
    void write(FILE* f, options* opt);
};
void free_genotype ( genotype* g);
genotype* initialize_genotype (options* opt);
genotype* copy_genotype (genotype* gt, options* opt);
void randomize_genotype (genotype* g, options* opt);
genotype* crossover_genotypes (genotype* g_1, genotype* g_2, options* opt);
int mutate_genotype (struct genotype* g, struct options* opt);

