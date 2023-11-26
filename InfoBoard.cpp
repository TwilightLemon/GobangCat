//
// Created by cz241 on 11/26/2023.
//

#include <iostream>
#include "InfoBoard.h"

static vector<InfoData> chat;
void InfoBoard::CatSays(std::string text, TextColor color) {
    chat.push_back(InfoData(text,color));
    DrawBoard();
}

void DrawCopyRight() {
    cout << "\033[37m";
    cout << "GobangCat v1.0.0.0" << endl;
    cout << "Powered by TwilightLemon" << endl;
    cout << "\033[0m"<<endl;
}
void InfoBoard::DrawBoard() {
    system("cls");
    DrawCopyRight();
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
    for(const auto& info:chat){
        WhiteColor(info.color);
        cout<<"~@GobangCat: "<<info.text<<endl;
        cout<<"\033[0m";
    }
}
void InfoBoard::Clear() {
    chat.clear();
    DrawBoard();
}
