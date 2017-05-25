#ifndef POPULATION_H
#define POPULATION_H
#ifndef SINGLEFILE
#include "shared.h"
#include "options.h"
#endif
//#include "structs.h"

struct population {
    int size;
    struct phenotype** in;
    phenotype** begin() const { return in; }
    phenotype** end() const { return in + size; }
};

struct pop : public vector<phenotype>
{

};

struct population* initialize_population_pool (int size);
void free_population_pool (struct population* p);
void swap_populations (struct population** population_1, struct population** population_2);
struct population* sample_population (struct population* p, int n, const options* opt);
struct phenotype* select_best_individual (struct population* p);
struct phenotype* mate_individuals (struct phenotype* phenotype_1, struct phenotype* phenotype_2, const options* opt);

#endif
