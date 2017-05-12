//
// Created by prwang on 2017/4/22.
//
#ifndef SINGLEFILE
#include "main.h"
#endif

int currBotColor;
int enemyColor;
mt19937 RAND((random_device()) ());



int main() {
    cerr << sizeof(game_manager) << endl;
    // 加速输入
    istream::sync_with_stdio(false);
    int turnID, blockType;

    game_manager g;
    int nextTypeForColor[2];
    cin >> turnID;

    // 先读入第一回合，得到自己的颜色
    // 双方的第一块肯定是一样的
    cin >> blockType >> currBotColor;
    enemyColor = 1 - currBotColor;
    nextTypeForColor[0] = blockType;
    nextTypeForColor[1] = blockType;
    g.type_count[0][blockType]++;
    g.type_count[1][blockType]++;

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
        g.gb[currBotColor].place(currTypeForColor[currBotColor],x, y, o);

        // 我给对方什么块来着？
        g.type_count[enemyColor][blockType]++;
        nextTypeForColor[enemyColor] = blockType;

        // 然后读自己的输入，也就是对方的行为
        // 裁判给自己的输入是对方的最后一步
        cin >> blockType >> x >> y >> o;

        // 对方当时把上一块落到了 x y o！
        g.gb[enemyColor].place(currTypeForColor[enemyColor],x, y, o);

        // 对方给我什么块来着？
        g.type_count[currBotColor][blockType]++;
        nextTypeForColor[currBotColor] = blockType;

        g.fixup();
    }

    int blockForEnemy, finalX, finalY, finalO, val;


    // 遇事不决先输出（平台上编译不会输出）
    g.printField();

    tie(finalX, finalY, finalO, val) = g.gb[currBotColor].
            get_decision(nextTypeForColor[currBotColor]);
    blockForEnemy = g.worst_for_enemy(currBotColor);

    // 决策结束，输出结果（你只需修改以上部分）

    cout << blockForEnemy << " " << finalX << " " << finalY << " " << finalO;

    return 0;
}
