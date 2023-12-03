//
// Created by cz241 on 11/3/2023.
//
#include "BoardDrawer.h"
#include "InfoBoard.h"

//defined at BoardDrawer.h
const float Board_Size=840;
const float Margin =20.0f;
const int LineThick=2;
const int PieceSize =20;
const int GridSize =  60;

//defined at DataType.h
ChessMap MapData=new PieceStatus[15][15]{PieceStatus::None};
//记录每一步的历史
stack<Point> StepHistory;
//黑子先手
PieceStatus CurrentPlayer=PieceStatus::Black;
//玩家列表
IPlayer* Players[2];

int main(){
    system("chcp 65001");
    InitWindow(Board_Size+60,Board_Size+90,"GobangCat");
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
/*    Players[1] = (new HumanPlayer())
            ->SetPlayer(PieceStatus::White);*/
    Players[1] = (new ChessTreeRobot())
            ->SetPlayer(PieceStatus::White)
            ->SetEnableTreeSearch(true)
            ->SetEvaluator(EvaluatorType::ModelChecking)
            ->SetTreeDepth(6);

    while(!WindowShouldClose()){
        BeginDrawing();
        //region 基础绘制部分：棋盘背景、棋子
        ClearBackground(SKYBLUE);
        //绘制棋盘背景：
        BoardDrawer::DrawBackground();
        //显示上一步
        BoardDrawer::HighlightLastPoint();
        //绘制棋子：
        BoardDrawer::DrawPieces();
        //endregion

        //region 如果无人胜出则进行一轮博弈
        BoardDrawer::Round(0,true);
        //endregion

        //region 判断有无胜出
        bool drew=false;//是否和棋
        auto winner=BoardDrawer::IfWined(drew);
        if(winner!=PieceStatus::None || drew){
            string win=drew?"No one":(winner==PieceStatus::Black?"Black":"White");
            win.append(" Wined in "+to_string(BoardDrawer::GetSteps())+" steps!");

            DrawRectangle(0,Board_Size/2-10,Board_Size+60,90,BLUE);
            DrawText(win.c_str(),20,Board_Size/2+10,30,WHITE);
            DrawText("Press Enter to Restart",20,Board_Size/2+45,20,WHITE);

            if(IsKeyPressed(KEY_ENTER)){
                BoardDrawer::Restart();
            }
        }
        //endregion

        //region 悔棋
        if(IsKeyPressed(KEY_UP)){
            InfoBoard::CatSays("打不过就悔棋是吧- -",TextColor::Red);
            BoardDrawer::RegretAStep(2);
        }
        //endregion

        EndDrawing();
    }
    CloseWindow();
    return 0;
}