//
// Created by cz241 on 11/21/2023.
//

#ifndef C2023_CHALLENGE_MODELCHECKER_H
#define C2023_CHALLENGE_MODELCHECKER_H

#include <vector>
#include "DataType.hpp"
#include <chrono>

enum ModelType{
    //10种模型
    M2,H2,M3,H3,Cube3,C4,Cube4,H4,Win
};

class ChessModel{
public:
    static string GetModelName(ModelType type);
    //模型类型
    ModelType Type;
    //符合该模型的所有点坐标
    vector<Point> Points;
    //是谁的模型
    PieceStatus Whose;
    //空子坐标
    vector<Point> Ava;
};
class ChessModel_Single{
public:
    ModelType Type;
    Point Ava;
    Point Point;
    PieceStatus Whose;
};

class ModelChecker {
public:
    static long long CheckerTime;
    static int Evaluate(PieceStatus player,const ChessMap& map);
    static vector<Point> GetAvaPoints(const ChessMap& map,int MaxCount);
    static vector<ChessModel> CheckModel(const ChessMap& map);
    static vector<ChessModel> PrintMapModel(const ChessMap& map,string ptr="");
};

#endif //C2023_CHALLENGE_MODELCHECKER_H
