#pragma once
#ifndef SINGLEFILE
#include "shared.h"
#include "genotype.h"
#include "board.h"
#endif
struct phenotype {
    int fitness;
    genotype gen;
};
struct alternative
{
    int o, x, y;
    float static_score, dynamic_score;
    board b;
    bool operator<(const alternative& other) const { return static_score > other.static_score; }
};

phenotype* copy_phenotype (phenotype* phenotype, const options* opt);
phenotype* initialize_phenotype ();
void free_phenotype (phenotype * phenotype);
using  alt_c_t = insert_sort_container<alternative, MAX_SOLUTIONS>;
alt_c_t  _look_ahead(board *brd, phenotype *phenotype,  int Ty1, const options* opt);
phenotype* init_from_weight(const float* fwt, const options& opt);
