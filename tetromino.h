#pragma once
#include "shared.h"

struct tetromino {
    uint16_t lines[4];
    //padding
    int p_top;
    int p_left;
    int p_right;
    int p_bottom;
    tetromino&operator=(const tetromino&) = delete;
    tetromino(const tetromino&) = delete;

};


constexpr tetromino tetrominos[19] = {
        {
                .lines = {
                        0x0000, //0 0b0000000000000000
                        0x3000, //1 0b0011000000000000
                        0x3000, //2 0b0011000000000000
                        0x0000, //3 0b0000000000000000
                                 //___0123456789
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
    const tetromino * tetromino;
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


INLINE void shift_lines(uint16_t lines[], int position, ctet tr);
int random_tetromino (struct options* opt);
inline tuple<__cord, int> __toxy2Txy(SHAPES t, ORI o, int x, int y)
{
    __cord c0 = __cord{x - 1, 20 - y};
    using CA = __cord[];
    if (t == _O_) return make_tuple(c0 + ((__cord[])  {{-2, -2}, {-2, -1}, {-1, -1}, {-1, -2}})[o], 0); // _O_
    else if (t == _I_) return o & 1 ? make_tuple(c0 + (CA{{-2, -1 }, {-1, -1}})[o >> 1], 1) : make_tuple(c0 + (CA{{-2, -2}, {-2, -1}})[o >> 1], 2);
    else if (t == _S_) return ~o & 1 ? make_tuple(c0 + (CA{{-2, -1}, {-2, -2}})[o >> 1], 3) : make_tuple(c0 + (CA{{-2, -1}, {-3, -1}})[o >> 1], 4);
    else if (t == _Z_) return ~o & 1 ? make_tuple(c0 + (CA{{-2, -1}, {-2, -2}})[o >> 1], 5) : make_tuple(c0 + (CA{{-2, -1}, {-3, -1}})[o >> 1], 6);
    else if (t == _L_) return make_tuple(c0 +  __cord{-2, -1}, 7 + o);
    else if (t == _J_) return make_tuple(c0 + __cord{-2, -1} , 11 + o);
    else if (t == _T_) return make_tuple(c0 + __cord{-2, -1}, ((int[]){2, 3, 0, 1})[o] + 15);
}
using cptr_tet = const tetromino*;
using ctet = const tetromino&;
inline tuple<const tetromino*, int , int > toxy2TXY(SHAPES t, ORI o, int x, int y)
{
    auto r = __toxy2Txy(t, o, x, y);
    return make_tuple(&tetrominos[get<1>(r)], get<0>(r).x, get<0>(r).y);
}
inline tuple<int, int, int, int> TXY2toxy(cptr_tet Ty, int x, int y)
{


};
