#pragma once
#ifndef SINGLEFILE
#include "shared.h"
#include "game_board.h"

#endif
struct half_game
{
    game_board gb;
    int type_count[7];
};
enum SEARCH_TYPE { FOR_POS, FOR_TYPE };
constexpr int MAX_DEPTH = 2;
// 1. dfs(self, FOR_POS, 0) -> 输出类型 ==> ...
// 2. dfs(enemy, FOR_POS, 0) ==> dfs(self, FOR_TYPE) ->记录每一种TYPE的敌方最大的分数， 在最上层进行决策 ==> ...

void dfs(half_game hf, SEARCH_TYPE st, int depth);



