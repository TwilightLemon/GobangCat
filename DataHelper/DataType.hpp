//
// Created by cz241 on 11/3/2023.
//

#ifndef C2023_CHALLENGE_DATATYPE_H
#define C2023_CHALLENGE_DATATYPE_H

#include <string>
#include <stack>
#include <iostream>
using namespace std;

enum class PieceStatus {
    None,
    Black,
    White
};
static PieceStatus Opponent(const PieceStatus& player)
{
    return player==PieceStatus::Black?PieceStatus::White:PieceStatus::Black;
}
typedef PieceStatus (*ChessMap)[15];
extern ChessMap MapData;
extern PieceStatus CurrentPlayer;

struct Point{
    int x,y;
    [[nodiscard]]
    bool Equal(const Point p) const{
        return x==p.x&&y==p.y;
    }
    [[nodiscard]]
    bool EmptyInMap(const ChessMap& map) const{
        if(x<0||x>=15||y<0||y>=15)return true;
        return map[x][y]==PieceStatus::None;
    }
    [[nodiscard]]
    bool EqualInMap(const Point& p,const ChessMap& map) const{
        if(x<0||x>=15||y<0||y>=15)return false;
        return map[x][y]==map[p.x][p.y];
    }
    [[nodiscard]]
    bool EqualInMap(PieceStatus status,const ChessMap& map)const {
        if(x<0||x>=15||y<0||y>=15)return false;
        return map[x][y]==status;
    }
};
extern stack<Point> StepHistory;
static void ShowMap(const ChessMap& map,int depth,int alpha,int beta,Point lastp){
    cout<<"Depth: "<<depth<< "  "<<"LastPoint: "<<lastp.x<<","<<lastp.y<<endl;
    cout<<"Alpha: "<<alpha<<" Beta: "<<beta<<endl;
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
#endif //C2023_CHALLENGE_DATATYPE_H
