//
// Created by prwang on 17-5-12.
//

#ifndef SINGLEFILE

#include "game_manager.h"

#endif

// 打印场地用于调试
void game_manager::printField() {
#ifndef _BOTZONE_ONLINE
    static const char *i2s[] = {
            "~~",
            "~~",
            "  ",
            "[]",
            "##"
    };

    clog << "cur:                       enemy:\n";
    for (int y = 0; y < MAPHEIGHT; y++) {
        clog << "~~";
        for (int x = 0; x < MAPWIDTH; x++)
            clog << i2s[gb[curBotColor].get_tile(x, y) + 2];
        clog << "~~";
        for (int x = 0; x < MAPWIDTH; x++)
            clog << i2s[gb[enemyColor].get_tile(x, y) + 2];
        clog << "~~";
        clog << endl;
    }
    for (int i = 0; i < 43; i++)
        clog << "~~";
    clog << endl;
#endif
}

int game_manager::transfer() //返回赢家
{
    int color1 = 0, color2 = 1;
    if (gb[color1].transCount == 0 && gb[color2].transCount == 0)
        return -1;

    gb[color1].maxHeight += gb[color2].transCount;
    gb[color2].maxHeight += gb[color1].transCount;
#define RANDOM_RET_IF_DRAW(die1, die2, r1, r2)\
    switch( int(die1) | int(die2)  << 1) \
    { \
    case 0: break; \
    case 1:  \
        return(r2); \
    case 2:  \
        return(r1); \
    default: \
        return int(RAND() & 1); \
    }
    RANDOM_RET_IF_DRAW(gb[color1].maxHeight > MAPHEIGHT, gb[color2].maxHeight > MAPHEIGHT, color1, color2);

    for (int i = 0; i < MAPHEIGHT - gb[color2].transCount; i++)
        gb[color1].lines[i] = gb[color1].lines[i + gb[color2].transCount];
    for (int i = MAPHEIGHT - gb[color2].transCount; i < MAPHEIGHT; i++)
        gb[color1].lines[i] = gb[color2].trans[i - MAPHEIGHT + gb[color2].transCount];

    for (int i = 0; i < MAPHEIGHT - gb[color1].transCount; i++)
        gb[color2].lines[i] = gb[color2].lines[i + gb[color1].transCount];
    for (int i = MAPHEIGHT - gb[color1].transCount; i < MAPHEIGHT; i++)
        gb[color2].lines[i] = gb[color1].trans[i - MAPHEIGHT + gb[color1].transCount];

    return -1;

}
game_manager::game_manager(int blockType, int _curBotColor, const float* wg1, const float* wg2, options& opt)
{

    memset(type_count, 0, sizeof(type_count));
    curBotColor = _curBotColor;
    enemyColor = 1 - _curBotColor;
    nextTypeForColor[0] = nextTypeForColor[1] = (SHAPES) blockType;

    ++type_count[0][blockType], ++type_count[1][blockType];
    gamePhenotypes[curBotColor] = init_from_weight(wg1, opt);
    gamePhenotypes[enemyColor] = init_from_weight(wg2, opt);

}

void game_manager::recoverBot(int blockType, int x, int y, int o) {
    curTypeForColor[curBotColor] = nextTypeForColor[curBotColor];
    auto r = toxy2TXY((SHAPES) curTypeForColor[curBotColor], (ORI) o, x, y);
    gb[curBotColor].put_eliminate(get<0>(r), get<1>(r), get<2>(r));
}

void game_manager::recoverEnemy(int blockType, int x, int y, int o) {
    curTypeForColor[enemyColor] = nextTypeForColor[enemyColor];
    auto r = toxy2TXY((SHAPES) curTypeForColor[enemyColor], (ORI) o, x, y);
    gb[enemyColor].put_eliminate(get<0>(r), get<1>(r), get<2>(r));
}

int game_manager::recover(int blockTypeBot, int xBot, int yBot, int oBot,
                          int blockTypeEnemy, int xEnemy, int yEnemy, int oEnemy) {
    recoverBot(blockTypeBot, xBot, yBot, oBot);
    recoverEnemy(blockTypeEnemy, xEnemy, yEnemy, oEnemy);

    type_count[enemyColor][blockTypeBot]++;
    nextTypeForColor[enemyColor] = (SHAPES) blockTypeBot;
    type_count[curBotColor][blockTypeEnemy]++;
    nextTypeForColor[curBotColor] = (SHAPES) blockTypeEnemy;
    return transfer();
}

template<int MAXDEPTH> int game_manager::make_decisions(int player) {
    best_alt_g = {-1, -1, -1};
    search_for_pos<MAXDEPTH>(
            half_game(type_count[player], gb[player], nextTypeForColor[player], gamePhenotypes[player]), 0);
    if (best_alt_g.o == -1) return -1;

    for (int i = 0; i < 7; ++i) best_for_type[i] = -FLT_MAX, type_ok[i] = false;
    search_for_pos<MAXDEPTH>(
            half_game(type_count[1 - player], gb[1 - player], nextTypeForColor[1 - player], gamePhenotypes[player]), -1);
    int ty = -1; float tc = FLT_MAX;
    for (int i = 0; i < 7; ++i) if (type_ok[i] && best_for_type[i] < tc)
        {
            tc = best_for_type[i];
            ty = i;
        }
    return ty >= 0 ? shape_order[ty] : first_type(type_count[1 - player]);

}
template int game_manager::make_decisions<2>(int player);
template int game_manager::make_decisions<1>(int player);
template int game_manager::make_decisions<0>(int player);


template<int MAXDEPTH, bool PRINT_FLAG> int game_manager::auto_game()
{
    int cnt = 0;
    while (true)
    {
        if (PRINT_FLAG) {
            printField();
            fprintf(stderr, "%d\n", ++cnt);
        }
        int xbot, ybot, obot, xenemy, yenemy, oenemy;
        int enm_blk = make_decisions<MAXDEPTH>(curBotColor);
        xbot = best_alt_g.x;
        ybot = best_alt_g.y;
        obot = best_alt_g.o;
        int cur_blk = make_decisions<MAXDEPTH>(enemyColor);
        xenemy = best_alt_g.x;
        yenemy = best_alt_g.y;
        oenemy = best_alt_g.o;

        RANDOM_RET_IF_DRAW(enm_blk == -1, cur_blk == -1, curBotColor, enemyColor);
        int rsu_tr = recover(enm_blk, xbot, ybot, obot, cur_blk, xenemy, yenemy, oenemy);
        if (rsu_tr > 0)
            return rsu_tr;
    }
}
template int game_manager::auto_game<2>();
template int game_manager::auto_game<1>();
template int game_manager::auto_game<0>();
template int game_manager::auto_game<0, true>();
