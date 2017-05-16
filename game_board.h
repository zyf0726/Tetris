#pragma once
#ifndef SINGLEFILE
#include "shared.h"
#include "board.h"
#include "tetromino.h"
#endif
class game_board : public board
{
// 代表分别向对方转移的行
    uint16_t trans[4]= {  };

// 转移行g
    int transCount{};

// 运行eliminate后的当前高度
    int maxHeight{};


public:
    INLINE game_board(){}
    // 消去行
    // 转移双方消去的行，返回-1表示继续，否则返回输者
    // 判断当前位置是否合法

    friend class game_manager;
    void put_eliminate(const tetromino *tr, int x, int y);
};
