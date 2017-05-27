#pragma GCC optimize("unroll-loops")
//
// Created by prwang on 2017/4/22.
//

#pragma once
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

#define N_FEATURES 30

#define BOARD_WIDTH 10
#define BOARD_HEIGHT 20

struct __cord { int x, y; };
inline __cord operator +(const __cord& a, const __cord& b) { return {a.x + b.x, a.y + b.y}; }



constexpr float glb_fwt[]  ={ -171.79663,-4.74666,123.25094,-104.96412,-38.18864,-51.44960,-79.61923,-286.10596 };
constexpr int N_FE = sizeof(glb_fwt) / sizeof(float);
//= {5, -33, 9, -20, -76, -31, -2, -65};

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
constexpr SHAPES shape_order[] = { _O_, _I_, _S_, _Z_, _L_, _J_, _T_ };
constexpr int shape_order_rev[] = { 4, 5, 2, 3, 6, 1, 0};
enum ORI
{
    _UP_,
    _LF_,
    _DN_,
    _RT_
};
//    x    y    newTy



enum selection {
    TOURNAMENT,
    SUS,
    SIGMA,
};

extern mt19937_64 RAND;
constexpr int elimBonus[4] = { 1, 3, 5, 7 };
template<class T> inline void mint(T& a, T b) { if (a > b) a = b; }
template<class T> inline bool mint1(T& a, T b) { return a > b ? (a = b, true) : false;  }
template<class T> inline void maxt(T& a, T b) { if (a < b) a = b; }
template<class T> inline bool maxt1(T& a, T b) { return a < b ? (a = b, true) : false; }
#ifdef SINGLEFILE
#define INLINE inline
#else
#define INLINE
#endif

constexpr size_t MAX_SOLUTIONS = 24;

template<class T, size_t max_size>struct insert_sort_container
{
    T* begin() { return data; }
    T* end() { return data + real_size; }
    insert_sort_container() { real_size = 0; }
    void push_back(const T& x)
    {
        size_t p = real_size;
        while (p > 0 && x <  data[p - 1])
        {
            data[p] = data[p - 1];
            --p;
        }
        data[p] = x;
        mint(++real_size, max_size);
    }
    size_t size() { return real_size; }
private:
    T data[max_size + 1];
    size_t real_size;
};
