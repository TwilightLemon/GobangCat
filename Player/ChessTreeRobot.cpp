#include "IPlayer.h"
#include <vector>
#include <iostream>
#include <algorithm>
#include "../ChessBoard.h"
#include "../DataHelper/ModelChecker.h"
#include "../DataHelper/ChessTree.h"
#include "../DataHelper/CountingEvaluator.h"
#include "../InfoBoard.h"

using namespace std;

/*
 * Design:
 * 两个步骤：
 * Step1:检索几个常见的模型,预计我方和对手可能的下一步
 * Step2:搜索博弈树，预算分数
 * Step3:返回最高分数的点
*/
vector<Point> AvaPointsOfOpponent;
int predictCount=0;

ChessTree* tree;
void ChessTreeRobot::Break() {
    tree->Stop=true;
    InfoBoard::CatSays("Searching is disrupted.");
}

Point ChessTreeRobot::NextStep() {
    InfoBoard::Clear();
    //region 看一下模型能预判到对手多少点
    bool found = false;
    Point olast={-1,-1};
    if(!StepHistory.empty()) {
        olast = StepHistory.top();
        for (const auto &p: AvaPointsOfOpponent) {
            if (p.x == olast.x && p.y == olast.y) {
                predictCount++;
                found = true;
                break;
            }
        }
    }
    AvaPointsOfOpponent.clear();
    //endregion
    //检索几个常见的模型
    auto list= ModelChecker::PrintMapModel(MapData,"SUPPLYiNG");
    //region 没有匹配到模型，随便下
    if(list.empty()){
        ChessBoard::CatChat("Let me play randomly...", WHITE, 1);
        return (new RandomRobot())->NextStep();
    }
    //endregion
    //region 猜测对手
    if(found)
        ChessBoard::CatChat("I‘ve calculated " + to_string(predictCount) + " steps of yours!", WHITE, 3, 0);
    for(const auto &item:list)
        for(const auto& p:item.ava)
            AvaPointsOfOpponent.push_back(p);
    //endregion
    if(this->EnableTreeSearch&&StepHistory.size()>=this->SkipStepCount*2) {
        //region 搜索博弈树，预算分数
        ChessBoard::CatChat("Thinking.....????!!!", WHITE, 30, 12);
        tree=new ChessTree();
        //装配估值器
        if(this->Evaluator==EvaluatorType::Counting)
            //使用计数估值器
            tree->Evaluator=CountingEvaluator::Evaluate;
        //使用模型匹配估值器
        else tree->Evaluator=ModelChecker::Evaluate;
        //装配可走点生成器
        tree->AvaPointGenerator=ModelChecker::GetAvaPoints;
        //设置模拟走子MaxCount
        tree->MaxCount=MaxCount;
        tree->MaxRootCount=MaxRootCount;
        tree->GenerateTree(this->TreeDepth, this->PlayerColor);
        auto result = tree->AlphaBetaSearch();
        delete tree;
        ChessBoard::StopChatting();
        return result;
        //endregion
    }
    else return list[0].ava[0];
}