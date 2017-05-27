//
// Created by prwang on 2017/4/22.
//
#ifndef SINGLEFILE

#include "main.h"
#include "feature_helpers.h"

#endif

int currBotColor;
int enemyColor;

/*
 Feature Value
3. Holes -35
6. Landing height -51
7. Cell transitions -46
8. Deep wells -12
10. Height differences 19
11. Mean height 6
12. ∆ max height 50
13. ∆ holes 25
15. ∆ mean height 17
18. Wells -19
21. Row transitions -38
23. Cumulative wells -42
24. Min height -41
26. Mean - min height -60
 29. Adjacent column holes -155
 */

options opt = {};
int main() {
#ifndef SINGLEFILE
    freopen("in.txt", "r", stdin);
#endif
    initialize_feature_helpers(&opt);


    // 加速输入
    istream::sync_with_stdio(false);
    int turnID, blockType;

    SHAPES nextTypeForColor[2];
    cin >> turnID;

    // 先读入第一回合，得到自己的颜色
    // 双方的第一块肯定是一样的
    cin >> blockType >> currBotColor;
    game_manager g(blockType, currBotColor, glb_fwt, glb_fwt, opt);

    // 然后分析以前每回合的输入输出，并恢复状态
    // 循环中，color 表示当前这一行是 color 的行为
    // 平台保证所有输入都是合法输入
    for (int i = 1; i < turnID; i++) {
        int blockTypeBot, xBot, yBot, oBot;
        int blockTypeEnemy, xEnemy, yEnemy, oEnemy;
        // 根据这些输入输出逐渐恢复状态到当前回合

        // 先读自己的输出，也就是自己的行为
        // 自己的输出是自己的最后一步
        // 然后模拟最后一步放置块
        // 然后读自己的输入，也就是对方的行为
        // 裁判给自己的输入是对方的最后一步

        cin >> blockTypeBot >> xBot >> yBot >> oBot;
        cin >> blockTypeEnemy >> xEnemy >> yEnemy >> oEnemy;
        g.recover(blockTypeBot, xBot, yBot, oBot,
                  blockTypeEnemy, xEnemy, yEnemy, oEnemy);
    }

    // 遇事不决先输出（平台上编译不会输出）
    g.printField();

    int blockForEnemy = g.make_decisions<2>(g.curBotColor);
    // 决策结束，输出结果
    cout << blockForEnemy << " " << best_alt_g.x << " " << best_alt_g.y << " " << best_alt_g.o << endl;

    return 0;
}
