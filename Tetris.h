//
// Created by prwang on 2017/4/22.
//

#ifndef TETRIS_TETRIS_H
#define TETRIS_TETRIS_H
#include "shared.h"


class Tetris
{
public:
    const int blockType;   // 标记方块类型的序号 0~6
    int blockX;            // 旋转中心的x轴坐标
    int blockY;            // 旋转中心的y轴坐标
    int orientation;       // 标记方块的朝向 0~3
    const int(*shape)[8]; // 当前类型方块的形状定义
    int color;
    Tetris(int t, int color) : blockType(t), shape(blockShape[t]), color(color) { }

    INLINE Tetris &set(int x = -1, int y = -1, int o = -1);

    // 判断当前位置是否合法
    INLINE bool isValid(int x = -1, int y = -1, int o = -1);


    // 将方块放置在场地上
    INLINE bool place();


    // 判断是否落地
    INLINE bool onGround();
    // 检查能否逆时针旋转自己到o
    INLINE bool rotation(int o);
};

#endif //TETRIS_TETRIS_H
