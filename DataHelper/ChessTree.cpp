//
// Created by cz241 on 11/21/2023.
//

#include <iostream>
#include "ChessTree.h"
#include "../InfoBoard.h"
#include <chrono>
using namespace std::chrono;

void ChessTree::GenerateTree(int depth, PieceStatus player) {
    InfoBoard::CatSays("待我耕耘一颗树...ᓚᘏᗢ");
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
    }

Point ChessTree::AlphaBetaSearch()const{
    InfoBoard::CatSays("开始搜索博弈树...");
    auto begin=high_resolution_clock::now();
    ChessNode* result;
    if(this->root->NextAvaPoints.size()==1){
        //如果root节点只有一个点，则返回root的第一个子节点
        InfoBoard::CatSays("算了吧 没什么可搜的ㄟ( ▔, ▔ )ㄏ",TextColor::Green);
        return root->NextAvaPoints[0];
    }
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
    int count=0;
    while(true) {
        if(parent->NextAvaPoints.empty()) {
            //如果该层节点没有可走点(已检索完成)，则向上一层
            if(parent->depth==0) {
                //查找root->children中最大的score
                int maxScore=INT_MIN;
                for(const auto& node:parent->children) {
                    InfoBoard::CatSays("Point " + to_string(node->point.x) + " " + to_string(node->point.y) + " Score:" +to_string(node->score));
                    string road="Road: ";
                    auto ptr=node->children[0];
                    while(ptr->children.size()!=0){
                        road.append(to_string(ptr->point.x)+" "+to_string(ptr->point.y)+" -> ");
                        ptr=ptr->children[0];
                    }
                    road.append("end");
                    InfoBoard::CatSays(road);
                }
                for(const auto& node:parent->children){
                    if(node->score>maxScore){
                        maxScore=node->score;
                        result=node;
                    }
                }
                break;
            }
            parent = parent->parent;
            continue;
        }
        for (int i = parent->depth+1; i <= depth; i++) {
            //从parent节点开始，一直生成到第depth层
            nodePlayer = Opponent(nodePlayer);
            auto node = new ChessNode();
            Point p = parent->NextAvaPoints[0];
            parent->NextAvaPoints.erase(parent->NextAvaPoints.begin());
            node->depth = i;
            node->parent = parent;
            node->point = p;
            newMap[p.x][p.y] = nodePlayer;
            //如果是己方节点(Max)，则初始化分数为最小值，否则为最大值
            node->score = nodePlayer== this->BenefitPlayer ? INT_MIN : INT_MAX;
            node->map = newMap;
            //该层节点玩家
            node->whose = nodePlayer;
            node->NextAvaPoints = this->AvaPointGenerator(newMap);
            parent->children.push_back(node);
            parent = node;
        }
        while (!parent->NextAvaPoints.empty()) {
            //最后一层节点
            nodePlayer = Opponent(nodePlayer);
            Point p = parent->NextAvaPoints[0];
            parent->NextAvaPoints.erase(parent->NextAvaPoints.begin());
            newMap[p.x][p.y] = nodePlayer;
            //计算该节点分数
            int score = this->Evaluator(this->BenefitPlayer, newMap);
            count++;
            auto IntroPtr = parent;
            bool skip=false;
            while(IntroPtr->depth!=0){
                if (IntroPtr->whose == this->BenefitPlayer){//Max节点
                    if(score>IntroPtr->score){
                        IntroPtr->score=score;
                    }else{
                        if(IntroPtr->depth!=parent->depth) {
                            //剪枝
                            skip = true;
                            break;
                        }
                    }
                }else{//Min节点
                    if(score<IntroPtr->score){
                        IntroPtr->score=score;
                    }else{
                        if(IntroPtr->depth!=parent->depth) {
                            //剪枝
                            skip = true;
                            break;
                        }
                    }
                }
                IntroPtr=IntroPtr->parent;
            }
            //更新完成之后撤销更改
            newMap[p.x][p.y] = PieceStatus::None;
            //如果发生剪枝，则不再生成该节点的子节点
            if(skip) {
                parent->NextAvaPoints.clear();
                break;
            }
        }
        //撤销更改并向上一层
        newMap[parent->point.x][parent->point.y] = PieceStatus::None;
        parent = parent->parent;
    }
    auto end=high_resolution_clock::now();
    InfoBoard::CatSays("已经暴力搜索"+to_string(count)+"个对局了！");
    InfoBoard::CatSays("搜索完成，耗时"+to_string(duration_cast<milliseconds>(end-begin).count())+"ms");
    return result->point;
}