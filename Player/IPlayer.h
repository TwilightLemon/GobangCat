#ifndef C2023_CHALLENGE_IPLAYER_H
#define C2023_CHALLENGE_IPLAYER_H
//Player的抽象接口
//定义了玩家的基本行为
//所有玩家于此定义
#include "../DataHelper/DataType.hpp"

class IPlayer
{
public:
    PieceStatus PlayerColor;
    //是否异步执行NextStep
    bool EnableAsync=false;
    virtual void Break()=0;
    virtual Point NextStep() = 0;
};

class RandomRobot : public IPlayer
{
public:
    void Break() override{};
    Point NextStep() override;
};
class HumanPlayer : public IPlayer
{
public:
    HumanPlayer* SetPlayer(PieceStatus player){
        this->PlayerColor=player;
        return this;
    }
    void Break() override{};
    Point NextStep() override;
};
enum EvaluatorType{
    Counting,
    ModelChecking
};
class ChessTreeRobot : public IPlayer
{
    public:
    ChessTreeRobot(){
        this->EnableAsync=true;
    }
    void Break() override;
    ChessTreeRobot* SetEvaluator(EvaluatorType type){
        this->Evaluator=type;
        return this;
    }
    ChessTreeRobot* SetPlayer(PieceStatus player){
        this->PlayerColor=player;
        return this;
    }
    ChessTreeRobot* SetEnableTreeSearch(bool enable){
        this->EnableTreeSearch=enable;
        return this;
    }
    ChessTreeRobot* SetDynamicSetter(void setter(int& depth, int& root,int& child)){
        DynamicSetter=setter;
        return this;
    }
    Point NextStep() override;
    void (*DynamicSetter)(int& depth, int& root,int& child);
    int TreeDepth=4;
    bool EnableTreeSearch=true;
    EvaluatorType Evaluator;
};

#endif