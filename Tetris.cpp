//
// Created by prwang on 2017/4/22.
//

#include "Tetris.h"

Tetris &Tetris::set(int x, int y, int o) {
    blockX = x == -1 ? blockX : x;
    blockY = y == -1 ? blockY : y;
    orientation = o == -1 ? orientation : o;
    return *this;
}

// 判断当前位置是否合法
bool Tetris::isValid(int x, int y, int o) {
    x = x == -1 ? blockX : x;
    y = y == -1 ? blockY : y;
    o = o == -1 ? orientation : o;
    if (o < 0 || o > 3)
        return false;

    int i, tmpX, tmpY;
    for (i = 0; i < 4; i++) {
        tmpX = x + shape[o][2 * i];
        tmpY = y + shape[o][2 * i + 1];
        if (tmpX < 1 || tmpX > MAPWIDTH ||
            tmpY < 1 || tmpY > MAPHEIGHT ||
            gridInfo[color][tmpY][tmpX] != 0)
            return false;
    }
    return true;
}

// 判断是否落地
inline bool Tetris::onGround() {
    if (isValid() && !isValid(-1, blockY - 1))
        return true;
    return false;
}

// 将方块放置在场地上
bool Tetris::place() {
    if (!onGround())
        return false;

    int i, tmpX, tmpY;
    for (i = 0; i < 4; i++) {
        tmpX = blockX + shape[orientation][2 * i];
        tmpY = blockY + shape[orientation][2 * i + 1];
        gridInfo[color][tmpY][tmpX] = 2;
    }
    return true;
}

// 检查能否逆时针旋转自己到o
bool Tetris::rotation(int o) {
    if (o < 0 || o > 3)
        return false;

    if (orientation == o)
        return true;

    int fromO = orientation;
    while (true) {
        if (!isValid(-1, -1, fromO))
            return false;

        if (fromO == o)
            break;

        fromO = (fromO + 1) % 4;
    }
    return true;
}
