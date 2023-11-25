#ifndef C2023_CHALLENGE_IPLAYER_H
#define C2023_CHALLENGE_IPLAYER_H
//Player的抽象接口
//定义了玩家的基本行为
//所有玩家于此定义
#include "../DataHelper/DataType.h"

class IPlayer
{
public:
    PieceStatus PlayerColor;
    virtual Point NextStep() = 0;
};

class RandomRobot : public IPlayer
{
public:
    Point NextStep() override;
};
class HumanPlayer : public IPlayer
{
public:
    HumanPlayer* SetPlayer(PieceStatus player){
        this->PlayerColor=player;
        return this;
    }
    Point NextStep() override;
};
enum EvaluatorType{
    Counting,
    ModelChecking
};
class ChessTreeRobot : public IPlayer
{
    public:
    ChessTreeRobot* SetEvaluator(EvaluatorType type){
        this->Evaluator=type;
        return this;
    }
    ChessTreeRobot* SetPlayer(PieceStatus player){
        this->PlayerColor=player;
        return this;
    }
    ChessTreeRobot* SetTreeDepth(int depth){
        this->TreeDepth=depth;
        return this;
    }
    ChessTreeRobot* SetEnableTreeSearch(bool enable){
        this->EnableTreeSearch=enable;
        return this;
    }
    ChessTreeRobot* SetSkipStepCount(int count){
        this->SkipStepCount=count;
        return this;
    }
    Point NextStep() override;
    int TreeDepth=4;
    int SkipStepCount=0;
    bool EnableTreeSearch=true;
    EvaluatorType Evaluator;
};

#endif