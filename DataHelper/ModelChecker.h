//
// Created by cz241 on 11/21/2023.
//

#ifndef C2023_CHALLENGE_MODELCHECKER_H
#define C2023_CHALLENGE_MODELCHECKER_H

#include <vector>
#include "DataType.h"

enum ModelType{
    //10种模型
    M2,H2,M3,H3,Cube3,C4,Cube4,H4,Win
};

class ChessModel{
public:
    static string GetModelName(ModelType type);
    //模型类型
    ModelType type;
    //符合该模型的所有点坐标
    vector<Point> points;
    //是谁的模型
    PieceStatus whose;
    //空子坐标
    vector<Point> ava;
};
class ChessModel_Single{
public:
    ModelType type;
    Point ava;
    Point point;
    PieceStatus whose;
};

class ModelChecker {
public:
    static int Evaluate(PieceStatus player,const ChessMap& map);
    static vector<Point> GetAvaPoints(const ChessMap& map);
    static vector<ChessModel> CheckModel(const ChessMap& map);
};

#endif //C2023_CHALLENGE_MODELCHECKER_H
