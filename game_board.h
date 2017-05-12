#pragma once
#ifndef SINGLEFILE
#include "shared.h"
#endif
class game_board
{

    // 检查能否从场地顶端直接落到当前位置
    INLINE bool checkDirectDropTo(int blockType, int x, int y, int o);

    // 颜色方还能否继续游戏
    INLINE bool canPut(int blockType);

    int gridInfo[MAPHEIGHT + 2][MAPWIDTH + 2] = {};
// 代表分别向对方转移的行
    int trans[4][MAPWIDTH + 2] = {  };

// 转移行g
    int transCount;

// 运行eliminate后的当前高度
    int maxHeight;

// 总消去行数的分数之和
    int elimTotal;
public:
    INLINE tuple<int, int, int, int> get_decision(int ty);
    INLINE game_board();
    // 消去行
    INLINE void eliminate();
    // 转移双方消去的行，返回-1表示继续，否则返回输者
    INLINE int evaluate(int);
    // 判断当前位置是否合法
    INLINE bool is_valid(int ty, int x, int y, int o) const ;
    INLINE bool is_on_ground(int ty, int x, int y, int o) const ;
    INLINE bool place(int ty, int x, int y, int o);
    friend class element;
    friend class game_manager;
};
