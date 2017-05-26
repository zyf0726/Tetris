#pragma once
#ifndef SINGLEFILE
#include "shared.h"
#include "game_board.h"
#include "search.h"
#include "options.h"
#endif

struct game_manager {
    game_board gb[2];
    int type_count[2][7]{};
// 转移双方消去的行，返回-1表示继续，否则返回输者
    SHAPES nextTypeForColor[2];
    int curTypeForColor[2];
    int curBotColor, enemyColor, turnID;
    phenotype *gamePhenotypes[2];


    INLINE int transfer();
    INLINE void printField();
    INLINE game_manager(int blockType, int _curBotColor, const float*, const float*, options&);
    INLINE void recoverBot(int blockType, int x, int y, int o);
    INLINE void recoverEnemy(int blockType, int x, int y, int o);
    INLINE int recover(int blockTypeBot, int xBot, int yBot, int oBot,
                        int blockTypeEnemy, int xEnemy, int yEnemy, int oEnemy);
    template<int MAXDEPTH> INLINE int make_decisions(int teamColor);
    template<int MAXDEPTH, bool PRINT_FLAG = false>INLINE int auto_game();
    INLINE ~game_manager()
    {
        free_phenotype(gamePhenotypes[0]);
        free_phenotype(gamePhenotypes[1]);
    }
};
