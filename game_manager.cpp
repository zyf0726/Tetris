//
// Created by prwang on 17-5-12.
//

#ifndef SINGLEFILE

#include "game_manager.h"

#endif

// 打印场地用于调试
void game_manager::printField()
{
#ifndef _BOTZONE_ONLINE
    static const char *i2s[] = {
            "~~",
            "~~",
            "  ",
            "[]",
            "##"
    };

    for (int y = 0; y < MAPHEIGHT; y++)
    {
        clog << "~~";
        for (int x = 0; x < MAPWIDTH; x++)
            clog << i2s[gb[0].get_tile(x, y) + 2];
        clog << "~~";
        for (int x = 0; x < MAPWIDTH; x++)
            clog << i2s[gb[1].get_tile(x, y) + 2];
        clog << "~~";
        clog << endl;
    }
    for (int i = 0; i < 43; i++)
        clog << "~~";
    clog << endl;
#endif
}

int game_manager::transfer()
{
    int color1 = 0, color2 = 1;
    if (gb[color1].transCount == 0 && gb[color2].transCount == 0)
        return -1;

    gb[color1].maxHeight += gb[color2].transCount;
    gb[color2].maxHeight += gb[color1].transCount;
    if (gb[color1].maxHeight > MAPHEIGHT)
        return color1;
    if (gb[color2].maxHeight > MAPHEIGHT)
        return color2;

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

void game_manager::init(int blockType, int _curBotColor) {
    curBotColor = _curBotColor;
    enemyColor = 1 - _curBotColor;
    nextTypeForColor[0] = nextTypeForColor[1] = (SHAPES) blockType;
    ++type_count[0][blockType], ++type_count[1][blockType];
}

void game_manager::recoverBot(int blockType, int x, int y, int o) {
    curTypeForColor[curBotColor] = nextTypeForColor[curBotColor];
    // 我当时把上一块落到了x y o
    auto r = toxy2TXY((SHAPES)curTypeForColor[curBotColor], (ORI) o, x, y);
    gb[curBotColor].put_eliminate(get<0>(r), get<1>(r), get<2>(r));
    // 我给对方什么块来着
}

void game_manager::recoverEnemy(int blockType, int x, int y, int o) {
    curTypeForColor[enemyColor] = nextTypeForColor[enemyColor];
    // 对方当时把上一块落到了 x y o！
    auto r = toxy2TXY((SHAPES) curTypeForColor[enemyColor], (ORI) o, x, y);
    gb[enemyColor].put_eliminate(get<0>(r), get<1>(r), get<2>(r));
    // 对方给我什么块来着？
}

void game_manager::recover(int blockTypeBot, int xBot, int yBot, int oBot,
                           int blockTypeEnemy, int xEnemy, int yEnemy, int oEnemy) {
    recoverBot(blockTypeBot, xBot, yBot, oBot);
    recoverEnemy(blockTypeEnemy, xEnemy, yEnemy, oEnemy);

    type_count[enemyColor][blockTypeBot]++;
    nextTypeForColor[enemyColor] = (SHAPES)blockTypeBot;
    type_count[curBotColor][blockTypeEnemy]++;
    nextTypeForColor[curBotColor] = (SHAPES)blockTypeEnemy;
    fixup();
}

int game_manager::make_decisions() {
    best_alt_g = {-1, -1, -1};


    search_for_pos(half_game(type_count[curBotColor], gb[curBotColor], nextTypeForColor[curBotColor]), 0);
    return worst_for_enemy(*this, enemyColor, nextTypeForColor[enemyColor]);
}
