#ifndef SINGLEFILE
#include "shared.h"

#include "board.h"
#include "feature_functions.h"
#include "feature_helpers.h"
#include "genotype.h"
#include "options.h"
#include "phenotype.h"
#endif


phenotype *copy_phenotype(phenotype *ori, const options *opt)
{
    phenotype *Copy = (phenotype *) malloc(sizeof(phenotype));

    *Copy = *ori;

    return Copy;
}

phenotype *initialize_phenotype()
{
    phenotype *p = (phenotype *) malloc(sizeof(phenotype));
    p->fitness = 0;
    return p;
}
phenotype* init_from_weight(const float* fwt, const options& opt)
{
    phenotype *p = initialize_phenotype();
    copy(fwt, fwt + N_FE, p->gen.feature_weights);
    return p;
}

void free_phenotype(phenotype *phenotype)
{
    if (phenotype != NULL)
    {
        free(phenotype);
    }
}


inline float score_dyn(board *new_board, board *old_board, phenotype *phenotype, t_last_placement *tlp)
{
#define FN(f, i) f(new_board,old_board,tlp) * phenotype->gen.feature_weights[i]
    return FN(f_landing_height, 1) + FN(f_eroded_piece_cells, 2);
};
inline float score_sta(board *new_board, board *old_board, phenotype *phenotype, t_last_placement *tlp)
{
    return FN(f_n_holes, 0) +  FN(f_row_transitions, 3) + FN(f_column_transitions, 4)
           + FN(f_cumulative_wells_fast, 5) + FN(f_hole_depths, 6) + FN(f_n_rows_with_holes, 7);
#undef FN
}

void output_state(ctet te, int __state[BOARD_HEIGHT + 5][BOARD_WIDTH + 4])
{
#define ST(y, x) (__state[(y) + 4][(x) + 4])
    int begin_x = -te.p_left, end_x = te.p_right + BOARD_WIDTH - 3;
    for (int yy = -te.p_bottom; yy < BOARD_HEIGHT - 3 + te.p_bottom; ++yy, puts(""))
        for (int xx = begin_x; xx < end_x; ++xx)
            putchar(ST(yy, xx) + '0');
}


alt_c_t _look_ahead(board *brd, phenotype *phenotype,  int Ty1, const options* opt)
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
                            alt.static_score = score_sta(&cp, brd, phenotype, &tlp);
                            alt.dynamic_score = score_dyn(&cp, brd, phenotype, &tlp);
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

