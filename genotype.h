#pragma once
#ifndef SINGLEFILE
#include "shared.h"
#include "options.h"
#endif
struct genotype {
    float  feature_weights[N_FE];
    void write(FILE* f, const options* opt);
};
void free_genotype ( genotype* g);
genotype* initialize_genotype (const options* opt);
genotype* copy_genotype (genotype* gt, const options* opt);
void randomize_genotype (genotype* g, const options* opt);
void mutate_genotype (struct genotype* g,  const options* opt);
void crossover_genotypes (const genotype* g_1, const genotype* g_2, genotype* g, const  options* opt);

