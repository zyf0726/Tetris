#ifndef SINGLEFILE
#include "game_board.h"
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
bool game_board::checkDirectDropTo(int blockType, int x, int y, int o)
{
    auto &def = blockShape[blockType][o];
    for (; y <= MAPHEIGHT; y++)
        for (int i = 0; i < 4; i++)
        {
            int _x = def[i * 2] + x, _y = def[i * 2 + 1] + y;
            if (_y > MAPHEIGHT)
                continue;
            if (_y < 1 || _x < 1 || _x > MAPWIDTH || gridInfo[_y][_x])
                return false;
        }
    return true;
}

// 消去行
void game_board::eliminate()
{
    int &count = transCount = 0;
    int i, j, emptyFlag, fullFlag;
    maxHeight = MAPHEIGHT;
    for (i = 1; i <= MAPHEIGHT; i++)
    {
        emptyFlag = 1;
        fullFlag = 1;
        for (j = 1; j <= MAPWIDTH; j++)
        {
            if (gridInfo[i][j] == 0)
                fullFlag = 0;
            else
                emptyFlag = 0;
        }
        if (fullFlag)
        {
            for (j = 1; j <= MAPWIDTH; j++)
            {
                // 注意这里只转移以前的块，不包括最后一次落下的块（“撤销最后一步”）
                trans[count][j] = gridInfo[i][j] == 1 ? 1 : 0;
                gridInfo[i][j] = 0;
            }
            count++;
        }
        else if (emptyFlag)
        {
            maxHeight = i - 1;
            break;
        }
        else
            for (j = 1; j <= MAPWIDTH; j++)
            {
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
INLINE bool game_board::canPut(int blockType)
{
    for (int y = MAPHEIGHT; y >= 1; y--)
        for (int x = 1; x <= MAPWIDTH; x++)
            for (int o = 0; o < 4; o++)
            {
                if (is_valid(blockType, x, y, o) && checkDirectDropTo(blockType, x, y, o))
                    return true;
            }
    return false;
}


tuple<int, int, int> game_board::get_decision(int ty) {

    int  finalX(-1), finalY(-1), finalO(-1);
    int minWeird = 2147483647;
    for (int y = 1; y <= MAPHEIGHT; y++)
        for (int x = 1; x <= MAPWIDTH; x++)
            for (int o = 0; o < 4; o++) {
                if (is_valid(ty, x, y, o) &&
                    is_on_ground(ty, x, y, o)&&
                    checkDirectDropTo(ty, x, y, o)) {
                    game_board copy(*this);
                    copy.place(ty, x, y, o);
                    int currWeird = copy.evaluate(y + blockHeight[ty][o]);
                    if (currWeird < minWeird) {
                        finalX = x;
                        finalY = y;
                        finalO = o;
                        minWeird = currWeird;
                    }
                }
            }
    return tie(finalX, finalY, finalO);
}
bool game_board::is_valid(int ty, int x, int y, int o) const {

    const int (*shape)[8] = blockShape[ty];
    if (o < 0 || o > 3)
        return false;

    int i, tmpX, tmpY;
    for (i = 0; i < 4; i++) {
        tmpX = x + shape[o][2 * i];
        tmpY = y + shape[o][2 * i + 1];
        if (tmpX < 1 || tmpX > MAPWIDTH ||
            tmpY < 1 || tmpY > MAPHEIGHT ||
            gridInfo[tmpY][tmpX] != 0)
            return false;
    }
    return true;
}
bool game_board::is_on_ground(int ty, int x, int y, int o) const
{
    return is_valid(ty, x, y, o) && !is_valid(ty, x, y - 1, o);
}
bool game_board::place(int ty, int x, int y, int o)
{
    if (!is_on_ground(ty, x, y, o))
        return false;
    const int (*shape)[8] = blockShape[ty];
    int i, tmpX, tmpY;
    for (i = 0; i < 4; i++) {
        tmpX = x + shape[o][2 * i];
        tmpY = y + shape[o][2 * i + 1];
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
int game_board::evaluate(int _curHeight) {
    int tempGrid[MAPHEIGHT + 2][MAPWIDTH + 2] = {0};
    for (int i = 0; i < MAPHEIGHT + 2; i++)
        for (int j = 0; j < MAPWIDTH + 2; j++)
            tempGrid[i][j] = gridInfo[i][j];


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


}

