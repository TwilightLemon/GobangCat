//
// Created by cz241 on 11/21/2023.
//

#include <iostream>
#include "ChessTree.h"
#include "../InfoBoard.h"
#include "ModelChecker.h"
#include <chrono>

using namespace std::chrono;

void ChessTree::GenerateTree(int depth, PieceStatus player) {
    InfoBoard::CatSays("待我耕耘一颗树...ᓚᘏᗢ");
    this->BenefitPlayer = player;
    this->MaxDepth = depth;
    this->root = new ChessNode();
    this->root->map = MapData;
    this->root->depth = 0;
    this->root->parent = nullptr;
    //第一层节点由对手下产生
    //root下一层的可走点根据现有棋局产生
    this->root->NextAvaPoints = this->AvaPointGenerator(MapData, MaxRootCount);
    this->root->whose = Opponent(player);
}


Point ChessTree::AlphaBetaSearch() {
    InfoBoard::CatSays("开始搜索博弈树...");
    ModelChecker::CheckerTime = 0;
    auto begin = high_resolution_clock::now();
    ChessNode *result = nullptr;
    if (this->root->NextAvaPoints.size() == 1) {
        //如果root节点只有一个点，则返回root的第一个子节点
        InfoBoard::CatSays("算了吧 没什么可搜的ㄟ( ▔, ▔ )ㄏ", TextColor::Green);
        return root->NextAvaPoints[0];
    }
    Point firstPoint = root->NextAvaPoints[0];
    for (const auto &point: root->NextAvaPoints) {
        InfoBoard::CatSays("可走点: " + to_string(point.x) + " " + to_string(point.y));
    }
    //拷贝一份map
    auto parent = this->root;
    auto newMap = new PieceStatus[15][15];
    for (int x = 0; x < 15; x++) {
        for (int y = 0; y < 15; y++) {
            newMap[x][y] = parent->map[x][y];
        }
    }

    const int depth = this->MaxDepth;
    int count = 0;
    while (true) {
        if (Stop)return Point{-1, -1};
        //接受回溯过程中的剪枝
        if (parent->alpha >= parent->beta) {
            parent->NextAvaPoints.clear();
        }
        //搜索完成
        if (parent->NextAvaPoints.empty() && parent->depth == 0) {
            //查找root->children中最大的score
            //#region 生成视图
            int maxScore = INT_MIN;
            for (const auto &node: parent->children) {
                auto newMape = new PieceStatus[15][15];
                for (int x = 0; x < 15; x++) {
                    for (int y = 0; y < 15; y++) {
                        newMape[x][y] = root->map[x][y];
                    }
                }
                newMape[node->point.x][node->point.y] = node->whose;

                InfoBoard::CatSays("Point " + to_string(node->point.x) + " " + to_string(node->point.y));
                string road = "Road: ";
                auto ptr = node;
                while (ptr->children.size() != 0) {
                    ptr = ptr->children[ptr->children.size() - 1];
                    newMape[ptr->point.x][ptr->point.y] = ptr->whose;
                    road.append(ptr->whose == PieceStatus::Black ? "B" : "W");
                    road.append(ptr->depth % 2 == 1 ? "+" : "-");
                    road.append(to_string(ptr->point.x) + " " + to_string(ptr->point.y) + " -> ");
                }
                road.append("end");
                ShowMap(newMape, depth, node->alpha, node->beta, node->point);
                InfoBoard::CatSays(road);

            }
            //#endregion
            for (const auto &node: parent->children) {
                if (node->alpha > maxScore && node->alpha < node->beta) {
                    maxScore = node->alpha;
                    result = node;
                }
            }
            break;
        }
        if (!parent->NextAvaPoints.empty()) {
            //从parent节点开始，一直生成到第depth-1层
            int i;
            for (i = parent->depth + 1; i <= depth - 1; i++) {
                if (parent->NextAvaPoints.empty()) {
                    /* cout<<"parent已经没有可以走的点了，则撤销更改、返回上一层、重入AvaPoint"<<endl;*/
                    Point p = parent->point;
                    newMap[p.x][p.y] = PieceStatus::None;
                    parent = parent->parent;
                    parent->children.erase(parent->children.end() - 1);
                    parent->NextAvaPoints.insert(parent->NextAvaPoints.begin(), p);
                    i--;
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
                node->alpha = parent->alpha;
                node->beta = parent->beta;
                node->map = newMap;
                //该层节点玩家
                node->NextAvaPoints = this->AvaPointGenerator(newMap, MaxCount);
                parent->children.push_back(node);
                parent = node;
            }
            //此时parent为最底层非叶子节点
            //生成叶子节点，parent为叶子节点的父节点
            while (!parent->NextAvaPoints.empty()) {
                Point p = parent->NextAvaPoints[0];
                parent->NextAvaPoints.erase(parent->NextAvaPoints.begin());
                auto node = new ChessNode();
                node->point = p;
                node->depth = i;
                node->parent = parent;
                node->whose = Opponent(parent->whose);
                parent->children.push_back(node);
                newMap[p.x][p.y] = node->whose;
                //计算该节点分数
                int score = this->Evaluator(this->BenefitPlayer, newMap);
                if (parent->depth == 0) {
                    node->alpha = score;
                }
                count++;

                //可以不考虑胜利节点，因为可走点由生成器提供
                //胜利时可走点生成器不会提供下一步点
                //更新父节点的alpha或beta
                if (parent->whose == this->BenefitPlayer) {//Min层
                    //更新父节点的beta
                    parent->beta = parent->beta < score ? parent->beta : score;
                } else {
                    //更新父节点的alpha
                    parent->alpha = parent->alpha > score ? parent->alpha : score;
                }

                //更新完成之后撤销更改
                newMap[p.x][p.y] = PieceStatus::None;
                //如果发生剪枝，则不再生成parent的其它子节点
                if (parent->alpha >= parent->beta) {
                    break;
                }
            }
        }
        //撤销更改并向上一层，同时传递alpha或beta
        newMap[parent->point.x][parent->point.y] = PieceStatus::None;
        if (parent->parent == nullptr)continue;
        if (parent->parent->whose == this->BenefitPlayer) {
            //如果父节点是Min层，则传递beta=min{自己的beta,子节点的alpha,子节点的beta}
            parent->parent->beta = min(parent->parent->beta, min(parent->alpha, parent->beta));
        } else {
            //如果父节点是Max层，则传递alpha
            parent->parent->alpha = max(parent->parent->alpha, max(parent->alpha, parent->beta));
        }
        //跳转到上一层
        parent = parent->parent;
    }
    delete[] newMap;
    auto end = high_resolution_clock::now();
    InfoBoard::CatSays("已经暴力搜索" + to_string(count) + "个对局了！");
    InfoBoard::CatSays("搜索完成，耗时" + to_string(duration_cast<milliseconds>(end - begin).count()) + "ms");
    InfoBoard::CatSays("ModelChecker耗时" + to_string(ModelChecker::CheckerTime) + "ms  占比:" +
                       to_string(ModelChecker::CheckerTime * 100 / duration_cast<milliseconds>(end - begin).count()) +
                       "%");
    return result == nullptr ? firstPoint : result->point;
}
