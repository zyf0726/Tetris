#ifndef SINGLEFILE

#include "tetromino.h"
#include "random.h"
#endif
const tetromino tetrominos[19] = 
{
tetromino(
                        0x0000, //0 0b0000000000000000
                        0x3000, //1 0b0011000000000000
                        0x3000, //2 0b0011000000000000
                        0x0000, //3 0b0000000000000000
                        //___0123456789


                1,
                1,
                1,
                1),
tetromino(
                        0x0000, // 0b0000000000000000
                        0x7800, // 0b0111100000000000
                        0x0000, // 0b0000000000000000
                        0x0000, // 0b0000000000000000


                1,
                0,
                0,
                2
),
tetromino(
                        0x1000, // 0b0001000000000000
                        0x1000, // 0b0001000000000000
                        0x1000, // 0b0001000000000000
                        0x1000, // 0b0001000000000000


                0,
                2,
                1,
                0
),
tetromino(
                        0x0000, // 0b0000000000000000
                        0x1800, // 0b0001100000000000
                        0x3000, // 0b0011000000000000
                        0x0000, // 0b0000000000000000


                1,
                1,
                0,
                1
),
tetromino(
                        0x1000, // 0b0001000000000000
                        0x1800, // 0b0001100000000000
                        0x0800, // 0b0000100000000000
                        0x0000, // 0b0000000000000000


                0,
                2,
                0,
                1
),
tetromino(
                        0x0000, // 0b0000000000000000
                        0x3000, // 0b0011000000000000
                        0x1800, // 0b0001100000000000
                        0x0000, // 0b0000000000000000


                1,
                1,
                0,
                1
),
tetromino(
                        0x0800, // 0b0000100000000000
                        0x1800, // 0b0001100000000000
                        0x1000, // 0b0001000000000000
                        0x0000, // 0b0000000000000000


                0,
                2,
                0,
                1
),
tetromino(
                        0x0000, // 0b0000000000000000
                        0x3800, // 0b0011100000000000
                        0x2000, // 0b0010000000000000
                        0x0000, // 0b0000000000000000


                1,
                1,
                0,
                1
),
tetromino(
                        0x1000, // 0b0001000000000000
                        0x1000, // 0b0001000000000000
                        0x1800, // 0b0001100000000000
                        0x0000, // 0b0000000000000000


                0,
                2,
                0,
                1
),
tetromino(
                        0x0800, // 0b0000100000000000
                        0x3800, // 0b0011100000000000
                        0x0000, // 0b0000000000000000
                        0x0000, // 0b0000000000000000


                0,
                1,
                0,
                2
),
tetromino(
                        0x3000, // 0b0011000000000000
                        0x1000, // 0b0001000000000000
                        0x1000, // 0b0001000000000000
                        0x0000, // 0b0000000000000000


                0,
                1,
                1,
                1
),
tetromino(
                        0x0000, // 0b0000000000000000
                        0x3800, // 0b0011100000000000
                        0x0800, // 0b0000100000000000
                        0x0000, // 0b0000000000000000


                1,
                1,
                0,
                1
),
tetromino(
                        0x1800, // 0b0001100000000000
                        0x1000, // 0b0001000000000000
                        0x1000, // 0b0001000000000000
                        0x0000, // 0b0000000000000000


                0,
                2,
                0,
                1
),
tetromino(
                        0x2000, // 0b0010000000000000
                        0x3800, // 0b0011100000000000
                        0x0000, // 0b0000000000000000
                        0x0000, // 0b0000000000000000


                0,
                1,
                0,
                2
),
tetromino(
                        0x1000, // 0b0001000000000000
                        0x1000, // 0b0001000000000000
                        0x3000, // 0b0011000000000000
                        0x0000, // 0b0000000000000000


                0,
                1,
                1,
                1
),
tetromino(
                        0x0000, // 0b0000000000000000
                        0x3800, // 0b0011100000000000
                        0x1000, // 0b0001000000000000
                        0x0000, // 0b0000000000000000


                1,
                1,
                0,
                1
),
tetromino(
                        0x1000, // 0b0001000000000000
                        0x1800, // 0b0001100000000000
                        0x1000, // 0b0001000000000000
                        0x0000, // 0b0000000000000000


                0,
                2,
                0,
                1
),
tetromino(
                        0x1000, // 0b0001000000000000
                        0x3800, // 0b0011100000000000
                        0x0000, // 0b0000000000000000
                        0x0000, // 0b0000000000000000


                0,
                1,
                0,
                2
),
tetromino(
                        0x1000, // 0b0001000000000000
                        0x3000, // 0b0011000000000000
                        0x1000, // 0b0001000000000000
                        0x0000, // 0b0000000000000000


                0,
                1,
                1,
                1
)
};


int random_tetromino (struct options* opt) {
    int tetromino, rotations,
        random_t = l_rand(0, 7, opt);

    N_TETROMINO(&tetromino, random_t);
    N_ROTATIONS(&rotations, tetromino);

    return tetromino + l_rand(0, rotations, opt);
}
