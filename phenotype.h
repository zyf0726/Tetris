#pragma once
#ifndef SINGLEFILE
#include "shared.h"
#include "genotype.h"
#include "board.h"
#endif
struct phenotype {
    int fitness;
    genotype* gen;
};
struct alternative
{
    int o, x, y;
    float static_score, dynamic_score;
    board b;
    bool operator<(const alternative& other) const { return static_score > other.static_score; }
};

phenotype* copy_phenotype (phenotype* phenotype, const options* opt);
phenotype* initialize_phenotype (genotype* g);
void free_phenotype (phenotype * phenotype);
int compare_phenotypes (const void * p_1, const void * p_2);
void write_phenotype (FILE * stream, phenotype * phenotype, const options * opt);
template<bool enable_dynamic = false, bool enable_static = true> float board_score (board * new_board, board * old_board, phenotype* phenotype, t_last_placement * tlp, options * opt);
using  alt_c_t = insert_sort_container<alternative, MAX_SOLUTIONS>;
alt_c_t  _look_ahead(board *brd, phenotype *phenotype,  int Ty1, const options* opt);
phenotype* init_from_weight(const float* fwt, const options& opt);
