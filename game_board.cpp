#ifndef SINGLEFILE

#include "game_board.h"
#include "board.h"

#endif

INLINE game_board::game_board() {
// 围一圈护城河
    int i;
    for (i = 0; i < MAPHEIGHT + 2; i++) {
        gridInfo[i][0] = gridInfo[i][MAPWIDTH + 1] = -2;
    }
    for (i = 0; i < MAPWIDTH + 2; i++) {
        gridInfo[0][i] = gridInfo[MAPHEIGHT + 1][i] = -2;
    }
}

// 检查能否从场地顶端直接落到当前位置
bool game_board::checkDirectDropTo(int blockType, int x, int y, int o) {
    auto &def = blockShape[blockType][o];
    for (; y <= MAPHEIGHT; y++)
        for (int i = 0; i < 4; i++) {
            int _x = def[i].x + x, _y = def[i].y + y;
            if (_y > MAPHEIGHT)
                continue;
            if (_y < 1 || _x < 1 || _x > MAPWIDTH || gridInfo[_y][_x])
                return false;
        }
    return true;
}

// 消去行
void game_board::eliminate() {
    sbyte &count = transCount = 0;
    int i, j, emptyFlag, fullFlag;
    maxHeight = MAPHEIGHT;
    for (i = 1; i <= MAPHEIGHT; i++) {
        emptyFlag = 1;
        fullFlag = 1;
        for (j = 1; j <= MAPWIDTH; j++) {
            if (gridInfo[i][j] == 0)
                fullFlag = 0;
            else
                emptyFlag = 0;
        }
        if (fullFlag) {
            for (j = 1; j <= MAPWIDTH; j++) {
                // 注意这里只转移以前的块，不包括最后一次落下的块（“撤销最后一步”）
                trans[count][j] = gridInfo[i][j] == 1 ? 1 : 0;
                gridInfo[i][j] = 0;
            }
            count++;
        } else if (emptyFlag) {
            maxHeight = i - 1;
            break;
        } else
            for (j = 1; j <= MAPWIDTH; j++) {
                gridInfo[i - count][j] =
                        gridInfo[i][j] > 0 ? 1 : gridInfo[i][j];
                if (count)
                    gridInfo[i][j] = 0;
            }
    }
    maxHeight -= count;
    elimTotal += elimBonus[count];
}


// 颜色方还能否继续游戏
INLINE bool game_board::canPut(int blockType) {
    for (int y = MAPHEIGHT; y >= 1; y--)
        for (int x = 1; x <= MAPWIDTH; x++)
            for (int o = 0; o < 4; o++) {
                if (is_valid(blockType, x, y, o) && checkDirectDropTo(blockType, x, y, o))
                    return true;
            }
    return false;
}



bool game_board::is_valid(int ty, int x, int y, int o) const {

    auto &shape = blockShape[ty];
    if (o < 0 || o > 3)
        return false;

    int i, tmpX, tmpY;
    for (i = 0; i < 4; i++) {
        tmpX = x + shape[o][i].x;
        tmpY = y + shape[o][i].y;
        if (tmpX < 1 || tmpX > MAPWIDTH ||
            tmpY < 1 || tmpY > MAPHEIGHT ||
            gridInfo[tmpY][tmpX] != 0)
            return false;
    }
    return true;
}

bool game_board::is_on_ground(int ty, int x, int y, int o) const {
    return is_valid(ty, x, y, o) && !is_valid(ty, x, y - 1, o);
}

bool game_board::place(int ty, int x, int y, int o) {
    if (!is_on_ground(ty, x, y, o))
        return false;
    auto &shape = blockShape[ty];
    int i, tmpX, tmpY;
    for (i = 0; i < 4; i++) {
        tmpX = x + shape[o][i].x;
        tmpY = y + shape[o][i].y;
        gridInfo[tmpY][tmpX] = 2;
    }
    return true;
}

/*

bool element::rotation(const game_board& gb, int o) {
    if (o < 0 || o > 3)
        return false;

    if (orientation == o)
        return true;

    int fromO = orientation;
    while (true) {
        if (!isValid(gb, -1, -1, fromO))
            return false;

        if (fromO == o)
            break;

        fromO = (fromO + 1) % 4;
    }
    return true;
}
 *
 * */
game_board::operator board()
{
    board b;
    for (int i = 1;i <= MAPHEIGHT; ++i)
        for (int j = 1; j <= MAPWIDTH; ++j)
            b.lines[MAPHEIGHT - i] |= cell_masks[j - 1];
    return b;
}
