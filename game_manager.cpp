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
    clog << "~~：墙，[]：块，##：新块" << endl;
    for (int y = MAPHEIGHT + 1; y >= 0; y--)
    {
        for (int x = 0; x <= MAPWIDTH + 1; x++)
            clog << i2s[gb[0].gridInfo[y][x] + 2];
        for (int x = 0; x <= MAPWIDTH + 1; x++)
            clog << i2s[gb[1].gridInfo[y][x] + 2];
        clog << endl;
    }
#endif
}

int game_manager::transfer()
{
    int color1 = 0, color2 = 1;
    if (gb[color1].transCount == 0 && gb[color2].transCount == 0)
        return -1;
    if (gb[color1].transCount == 0 || gb[color2].transCount == 0)
    {
        if (gb[color1].transCount == 0 && gb[color2].transCount > 0)
            swap(color1, color2);
        int h2;
        gb[color2].maxHeight = h2 = gb[color2].maxHeight + gb[color1].transCount;
        if (h2 > MAPHEIGHT)
            return color2;
        int i, j;

        for (i = h2; i > gb[color1].transCount; i--)
            for (j = 1; j <= MAPWIDTH; j++)
                gb[color2].gridInfo[i][j] = gb[color2].gridInfo[i - gb[color1].transCount][j];

        for (i = gb[color1].transCount; i > 0; i--)
            for (j = 1; j <= MAPWIDTH; j++)
                gb[color2].gridInfo[i][j] = gb[color1].trans[i - 1][j];
        return -1;
    } else
    {
        int h1, h2;
        gb[color1].maxHeight = h1 = gb[color1].maxHeight + gb[color2].transCount;//移动count1去color2
        gb[color2].maxHeight = h2 = gb[color2].maxHeight + gb[color1].transCount;

        if (h1 > MAPHEIGHT) return color1;
        if (h2 > MAPHEIGHT) return color2;

        int i, j;
        for (i = h2; i > gb[color1].transCount; i--)
            for (j = 1; j <= MAPWIDTH; j++)
                gb[color2].gridInfo[i][j] = gb[color2].gridInfo[i - gb[color1].transCount][j];

        for (i = gb[color1].transCount; i > 0; i--)
            for (j = 1; j <= MAPWIDTH; j++)
                gb[color2].gridInfo[i][j] = gb[color1].trans[i - 1][j];

        for (i = h1; i > gb[color2].transCount; i--)
            for (j = 1; j <= MAPWIDTH; j++)
                gb[color1].gridInfo[i][j] = gb[color1].gridInfo[i - gb[color2].transCount][j];

        for (i = gb[color2].transCount; i > 0; i--)
            for (j = 1; j <= MAPWIDTH; j++)
                gb[color1].gridInfo[i][j] = gb[color2].trans[i - 1][j];

        return -1;
    }
}
int game_manager::worst_for_enemy(int player)
{
    int M, m, *count = type_count[!player];
    auto m1 = minmax_element(count, count + 7);
    tie(M, m) = tie(*m1.first, *m1.second);

    vector<tuple<int, int> > de; de.reserve(8);
    for (int i = 0; i < 7; ++i)
        if (count[i] < M || m >= M + 2)
        {
            int x, y, o, w; tie(x, y, o, w) = game_board(gb[!player]).get_decision(i);
            de.emplace_back(w, i);
        }
    return get<1>(*max_element(begin(de), end(de)));
}

