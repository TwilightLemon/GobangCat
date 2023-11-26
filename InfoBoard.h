//
// Created by cz241 on 11/26/2023.
//

#ifndef GOBANGCAT_INFOBOARD_H
#define GOBANGCAT_INFOBOARD_H
#include <string>
#include <vector>

using namespace std;
enum TextColor{
    White,Red,Blue,Green
};
class InfoData{
public:
    string text;
    TextColor color;
    InfoData(string text,TextColor color){
        this->text=text;
        this->color=color;
    }
};
class InfoBoard {
public:
    static void CatSays(string text,TextColor color=White);
    static void Clear();
    static void DrawBoard();
};



#endif //GOBANGCAT_INFOBOARD_H
