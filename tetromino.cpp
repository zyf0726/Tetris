#include <stdlib.h>

#include "tetromino.h"
#include "board.h"
#include "random.h"


int place_tetromino (struct board * board, struct tetromino * tetromino, int position, int * placement) {
    int i;

    uint16_t lines[4];

    for (i = 0; i < 4; i++) {
        if (position == 0) {
            lines[i] = tetromino->lines[i];
        } else if (position > 0) {
            lines[i] = tetromino->lines[i] >> position;
        } else {
            lines[i] = tetromino->lines[i] << -position;
        }

        lines[i] |= EMPTY_LINE;
    }

    for (i = -tetromino->p_top; i < BOARD_HEIGHT + tetromino->p_bottom - 4 + 1; i++) {
        for (int y = tetromino->p_top; y < 4 - tetromino->p_bottom; y++) {
            if ((board->lines[i + y] & lines[y]) != EMPTY_LINE) {
                i--;
                goto found_i;
            }
        }
    }
/*

----------
**********
**********
**********
**********
----------
----------
----------
----------
----------


 * */
    i--;

    found_i:

    //printf("Found i to be %d..\n", i);

    if (i < -tetromino->p_top) {
        return 1;
    }

    if (placement != NULL) {
        *placement = i;
    }

    for (int y = tetromino->p_top; y < 4 - tetromino->p_bottom; y++) {
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