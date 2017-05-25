#pragma once
#ifndef SINGLEFILE
#include "phenotype.h"
#endif
struct phenotype** select_parent_pairs (struct population* ppl, const options* opt);
struct phenotype* select_by_tournament (struct population* ppl, const options* opt);
struct phenotype** select_with_cumulation (struct population* population, int n, float cumulation[], const options* opt);
struct phenotype** select_by_sus (struct population* population, int n, const options* opt);
struct phenotype** select_by_sigma_scaling (struct population* population, int n, const options* opt);

