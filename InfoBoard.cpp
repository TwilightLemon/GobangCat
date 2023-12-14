//
// Created by cz241 on 11/26/2023.
//

#include <iostream>
#include "InfoBoard.h"
#include "ChessBoard.h"

auto WhiteColor=[](TextColor color){
    switch (color){
        case White:
            cout<<"\033[37m";
            break;
        case Red:
            cout<<"\033[31m";
            break;
        case Blue:
            cout<<"\033[34m";
            break;
        case Green:
            cout<<"\033[32m";
            break;
    }
};
void InfoBoard::CatSays(std::string text, TextColor color) {
    WhiteColor(color);
    cout<<"~@GobangCat: "<<text<<endl;
    cout << "\033[0m";
}
void DrawCopyRight() {
    cout << "\033[37m";
    cout << "GobangCat v1.0.0.0" << endl;
    cout << "Powered by TwilightLemon" << endl;
    cout << "\033[0m"<<endl;
}
void DrawWinCount(){
    int b,w;
    ChessBoard::GetWinCount(b, w);
    cout << "\033[32m-----------------------------"<<endl;
    cout << "对局情况: " << endl;
    cout << "Black: " <<b<< endl;
    cout << "White: " <<w<< endl;
    cout << "-----------------------------\033[0m"<<endl;
}
void InfoBoard::Clear() {
    system("cls");
    DrawCopyRight();
    DrawWinCount();
}
