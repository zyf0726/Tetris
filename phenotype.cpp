#ifndef SINGLEFILE
#include "shared.h"

#include "board.h"
#include "feature_functions.h"
#include "feature_helpers.h"
#include "genotype.h"
#include "options.h"
#include "phenotype.h"
#endif


phenotype *copy_phenotype(phenotype *ori, options *opt)
{
    phenotype *Copy = (phenotype *) malloc(sizeof(phenotype));

    *Copy = *ori;
    Copy->gen = copy_genotype(ori->gen, opt);

    return Copy;
}

phenotype *initialize_phenotype(genotype *g)
{
    phenotype *p = (phenotype *) malloc(sizeof(phenotype));

    p->fitness = 0;
    p->gen = g;

    return p;
}

void free_phenotype(phenotype *phenotype)
{
    if (phenotype != NULL)
    {
        free_genotype(phenotype->gen);
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
            max_feature_length = (int)strlen(features[opt->enabled_f_indices[i]].name);
        }
    }

    int weight_i = 0;

    for (int i = 0; i < opt->n_features_enabled; i++)
    {
            for (int a = 0; a < features[opt->enabled_f_indices[i]].weights; a++)
            {
                fprintf(stream, "%-*s % .2f\n",
                        max_feature_length,
                        features[opt->enabled_f_indices[i]].name,
                        phenotype->gen->feature_weights[weight_i++]);
        }
    }
}

template<bool enable_dynamic, bool enable_static>
float board_score(board *new_board, board *old_board, phenotype *phenotype, t_last_placement *tlp, options *opt)
{
    float score = 0;
    int weight_i = 0;
    reset_feature_caches(opt);
    for (int i = 0; i < opt->n_features_enabled; i++)
        {
            const feature& pt = features[opt->enabled_f_indices[i]];
            if ((pt.dynamic && enable_dynamic)
                || (!pt.dynamic && enable_static))
                for (int a = 0; a < pt.weights; a++)
                    score += phenotype->gen->feature_weights[weight_i++] * call_feature(opt->enabled_f_indices[i], new_board, old_board, tlp);
        }
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
void output_state(ctet te, int __state[BOARD_HEIGHT + 5][BOARD_WIDTH + 4])
{
#define ST(y, x) (__state[(y) + 4][(x) + 4])
    int begin_x = -te.p_left, end_x = te.p_right + BOARD_WIDTH - 3;
    for (int yy = -te.p_bottom; yy < BOARD_HEIGHT - 3 + te.p_bottom; ++yy, puts(""))
        for (int xx = begin_x; xx < end_x; ++xx)
            putchar(ST(yy, xx) + '0');
}


alt_c_t _look_ahead(board *brd, phenotype *phenotype,  int Ty1, options* opt)
{
    alt_c_t f;
    int tet_offset_all, tet_base;
    N_TETROMINO(&tet_base, Ty1);
    N_ROTATIONS(&tet_offset_all, tet_base);
    int n_boards = 0;

    for (int tet_offset = 0; tet_offset < tet_offset_all; tet_offset++)
    {
        const tetromino &te = tetrominos[tet_base + tet_offset];
        //n_boards += BOARD_WIDTH - 4 + 1 + te.p_left + te.p_right;
        int __state[BOARD_HEIGHT + 5][BOARD_WIDTH + 4];
        memset(__state, 0, sizeof(__state));


        int begin_x = -te.p_left, end_x = te.p_right + BOARD_WIDTH - 3;
        for (int i = begin_x; i <  end_x; ++i) ST(te.p_bottom - 4, i) = true;
        for (int yy = te.p_bottom - 4; yy < BOARD_HEIGHT - 3 + te.p_bottom; ++yy)
        {
            //i->i + 1
            for (int xx = begin_x; xx < end_x; ++xx)
                if (ST(yy, xx))
                {
                    if (!brd->valid_pos_except_top(te, xx, yy + 1))
                    {
                        //if i >=足够使进入棋局的位置, then成为可行决策
                        if (yy >= -te.p_top)
                        {
                            ORI o;
                            int x, y;
                            tie(o, x, y) = TXY2toxy(Ty1, tet_offset, __cord{xx, yy});
                            alternative alt = {o, x, y};
                            t_last_placement tlp = {&te, .x = xx, .y = yy,};
                            board cp(*brd);
                            cp.place(te, xx, yy);
                            cp.remove_lines(&tlp);
                            alt.static_score = board_score<false, true>(&cp, brd, phenotype, &tlp, opt);
                            alt.dynamic_score = board_score<true, false>(&cp, brd, phenotype, &tlp, opt);
                            alt.b = cp;
                            free(tlp.lines_removed);
                            f.push_back(alt);
                        }
                    } else
                    {
                        if (!ST(yy + 1, xx))
                        {
                            ST(yy + 1, xx) = true;
                            for (int k = -1; k + xx >= begin_x; --k)
                                if (!ST(yy + 1, xx + k) && brd->valid_pos_except_top(te, xx + k, yy + 1)) ST(yy + 1, xx + k) = true;
                                else break;
                            for (int k = 1; k + xx < end_x; ++k)
                                if (!ST(yy + 1, xx + k) && brd->valid_pos_except_top(te, xx + k, yy + 1)) ST(yy + 1, xx + k) = true;
                                else break;

                        }
                    }
                }
        }
#undef ST
    }
    return f;
}

