//
// Created by cz241 on 11/22/2023.
//通过计数的方式估分
//

#include "CountingEvaluator.h"

int CountingEvaluator::Evaluate(PieceStatus player, const ChessMap &map) {
    int score=0;
    auto Check=[&](const vector<Point> PointSet){
        int me=0,empty=0,opp=0;
        for(const auto& point:PointSet){
            if(point.x==-1)
                break;
            if(point.EqualInMap(player,map))
                me++;
            else if(point.EmptyInMap(map))
                empty++;
            else opp++;
        }
        if(me == 5)
            return 120000;
        if(opp == 5)
            return -100000;
        if(me == 4 && empty == 1)
            return 10000;
        if(opp == 4 && empty == 1)
            return -10000;
        if(me == 3 && empty == 2)
            return 1000;
        if(opp == 3 && empty == 2)
            return -1000;
        if(me == 2 && empty == 3)
            return 100;
        if(opp == 2 && empty == 3)
            return -100;
        if(me == 1 && empty == 4)
            return 10;
        if(opp == 1 && empty == 4)
            return -10;
        return 0;
    };

    for(int x=0;x<15;x++){
        for(int y=0;y<15;y++){
            vector<vector<Point>> region;
            if(y<=10) {
                //取纵向+
                vector<Point> p = { {x, y}, {x, y + 1},  {x, y + 2}, {x, y + 3},  {x, y + 4}};
                if(y<=9)
                    p.push_back({x,y+5});
                else p.push_back({-1,-1});
                region.push_back(p);
            }
            if(y>=4){
                //取纵向-
                vector<Point> p = { {x, y}, {x, y - 1},  {x, y - 2}, {x, y - 3},  {x, y - 4}};
                if(y>=5)
                    p.push_back({x,y-5});
                else p.push_back({-1,-1});
                region.push_back(p);
            }
            if(x<=10) {
                //取横向+
                vector<Point> p = { {x, y}, {x+1, y},  {x+2, y}, {x+3, y},  {x+4, y}};
                if(x<=9)
                    p.push_back({x+5,y});
                else p.push_back({-1,-1});
                region.push_back(p);
            }
            if(x>=4) {
                //取横向-
                vector<Point> p = { {x, y}, {x-1, y},  {x-2, y}, {x-3, y},  {x-4, y}};
                if(x>=5)
                    p.push_back({x-5,y});
                else p.push_back({-1,-1});
                region.push_back(p);
            }
            if(x<=10&&y<=10) {
                //取左上到右下
                vector<Point> p = { {x, y}, {x+1, y + 1},  {x+2, y + 2}, {x+3, y + 3},  {x+4, y + 4}};
                if(x<=9&&y<=9)
                    p.push_back({x+5,y+5});
                else p.push_back({-1,-1});
                region.push_back(p);
            }
            if(x>=4&&y>=4) {
                //取左上到右下
                vector<Point> p = { {x, y}, {x-1, y - 1},  {x-2, y - 2}, {x-3, y - 3},  {x-4, y - 4}};
                if(x>=5&&y>=5)
                    p.push_back({x-5,y-5});
                else p.push_back({-1,-1});
                region.push_back(p);
            }
            if(x>=4&&y<=10) {
                //取右上到左下
                vector<Point> p = { {x, y}, {x-1, y + 1},  {x-2, y + 2}, {x-3, y + 3},  {x-4, y + 4}};
                if(x>=5&&y<=9)
                    p.push_back({x-5,y+5});
                else p.push_back({-1,-1});
                region.push_back(p);
            }
            if(x<=10&&y>=4) {
                //取右上到左下
                vector<Point> p = { {x, y}, {x+1, y - 1},  {x+2, y - 2}, {x+3, y - 3},  {x+4, y - 4}};
                if(x<=9&&y>=5)
                    p.push_back({x+5,y-5});
                else p.push_back({-1,-1});
                region.push_back(p);
            }
            for(const auto& p:region){
                score+=Check(p);
            }
        }
    }
    return score;
}
