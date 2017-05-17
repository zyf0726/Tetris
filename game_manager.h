#pragma once
#ifndef SINGLEFILE
#include "shared.h"
#include "game_board.h"
#include "search.h"
#endif

struct game_manager {
    game_board gb[2];
    int type_count[2][7]{};
// 转移双方消去的行，返回-1表示继续，否则返回输者
    SHAPES nextTypeForColor[2];
    int curTypeForColor[2];
    int curBotColor, enemyColor, turnID;


    INLINE int transfer();
    INLINE void printField();
    INLINE void fixup() { transfer(); }
    friend int main();
    INLINE void init(int blockType, int _curBotColor);
    INLINE void recoverBot(int blockType, int x, int y, int o);
    INLINE void recoverEnemy(int blockType, int x, int y, int o);
    INLINE void recover(int blockTypeBot, int xBot, int yBot, int oBot,
                        int blockTypeEnemy, int xEnemy, int yEnemy, int oEnemy);
    INLINE int make_decisions();
};
