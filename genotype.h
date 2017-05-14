#pragma once
struct genotype {
    float * feature_weights;
    int * feature_enabled;
};
void free_genotype (struct genotype* g);
struct genotype* initialize_genotype (struct options* opt);
struct genotype* copy_genotype (struct genotype* gt, struct options* opt);
void randomize_genotype (struct genotype* g, struct options* opt);
struct genotype* crossover_genotypes (struct genotype* g_1, struct genotype* g_2, struct options* opt);
int mutate_genotype (struct genotype* g, struct options* opt);

