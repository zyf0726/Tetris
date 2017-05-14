#include "shared.h"

#include "genotype.h"
#include "board.h"
struct phenotype {
    int fitness;
    int has_fitness;
    genotype* genotype;
};

phenotype* copy_phenotype (phenotype* phenotype, options* opt);
phenotype* initialize_phenotype (genotype* g);
void free_phenotype (phenotype * phenotype);
int compare_phenotypes (const void * p_1, const void * p_2);
void write_phenotype (FILE * stream, phenotype * phenotype, options * opt);
float board_score (board * new_board, board * old_board, phenotype* phenotype, t_last_placement * tlp, options * opt);
int phenotype_fitness (phenotype* phenotype, options* opt);
int average_phenotype_fitness (phenotype* pt, options* opt);
int continue_board(board * board, phenotype* phenotype, int next_tetrominos[], options* opt);

