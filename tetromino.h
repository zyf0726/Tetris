#pragma once
#include "shared.h"

struct tetromino {
    uint16_t lines[4];
    //padding
    int p_top;
    int p_left;
    int p_right;
    int p_bottom;


};
constexpr tetromino tetrominos[19] = {
        {
                .lines = {
                        0x0000, // 0b0000000000000000
                        0x3000, // 0b0011000000000000
                        0x3000, // 0b0011000000000000
                        0x0000, // 0b0000000000000000
                },

                .p_top = 1,
                .p_left = 1,
                .p_right = 1,
                .p_bottom = 1,
        }, {
                .lines = {
                        0x0000, // 0b0000000000000000
                        0x7800, // 0b0111100000000000
                        0x0000, // 0b0000000000000000
                        0x0000, // 0b0000000000000000
                },

                .p_top = 1,
                .p_left = 0,
                .p_right = 0,
                .p_bottom = 2,
        }, {
                .lines = {
                        0x1000, // 0b0001000000000000
                        0x1000, // 0b0001000000000000
                        0x1000, // 0b0001000000000000
                        0x1000, // 0b0001000000000000
                },

                .p_top = 0,
                .p_left = 2,
                .p_right = 1,
                .p_bottom = 0,
        }, {
                .lines = {
                        0x0000, // 0b0000000000000000
                        0x1800, // 0b0001100000000000
                        0x3000, // 0b0011000000000000
                        0x0000, // 0b0000000000000000
                },

                .p_top = 1,
                .p_left = 1,
                .p_right = 0,
                .p_bottom = 1,
        }, {
                .lines = {
                        0x1000, // 0b0001000000000000
                        0x1800, // 0b0001100000000000
                        0x0800, // 0b0000100000000000
                        0x0000, // 0b0000000000000000
                },

                .p_top = 0,
                .p_left = 2,
                .p_right = 0,
                .p_bottom = 1,
        }, {
                .lines = {
                        0x0000, // 0b0000000000000000
                        0x3000, // 0b0011000000000000
                        0x1800, // 0b0001100000000000
                        0x0000, // 0b0000000000000000
                },

                .p_top = 1,
                .p_left = 1,
                .p_right = 0,
                .p_bottom = 1,
        }, {
                .lines = {
                        0x0800, // 0b0000100000000000
                        0x1800, // 0b0001100000000000
                        0x1000, // 0b0001000000000000
                        0x0000, // 0b0000000000000000
                },

                .p_top = 0,
                .p_left = 2,
                .p_right = 0,
                .p_bottom = 1,
        }, {
                .lines = {
                        0x0000, // 0b0000000000000000
                        0x3800, // 0b0011100000000000
                        0x2000, // 0b0010000000000000
                        0x0000, // 0b0000000000000000
                },

                .p_top = 1,
                .p_left = 1,
                .p_right = 0,
                .p_bottom = 1,
        }, {
                .lines = {
                        0x1000, // 0b0001000000000000
                        0x1000, // 0b0001000000000000
                        0x1800, // 0b0001100000000000
                        0x0000, // 0b0000000000000000
                },

                .p_top = 0,
                .p_left = 2,
                .p_right = 0,
                .p_bottom = 1,
        }, {
                .lines = {
                        0x0800, // 0b0000100000000000
                        0x3800, // 0b0011100000000000
                        0x0000, // 0b0000000000000000
                        0x0000, // 0b0000000000000000
                },

                .p_top = 0,
                .p_left = 1,
                .p_right = 0,
                .p_bottom = 2,
        }, {
                .lines = {
                        0x3000, // 0b0011000000000000
                        0x1000, // 0b0001000000000000
                        0x1000, // 0b0001000000000000
                        0x0000, // 0b0000000000000000
                },

                .p_top = 0,
                .p_left = 1,
                .p_right = 1,
                .p_bottom = 1,
        }, {
                .lines = {
                        0x0000, // 0b0000000000000000
                        0x3800, // 0b0011100000000000
                        0x0800, // 0b0000100000000000
                        0x0000, // 0b0000000000000000
                },

                .p_top = 1,
                .p_left = 1,
                .p_right = 0,
                .p_bottom = 1,
        }, {
                .lines = {
                        0x1800, // 0b0001100000000000
                        0x1000, // 0b0001000000000000
                        0x1000, // 0b0001000000000000
                        0x0000, // 0b0000000000000000
                },

                .p_top = 0,
                .p_left = 2,
                .p_right = 0,
                .p_bottom = 1,
        }, {
                .lines = {
                        0x2000, // 0b0010000000000000
                        0x3800, // 0b0011100000000000
                        0x0000, // 0b0000000000000000
                        0x0000, // 0b0000000000000000
                },

                .p_top = 0,
                .p_left = 1,
                .p_right = 0,
                .p_bottom = 2,
        }, {
                .lines = {
                        0x1000, // 0b0001000000000000
                        0x1000, // 0b0001000000000000
                        0x3000, // 0b0011000000000000
                        0x0000, // 0b0000000000000000
                },

                .p_top = 0,
                .p_left = 1,
                .p_right = 1,
                .p_bottom = 1,
        }, {
                .lines = {
                        0x0000, // 0b0000000000000000
                        0x3800, // 0b0011100000000000
                        0x1000, // 0b0001000000000000
                        0x0000, // 0b0000000000000000
                },

                .p_top = 1,
                .p_left = 1,
                .p_right = 0,
                .p_bottom = 1,
        }, {
                .lines = {
                        0x1000, // 0b0001000000000000
                        0x1800, // 0b0001100000000000
                        0x1000, // 0b0001000000000000
                        0x0000, // 0b0000000000000000
                },

                .p_top = 0,
                .p_left = 2,
                .p_right = 0,
                .p_bottom = 1,
        }, {
                .lines = {
                        0x1000, // 0b0001000000000000
                        0x3800, // 0b0011100000000000
                        0x0000, // 0b0000000000000000
                        0x0000, // 0b0000000000000000
                },

                .p_top = 0,
                .p_left = 1,
                .p_right = 0,
                .p_bottom = 2,
        }, {
                .lines = {
                        0x1000, // 0b0001000000000000
                        0x3000, // 0b0011000000000000
                        0x1000, // 0b0001000000000000
                        0x0000, // 0b0000000000000000
                },

                .p_top = 0,
                .p_left = 1,
                .p_right = 1,
                .p_bottom = 1,
        },
};
struct t_last_placement {
    struct tetromino * tetromino;
    int x;
    int y;
    int n_lines_removed;
    int * lines_removed;
};
//方块的种类
#define N_TETROMINO(tetromino, n) \
    if (n == 0) {                 \
        *tetromino = 0;           \
    } else if(n == 1) {           \
        *tetromino = 1;           \
    } else if(n == 2) {           \
        *tetromino = 3;           \
    } else if(n == 3) {           \
        *tetromino = 5;           \
    } else if(n == 4) {           \
        *tetromino = 7;           \
    } else if(n == 5) {           \
        *tetromino = 11;          \
    } else if(n == 6) {           \
        *tetromino = 15;          \
    }
// 每一个方块有多少种方向
#define N_ROTATIONS(rotation, n) \
    if (n == 0) {                \
        *rotation = 1;           \
    } else if(n == 1) {          \
        *rotation = 2;           \
    } else if(n == 3) {          \
        *rotation = 2;           \
    } else if(n == 5) {          \
        *rotation = 2;           \
    } else if(n == 7) {          \
        *rotation = 4;           \
    } else if(n == 11) {         \
        *rotation = 4;           \
    } else if(n == 15) {         \
        *rotation = 4;           \
    }
int place_tetromino (struct board * board, struct tetromino * tetromino, int position, int * placement);
int random_tetromino (struct options* opt);

