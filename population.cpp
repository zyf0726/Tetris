#include <stdlib.h>
#include <string.h>

#include "genotype.h"
#include "options.h"
#include "phenotype.h"
#include "population.h"
#include "random.h"

population* initialize_population_pool (int size) {
    population* p = (population*)malloc(sizeof(population));

    p->size = size;
    p->individuals = (phenotype**)malloc(sizeof(phenotype*) * size);

    return p;
}

void free_population_pool (population* p) {
    free(p->individuals);
    free(p);
}

void swap_populations (population** population_1, population** population_2) {
    population* temp_pool = *population_1;

    (*population_1) = *population_2;
    (*population_2) = temp_pool;
}

population* sample_population (population* p, int n, struct options* opt) {
    if (p->size < n) {
        n = p->size;
    }

    phenotype* population_copy[p->size];
    population* sample = initialize_population_pool(n);

    memcpy(population_copy, p->individuals, sizeof(phenotype*) * p->size);

    int r;

    for (int i = 0; i < n; i++) {
        r = i + l_rand(0, p->size - i, opt);

        sample->individuals[i] = population_copy[r];
        population_copy[r] = population_copy[i];
    }

    return sample;
}

phenotype* select_best_individual (population* p) {
    int max_fitness = -1;
    phenotype* max_individual;

    for (int i = 0; i < p->size; i++) {
        if (p->individuals[i]->fitness > max_fitness) {
            max_fitness = p->individuals[i]->fitness;
            max_individual = p->individuals[i];
        }
    }

    return max_individual;
}

phenotype* mate_individuals (phenotype* phenotype_1, phenotype* phenotype_2, struct options* opt) {
    phenotype* offspring;

    if (f_rand(opt) < opt->crossover_rate) {
        offspring = initialize_phenotype(crossover_genotypes(phenotype_1->gen, phenotype_2->gen, opt));
    } else {
        offspring = copy_phenotype(b_rand(opt) ? phenotype_1 : phenotype_2, opt);
    }

    if (mutate_genotype(offspring->gen, opt)) {
    }

    return offspring;
}