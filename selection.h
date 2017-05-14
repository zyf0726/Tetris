#pragma once
#include "phenotype.h"
struct phenotype** select_parent_pairs (struct population* ppl, struct options* opt);
struct phenotype* select_by_tournament (struct population* ppl, struct options* opt);
struct phenotype** select_with_cumulation (struct population* population, int n, float cumulation[], struct options* opt);
struct phenotype** select_by_sus (struct population* population, int n, struct options* opt);
struct phenotype** select_by_sigma_scaling (struct population* population, int n, struct options* opt);

