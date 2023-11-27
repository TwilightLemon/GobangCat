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

    //加载玩家：
    Players[0] = (new HumanPlayer())
            ->SetPlayer(PieceStatus::Black);
    /*Players[1] = (new HumanPlayer())
            ->SetPlayer(PieceStatus::White);*/
    Players[1] = (new ChessTreeRobot())
            ->SetPlayer(PieceStatus::White)
            ->SetEnableTreeSearch(true)
            ->SetEvaluator(EvaluatorType::ModelChecking)
            ->SetTreeDepth(6);

    //是否已经结束
    bool ended=false;

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
        if(!ended) {
            BoardDrawer::Round(0,true);
        }
        //endregion

        //region 判断有无胜出
        bool drew=false;//是否和棋
        auto winner=BoardDrawer::IfWined(drew);
        if(winner!=PieceStatus::None || drew){
            ended=true;
            string win=drew?"No one":(winner==PieceStatus::Black?"Black":"White");
            win.append(" Wined in "+to_string(BoardDrawer::GetSteps())+" steps!");
            DrawText(win.c_str(),20,Board_Size+40,20,BLACK);
            DrawText("Press ENTER to restart",20,Board_Size+60,20,BLACK);
            if(IsKeyPressed(KEY_ENTER)){
                ended=false;
                BoardDrawer::Restart();
            }
        }
        //endregion

        //region 悔棋
        if(IsKeyPressed(KEY_UP)){
            InfoBoard::CatSays("打不过就悔棋是吧- -",TextColor::Red);
            BoardDrawer::RegretAStep(2);
            if(ended)ended=false;
        }
        //endregion

        EndDrawing();
    }
    CloseWindow();
    return 0;
}