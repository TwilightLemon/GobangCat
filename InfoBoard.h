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
//负责绘制信息面板
class InfoBoard {
public:
    static void CatSays(string text,TextColor color=White);
    static void Clear();
};



#endif //GOBANGCAT_INFOBOARD_H
