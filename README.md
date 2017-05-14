#程设: Tetris的AI

在https://wiki.botzone.org/index.php?title=Tetris
##用法
提交前使用```make```生成```Submit.cc```


## TODO lists
### include<Tetris-ea>

步骤：
1. (t, x, y, o) -> (T, x, y)
![](https://wiki.botzone.org/images/e/e5/Tetris.Blocks.png)
1. 重写恢复棋盘状态
2. 给Tea加上bfs寻路径操作
3. 重写寻找最优方法：phenotype.cpp:look_ahead

### 确定估价函数

重新编译training/tetris.cpp
并且在mac机房运行。
如果时间紧任务重就用论文里面已经训好的估价参数。

### 加入搜索


