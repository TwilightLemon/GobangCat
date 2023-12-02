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
    this->BenefitPlayer=player;
    this->maxDepth=depth;
    this->root=new ChessNode();
    this->root->map=MapData;
    this->root->score=INT_MIN;
    this->root->depth=0;
    this->root->parent= nullptr;
    //第一层节点由对手下产生
    //root下一层的可走点根据现有棋局产生
    this->root->NextAvaPoints=this->AvaPointGenerator(MapData);
    this->root->whose=Opponent(player);
    }

void ShowMap(const ChessMap& map,int depth,int score,Point lastp){
    cout<<"Depth "<<depth<< "  "<<"LastPoint:"<<lastp.x<<","<<lastp.y<<endl;
    cout<<"Score: "<<score<<endl;
    int count=0;
    for(int x=0;x<15;x++){
        for(int y=0;y<15;y++){
            if(map[y][x]==PieceStatus::None)
                cout<<"- ";
            else if(map[y][x]==PieceStatus::Black) {
                cout << "B ";
                count++;
            }
            else {
                cout << "W ";
                count++;
            }
        }
        cout<<endl;
    }
    cout<<"Piece Count: "<<count<<endl;
}
Point ChessTree::AlphaBetaSearch(){
    InfoBoard::CatSays("开始搜索博弈树...");
    auto begin=high_resolution_clock::now();
    ChessNode* result;
    if(this->root->NextAvaPoints.size()==1){
        //如果root节点只有一个点，则返回root的第一个子节点
        InfoBoard::CatSays("算了吧 没什么可搜的ㄟ( ▔, ▔ )ㄏ",TextColor::Green);
        return root->NextAvaPoints[0];
    }
    for(const auto& point:root->NextAvaPoints){
        InfoBoard::CatSays("尝试走点: "+to_string(point.x)+" "+to_string(point.y));
    }
    //拷贝一份map
    auto parent=this->root;
    auto newMap=new PieceStatus[15][15];
    for(int x=0;x<15;x++){
        for(int y=0;y<15;y++){
            newMap[x][y]=parent->map[x][y];
        }
    }

    const int depth=this->maxDepth;
    int count=0;
    while(true) {
        if(parent->NextAvaPoints.empty()) {
            //如果该层节点没有可走点(已检索完成)，则向上一层
            if(parent->depth==0) {
                //查找root->children中最大的score
                int maxScore=INT_MIN;
                for(const auto& node:parent->children) {
                    auto newMape=new PieceStatus[15][15];
                    for(int x=0;x<15;x++){
                        for(int y=0;y<15;y++){
                            newMape[x][y]=root->map[x][y];
                        }
                    }
                    newMape[node->point.x][node->point.y]=node->whose;

                    InfoBoard::CatSays("Point " + to_string(node->point.x) + " " + to_string(node->point.y) + " Score:" +to_string(node->score));
                    string road="Road: ";
                    auto ptr=node;
                    while(ptr->children.size()!=0){
                        ptr=ptr->children[0];
                        newMape[ptr->point.x][ptr->point.y]=ptr->whose;
                        road.append(ptr->whose==PieceStatus::Black?"B":"W");
                        road.append(ptr->depth%2==1?"+":"-");
                        road.append(to_string(ptr->point.x)+" "+to_string(ptr->point.y)+" -> ");
                    }
                    road.append("end");
                    ShowMap(newMape,depth,node->score,node->point);
                    InfoBoard::CatSays(road);

                }
                for(const auto& node:parent->children){
                    if(node->score>maxScore&&node->score!=INT_MAX){
                        maxScore=node->score;
                        result=node;
                    }
                }
                break;
            }
            parent = parent->parent;
            continue;
        }
        //从parent节点开始，一直生成到第depth-1层
        int i;
        for (i = parent->depth+1; i <= depth-1; i++) {
            if(parent->NextAvaPoints.empty()) {
               /* cout<<"parent已经没有可以走的点了，则撤销更改、返回上一层、重入AvaPoint"<<endl;*/
                Point p=parent->point;
                newMap[p.x][p.y] = PieceStatus::None;
                parent = parent->parent;
                parent->NextAvaPoints.push_back(p);
                break;
            }
            auto node = new ChessNode();
            Point p = parent->NextAvaPoints[0];
            parent->NextAvaPoints.erase(parent->NextAvaPoints.begin());
            node->depth = i;
            node->parent = parent;
            node->point = p;
            node->whose = Opponent(parent->whose);
            newMap[p.x][p.y] = node->whose;
            //cout<<"深入节点"<<endl;
            //ShowMap(newMap,i);
            //如果是己方节点(MIN节点)，则初始化为最大值，否则初始化为最小值
            node->score = node->whose== this->BenefitPlayer ? INT_MAX:INT_MIN;
            node->map = newMap;
            //该层节点玩家
            node->NextAvaPoints = this->AvaPointGenerator(newMap);
            parent->children.push_back(node);
            parent = node;
        }
        while (!parent->NextAvaPoints.empty()) {
            /*cout<<"最后一层节点"<<endl;*/
            Point p = parent->NextAvaPoints[0];
            parent->NextAvaPoints.erase(parent->NextAvaPoints.begin());
            auto node=new ChessNode();
            node->point=p;
            node->depth=i;
            node->parent=parent;
            node->whose= Opponent(parent->whose);
            parent->children.push_back(node);
            newMap[p.x][p.y] = node->whose;
            //计算该节点分数
            int score = this->Evaluator(this->BenefitPlayer, newMap);
/*            cout<<"估分节点:"<<endl;
            ShowMap(newMap,i,score,p);*/
            count++;
            node->score = score;
            //更新alpha beta
            // 在遍历子节点时，更新Alpha值和Beta值
            if (parent->whose == this->BenefitPlayer) { // 如果父节点由我方下，则父节点是Min节点
                if (node->score < this->beta) {
                    this->beta = node->score; // 更新Beta值
                }
            } else { // 如果父节点是Max节点
                if (node->score > this->alpha) {
                    this->alpha = node->score; // 更新Alpha值
                }
            }

            //可以不考虑胜利节点，因为可走点由生成器提供
            //胜利时可走点生成器不会提供下一步点

            //回溯
            auto IntroPtr = parent;
            stack<ChessNode*> IntroRoad;
            bool skip=false;
            while(IntroPtr->depth!=0){
                if (IntroPtr->whose != this->BenefitPlayer){//我方局，选择对我方有利的Max节点
                    if(score>IntroPtr->score&&score>=this->alpha){
                        IntroPtr->score=score;
                    }else{
                        if(IntroPtr->depth!=parent->depth) {
                            //剪枝
                            skip = true;
                            break;
                        }
                    }
                }
                else{//对手局，对手倾向于选择对我方不利的点(Min节点)
                    if(score<IntroPtr->score&&score<=this->beta){
                        IntroPtr->score=score;
                    }else{
                        if(IntroPtr->depth!=parent->depth) {
                            //剪枝
                            skip = true;
                            break;
                        }
                    }
                }
                IntroRoad.push(IntroPtr);
                IntroPtr=IntroPtr->parent;
            }
            //更新完成之后撤销更改
            newMap[p.x][p.y] = PieceStatus::None;
            //如果发生剪枝，则不再生成该节点的子节点
            if(skip) {
                auto pre=IntroRoad.top();
                while(!IntroRoad.empty()){
                    parent=IntroRoad.top();
                    IntroRoad.pop();
                    parent->NextAvaPoints.clear();
                    newMap[parent->point.x][parent->point.y]=PieceStatus::None;
                }
                parent=pre->parent;
                break;
            }
        }
        //撤销更改并向上一层
        newMap[parent->point.x][parent->point.y] = PieceStatus::None;
        if(parent->depth!=0)
          parent = parent->parent;
    }
    delete[] newMap;
    auto end=high_resolution_clock::now();
    InfoBoard::CatSays("已经暴力搜索"+to_string(count)+"个对局了！");
    InfoBoard::CatSays("搜索完成，耗时"+to_string(duration_cast<milliseconds>(end-begin).count())+"ms");
    if(count==0)return Point{0,0};
    return result->point;
}