#pragma once

class CGen {
public:
    double weight[featureDimensions]; // 权重
    double fitness; // 评价函数
    double lineCleared; // 仅用于输出
    double lifeMove; // 仅用于输出

    CGen() {
        memset(weight, 0, sizeof(weight));
        fitness = 0;
    }

    CGen(const CGen &another) {
        fitness = another.fitness;
        lineCleared = another.lineCleared;
        lifeMove = another.lifeMove;
        for (int i = 0; i < featureDimensions; i++)
            weight[i] = another.weight[i];
    }

    void unit() {
        // 单位化

        double squareSum = 0;
        for (int i = 0; i < featureDimensions; i++)
            squareSum += weight[i] * weight[i];
        for (int i = 0; i < featureDimensions; i++) {
            weight[i] = 1000 * (weight[i] / sqrt(squareSum));
        }
    }

    CGen(int _in) {
        // 用于产生新的物种

        fitness = lineCleared = lifeMove = 0;
        if (_in == 1) {
            double squareSum = 0;
            for (int i = 0; i < featureDimensions; i++) {
                weight[i] = get_int_random(7031);
                squareSum += weight[i] * weight[i];
            }
            for (int i = 0; i < featureDimensions; i++) {
                weight[i] = 1000 * (weight[i] / sqrt(squareSum));
                if(i!=2) weight[i]=-weight[i];
            }
        }
    }


    double evaluate(CBlock &tempBlock) {
        // 获得估价

        tempBlock.get_feature();
        double ans = 0;
        for (int i = 0; i < featureDimensions; i++)
            ans += weight[i] * tempBlock.features[i];
        return;
    }
};
