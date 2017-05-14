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


int random_tetromino (struct options* opt) {
    int tetromino, rotations,
        random_t = l_rand(0, 7, opt);

    N_TETROMINO(&tetromino, random_t);
    N_ROTATIONS(&rotations, tetromino);

    return tetromino + l_rand(0, rotations, opt);
}