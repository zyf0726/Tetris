//
// Created by prwang on 2017/4/22.
//

#ifndef TETRIS_SHARED_H
#define TETRIS_SHARED_H
#include <iostream>
#include <cassert>
#include <cstdio>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <atomic>
#include <queue>
#include <cstring>
#include <vector>
#include <tuple>
#include <algorithm>
#include <cstdlib>
#include <climits>
#include <cfloat>
#include <random>
using namespace std;
constexpr int MAPWIDTH =  10;
constexpr int  MAPHEIGHT =  20;

#define N_FEATURES 32

#define BOARD_WIDTH 10
#define BOARD_HEIGHT 20

constexpr int featureDimensions = 7;
struct __cord { int x, y; };
inline __cord operator +(const __cord& a, const __cord& b) { return {a + b}; }
constexpr __cord blockShape[7][4][4] = {
        { { 0,0,1,0,-1,0,-1,-1 },{ 0,0,0,1,0,-1,1,-1 },{ 0,0,-1,0,1,0,1,1 },{ 0,0,0,-1,0,1,-1,1 } },
        { { 0,0,-1,0,1,0,1,-1 },{ 0,0,0,-1,0,1,1,1 },{ 0,0,1,0,-1,0,-1,1 },{ 0,0,0,1,0,-1,-1,-1 } },
        { { 0,0,1,0,0,-1,-1,-1 },{ 0,0,0,1,1,0,1,-1 },{ 0,0,-1,0,0,1,1,1 },{ 0,0,0,-1,-1,0,-1,1 } },
        { { 0,0,-1,0,0,-1,1,-1 },{ 0,0,0,-1,1,0,1,1 },{ 0,0,1,0,0,1,-1,1 },{ 0,0,0,1,-1,0,-1,-1 } },
        { { 0,0,-1,0,0,1,1,0 },{ 0,0,0,-1,-1,0,0,1 },{ 0,0,1,0,0,-1,-1,0 },{ 0,0,0,1,1,0,0,-1 } },
        { { 0,0,0,-1,0,1,0,2 },{ 0,0,1,0,-1,0,-2,0 },{ 0,0,0,1,0,-1,0,-2 },{ 0,0,-1,0,1,0,2,0 } },
        { { 0,0,0,1,-1,0,-1,1 },{ 0,0,-1,0,0,-1,-1,-1 },{ 0,0,0,-1,1,-0,1,-1 },{ 0,0,1,0,0,1,1,1 } }
};// 7种形状(长L| 短L| 反z| 正z| T| 直一| 田格)，4种朝向(上左下右)，8:每相邻的两个分别为x，y

enum SHAPES
{
    _L_,
    _J_,
    _S_,
    _Z_,
    _T_,
    _I_,
    _O_
};
enum ORI
{
    _UP_,
    _LF_,
    _DN_,
    _RT_
};
//    x    y    newTy
tuple<__cord,  int> toxyAdapter(SHAPES t, int o, int x, int y)
{
    __cord c0 = __cord{x - 1, 20 - y};
    int newshape;
    using CA = __cord[];

    if (t == _O_)
        return make_tuple(c0 + ((__cord[])  {{-2, -2}, {-2, -1}, {-1, -1}, {-1, -2}})[o], 0); // _O_
    else if (t == _I_)
    {
        if ((o & 1))
            return make_tuple(c0 + (CA{{-2, -1 }, {-1, -1}})[o >> 1], 1);
        else return make_tuple(c0 + (CA{{-2, -2}, {-2, -1}})[o >> 1], 2);
    } else if (t == _S_)
    {
        if (~o & 1)
            return make_tuple(c0 + (CA{{-2, -1}, {-2, -2}})[o >> 1], 3);
        else  return make_tuple(c0 + (CA{{-2, -1}, {-3, -1}})[o >> 1], 4);
    } else if (t == _Z_)
    {
        if (~o & 1)
            return make_tuple(c0+ (CA{{-2, -1}, {-2, -2}})[o >> 1], 5);
        else return make_tuple(c0+ (CA{{-2, -1}, {-3, -1}})[o >> 1], 6);
    } else if (t == _L_) return make_tuple(c0 +  __cord{-2, -1}, 7 + o);
    else if (t == _J_) return make_tuple(c0 + __cord{-2, -1} , 11 + o);
    else if (t == _T_) return make_tuple(c0 + __cord{-2, -1}, ((int[]){2, 3, 0, 1})[o] + 15);
}
constexpr int blockHeight[7][4]={
        {0,1,1,1},
        {0,1,1,1},
        {0,1,1,1},
        {0,1,1,1},
        {1,1,0,1},
        {2,0,1,0},
        {1,0,0,1}
};

enum selection {
    TOURNAMENT,
    SUS,
    SIGMA,
};

extern mt19937_64 RAND;
constexpr int elimBonus[4] = { 1, 3, 5, 7 };
typedef unsigned char ubyte;
typedef signed char sbyte;
template<class T> inline void mint(T& a, T b) { if (a > b) a = b; }
template<class T> inline void maxt(T& a, T b) { if (a < b) a = b; }
#ifdef SINGLEFILE
#define INLINE inline
#else
#define INLINE
#endif

#endif //TETRIS_SHARED_H
