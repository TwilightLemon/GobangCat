//
// Created by cz241 on 11/6/2023.
//

#ifndef C2023_CHALLENGE_BOARDDRAWER_H
#define C2023_CHALLENGE_BOARDDRAWER_H
#include <raylib.h>
#include "DataHelper/DataType.hpp"
#include "Player/IPlayer.h"

//region UI绘制相关常量
extern const float Board_Size;
extern const float Margin;
extern const int LineThick;
extern const int PieceSize;
extern const int GridSize;
//endregion
//玩家列表
extern IPlayer* Players[2];

struct ChatData{
    time_t LastChatTime=0;
    string ChatText;
    Color ChatColor;
    int ChatDuration=0;
    int AnimateLength=0;
};

//负责UI绘制和游戏操作
class BoardDrawer {
private:
    //交换玩家
    static void ExchangePlayer();
public:
    //region 绘制UI
    //绘制棋盘背景
    static void DrawBackground();
    //根据MapData绘制棋子
    static void DrawPieces();
    //显示上一步
    static void HighlightLastPoint();
    //聊个天
    static void CatChat(string text,Color color=WHITE,int duration=3,int animateLength=3);
    static void StopChatting();
    //绘制ChatUI
    static void AnimateChat(Texture icon);
    //endregion

    //region 游戏操作
    //重置步数历史
    static void ResetStep();
    //反悔一步
    static void RegretAStep(int stepCount);
    //进行一轮博弈
    static void Round(int sleepTime=0,bool CheckModel=false);
    //重来
    static void Restart();
    //endregion

    //region 获取游戏数据
    //获得总计步骤
    static int GetSteps();
    //到底是谁在赢这个游戏
    static PieceStatus  IfWon(bool& drew);
    //获取对局情况
    static void GetWinCount(int& black,int& white);
    //endregion
};


#endif //C2023_CHALLENGE_BOARDDRAWER_H
