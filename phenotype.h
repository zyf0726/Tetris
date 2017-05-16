#pragma once
#ifndef SINGLEFILE
#include "shared.h"
#include "genotype.h"
#include "board.h"
#endif
struct phenotype {
    int fitness;
    int has_fitness;
    genotype* gen;
};
struct alternative
{
    int o, x, y;
    float score;
    board b;
    bool operator<(const alternative& other) const { return score < other.score; }
};

phenotype* copy_phenotype (phenotype* phenotype, options* opt);
phenotype* initialize_phenotype (genotype* g);
void free_phenotype (phenotype * phenotype);
int compare_phenotypes (const void * p_1, const void * p_2);
void write_phenotype (FILE * stream, phenotype * phenotype, options * opt);
float board_score (board * new_board, board * old_board, phenotype* phenotype, t_last_placement * tlp, options * opt);
vector<alternative> _look_ahead(board *brd, phenotype *phenotype,  int Ty1, options* opt);
