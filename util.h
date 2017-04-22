//
// Created by prwang on 2017/4/22.
//

#ifndef TETRIS_UTIL_H
#define TETRIS_UTIL_H
#ifndef SINGLEFILE
#include "shared.h"
#endif
namespace Util
{

    // 检查能否从场地顶端直接落到当前位置
    INLINE bool checkDirectDropTo(int color, int blockType, int x, int y, int o);
    // 消去行
    void eliminate(int color);

    // 转移双方消去的行，返回-1表示继续，否则返回输者
    int transfer();
    // 颜色方还能否继续游戏
    INLINE bool canPut(int color, int blockType);
    // 打印场地用于调试
    void printField();
}
#endif //TETRIS_UTIL_H
