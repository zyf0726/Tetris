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
    clog << "~~：墙，[]：块" << endl;
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

