//
// Created by cz241 on 11/3/2023.
//
#include "ChessBoard.h"
#include "InfoBoard.h"

//region UI常量
#define BackgroundColor GetColor(0xffDEB887)
#define BackgroundColor_Dark GetColor(0xFFCD853F)
//defined at BoardDrawer.h
const float Board_Size=840;
const float Margin =20.0f;
const int LineThick=2;
const int PieceSize =20;
const int GridSize =  60;
//endregion

//region 游戏数据->全局变量
//defined at DataType.h
ChessMap MapData=new PieceStatus[15][15]{PieceStatus::None};
//记录每一步的历史
stack<Point> StepHistory;
//黑子先手
PieceStatus CurrentPlayer=PieceStatus::Black;
//玩家列表
IPlayer* Players[2];
//endregion

int main(){
    system("chcp 65001");
    InitWindow(Board_Size+60,Board_Size+120,"GobangCat");
    SetTargetFPS(15);
    SetConfigFlags(FLAG_MSAA_4X_HINT);

    //加载图像资源
    Image piece_Black=LoadImage("Resource/cat_black.png");
    Image piece_White=LoadImage("Resource/cat_white.png");
    Texture texture_Black=LoadTextureFromImage(piece_Black);
    Texture texture_White=LoadTextureFromImage(piece_White);

    //加载玩家：
    Players[0] = (new HumanPlayer())
            ->SetPlayer(PieceStatus::Black);
    Players[1] = (new ChessTreeRobot())
            ->SetPlayer(PieceStatus::White)
            ->SetEnableTreeSearch(true)
            ->SetEvaluator(EvaluatorType::ModelChecking)
            ->SetDynamicSetter([](int& depth,int &root, int &child) {
                if (int step = ChessBoard::GetSteps();step < 8) {
                    depth=4;
                    root = 4;
                    child = 4;
                } else if (step < 20) {
                    depth=6;
                    root = 6;
                    child = 4;
                } else if (step < 40) {
                    depth=8;
                    root = 10;
                    child = 4;
                } else {
                    depth=8;
                    root = 15;
                    child = 4;
                }
            });

    while(!WindowShouldClose()){
        BeginDrawing();
        //region 基础绘制部分：棋盘背景、棋子
        ClearBackground(BackgroundColor);
        //绘制棋盘背景：
        ChessBoard::DrawBackground();
        //初始时绘制游戏提示框：
        static bool first=true;
        if(StepHistory.empty()&&first){
            first=false;
            ChessBoard::CatChat("Tips: Press UP to regret and Enter to restart.", BLACK, 3, 0);
            ChessBoard::CatChat("Good Luck!", BLACK, 2, 0);
        }
        //显示上一步
        ChessBoard::HighlightLastPoint();
        //绘制棋子：
        ChessBoard::DrawPieces();
        //绘制ChatUI
        ChessBoard::AnimateChat(texture_Black);
        //endregion

        //region 如果无人胜出则进行一轮博弈
        ChessBoard::Round(0, true);
        //endregion

        //region 判断有无胜出
        bool drew=false;//是否和棋
        auto winner= ChessBoard::IfWon(drew);
        if(winner!=PieceStatus::None || drew){
            //获胜情况
            string win=drew?"No one":(winner==PieceStatus::Black?"Black":"White");
            win.append(" won in " + to_string(ChessBoard::GetSteps()) + " steps!");
            DrawRectangle(0,Board_Size/2-10,Board_Size+60,90, BackgroundColor_Dark);
            DrawRectangle(0,Board_Size/2-10,Board_Size+60,90, BackgroundColor_Dark);
            DrawRectangle(0,Board_Size/2-10,Board_Size+60,90, BackgroundColor_Dark);
            DrawText(win.c_str(),20,Board_Size/2+10,30,BLACK);
            DrawText("Press Enter to Restart",20,Board_Size/2+45,20,BLACK);
            //对局情况
            int WinCount_Black=0,WinCount_White=0;
            ChessBoard::GetWinCount(WinCount_Black, WinCount_White);
            string black="Black: "+to_string(WinCount_Black)+" times";
            string white="White: "+to_string(WinCount_White)+" times";
            DrawText(black.c_str(),Board_Size-120,Board_Size/2+10,22,BLACK);
            DrawText(white.c_str(),Board_Size-120,Board_Size/2+42,22,BLACK);

            if(IsKeyPressed(KEY_ENTER)){
                ChessBoard::Restart();
            }
        }
        //endregion

        //region 悔棋
        if(IsKeyPressed(KEY_UP)){
            ChessBoard::CatChat("How dare u to regret?!", RED, 2, 0);
            ChessBoard::RegretAStep(2);
        }
        //endregion

        EndDrawing();
    }
    CloseWindow();
    return 0;
}