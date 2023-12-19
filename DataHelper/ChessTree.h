//
// Created by cz241 on 11/21/2023.
//

#ifndef C2023_CHALLENGE_CHESSTREE_H
#define C2023_CHALLENGE_CHESSTREE_H


#include <vector>
#include "DataType.hpp"

//博弈树的节点
class ChessNode{
public:
    PieceStatus (*map)[15]; // 棋盘状态
    Point point;// 落子点
    vector<Point> NextAvaPoints; // 下一步可走点
    int depth; // 深度
    int alpha=INT_MIN;
    int beta=INT_MAX;
    PieceStatus whose; // 轮到谁下
    ChessNode* parent; // 父节点
    vector<ChessNode*> children; // 子节点
};

class ChessTree{
    Point Wayout();
public:
    //根据现有局面生成树root并展开第一层
    void GenerateTree(int depth,PieceStatus player);
    //搜索最佳落子点
    [[nodiscard]] Point AlphaBetaSearch();
    //评估对player一方的分数
    int (*Evaluator)(PieceStatus player, const ChessMap& map);
    //中断搜索
    bool Stop=false;
    //获取可走点
    vector<Point> (*AvaPointGenerator)(const ChessMap& map,int MaxCount);
    ChessNode* root;
    //指示对哪一方有利
    PieceStatus BenefitPlayer;
    int MaxDepth;
    int MaxCount;
    int MaxRootCount;
};

#endif //C2023_CHALLENGE_CHESSTREE_H
