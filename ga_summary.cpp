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

#include "shared.h"

#include<sys/socket.h>
#include<netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

const int linesClearedReward=10;

/*
 * 以下常量用于遗传算法
 * */
const int populationMax = 2000; // 种群规模
const int split_size = 800;
const int popRemainRankPercent = 4; // 前4%名完全保留，不发生突变
const int popAbandonRankPercent = 70; // 没进前70%的全部淘汰
const int popMutationPossible = 8; // 每8个只有一个能发生变异
const int popCrossPossible = 7; // 每6个只有一个能与某一个发生重组
const int gameRound = 2500; // 每次实验的移动次数
const int iterationCount = 1000000; // 迭代步数


//随机数生成器，可能要重写
inline int get_int_random(int mod) {
    return rand() % mod;
}
class CGen {
public:
    int weight[featureDimensions]; // 权重
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
        for (int i = 0; i < featureDimensions; i++)
            weight[i] = another.weight[i];
    }

    void unit() {
        // 单位化

        int squareSum = 0;
        for (int i = 0; i < featureDimensions; i++)
            squareSum += weight[i] * weight[i];
        for (int i = 0; i < featureDimensions; i++) {
            weight[i] = int(10000 * (weight[i] / sqrt(double(squareSum)))) - 5000;
        }
    }

    CGen(int _in) {
        // 用于产生新的物种

        fitness = lineCleared = lifeMove = 0;
        if (_in == 1) {
            int squareSum = 0;
            for (int i = 0; i < featureDimensions; i++) {
                weight[i] = get_int_random(7031);
                squareSum += weight[i] * weight[i];
            }
            for (int i = 0; i < featureDimensions; i++) {
                weight[i] = int(10000 * (weight[i] / sqrt(double(squareSum)))) - 5000;
            }
        }
    }
};

bool cmp(const CGen &s1, const CGen &s2) {
    return s1.fitness > s2.fitness;
}

vector<CGen> round( vector<CGen> popSet, FILE* fp = stdout){

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
            int changeFeature = get_int_random(featureDimensions);
            tempPop.weight[changeFeature] += get_int_random(2000) - 1000;
            tempPop.unit();
            nextPopSet.push_back(tempPop);
        }
        curLuck = get_int_random(popCrossPossible * 10);
        //发生交叉遗传
        if (curLuck < 10) {
            CGen tempPop(popSet[i]);
            int rawGen = get_int_random(popAbandonRank);
            for (int featureID = 0; featureID < featureDimensions; featureID++) {
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
    for (int i = 0; i < featureDimensions; i++)
        printf("%d ", nextPopSet[0].weight[i]);
    printf("\n");
    printf("Its fitness is %d\n", nextPopSet[0].fitness);
    printf("Its life time is %d\n", nextPopSet[0].lifeMove);
    printf("The number of lines it cleared is %d\n", nextPopSet[0].lineCleared);

    //输出本轮迭代最优者
    for(int i=0;i<popRemainRank;i++) {
        for (int j = 0; j < featureDimensions; j++)
            fprintf(fp, "%d ", nextPopSet[i].weight[j]);
        fprintf(fp, "%d %d\n", nextPopSet[i].lifeMove, nextPopSet[i].lineCleared);
        fflush(fp);
    }

    //输出本轮迭代的信息
    printf("\nThe best one in current iteration provides weight:\n");
    for (int i = 0; i < featureDimensions; i++)
        printf("%d ", nextPopSet[0].weight[i]);
    printf("\n");
    printf("Its fitness is %d\n", nextPopSet[0].fitness);
    printf("Its life time is %d\n", nextPopSet[0].lifeMove);
    printf("The number of lines it cleared is %d\n", nextPopSet[0].lineCleared);
    return nextPopSet;
}
struct queue_t
{
    sockaddr_in P()
    {
        unique_lock<mutex> lock(mtx);
        cv.wait(lock, [=] { return !qu.empty(); });
        sockaddr_in ret = qu.front();
        qu.pop();
        return ret;
    }
    void V(sockaddr_in x)
    {
        unique_lock<mutex> lock(mtx);
        qu.push(x);
        cv.notify_all();
    }
private:
    queue<sockaddr_in> qu;
    mutex mtx;
    condition_variable cv;
} que;
typedef vector<CGen>::iterator Iter;
void satellite_interface(sockaddr_in subject, Iter begin, Iter end)
{


    int sockfd=socket(AF_INET,SOCK_STREAM,0);
    if(sockfd==-1)
    {
        printf("socket error\n");
        throw 233;
    }


    int err=connect(sockfd,(struct sockaddr *)&subject, sizeof(subject));
    if(err==-1)
    {
        printf("connect error\n");
        throw 233;
    }
    int n = end - begin;
    send(sockfd, &n, 4, 0);

    unique_ptr<int> buf(new int[n * featureDimensions]);
    int cnt = 0;
    for (Iter x = begin; x != end; ++x)
    {
        for (int j = 0; j < featureDimensions; ++j)
            buf.get()[cnt + j] =  x->weight[j];
        cnt += featureDimensions;
    }
    send(sockfd, buf.get(), sizeof(int) * n * featureDimensions, 0);
    recv(sockfd, buf.get(), sizeof(int) * n * 3, MSG_WAITALL);
    cnt = 0;
    for (Iter x = begin; x != end; ++x)
    {
        x->lifeMove = buf.get()[cnt];
        x->lineCleared = buf.get()[cnt + 1];
        x->fitness = buf.get()[cnt + 2];
        cnt += 3;
    }
    close(sockfd);
    que.V(subject);
}
int main()
{
    int satellite_cnt; cin >> satellite_cnt;
    for (int i = 0; i < satellite_cnt; ++i)
    {
        string s;  uint16_t p;
        cin >> s >> p;
        sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(p);
        addr.sin_addr.s_addr = inet_addr(s.c_str());
        que.V(addr);
    }

    // 随机选取若干个估价
    vector<CGen> popSet;
    for (int i = 0; i < populationMax; i++)
        popSet.push_back(CGen(1));
    FILE* extraOutput = fopen("output_all.txt", "a");
    for (int iteration = 0; iteration < iterationCount; iteration++)
    {
        // 对每个待选估价进行俄罗斯方块试验
        printf("Iteration #%d:\n", iteration);
//todo 用信号量来记录卫星的列表
        vector<unique_ptr<thread>> th;
        for (int i = 0; i < populationMax; i += split_size)
            th.push_back(unique_ptr<thread>(new thread(satellite_interface, que.P(),
                      popSet.begin() + i, popSet.begin() + min(i + split_size, populationMax))));

        for (auto &i : th) i->join();
        popSet = round(popSet, extraOutput);
        printf("Iteration #%d done\n\n\n", iteration);
    }
}