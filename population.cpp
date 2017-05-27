#ifndef SINGLEFILE

#include "genotype.h"
#include "options.h"
#include "phenotype.h"
#include "population.h"
#include "random.h"

#endif

population *initialize_population_pool(int size)
{
    population *p = (population *) malloc(sizeof(population));

    p->size = size;
    p->in = (phenotype **) malloc(sizeof(phenotype *) * size);

    return p;
}

void free_population_pool(population *p)
{
    free(p->in);
    free(p);
}


void swap_populations(population **population_1, population **population_2)
{
    population *temp_pool = *population_1;

    (*population_1) = *population_2;
    (*population_2) = temp_pool;
}

population *sample_population(population *p, int n, const options *opt)
{
    if (p->size < n)
    {
        n = p->size;
    }

    phenotype *population_copy[p->size];
    population *sample = initialize_population_pool(n);

    memcpy(population_copy, p->in, sizeof(phenotype *) * p->size);

    int r;

    for (int i = 0; i < n; i++)
    {
        r = i + l_rand(0, p->size - i);

        sample->in[i] = population_copy[r];
        population_copy[r] = population_copy[i];
    }

    return sample;
}

phenotype *select_best_individual(population *p)
{
    int max_fitness = -1;
    phenotype *max_individual;

    for (int i = 0; i < p->size; i++)
    {
        if (p->in[i]->fitness > max_fitness)
        {
            max_fitness = p->in[i]->fitness;
            max_individual = p->in[i];
        }
    }

    return max_individual;
}

phenotype *mate_individuals(phenotype *phenotype_1, phenotype *phenotype_2, const options *opt)
{
    phenotype *offspring = initialize_phenotype();
    if (f_rand() < opt->crossover_rate)
        crossover_genotypes(&phenotype_1->gen, &phenotype_2->gen, &offspring->gen, opt);
    else offspring->gen = b_rand() ? phenotype_1->gen : phenotype_2->gen;

    mutate_genotype(&offspring->gen, opt);

    return offspring;
}
