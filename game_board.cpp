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
    int &count = transCount = 0;
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


tuple<int, int, int, int> game_board::get_decision(int ty) {

    int finalX(-1), finalY(-1), finalO(-1);
    int minWeird = 2147483647;
    for (int y = 1; y <= MAPHEIGHT; y++)
        for (int x = 1; x <= MAPWIDTH; x++)
            for (int o = 0; o < 4; o++) {
                if (is_valid(ty, x, y, o) &&
                    is_on_ground(ty, x, y, o) &&
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
    return tie(finalX, finalY, finalO, minWeird);
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
int game_board::evaluate(int _curHeight) {
    bool blocks[MAPHEIGHT][MAPWIDTH] = {};
    for (int i = 0; i < MAPHEIGHT ; i++)
        for (int j = 0; j < MAPWIDTH ; j++)
            blocks[i][j] = (gridInfo[i + 1][j + 1]>0);
    int features[6];
    fill(features, features + 6, 0);
    //计算消去的行（第2维）
    for (int i = 0; i < 20; i++) {
        bool clearFlag = true;
        for (int j = 0; j < 10; j++)
            if (!blocks[i][j]) {
                clearFlag = false;
                break;
            };
        if (clearFlag) {
            features[2]++;
            for (int k = i; k < 19; k++)
                for (int j = 0; j < 10; j++) {
                    blocks[k][j] = blocks[k + 1][j];
                    blocks[k + 1][j] = 0;
                };
        };
    };
    //计算高度加权和（第0维）
    for (int i = 0; i < 20; i++)
        for (int j = 0; j < 10; j++)
            if (blocks[i][j])
                features[0] += i;
    //计算相邻高度差的平方（第3维）
    int maxHeight[10];
    for (int j = 0; j < 10; j++) {
        for (maxHeight[j] = 19; maxHeight[j] >= 0; maxHeight[j]--)
            if (blocks[maxHeight[j]][j])
                break;
        if (j > 0)
            features[3] += (maxHeight[j] - maxHeight[j - 1]) * (maxHeight[j] - maxHeight[j - 1]);
    }
    features[3] += maxHeight[0] * maxHeight[0];
    features[3] += maxHeight[10] * maxHeight[10];
    //计算洞的数目（第1维）
    for (int j = 0; j < 10; j++) {
        for (int i = 0; i < maxHeight[i]; i++)
            if (!blocks[i][j])
                features[1]++;
    }
    //计算最深的井（第4维）
    features[4] = 0;
    for (int j = 1; j < 9; j++) {
        for (int i = 19; i >= 0; i--)
            if (blocks[i][j - 1] && blocks[i][j + 1] && (!blocks[i][j])) {
                int curWell = 1;
                for (int k = i - 1; k > 0; k--) {
                    if (blocks[k][j]) break;
                    curWell++;
                };
                if (curWell > features[4]) features[4] = curWell;
                break;
            }
    }
    for (int i = 19; i >= 0; i--)
        if (blocks[i][1] && (!blocks[i][0])) {
            int curWell = 1;
            for (int k = i - 1; k >= 0; k--) {
                if (blocks[k][0]) break;
                curWell++;
            }
            if (curWell > features[4]) features[4] = curWell;
            break;
        }
    for (int i = 19; i >= 0; i--)
        if (blocks[i][8] && (!blocks[i][9])) {
            int curWell = 1;
            for (int k = i - 1; k >= 0; k--) {
                if (blocks[k][9]) break;
                curWell++;
            }
            if (curWell > features[4]) features[4] = curWell;
            break;
        }
    //计算有洞的列计数（第5维）
    features[5] = 0;
    for (int j = 0; j < 10; j++) {
        int flag = false;
        for (int i = 0; i < maxHeight[j]; i++)
            if (!blocks[i][j]) {
                flag = true;
                break;
            }
        if (flag) features[5]++;
    }
    //以下操作是尽量保证规模上统一
    features[1] *= 16;
    features[2] *= 256;
    features[4] *= 64;
    features[5] *= 32;

    int weight[6]={3408, 2147, -281, 167, 1423, -26};
    int ans=0;
    for(int i=0;i<6;i++)
        ans+=features[i]*weight[i];
    return ans;


}

