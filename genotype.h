#pragma once
#ifndef SINGLEFILE
#include "shared.h"
#include "options.h"
#endif
struct genotype {
    float * feature_weights;
    void write(FILE* f, const options* opt);
};
void free_genotype ( genotype* g);
genotype* initialize_genotype (const options* opt);
genotype* copy_genotype (genotype* gt, const options* opt);
void randomize_genotype (genotype* g, const options* opt);
genotype* crossover_genotypes (genotype* g_1, genotype* g_2, const options* opt);
int mutate_genotype (struct genotype* g,  const options* opt);

