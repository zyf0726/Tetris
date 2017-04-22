//
// Created by prwang on 2017/4/22.
//

#include "main.h"
#include "Tetris.h"
#include "util.h"

int currBotColor;
int enemyColor;
int gridInfo[2][MAPHEIGHT + 2][MAPWIDTH + 2] = {0};
int typeCountForColor[2][7] = {0};
mt19937 RAND((random_device()) ());

// 围一圈护城河
inline void init() {
    int i;
    for (i = 0; i < MAPHEIGHT + 2; i++) {
        gridInfo[1][i][0] = gridInfo[1][i][MAPWIDTH + 1] = -2;
        gridInfo[0][i][0] = gridInfo[0][i][MAPWIDTH + 1] = -2;
    }
    for (i = 0; i < MAPWIDTH + 2; i++) {
        gridInfo[1][0][i] = gridInfo[1][MAPHEIGHT + 1][i] = -2;
        gridInfo[0][0][i] = gridInfo[0][MAPHEIGHT + 1][i] = -2;
    }
}

int weird_evaluation(Tetris &currBlock) {
    int i, tmpX, tmpY;
    int tempGrid[MAPHEIGHT + 2][MAPWIDTH + 2] = {0};
    for (int i = 0; i < MAPHEIGHT + 2; i++)
        for (int j = 0; j < MAPWIDTH + 2; j++)
            tempGrid[i][j] = gridInfo[currBlock.color][i][j];
    for (i = 0; i < 4; i++) {
        tmpX = currBlock.blockX + currBlock.shape[currBlock.orientation][2 * i];
        tmpY = currBlock.blockY + currBlock.shape[currBlock.orientation][2 * i + 1];
        tempGrid[tmpY][tmpX] = 2;
    }

    int maxHeight = 0;
    int ttlMass = 0;
    int blockCount = 0;
    int elimCount = 0;
    int weirdGridCount = 0;
    for (int i = 1; i <= MAPHEIGHT; i++) {
        bool lineElimFlag = true;
        for (int j = 1; j <= MAPWIDTH; j++)
            if ((tempGrid[i][j] != 1) && (tempGrid[i][j] != 2))
                lineElimFlag = false;
        if (lineElimFlag) {
            elimCount++;
            for (int k = i; k <= MAPWIDTH - 1; k++)
                for (int j = 1; j <= MAPWIDTH; j++) {
                    tempGrid[k][j] = tempGrid[k + 1][j];
                    tempGrid[k + 1][j] = 0;
                };
        };
    };
    for (int i = 1; i <= MAPHEIGHT; i++) {
        for (int j = 1; j <= MAPWIDTH; j++) {
            if ((tempGrid[i][j] == 1) || (tempGrid[i][j] == 2)) {
                maxHeight = i;
                ttlMass += i;
                blockCount++;
            };

            if (tempGrid[i][j] == 0)
                if (tempGrid[i + 1][j] != 0) {
                    weirdGridCount += 8;
                    if (tempGrid[i + 1][j - 1] != 0)
                        weirdGridCount += 4;
                    if (tempGrid[i + 1][j + 1] != 0)
                        weirdGridCount += 4;
                    if ((tempGrid[i + 1][j - 1] != 0) && (tempGrid[i + 1][j + 1] != 0))
                        weirdGridCount += 8;
                    if ((j == 1) || (j == MAPWIDTH))
                        weirdGridCount += 4;
                };
        };
    };
    return
            -elimCount * (768 + maxHeight * maxHeight / 4)
            + ttlMass * 2
            + (8 + maxHeight) * (4 + maxHeight) * maxHeight
            + weirdGridCount * (400 - maxHeight * maxHeight) / 48;
};

int main() {
    // 加速输入
    istream::sync_with_stdio(false);

    init();

    int turnID, blockType;
    int nextTypeForColor[2];
    cin >> turnID;

    // 先读入第一回合，得到自己的颜色
    // 双方的第一块肯定是一样的
    cin >> blockType >> currBotColor;
    enemyColor = 1 - currBotColor;
    nextTypeForColor[0] = blockType;
    nextTypeForColor[1] = blockType;
    typeCountForColor[0][blockType]++;
    typeCountForColor[1][blockType]++;

    // 然后分析以前每回合的输入输出，并恢复状态
    // 循环中，color 表示当前这一行是 color 的行为
    // 平台保证所有输入都是合法输入
    for (int i = 1; i < turnID; i++) {
        int currTypeForColor[2] = {nextTypeForColor[0], nextTypeForColor[1]};
        int x, y, o;
        // 根据这些输入输出逐渐恢复状态到当前回合

        // 先读自己的输出，也就是自己的行为
        // 自己的输出是自己的最后一步
        // 然后模拟最后一步放置块
        cin >> blockType >> x >> y >> o;

        // 我当时把上一块落到了 x y o！
        Tetris myBlock(currTypeForColor[currBotColor], currBotColor);
        myBlock.set(x, y, o).place();

        // 我给对方什么块来着？
        typeCountForColor[enemyColor][blockType]++;
        nextTypeForColor[enemyColor] = blockType;

        // 然后读自己的输入，也就是对方的行为
        // 裁判给自己的输入是对方的最后一步
        cin >> blockType >> x >> y >> o;

        // 对方当时把上一块落到了 x y o！
        Tetris enemyBlock(currTypeForColor[enemyColor], enemyColor);
        enemyBlock.set(x, y, o).place();

        // 对方给我什么块来着？
        typeCountForColor[currBotColor][blockType]++;
        nextTypeForColor[currBotColor] = blockType;

        // 检查消去
        Util::eliminate(0);
        Util::eliminate(1);

        // 进行转移
        Util::transfer();
    }


    int blockForEnemy, finalX, finalY, finalO;

    // 做出决策（你只需修改以下部分）

    // 遇事不决先输出（平台上编译不会输出）
    Util::printField();

    int minWeird = 33554432;

    Tetris block(nextTypeForColor[currBotColor], currBotColor);
    for (int y = 1; y <= MAPHEIGHT; y++)
        for (int x = 1; x <= MAPWIDTH; x++)
            for (int o = 0; o < 4; o++) {
                if (block.set(x, y, o).isValid() &&
                    block.set(x, y, o).onGround() &&
                    Util::checkDirectDropTo(currBotColor, block.blockType, x, y, o)) {
                    int currWeird = weird_evaluation(block.set(x, y, o));
                    if (currWeird < minWeird) {
                        finalX = x;
                        finalY = y;
                        finalO = o;
                        minWeird = currWeird;
                    };
                }
            }

    determined:
    // 再看看给对方什么好

    int maxCount = 0, minCount = 99;
    for (int i = 0; i < 7; i++) {
        if (typeCountForColor[enemyColor][i] > maxCount)
            maxCount = typeCountForColor[enemyColor][i];
        if (typeCountForColor[enemyColor][i] < minCount)
            minCount = typeCountForColor[enemyColor][i];
    }
    if (maxCount - minCount == 2) {
        // 危险，找一个不是最大的块给对方吧
        for (blockForEnemy = 0; blockForEnemy < 7; blockForEnemy++)
            if (typeCountForColor[enemyColor][blockForEnemy] != maxCount)
                break;
    } else {
        blockForEnemy = RAND() % 7;
    }

    // 决策结束，输出结果（你只需修改以上部分）

    cout << blockForEnemy << " " << finalX << " " << finalY << " " << finalO;

    return 0;
}
