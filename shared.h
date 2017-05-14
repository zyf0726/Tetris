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
inline __cord operator +(const __cord& a, const __cord& b) { return {a.x + b.x, a.y + b.y}; }

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
constexpr SHAPES shape_prder[] = { _O_, _I_, _S_, _Z_, _L_, _J_, _T_ };
constexpr int shape_order_rev[] = { 4, 5, 2, 3, 6, 1, 0};
enum ORI
{
    _UP_,
    _LF_,
    _DN_,
    _RT_
};
//    x    y    newTy


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
