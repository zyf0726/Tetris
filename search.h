#pragma once
#ifndef SINGLEFILE
#include "shared.h"
#include "game_board.h"
#include "phenotype.h"

#endif

struct game_manager;
struct half_game
{
    board gb; int type_count[7], curr_type;
    phenotype* curPhenotype;
    INLINE vector<int> get_valid_types();
    INLINE half_game(const half_game &ori, const board &newgb);
    INLINE half_game(const int count[], const game_board &m_gb, SHAPES _curr_type, phenotype *tempPhenotype);
    INLINE half_game(const half_game &ori, int selected_type);
};

// 1. dfs(self, FOR_POS, 0) -> 输出类型 ==> ...
// 2. dfs(enemy, FOR_POS, 0) ==> dfs(self, FOR_TYPE) ->记录每一种TYPE的敌方最大的分数， 在最上层进行决策 ==> ...


extern float ans_g[7];  //TODO 初始化
extern alternative best_alt_g; //TODO 初始化
template<int MAX_DEPTH> INLINE float search_for_type(half_game g, int depth); //depth == 0 : output;
template<int MAX_DEPTH> INLINE float search_for_pos(half_game g, int depth);

template<int MAX_DEPTH> INLINE SHAPES worst_for_enemy(const game_manager &m, int subject, SHAPES last_type);


