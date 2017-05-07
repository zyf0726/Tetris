//
// Created by prwang on 2017/4/22.
//
#ifndef SINGLEFILE
#include "Tetris.h"
#endif

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
int Tetris::evaluate(int _curHeight) {
    int  tmpX, tmpY;
    int tempGrid[MAPHEIGHT + 2][MAPWIDTH + 2] = {0};
    for (int i = 0; i < MAPHEIGHT + 2; i++)
        for (int j = 0; j < MAPWIDTH + 2; j++)
            tempGrid[i][j] = gridInfo[color][i][j];
    for (int i = 0; i < 4; i++) {
        tmpX = blockX + shape[orientation][2 * i];
        tmpY = blockY + shape[orientation][2 * i + 1];
        tempGrid[tmpY][tmpX] = 2;
    }


    int curHeight=_curHeight;
    int maxHeight=0;
    int elimCount = 0;
    int rowDifference=0;
    int columnDifference=0;
    int holeCount=0;
    int wellCount=0;

    for (int i = 1; i <= MAPHEIGHT; i++) {
        bool lineElimFlag = true;
        for (int j = 1; j <= MAPWIDTH; j++)
            if ((tempGrid[i][j] != 1) && (tempGrid[i][j] != 2))
                lineElimFlag = false;
        if (lineElimFlag) {
            elimCount++;
            for(int t=i;t<MAPHEIGHT;t++)
                for(int j=1;j<=MAPWIDTH;j++) {
                    tempGrid[t][j] = tempGrid[t][j + 1];
                    tempGrid[t][j + 1] = 0;
                };
        }
    };


    for(int i=1;i<=MAPHEIGHT;i++)
        for(int j=1;j<MAPWIDTH;j++) {
            if ((tempGrid[i][j] == 0) xor (tempGrid[i][j + 1] == 0))
                rowDifference++;
            if(tempGrid[i][j])
                maxHeight=i;
        };

    for(int j=1;j<=MAPWIDTH;j++)
        for(int i=1;i<MAPHEIGHT;i++)
            if((tempGrid[i][j]==0)xor(tempGrid[i+1][j]==0))
                columnDifference++;

    for(int j=1;j<=MAPWIDTH;j++){
        bool startCount=false;
        for(int i=MAPHEIGHT;i>=1;i--){
            if(tempGrid[i][j])
                startCount=true;
            if(startCount&&(tempGrid[i][j]==0))
                holeCount++;
        }
    }

    for(int j=2;j<MAPWIDTH;j++) {
        bool startCount=false;
        for (int i = MAPHEIGHT; i >= 1; i--) {
            if (tempGrid[i][j]) break;
            if ((tempGrid[i][j - 1] == 0) && (tempGrid[i][j + 1] == 0)) startCount=true;
            if(startCount) wellCount++;
        }
    }
    {
        bool startCount=false;
        for (int i = MAPHEIGHT; i >= 1; i--) {
            if (tempGrid[i][1]) break;
            if (tempGrid[i][2] == 0) startCount=true;
            if(startCount) wellCount++;
        }
    }

    {
        bool startCount=false;
        for (int i = MAPHEIGHT; i >= 1; i--) {
            if (tempGrid[i][MAPWIDTH]) break;
            if (tempGrid[i][MAPWIDTH - 1] == 0) startCount=true;
            if(startCount) wellCount++;
        }
    }

#define CUR_H_W 4500
#define ELIM_C_W -5418
#define ELIM_C_H_W -71
#define ROW_DIFF_W 3218
#define COL_DIFF_W 9439
#define HOLE_C_W 7899
#define WELL_C_W 3386

    return
        CUR_H_W*curHeight+
        ELIM_C_W*elimCount+
        (ELIM_C_H_W*maxHeight*maxHeight)*elimCount+
        ROW_DIFF_W*rowDifference+
        COL_DIFF_W*columnDifference+
        HOLE_C_W*holeCount+
        WELL_C_W*wellCount;


};
