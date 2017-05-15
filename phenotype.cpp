#include "shared.h"

#include "board.h"
#include "feature_functions.h"
#include "feature_helpers.h"
#include "genotype.h"
#include "options.h"
#include "phenotype.h"
#include "random.h"
#include "tetromino.h"


phenotype *copy_phenotype(phenotype *ori, options *opt)
{
    phenotype *Copy = (phenotype *) malloc(sizeof(phenotype));

    *Copy = *ori;
    Copy->genotype = copy_genotype(ori->genotype, opt);

    return Copy;
}

phenotype *initialize_phenotype(genotype *g)
{
    phenotype *p = (phenotype *) malloc(sizeof(phenotype));

    p->fitness = 0;
    p->has_fitness = 0;
    p->genotype = g;

    return p;
}

void free_phenotype(phenotype *phenotype)
{
    if (phenotype != NULL)
    {
        free_genotype(phenotype->genotype);
        free(phenotype);
    }
}

int compare_phenotypes(const void *p_1, const void *p_2)
{
    phenotype *phenotype_1 = *(phenotype **) p_1;
    phenotype *phenotype_2 = *(phenotype **) p_2;

    return (phenotype_1->fitness > phenotype_2->fitness) - (phenotype_1->fitness < phenotype_2->fitness);
}

void write_phenotype(FILE *stream, phenotype *phenotype, options *opt)
{
    int max_feature_length = 0;

    for (int i = 0; i < opt->n_features_enabled; i++)
    {
        if (strlen(features[opt->enabled_f_indices[i]].name) > max_feature_length)
        {
            max_feature_length = strlen(features[opt->enabled_f_indices[i]].name);
        }
    }

    int weight_i = 0;

    for (int i = 0; i < opt->n_features_enabled; i++)
    {
        if (phenotype->genotype->feature_enabled[i])
        {
            for (int a = 0; a < features[opt->enabled_f_indices[i]].weights; a++)
            {
                fprintf(stream, "%-*s % .2f\n",
                        max_feature_length,
                        features[opt->enabled_f_indices[i]].name,
                        phenotype->genotype->feature_weights[weight_i++]);
            }
        }
    }
}

float board_score(board *new_board, board *old_board, phenotype *phenotype, t_last_placement *tlp, options *opt)
{
    float score = 0;
    int weight_i = 0;
    reset_feature_caches(opt);
    for (int i = 0; i < opt->n_features_enabled; i++)
        if (phenotype->genotype->feature_enabled[i])
            for (int a = 0; a < features[opt->enabled_f_indices[i]].weights; a++)
                score += phenotype->genotype->feature_weights[weight_i++] *
                         call_feature(opt->enabled_f_indices[i], new_board, old_board, tlp);
    return score;
}

/*
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
        fitness += brd.remove_lines( NULL);

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
float average_phenotype_fitness (phenotype * pt, options* opt) {
    float sum = 0;

    for (int i = 0; i < opt->n_trials; i++) {
        sum += phenotype_fitness(pt, opt);
    }

    return sum / opt->n_trials;
}
*/



inline vector<alternative> _look_ahead(board *brd, phenotype *phenotype,  int next_tetromino, options* opt)
{
    vector<alternative> f;
    int n_rotations;
    N_ROTATIONS(&n_rotations, next_tetromino);
    int n_boards = 0;

    for (int rotation_i = 0; rotation_i < n_rotations; rotation_i++)
    {
        const tetromino &te = tetrominos[next_tetromino + rotation_i];
        //n_boards += BOARD_WIDTH - 4 + 1 + te.p_left + te.p_right;
        bool __state[BOARD_HEIGHT + 5][BOARD_WIDTH + 4]{};
#define ST(x, y) __state[(x) + 4][(y) + 4]


        int bg = -te.p_left, ed = te.p_right + BOARD_HEIGHT - 3;
        for (int i = bg; i <  ed; ++i) ST(-te.p_bottom, i) = true;
        for (int i = -te.p_bottom; i < BOARD_HEIGHT - 3 + te.p_bottom; ++i) // TODO 要特判到底的情况
        {
            //i->i + 1
            for (int j = bg; j < ed; ++j) if (ST(i, j))
                if (!brd->valid_pos(te, i + 1, j))
                {
                    //if i >=足够使进入棋局的位置, then成为可行决策
                    if (i >= -te.p_top)
                    {
                        alternative alt =  {  i,  rotation_i, j };
                        t_last_placement tlp = {  &te, .x = i, .y = j, };
                        board cp(*brd); cp.place(te, i, j); cp.remove_lines(&tlp);
                        alt.score = board_score(&cp, brd, phenotype, &tlp, opt);
                        free(tlp.lines_removed);
                        f.push_back(alt);
                    }
                } else
                {
                    for (int k = -1; k + j >= bg; --k)
                        if (!ST(i + 1, j + k) && brd->valid_pos(te, i + 1, j + k)) ST(i + 1, j + k) = true;
                        else break;
                    for (int k = 1; k + j < ed; ++k)
                        if (!ST(i + 1, j + k) && brd->valid_pos(te, i + 1, j + k)) ST(i + 1, j + k) = true;
                        else break;
                }
        }
#undef ST
    }
    return f;
}



alternative continue_board(board *board, phenotype *phenotype, int next_tetromino, options *opt)
{
    auto f = _look_ahead(board, phenotype,  next_tetromino, opt);
    if (f.size() == 0) throw 1;
    else {
        alternative& max_alt = *max_element(f.begin(), f.end());
        board->directly_drop( &tetrominos[next_tetromino + max_alt.rotation_i], max_alt.position_i, NULL);
        return max_alt;
    }
}