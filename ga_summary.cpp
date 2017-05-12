/*
Features:

WeightedBlocks
VerticallyConnectedHoles
LinesCleared
Roughness
DeepestWell
ColumnsWithHoles

Fitness:

TotalLinesCleared*10-(TotalHeightIncreased*TotalHeightIncreased-RoundRemaining*RoundRemaining)

*/

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <algorithm>

using namespace std;


const int linesClearedReward=10;

/*
 * 以下常量用于遗传算法
 * */
const int populationMax = 2000; // 种群规模
const int popRemainRankPercent = 4; // 前4%名完全保留，不发生突变
const int popAbandonRankPercent = 70; // 没进前70%的全部淘汰
const int popMutationPossible = 8; // 每8个只有一个能发生变异
const int popCrossPossible = 6; // 每6个只有一个能与某一个发生重组
const int gameRound = 2500; // 每次实验的移动次数
const int iterationCount = 1000000; // 迭代步数

const int blockShape[7][4][8] = {
        {{0, 0, 1,  0,  -1, 0,  -1, -1}, {0, 0, 0,  1,  0,  -1, 1,  -1}, {0, 0, -1, 0,  1,  0,  1,  1},  {0, 0, 0,  -1, 0,  1,  -1, 1}},
        {{0, 0, -1, 0,  1,  0,  1,  -1}, {0, 0, 0,  -1, 0,  1,  1,  1},  {0, 0, 1,  0,  -1, 0,  -1, 1},  {0, 0, 0,  1,  0,  -1, -1, -1}},
        {{0, 0, 1,  0,  0,  -1, -1, -1}, {0, 0, 0,  1,  1,  0,  1,  -1}, {0, 0, -1, 0,  0,  1,  1,  1},  {0, 0, 0,  -1, -1, 0,  -1, 1}},
        {{0, 0, -1, 0,  0,  -1, 1,  -1}, {0, 0, 0,  -1, 1,  0,  1,  1},  {0, 0, 1,  0,  0,  1,  -1, 1},  {0, 0, 0,  1,  -1, 0,  -1, -1}},
        {{0, 0, -1, 0,  0,  1,  1,  0},  {0, 0, 0,  -1, -1, 0,  0,  1},  {0, 0, 1,  0,  0,  -1, -1, 0},  {0, 0, 0,  1,  1,  0,  0,  -1}},
        {{0, 0, 0,  -1, 0,  1,  0,  2},  {0, 0, 1,  0,  -1, 0,  -2, 0},  {0, 0, 0,  1,  0,  -1, 0,  -2}, {0, 0, -1, 0,  1,  0,  2,  0}},
        {{0, 0, 0,  1,  -1, 0,  -1, 1},  {0, 0, -1, 0,  0,  -1, -1, -1}, {0, 0, 0,  -1, 1,  -0, 1,  -1}, {0, 0, 1,  0,  0,  1,  1,  1}}
};

//随机数生成器，可能要重写
inline int get_int_random(int mod) {
    return rand() % mod;
}

class CBlock {
public:
    bool blocks[20][10];
    int features[6];

    CBlock() {
        memset(blocks, 0, sizeof(blocks));
        memset(features, 0, sizeof(features));
    };

    CBlock(CBlock &another) {
        for (int i = 0; i < 20; i++)
            for (int j = 0; j < 10; j++)
                blocks[i][j] = another.blocks[i][j];
        for (int i = 0; i < 6; i++)
            features[i] = another.features[i];
    }

    void get_feature() {
        //获得特征

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
        //计算洞的数目（第1维）
        for (int i = 0; i < 19; i++) {
            for (int j = 0; j < 10; j++)
                if ((!blocks[i][j]) && (blocks[i + 1][j]))
                    features[1]++;
        }
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
    }

    bool can_insert(int t, int x, int y, int o) {
        for (int i = 0; i < 4; i++) {
            if (x + blockShape[t][o][2 * i] < 0) return false;
            if (x + blockShape[t][o][2 * i] >= 20) return false;
            if (y + blockShape[t][o][2 * i + 1] < 0) return false;
            if (y + blockShape[t][o][2 * i + 1] >= 10) return false;
            if (blocks[x + blockShape[t][o][2 * i]][y + blockShape[t][o][2 * i + 1]])
                return false;
        }
        return true;
    }

    bool is_valid(int t, int x, int y, int o) {
        if (!can_insert(t, x, y, o)) return false;
        if (can_insert(t, x - 1, y, o)) return false;

        // 判定合法性的还需要改正
        for (int i = 0; i < 4; i++) {
            int curX = x + blockShape[t][o][2 * i];
            int curY = y + blockShape[t][o][2 * i + 1];
            for (int h = curX; h < 20; h++)
                if (blocks[h][curY])
                    return false;
        }
        return true;
    }

    void puts(int t, int x, int y, int o) {
        for (int i = 0; i < 4; i++)
            blocks[x + blockShape[t][o][2 * i]][y + blockShape[t][o][2 * i + 1]] = true;
    }

    /*
    void printBoard() {
        for (int i = 19; i >= 0; i--) {
            for (int j = 0; j < 10; j++)
                if (blocks[i][j])
                    printf("[]");
                else
                    printf("  ");
            printf("\n");
        }
    }
     */
};

class CGen {
public:
    int weight[6]; // 权重
    int fitness; // 评价函数
    int lineCleared; // 仅用于输出
    int lifeMove; // 仅用于输出

    CGen() {
        memset(weight, 0, sizeof(weight));
        fitness = 0;
    }

    CGen(const CGen &another) {
        fitness = another.fitness;
        lineCleared = another.lineCleared;
        lifeMove = another.lifeMove;
        for (int i = 0; i < 6; i++)
            weight[i] = another.weight[i];
    }

    void unit() {
        // 单位化

        int squareSum = 0;
        for (int i = 0; i < 6; i++)
            squareSum += weight[i] * weight[i];
        for (int i = 0; i < 6; i++) {
            weight[i] = int(10000 * (weight[i] / sqrt(double(squareSum)))) - 5000;
        }
    }

    CGen(int _in) {
        // 用于产生新的物种

        fitness = lineCleared = lifeMove = 0;
        if (_in == 1) {
            int squareSum = 0;
            for (int i = 0; i < 6; i++) {
                weight[i] = get_int_random(7031);
                squareSum += weight[i] * weight[i];
            }
            for (int i = 0; i < 6; i++) {
                weight[i] = int(10000 * (weight[i] / sqrt(double(squareSum)))) - 5000;
            }
        }
    }

    int evaluate(CBlock &tempBlock) {
        // 获得估价

        tempBlock.get_feature();
        int ans = 0;
        for (int i = 0; i < 6; i++)
            ans += weight[i] * tempBlock.features[i];
        return ans;
    }
};

bool cmp(const CGen &s1, const CGen &s2) {
    return s1.fitness > s2.fitness;
}

int main() {
    FILE* infp=fopen("experiment.txt","r");
    FILE* newfp=fopen("input.txt","r");
    FILE* fp=fopen("output.txt","w");

    vector<CGen> popSet;
    
    for(int i=0;i<populationMax;i++){
        CGen curGen;
        for(int j=0;j<6;j++)
            scanf("%d",&curGen.weight[j]);
        scanf("%d%d%d",&curGen.lifeMove,&curGen.lineCleared,&curGen.fitness);
        popSet.push_back(curGen);
    }

    //根据Fitness的结果从大到小进行排序
    sort(popSet.begin(), popSet.end(), cmp);
    vector<CGen> nextPopSet;

    int popAbandonRank=populationMax*popAbandonRankPercent/100;
    int popRemainRank=populationMax*popRemainRankPercent/100;

    // 表现不差的保留下来
    for (int i = 0; i < popAbandonRank; i++)
        nextPopSet.push_back(popSet[i]);

    // 表现不是最好也不是最差的有机会发生突变
    for (int i = popRemainRank; i < popAbandonRank; i++) {
        int curLuck = get_int_random(popMutationPossible * 10);
        //发生突变
        if (curLuck < 10) {
            CGen tempPop(popSet[i]);
            int changeFeature = get_int_random(6);
            tempPop.weight[changeFeature] += get_int_random(2000) - 1000;
            tempPop.unit();
            nextPopSet.push_back(tempPop);
        }
        curLuck = get_int_random(popCrossPossible * 10);
        //发生交叉遗传
        if (curLuck < 10) {
            CGen tempPop(popSet[i]);
            int rawGen = get_int_random(popAbandonRank);
            for (int featureID = 0; featureID < 6; featureID++) {
                int featureChange = get_int_random(2);
                if (featureChange == 1) {
                    tempPop.weight[featureID] = popSet[rawGen].weight[featureID];
                }
            }
            tempPop.unit();
            nextPopSet.push_back(tempPop);
        }
    }

    //更新估价集合
    for (int i = nextPopSet.size(); i < populationMax; i++)
        nextPopSet.push_back(CGen(1));
    if (nextPopSet.size() > populationMax) nextPopSet.resize(populationMax);
    popSet = nextPopSet;

    //输出本轮迭代的信息
    printf("\nThe best one in current iteration provides weight:\n");
    for (int i = 0; i < 6; i++)
        printf("%d ", nextPopSet[0].weight[i]);
    printf("\n");
    printf("Its fitness is %d\n", nextPopSet[0].fitness);
    printf("Its life time is %d\n", nextPopSet[0].lifeMove);
    printf("The number of lines it cleared is %d\n", nextPopSet[0].lineCleared);

    //输出本轮迭代最优者
    for(int i=0;i<popRemainRank;i++) {
        for (int j = 0; j < 6; j++)
            fprintf(fp, "%d ", nextPopSet[i].weight[j]);
        fprintf(fp, "%d %d\n", nextPopSet[i].lifeMove, nextPopSet[i].lineCleared);
        fflush(fp);
    }

    //输出新的训练集合
    for(int i=0;i<nextPopSet.size();i++) {
        for (int j = 0; j < 6; j++)
            fprintf(newfp, "%d ", nextPopSet[i].weight[j]);
        fprintf(newfp,"\n");
        fflush(newfp);
    }

}