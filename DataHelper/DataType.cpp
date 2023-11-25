#include <iostream>
#include "DataType.h"

void ShowMap(const ChessMap& map){
    cout<<"MODELCHECKER::Current Map:"<<endl;
    for(int x=0;x<15;x++){
        for(int y=0;y<15;y++){
            if(map[y][x]==PieceStatus::None)
                cout<<"- ";
            else if(map[y][x]==PieceStatus::Black)
                cout<<"B ";
            else cout<<"W ";
        }
        cout<<endl;
    }
}