#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "population.h"
#include "random.h"
#include "selection.h"
#include "options.h"

phenotype** select_parent_pairs (population* ppl, options* opt) {
    phenotype** pairs = (phenotype**)malloc(sizeof(phenotype*) * ppl->size * 2);

    if (opt->sel == TOURNAMENT) {
        for (int i = 0; i < ppl->size; i++) {
            pairs[i * 2 + 0] = select_by_tournament(ppl, opt);
            pairs[i * 2 + 1] = select_by_tournament(ppl, opt);
        }
    } else if (opt->sel == SUS) {
        pairs = select_by_sus(ppl, ppl->size * 2, opt);
    } else {
        pairs = select_by_sigma_scaling(ppl, ppl->size * 2, opt);
    }

    return pairs;
}

phenotype* select_by_tournament (population* ppl, options* opt) {
    population* tournament_group = sample_population(ppl, opt->tournament_group_size, opt);
    phenotype* selected_phenotype;

    if (f_rand(opt) > opt->tournament_group_random_selection) {
        selected_phenotype = select_best_individual(tournament_group);
    } else {
        selected_phenotype = tournament_group->individuals[0];
    }

    free_population_pool(tournament_group);

    return selected_phenotype;
}

phenotype** select_with_cumulation (population* population, int n, float cumulation[], options* opt) {
    phenotype** selection = (phenotype**)malloc(sizeof(phenotype*) * n);

    float sum      = cumulation[population->size - 1];
    float distance = (float) sum / (float) n;
    float start    = (float) (f_rand(opt) * sum) / (float) n;

    for (int i = 0, a = 0; i < n; i++) {
        while (cumulation[a] < start + i * distance) {
            a++;
        }

        selection[i] = population->individuals[a];
    }

    return selection;
}

phenotype** select_by_sus (population* population, int n, options* opt) {
    float cumulation[population->size];

    for (int i = 0; i < population->size; i++) {
        if (i == 0) {
            cumulation[i] = population->individuals[i]->fitness;
        } else {
            cumulation[i] = cumulation[i - 1] + population->individuals[i]->fitness;
        }
    }

    return select_with_cumulation(population, n, cumulation, opt);
}

phenotype** select_by_sigma_scaling (population* population, int n, options* opt) {
    float average = 0, std_deviation, square_deviations = 0;

    for (int i = 0; i < population->size; i++) {
        average += population->individuals[i]->fitness;
    }

    average /= population->size;

    for (int i = 0; i < population->size; i++) {
        square_deviations = powf(population->individuals[i]->fitness - average, 2);
    }

    std_deviation = sqrt(square_deviations / population->size);

    float cumulation[population->size];

    if (std_deviation == 0) {
        for (int i = 0; i < population->size; i++) {
            cumulation[i] = 1;
        }
    } else {
        for (int i = 0; i < population->size; i++) {
            if (i == 0) {
                cumulation[i] = 1 + (population->individuals[i]->fitness - average) / (2 * std_deviation);
            } else {
                cumulation[i] = cumulation[i - 1] + 1 + (population->individuals[i]->fitness - average) / (2 * std_deviation);
            }
        }
    }

    return select_with_cumulation(population, n, cumulation, opt);
}