//
// Created by prwang on 2017/4/22.
//

#ifndef TETRIS_SHARED_H
#define TETRIS_SHARED_H
#include <iostream>
#include <tuple>
#include <algorithm>
#include <cstdlib>
#include <random>
using namespace std;
constexpr int MAPWIDTH =  10;
constexpr int  MAPHEIGHT =  20;

struct __cord { int x, y; };
constexpr __cord blockShape[7][4][4] = {
        { { 0,0,1,0,-1,0,-1,-1 },{ 0,0,0,1,0,-1,1,-1 },{ 0,0,-1,0,1,0,1,1 },{ 0,0,0,-1,0,1,-1,1 } },
        { { 0,0,-1,0,1,0,1,-1 },{ 0,0,0,-1,0,1,1,1 },{ 0,0,1,0,-1,0,-1,1 },{ 0,0,0,1,0,-1,-1,-1 } },
        { { 0,0,1,0,0,-1,-1,-1 },{ 0,0,0,1,1,0,1,-1 },{ 0,0,-1,0,0,1,1,1 },{ 0,0,0,-1,-1,0,-1,1 } },
        { { 0,0,-1,0,0,-1,1,-1 },{ 0,0,0,-1,1,0,1,1 },{ 0,0,1,0,0,1,-1,1 },{ 0,0,0,1,-1,0,-1,-1 } },
        { { 0,0,-1,0,0,1,1,0 },{ 0,0,0,-1,-1,0,0,1 },{ 0,0,1,0,0,-1,-1,0 },{ 0,0,0,1,1,0,0,-1 } },
        { { 0,0,0,-1,0,1,0,2 },{ 0,0,1,0,-1,0,-2,0 },{ 0,0,0,1,0,-1,0,-2 },{ 0,0,-1,0,1,0,2,0 } },
        { { 0,0,0,1,-1,0,-1,1 },{ 0,0,-1,0,0,-1,-1,-1 },{ 0,0,0,-1,1,-0,1,-1 },{ 0,0,1,0,0,1,1,1 } }
};// 7种形状(长L| 短L| 反z| 正z| T| 直一| 田格)，4种朝向(上左下右)，8:每相邻的两个分别为x，y

constexpr int blockHeight[7][4]={
        {0,1,1,1},
        {0,1,1,1},
        {0,1,1,1},
        {0,1,1,1},
        {1,1,0,1},
        {2,0,1,0},
        {1,0,0,1}
};

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
