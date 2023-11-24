//
// Created by cz241 on 11/21/2023.
//

#include <iostream>
#include "ChessTree.h"
#include <chrono>
using namespace std::chrono;

void ChessTree::GenerateTree(int depth, PieceStatus player) {
    cout<<"Generating Tree..."<<endl;
    auto begin=high_resolution_clock::now();
    this->BenefitPlayer=player;
    this->maxDepth=depth;
    this->root=new ChessNode();
    this->root->map=MapData;
    this->root->score=INT_MIN;
    this->root->depth=0;
    this->root->parent= nullptr;
    //第一层节点由对手下产生
    PieceStatus nodePlayer=Opponent(player);
    //root下一层的可走点根据现有棋局产生
    this->root->NextAvaPoints=this->AvaPointGenerator(MapData);
    this->root->whose=nodePlayer;
    auto end=high_resolution_clock::now();
    cout<<"Tree Generated!  Time Cost: "<<duration_cast<milliseconds>(end-begin).count()<<"ms"<<endl;
}
void ChessTree::ShowTree()const {
    auto root=this->root;
    stack<ChessNode*> parents;
    parents.push(root);
    for(int i=0;i<this->maxDepth;i++) {
        ChessNode* parent;
        while (!parents.empty()) {
            parent = parents.top();
            for (const auto &node: parent->children) {
                cout << "Depth: " << node->depth << "  Whose: " <<(node->whose==PieceStatus::Black?"Black":"White")<<" Point:"<<node->point.x<<","<<node->point.y << endl;
            }
            parents.pop();
        }
        cout<<"-----------------"<<endl;
        for (const auto &node: parent->children)
            parents.push(node);
    }
}
ChessNode* ChessTree::AlphaBetaSearch()const{
    cout<<"Searching Started."<<endl;
    auto begin=high_resolution_clock::now();
    ChessNode* result;

    //拷贝一份map
    auto parent=this->root;
    auto newMap=new PieceStatus[15][15];
    for(int x=0;x<15;x++){
        for(int y=0;y<15;y++){
            newMap[x][y]=parent->map[x][y];
        }
    }
    PieceStatus nodePlayer=this->root->whose;
    int depth=this->maxDepth;
    while(true) {
        if(parent->NextAvaPoints.empty()) {
            //如果该层节点没有可走点(已检索完成)，则向上一层
            parent = parent->parent;
            if(parent->depth==0) {
                //查找root->children中最大的score
                int maxScore=INT_MIN;
                for(const auto& node:parent->children){
                    if(node->score>maxScore){
                        maxScore=node->score;
                        result=node;
                        break;
                    }
                }
                break;
            }
            continue;
        }
        for (int i = parent->depth+1; i < depth; i++) {
            //从parent节点开始，一直生成到第depth-1层
            nodePlayer = Opponent(nodePlayer);
            auto node = new ChessNode();
            Point p = parent->NextAvaPoints[0];
            parent->NextAvaPoints.erase(parent->NextAvaPoints.begin());
            node->depth = i;
            node->parent = parent;
            node->point = p;
            newMap[p.x][p.y] = nodePlayer;
            node->score = INT_MIN;
            node->map = newMap;
            //该层节点玩家
            node->whose = nodePlayer;
            node->NextAvaPoints = this->AvaPointGenerator(newMap);
            parent->children.push_back(node);
            parent = node;
            //cout<<"ADD NODE!!!  Depth: "<<node->depth<<"  Whose: "<<(node->whose==PieceStatus::Black?"Black":"White")<<" Point:"<<node->point.x<<","<<node->point.y<<endl;
        }
        while (!parent->NextAvaPoints.empty()) {
            //最后一层节点
            nodePlayer = Opponent(nodePlayer);
            Point p = parent->NextAvaPoints[0];
            parent->NextAvaPoints.erase(parent->NextAvaPoints.begin());
            newMap[p.x][p.y] = nodePlayer;
            //计算该节点分数
            int score = this->Evaluator(this->BenefitPlayer, newMap);
            auto IntroPtr = parent;
            //回溯更新父节点分数
            if (parent->whose == this->BenefitPlayer) {//Max结点
                if (score > parent->score)//如果该节点分数大于父节点分数，则更新父节点分数
                    while (IntroPtr->depth != 0) {
                        IntroPtr->score = score;
                        IntroPtr = IntroPtr->parent;
                    }
            } else {//Min结点
                if (score < parent->score)//如果该节点分数小于父节点分数，则更新父节点分数
                    while (IntroPtr->depth != 0) {
                        IntroPtr->score = score;
                        IntroPtr = IntroPtr->parent;
                    }
            }
            //更新完成之后撤销更改
            newMap[p.x][p.y] = PieceStatus::None;
        }
        //撤销更改并向上一层
        newMap[parent->point.x][parent->point.y] = PieceStatus::None;
        parent = parent->parent;
    }

    auto end=high_resolution_clock::now();
    cout<<"Searching Finished!  Time Cost: "<<duration_cast<milliseconds>(end-begin).count()<<"ms"<<endl;
    return result;
}