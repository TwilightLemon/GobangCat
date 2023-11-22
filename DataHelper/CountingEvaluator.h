//
// Created by cz241 on 11/22/2023.
//

#ifndef GOBANGCAT_COUNTINGEVALUATOR_H
#define GOBANGCAT_COUNTINGEVALUATOR_H


#include <vector>
#include "DataType.h"

class CountingEvaluator {
public:
    static int Evaluate(PieceStatus player,const ChessMap& map);
    //static vector<Point> GetAvaPoints(const ChessMap& map);
};


#endif //GOBANGCAT_COUNTINGEVALUATOR_H
