#include "shared.h"

#include "board.h"
#include "feature_functions.h"
#include "feature_helpers.h"
#include "genotype.h"
#include "options.h"
#include "phenotype.h"
#include "random.h"
#include "tetromino.h"


phenotype* copy_phenotype (phenotype* ori, options* opt)
{
    phenotype* Copy = (phenotype*)malloc(sizeof(phenotype));

    *Copy = *ori;
    Copy->genotype = copy_genotype(ori->genotype, opt);

    return Copy;
}

phenotype* initialize_phenotype (genotype* g) {
    phenotype* p = (phenotype*)malloc(sizeof(phenotype));

    p->fitness = 0;
    p->has_fitness = 0;
    p->genotype = g;

    return p;
}

void free_phenotype (phenotype * phenotype) {
    if (phenotype != NULL) {
        free_genotype(phenotype->genotype);
        free(phenotype);
    }
}

int compare_phenotypes (const void * p_1, const void * p_2) {
    phenotype* phenotype_1 = * (phenotype**) p_1;
    phenotype* phenotype_2 = * (phenotype**) p_2;

    return (phenotype_1->fitness > phenotype_2->fitness) - (phenotype_1->fitness < phenotype_2->fitness);
}

void write_phenotype (FILE * stream, phenotype * phenotype, options * opt) {
    int max_feature_length = 0;

    for (int i = 0; i < opt->n_features_enabled; i++) {
        if (strlen(features[opt->enabled_f_indices[i]].name) > max_feature_length) {
            max_feature_length = strlen(features[opt->enabled_f_indices[i]].name);
        }
    }

    int weight_i = 0;

    for (int i = 0; i < opt->n_features_enabled; i++) {
        if (phenotype->genotype->feature_enabled[i]) {
            for (int a = 0; a < features[opt->enabled_f_indices[i]].weights; a++) {
                fprintf(stream, "%-*s % .2f\n",
                    max_feature_length,
                    features[opt->enabled_f_indices[i]].name,
                    phenotype->genotype->feature_weights[weight_i++]);
            }
        }
    }
}

float board_score (board * new_board, board * old_board, phenotype * phenotype, t_last_placement * tlp, options * opt) {
    float score = 0;

    int weight_i = 0;

    reset_feature_caches(opt);

    for (int i = 0; i < opt->n_features_enabled; i++) {
        if (phenotype->genotype->feature_enabled[i]) {
            for (int a = 0; a < features[opt->enabled_f_indices[i]].weights; a++) {
                score += phenotype->genotype->feature_weights[weight_i++] * call_feature(opt->enabled_f_indices[i], new_board, old_board, tlp);
            }
        }
    }

    return score;
}

int phenotype_fitness (phenotype * phenotype, options* opt) {
    int fitness = 0;

    board brd;

    int next_tetrominos[opt->n_piece_lookahead + 1];

    for (int i = 0; i < opt->n_piece_lookahead + 1; i++) {
        int random_t = l_rand(0, 7, opt);

        N_TETROMINO(&next_tetrominos[i], random_t);
    }

    while (1) {
        // Place the next tetromino on the board. If the placement was unsuccessful, exit the loop.
        if (continue_board(&brd, phenotype, next_tetrominos, opt) == 1) {
            break;
        }

        // Remove lines and add to the current fitness value.
        fitness += remove_lines(&brd, NULL);

        // Fill the lookahead with a new tetromino.
        for (int i = 0; i < opt->n_piece_lookahead; i++) {
            next_tetrominos[i] = next_tetrominos[i + 1];
        }

        int random_t = l_rand(0, 7, opt);

        N_TETROMINO(&next_tetrominos[opt->n_piece_lookahead], random_t);

        if (opt->print_board) {
            print_board(stdout, &brd);
        }
    }

    return fitness;
}

int average_phenotype_fitness (phenotype * pt, options* opt) {
    float sum = 0;

    for (int i = 0; i < opt->n_trials; i++) {
        sum += phenotype_fitness(pt, opt);
    }

    return sum / opt->n_trials;
}

struct alternative {
    int position_i;
    int rotation_i;
    float score;
};

struct future {
    int size;
    alternative * alternatives;
};

void expand_future(future * f, alternative * alt) {
    if (f->size == 0) {
        f->alternatives = (alternative*)malloc(sizeof(alternative));
    } else {
        f->alternatives = (alternative*)realloc(f->alternatives, sizeof(alternative) * (f->size + 1));
    }

    f->alternatives[f->size++] = *alt;
}

void _look_ahead(future * f, board * brd, phenotype * phenotype, int n_ahead, alternative * alt, int next_tetrominos[], options* opt) {
    // See how many rotations exists of the next tetromino.
    int n_rotations;

    N_ROTATIONS(&n_rotations, next_tetrominos[n_ahead]);

    // Calculate the number of boards that need to be checked for fitness.
    int n_boards = 0;

    for (int rotation_i = 0; rotation_i < n_rotations; rotation_i++) {
        tetromino tetromino = tetrominos[next_tetrominos[n_ahead] + rotation_i];
        //printf("p left is %d..\n", tetromino.p_left);
        n_boards += BOARD_WIDTH - 4 + 1 + tetromino.p_left + tetromino.p_right;
    }

    // Initialize N boards in order to perform the trials.
    board * boards = (board*)malloc(sizeof(board) * n_boards);

    for (int i = 0; i < n_boards; i++) {
        boards[i] = copy_board(brd);
    }

    // Place the tetromino in all possible ways on the boards.
    int board_i = 0;

    t_last_placement tlp[n_boards];

    for (int rotation_i = 0; rotation_i < n_rotations; rotation_i++) {
        tetromino tetromino = tetrominos[next_tetrominos[n_ahead] + rotation_i];

        int positions = BOARD_WIDTH - 4 + 1 + tetromino.p_left + tetromino.p_right;

        for (int position_i = -tetromino.p_left; position_i < positions - tetromino.p_left; position_i++) {
            int y;

            if (place_tetromino(&boards[board_i], &tetromino, position_i, &y) == 0) {
                if (n_ahead == 0) {
                    alt = & (alternative) {
                        .position_i = position_i,
                        .rotation_i = rotation_i,
                    };
                }

                if (n_ahead == opt->n_piece_lookahead) {
                    t_last_placement tlp = {
                        .tetromino = &tetromino,
                        .x = position_i,
                        .y = y,
                    };

                    remove_lines(&boards[board_i], &tlp);

                    alt->score = board_score(&boards[board_i], brd, phenotype, &tlp, opt);

                    free(tlp.lines_removed);

                    expand_future(f, alt);
                } else {
                    remove_lines(&boards[board_i], NULL);

                    _look_ahead(f, &boards[board_i], phenotype, n_ahead + 1, alt, next_tetrominos, opt);
                }

                board_i++;
            }
        }
    }

    for (int i = 0; i < n_boards; i++) {
        free_board(&boards[i]);
    }

    free(boards);
}

void look_ahead(future * f, board * board, phenotype* phenotype, int next_tetrominos[], options* opt) {
    _look_ahead(f, board, phenotype, 0, NULL, next_tetrominos, opt);
}

int continue_board(board * board, phenotype* phenotype, int next_tetrominos[], options* opt) {
    future f = {
        .size = 0,
    };

    look_ahead(&f, board, phenotype, next_tetrominos, opt);

    if (f.size == 0) {
        free(f.alternatives);
        return 1;
    } else {
        alternative max_alt = f.alternatives[0];

        for (int i = 1; i < f.size; i++) {
            if (f.alternatives[i].score > max_alt.score) {
                max_alt = f.alternatives[i];
            }
        }

        free(f.alternatives);

        place_tetromino(
            board,
            &tetrominos[next_tetrominos[0] + max_alt.rotation_i],
            max_alt.position_i, NULL);

        return 0;
    }
}