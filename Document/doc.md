# Project GobangCat
#### powered by TwilightLemon
这是一只不怎么会下五子棋的人工智能猫咪，棋法古怪，棋力看心情
## 需求
游戏体验：
- [x] 1.基本游戏功能（下子、悔棋、重开）
- [x] 2.可自由选择玩家、先手
- [x] 3.AI能打过我
- [x] 4.可选难度  
- [x] 5.趣味性

开发体验：
- [x] 1.代码风格规范统一，易读
- [x] 2.代码结构清晰，模块化
- [x] 3.易于维护、拓展、调试、复用

## 设计与实现
### 遵从几个基本设计原则：  
- 源码组织 - 模块化、低耦合、高内聚
- - 按照功能分模块，依类别建立文件夹
- 代码风格 - 一致性、可读性、可维护性  
- - 命名规范 - 驼峰命名法(略偏向C#/Java)
- - 链式调用，例如：
```c++
    Players[1] = (new ChessTreeRobot())
    ->SetPlayer(PieceStatus::Black)
    ->SetEnableTreeSearch(true)
    ->SetEvaluator(EvaluatorType::ModelChecking)
    ->SetDynamicSetter([](int& depth,int &root, int &child) {...});
```
- MVVM设计模式
```mermaid
graph TD;
    Model[Model:算法逻辑,即IPlayer逻辑]-->ViewModel[ViewModel:数据储存MapData]
    ViewModel-->Model
    View[View:BoardDrawer UI呈现和基础游戏操作]<-->ViewModel
```
- 面对对象原则 - 封装、继承、抽象、多态
```mermaid
graph TD;
    ip(interface:\nIPlayer)
    subgraph Composition
    ip-.虚函数.->NextPoint函数
    ip-.虚函数.->Break
    ip-.成员.->EnableAsync
    ip-.成员.->PlayerColor
    end;
    ip-->p(派生)--> HumanPlayer
    p--> ct[ChessTreeRobot]
    p--> RandomRobot
```

### 1.整体结构
#### 主要全局变量 (Model Data)
```mermaid
    graph TD
        q[MapData]
        CurrentPlayer
        StepHistory
```
#### 游戏基本逻辑构架
```mermaid
graph LR;
    main[主函数]<--查询数据-->board[ChessBoard\n响应操作和绘制UI]-->Round函数\n进行一轮对局--async-->ip1
    main--游戏操作-->board
    ip[抽象接口IPlayer]-.虚函数.->ip1(NextPoint函数)
```
### 2.ChessTree:博弈树搜索算法
#### 组成
```mermaid
graph TD;
    tree[ChessTree]-.成员.->e(Evaluator)
    tree-.成员.->g[AvaPointGenerator]
    tree-.成员.->who[BenefitPlayer]
    tree-.成员.->dep[MaxDepth]
    tree-.方法.->GenerateTree
    tree-.方法.->AlphaBetaSearch
```
#### 如何走？ (深度优先)
```mermaid
graph TD;
    GenerateTree--生成根节点-->root[root];
    root-->g(AvaPointGenerator)--生成-->下一步可走点集-->在newMap中模拟落子-->if{是否达到最大深度\n或不可再走}-->|是|eva(Evaluator)-->评估值
    if-->|否|g
```
#### 博弈树结构——动态而不确定
整棵树结构如下：
```mermaid
graph TD
    A[Root] --> B1[Min]
    A --> B2[Min]
    B1 --> C1[Max]
    B1 --> C2[没有可走点]
    B2 --> C3[Black胜出]
    B2 -..-> C4[Max]
    C1 --> D1[Min]
    C1 --> D2[Min]
    C2 -..-> D3[Min]
    C2 -..-> D4[Min]
    C4 -..-> D7[Min]
    C4 -..-> D8[Min]
    D1 --> E1[Max]
    D1 --> E2[Max]
    D2 --> E3[Max]
    D2 --> E4[Max]
    D3 -..-> E5[Max]
    D3 -..-> E6[Max]
    D4 -..-> E7[Max]
    D7 -..-> E13[Max]
    D7 -..-> E14[Max]
    D8 -..-> E15[Max]
    D8 -..-> E16[Max]
```
节点结构如下：
```mermaid
graph LR;
    node[ChessNode]-.成员.->map[map]
    node-.成员.->p[Point]
    node-.成员.->NextAvaPoints[NextAvaPoints]
    node-.成员.->alpha[alpha]
    node-.成员.->beta[beta]
    node-.成员.->depth[depth]
    node-.成员.->Whose[Whose]
    node-.成员.->parent[parent]
    node-.成员.->children[children]
```

##### 动态生成和遍历树——根->枝->叶
```mermaid
graph 
    A[开始] --> B[生成根节点]
    B --> C{检查root节点的可走点数量}
    C -->|只有一个可走点| D[返回该点]
    C -->|有多个可走点| E[对每一个可走点进行搜索]
    E --> F[拷贝一份map]
    F --> G[进入循环]
    G --> H{检查剪枝条件}
    H -->|满足剪枝条件| I[清空可走点]
    H -->|不满足剪枝条件| J{检查可走点和深度}
    J -->|可走点为空且深度为0| K[查找最大的score并返回对应的点]
    J -->|可走点不为空| L[生成到第depth-1层]
    L --> M{检查parent节点的可走点}
    M -->|没有可走的点| N[撤销更改,返回上一层,重入AvaPoint]
    M -->|有可走的点| O[生成叶子节点,计算该节点分数,更新父节点的alpha或beta]
    O --> P{检查是否发生剪枝}
    P -->|发生剪枝| Q[不再生成parent的其它子节点]
    P -->|未发生剪枝| R[撤销更改并向上一层,同时传递alpha或beta]
    R --> G
    Q --> G
    N --> G
    K --> Z[结束]
    D --> Z
```

##### α-β搜索规则：
- 1.root由对手下子得到，则下一层为我方，选择最有利我方的节点，故root为Max节点  
- 2.上一层为我方下，则本层由对手下，选择对我方最不利节点，故为Min节点，依此类推
- 3.每生成下一个子节点，传递alpha和beta值，用于剪枝
- 4.直到达到最大深度或无子可下，由Evaluator评估后更新alpha或beta值
- 5.回溯更新：对于Max节点，alpha=max{本节点alpha,child.alpha,child.beta}；对于Min节点，beta=min{本节点beta,child.alpha,child.beta}
- 6.剪枝条件：alpha>=beta，不再生成子节点
- 7.搜索完成之后，检索root的子节点，选择alpha值最大且未被剪枝的节点

###### 具体在算法中有两处响应剪枝:
- 生成叶节点时触发
- 回溯更新时触发
### 3.ModelChecker:走子预判和评估
#### 组成
```mermaid
graph TD;
        checker-.方法.->get[GetAvaPoint]-->match-->AvaPointGenerator
        checker[ModelChecker]-.方法.->match[CheckModel]
        checker-.方法.->eva[Evaluate]-->match-->Evaluator
```
两个方法都会通过CheckModel来检查匹配到的模型，
每个模型结构如下：
```mermaid
graph LR;
    model[ChessModel]-.成员.->Type[Type:模型类型]
    model-.成员.->Ava[Ava:可走点集]
    model-.成员.->w[Whose:该模型属于哪方]
    model-.成员.->p[Points:符合模型的点坐标集]
```
这些模型指示双方可走的点位以及危险等级。
#### CheckModel方法:传统的眠活冲模型以及二维模型检测
```mermaid
graph TD;
    a[遍历棋局]-->b[根据眠活冲模型匹配]-->c[检索重复的可走点]-->d[以相交的两个模型创建二维模型]
```
一些细节:
```c++
for (const auto &item: Checked) {
         //平行的重复点不是Cube模型
         int vec_x = item.Ava.x - item.Point.x, vec_y = item.Ava.y - item.Point.y;
         int inc_x = p.x - model.Points[0].x, inc_y = p.y - model.Points[0].y;
         //要求两模型为同一玩家
         if (item.Whose == model.Whose && item.Ava.Equal(p) && (vec_x * inc_y != vec_y * inc_x)) {
                 found = true;
                 foundModel = item;
                 break;
         }
}
```
#### GetAvaPoint方法:根据模型生成可走点
```mermaid
graph TD;
    a[通过ModelChecker生成可走点]--匹配条件-->c[不重复 \ 不超过单个模型可走点量 \ 不超过双方模型可走总量]
```
#### Evaluate方法:根据模型评估局势
```mermaid
graph TD;
    a[通过ModelChecker生成模型]-->c[根据模型类型进行赋分]
```

## 优化
### 1.棋盘数据和树的构建
```mermaid
graph TD;
    first==低效==>first
    first-.高内存占用.->first
    first(初代: 每个节点生成一个新Map\n树由GenerateTree一次性全部生成到最大深度)-->second(优化: 游戏数据与树的虚拟棋盘分离, 每棵树只需要用一个Map,\n树由AlphaBetaSearch逐层生成, 同时更新Map,回溯时撤销更改)
    second-.优势.->adv(解除了游戏棋盘数据和树的耦合, 降低了内存占用, 有利于异步操作)
    
    f[发现bug:\n 按照搜索规则发现无路可走时会摆烂]-->s(解决: 搜索完成发现我方可能必输,则进行人工评判)
```
### 2.评估函数和可走点生成器
内置不同的评估函数(和可走点生成器)，以达到不同难度的AI
这里着重介绍ModelChecker, CountingEvaluator就是个简单的计数计分，不再赘述

```mermaid
graph TD;
    first(初代: 简单的眠 活 冲模型检测器\n依据模型可知道双方可走点)-->second(优化: 加入二维模型检测, 和一些人工优化\n例如优先走点, 对生成模型进行价值排序, 限制生成器可走点数量等)
    second-->prob(出现Bug:\n即使已经经过排序, 仍会出现可走点全为对方的情况)-->solve(解决: 分玩家限制可走点数量, 保证生成节点的完整性)
    solve-->two(再次优化:\n根据棋盘复杂度动态调整搜索深度和每轮试子数)
```
### 3.易用性改善
```mermaid
graph TD;
    问题:计算时卡死UI线程-->async(解决:异步操作)-->UI显示加载动画
    async-->CatChat
```
### 4.可维护性改善
```mermaid
graph TD;
    问题:代码杂乱-->使用region分块
    问题:难以调试-->使用InfoBoard配合标准输出显示调试信息
```


