//
// Created by prwang on 2017/4/22.
//

#ifndef SINGLEFILE
#include "util.h"
#include "Tetris.h"
#endif
namespace Util
{
// 代表分别向对方转移的行
    int trans[2][4][MAPWIDTH + 2] = { 0 };

// 转移行数
    int transCount[2] = { 0 };

// 运行eliminate后的当前高度
    int maxHeight[2] = { 0 };

// 总消去行数的分数之和
    int elimTotal[2] = { 0 };

    // 检查能否从场地顶端直接落到当前位置
    bool checkDirectDropTo(int color, int blockType, int x, int y, int o)
    {
        auto &def = blockShape[blockType][o];
        for (; y <= MAPHEIGHT; y++)
            for (int i = 0; i < 4; i++)
            {
                int _x = def[i * 2] + x, _y = def[i * 2 + 1] + y;
                if (_y > MAPHEIGHT)
                    continue;
                if (_y < 1 || _x < 1 || _x > MAPWIDTH || gridInfo[color][_y][_x])
                    return false;
            }
        return true;
    }

    // 消去行
    void eliminate(int color)
    {
        int &count = transCount[color] = 0;
        int i, j, emptyFlag, fullFlag;
        maxHeight[color] = MAPHEIGHT;
        for (i = 1; i <= MAPHEIGHT; i++)
        {
            emptyFlag = 1;
            fullFlag = 1;
            for (j = 1; j <= MAPWIDTH; j++)
            {
                if (gridInfo[color][i][j] == 0)
                    fullFlag = 0;
                else
                    emptyFlag = 0;
            }
            if (fullFlag)
            {
                for (j = 1; j <= MAPWIDTH; j++)
                {
                    // 注意这里只转移以前的块，不包括最后一次落下的块（“撤销最后一步”）
                    trans[color][count][j] = gridInfo[color][i][j] == 1 ? 1 : 0;
                    gridInfo[color][i][j] = 0;
                }
                count++;
            }
            else if (emptyFlag)
            {
                maxHeight[color] = i - 1;
                break;
            }
            else
                for (j = 1; j <= MAPWIDTH; j++)
                {
                    gridInfo[color][i - count][j] =
                            gridInfo[color][i][j] > 0 ? 1 : gridInfo[color][i][j];
                    if (count)
                        gridInfo[color][i][j] = 0;
                }
        }
        maxHeight[color] -= count;
        elimTotal[color] += elimBonus[count];
    }

    // 转移双方消去的行，返回-1表示继续，否则返回输者
    int transfer()
    {
        int color1 = 0, color2 = 1;
        if (transCount[color1] == 0 && transCount[color2] == 0)
            return -1;
        if (transCount[color1] == 0 || transCount[color2] == 0)
        {
            if (transCount[color1] == 0 && transCount[color2] > 0)
                swap(color1, color2);
            int h2;
            maxHeight[color2] = h2 = maxHeight[color2] + transCount[color1];
            if (h2 > MAPHEIGHT)
                return color2;
            int i, j;

            for (i = h2; i > transCount[color1]; i--)
                for (j = 1; j <= MAPWIDTH; j++)
                    gridInfo[color2][i][j] = gridInfo[color2][i - transCount[color1]][j];

            for (i = transCount[color1]; i > 0; i--)
                for (j = 1; j <= MAPWIDTH; j++)
                    gridInfo[color2][i][j] = trans[color1][i - 1][j];
            return -1;
        }
        else
        {
            int h1, h2;
            maxHeight[color1] = h1 = maxHeight[color1] + transCount[color2];//从color1处移动count1去color2
            maxHeight[color2] = h2 = maxHeight[color2] + transCount[color1];

            if (h1 > MAPHEIGHT) return color1;
            if (h2 > MAPHEIGHT) return color2;

            int i, j;
            for (i = h2; i > transCount[color1]; i--)
                for (j = 1; j <= MAPWIDTH; j++)
                    gridInfo[color2][i][j] = gridInfo[color2][i - transCount[color1]][j];

            for (i = transCount[color1]; i > 0; i--)
                for (j = 1; j <= MAPWIDTH; j++)
                    gridInfo[color2][i][j] = trans[color1][i - 1][j];

            for (i = h1; i > transCount[color2]; i--)
                for (j = 1; j <= MAPWIDTH; j++)
                    gridInfo[color1][i][j] = gridInfo[color1][i - transCount[color2]][j];

            for (i = transCount[color2]; i > 0; i--)
                for (j = 1; j <= MAPWIDTH; j++)
                    gridInfo[color1][i][j] = trans[color2][i - 1][j];

            return -1;
        }
    }

    // 颜色方还能否继续游戏
    INLINE bool canPut(int color, int blockType)
    {
        Tetris t(blockType, color);
        for (int y = MAPHEIGHT; y >= 1; y--)
            for (int x = 1; x <= MAPWIDTH; x++)
                for (int o = 0; o < 4; o++)
                {
                    t.set(x, y, o);
                    if (t.isValid() && checkDirectDropTo(color, blockType, x, y, o))
                        return true;
                }
        return false;
    }

    // 打印场地用于调试
    void printField()
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
                clog << i2s[gridInfo[0][y][x] + 2];
            for (int x = 0; x <= MAPWIDTH + 1; x++)
                clog << i2s[gridInfo[1][y][x] + 2];
            clog << endl;
        }
#endif
    }
}
