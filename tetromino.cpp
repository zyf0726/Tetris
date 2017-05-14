#include <stdlib.h>

#include "tetromino.h"
#include "board.h"
#include "random.h"

void shift_lines(uint16_t lines[], int position, const tetromino* tr)
{

    for (int i = 0; i < 4; i++) {
        if (position == 0) {
            lines[i] = tr->lines[i];
        } else if (position > 0) {
            lines[i] = tr->lines[i] >> position;
        } else {
            lines[i] = tr->lines[i] << -position;
        }

        lines[i] |= EMPTY_LINE;
    }

}
int place_tetromino (struct board * board, const tetromino * tr, int position, int * placement) {

    uint16_t lines[4]; shift_lines(lines, position, tr);
    int i;
    for (i = -tr->p_top; i < BOARD_HEIGHT + tr->p_bottom - 4 + 1; i++) {
        for (int y = tr->p_top; y < 4 - tr->p_bottom; y++) {
            if ((board->lines[i + y] & lines[y]) != EMPTY_LINE) {
                i--;
                goto found_i;
            }
        }
    }
    i--;

    found_i:


    if (i < -tr->p_top) {
        return 1;
    }

    if (placement != NULL) {
        *placement = i;
    }

    for (int y = tr->p_top; y < 4 - tr->p_bottom; y++) {
        board->lines[i + y] |= lines[y];
    }


    return 0;
}

int random_tetromino (struct options* opt) {
    int tetromino, rotations,
        random_t = l_rand(0, 7, opt);

    N_TETROMINO(&tetromino, random_t);
    N_ROTATIONS(&rotations, tetromino);

    return tetromino + l_rand(0, rotations, opt);
}