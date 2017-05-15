#include <stdlib.h>

#include "tetromino.h"
#include "board.h"
#include "random.h"



int random_tetromino (struct options* opt) {
    int tetromino, rotations,
        random_t = l_rand(0, 7, opt);

    N_TETROMINO(&tetromino, random_t);
    N_ROTATIONS(&rotations, tetromino);

    return tetromino + l_rand(0, rotations, opt);
}