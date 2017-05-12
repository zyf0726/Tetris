#pragma once
#ifndef SINGLEFILE
#include "shared.h"
#include "game_board.h"
#endif


class game_manager {
    game_board gb[2];
    int type_count[2][7] = {0};
// 转移双方消去的行，返回-1表示继续，否则返回输者
    INLINE int transfer();
public:
    INLINE void printField();
    INLINE void fixup() { gb[0].eliminate(); gb[1].eliminate(); transfer(); }
    INLINE int worst_for_enemy(int player);
    friend int main();
};


