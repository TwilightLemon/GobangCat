//
// Created by cz241 on 11/6/2023.
//

#include "BoardDrawer.h"
#include "DataHelper/ModelChecker.h"
#include "InfoBoard.h"
#include <iostream>
#include <thread>
#include <future>
#include <ctime>
#include <queue>

//region 游戏数据
bool ended;
int WinCount_Black=0,WinCount_White=0;
//endregion

//region UI绘制

vector<ChatData> ChatHistory;
void BoardDrawer::CatChat(string text,Color color,int duration,int animateLength){
    ChatHistory.push_back({0,text,color,duration,animateLength});
}
void BoardDrawer::StopChatting(){
    ChatHistory.clear();
}
void BoardDrawer::AnimateChat(Texture icon){
    if(ChatHistory.empty())return;
    DrawRectangle(0,Board_Size+60,Board_Size+60,60,BLUE);
    DrawTexture(icon,20,Board_Size+77,WHITE);
    auto data=ChatHistory[0];
    if(data.LastChatTime==0) {
        data.LastChatTime = time(nullptr);
        ChatHistory[0]=data;
    }
    if(time(nullptr)-data.LastChatTime>data.ChatDuration){
        ChatHistory.erase(ChatHistory.begin());
        return;
    }
    string str=data.AnimateLength!=0?data.ChatText.substr(0,data.ChatText.length()-data.AnimateLength+1+(time(nullptr)-data.LastChatTime)%data.AnimateLength):data.ChatText;
    DrawText(str.c_str(),90,Board_Size+78,28,data.ChatColor);
}

void BoardDrawer::HighlightLastPoint() {
    if(StepHistory.empty())return;
    auto p=StepHistory.top();
    DrawCircle(Margin+p.x*GridSize,Margin+p.y*GridSize,PieceSize+3,PINK);
}

void BoardDrawer::DrawBackground() {
    //绘制阴阳线：
    for(int i=0;i<15;i++){
        DrawLineEx({Margin,Margin+i*GridSize},{Board_Size+Margin,Margin+i*GridSize},LineThick,BLACK);
        DrawLineEx({Margin+i*GridSize,Margin},{Margin+i*GridSize,Board_Size+Margin},LineThick,BLACK);
    }
    //绘制4个点：
    int radius=8;
    DrawCircle(Margin+3*GridSize,Margin+3*GridSize,radius,BLACK);
    DrawCircle(Margin+3*GridSize,Margin+11*GridSize,radius,BLACK);
    DrawCircle(Margin+11*GridSize,Margin+3*GridSize,radius,BLACK);
    DrawCircle(Margin+11*GridSize,Margin+11*GridSize,radius,BLACK);
}

void BoardDrawer::DrawPieces(){
    for(int x=0;x<15;x++){
        for(int y=0;y<15;y++){
            if(MapData[x][y] == PieceStatus::Black){
                DrawCircle(Margin+x*GridSize,Margin+y*GridSize,PieceSize,BLACK);
            }else if(MapData[x][y]==PieceStatus::White){
                DrawCircle(Margin+x*GridSize,Margin+y*GridSize,PieceSize,WHITE);
            }
        }
    }
}
//endregion

//region 获取游戏数据
int BoardDrawer::GetSteps() {
    return StepHistory.size();
}

void BoardDrawer::GetWinCount(int& black,int& white){
    black=WinCount_Black;
    white=WinCount_White;
}

PieceStatus  BoardDrawer::IfWon(bool& drew){
    //TODO:可以从最后一步开始判断，效率更高

    Color LineColor=BLUE;
    Vector2 p_start,p_end;
    PieceStatus IfWin=PieceStatus::None;
    int pieceCount=0;
    //判断是否胜利，一个没有效率的方法：
    for(int x=0;x<15;x++) {
        for (int y = 0; y < 15; y++) {
            if (MapData[x][y] == PieceStatus::None)continue;
            pieceCount++;
            //判断横向：
            if (x <= 10 && MapData[x][y] == MapData[x + 1][y] && MapData[x][y] == MapData[x + 2][y] &&
                MapData[x][y] == MapData[x + 3][y] && MapData[x][y] == MapData[x + 4][y]) {
                p_start={Margin + x * GridSize, Margin + y * GridSize};
                p_end={Margin + (x + 4) * GridSize, Margin + y * GridSize};
                IfWin=MapData[x][y];
            }
            //判断纵向：
            if (y <= 10 && MapData[x][y] == MapData[x][y + 1] && MapData[x][y] == MapData[x][y + 2] &&
                MapData[x][y] == MapData[x][y + 3] && MapData[x][y] == MapData[x][y + 4]) {
                p_start={Margin + x * GridSize, Margin + y * GridSize};
                p_end={Margin + x * GridSize, Margin + (y + 4) * GridSize};
                IfWin=MapData[x][y];
            }
            //判断左上到右下:
            if (x <= 10 && y <= 10 && MapData[x][y] == MapData[x + 1][y + 1] &&
                MapData[x][y] == MapData[x + 2][y + 2] && MapData[x][y] == MapData[x + 3][y + 3] &&
                MapData[x][y] == MapData[x + 4][y + 4]) {
                p_start={Margin + x * GridSize, Margin + y * GridSize};
                p_end={Margin + (x + 4) * GridSize, Margin + (y + 4) * GridSize};
                IfWin=MapData[x][y];
            }
            //判断右上到左下:
            if (x >= 4 && y <= 10 && MapData[x][y] == MapData[x - 1][y + 1] && MapData[x][y] == MapData[x - 2][y + 2] &&
                MapData[x][y] == MapData[x - 3][y + 3] && MapData[x][y] == MapData[x - 4][y + 4]) {
                p_start={Margin + x * GridSize, Margin + y * GridSize};
                p_end={Margin + (x - 4) * GridSize, Margin + (y + 4) * GridSize};
                IfWin=MapData[x][y];
            }
        }
    }
    if(IfWin!=PieceStatus::None){
        DrawLineEx(p_start, p_end, LineThick+2, LineColor);
        if(!ended) {
            if (IfWin == PieceStatus::Black) {
                WinCount_Black++;
            } else {
                WinCount_White++;
            }
        }
        ended=true;
        return IfWin;
    }else{
        if(pieceCount==225){
            ended=true;
            drew=true;
        }
    }
    return PieceStatus::None;
}
//endregion

//region 游戏控制
void BoardDrawer::ResetStep(){
    while(!StepHistory.empty())
        StepHistory.pop();
}
void BoardDrawer::Restart() {
    ResetStep();
    //重新开始：
    for(int x=0;x<15;x++)
        for(int y=0;y<15;y++)
            MapData[x][y]=PieceStatus::None;
    CurrentPlayer=PieceStatus::Black;
    ended=false;
}
static std::future<Point> AsyncStepResult;
void BoardDrawer::RegretAStep(int stepCount){
    int count=0;
    if(AsyncStepResult.valid()) {
        stepCount = 1;
        StopChatting();
    }
    for(int i=0;i<stepCount;i++)
    if(!StepHistory.empty()){
        auto p1=StepHistory.top();
        StepHistory.pop();
        MapData[p1.x][p1.y]=PieceStatus::None;
        count++;
    }
    if(count%2==1)ExchangePlayer();
    if(ended)ended=false;
}
void BoardDrawer::Round(int sleepTime,bool CheckModel){
    if(ended)return;
    //绘制当前玩家：
    string current= "CurrentPlayer: ";
    current.append(CurrentPlayer==PieceStatus::Black?"Black":"White");
    DrawText(current.c_str(),20,Board_Size+35,24,BLACK);
    auto list =CheckModel?ModelChecker::CheckModel(MapData):vector<ChessModel>();
    //应该轮到谁下棋
    IPlayer*player=Players[0]->PlayerColor==CurrentPlayer?Players[0]:Players[1];
    Point p={-1,-1};

    if(player->EnableAsync){
        //异步获取下一步
        if(AsyncStepResult.valid()){
            if(AsyncStepResult.wait_for(std::chrono::seconds(0)) == std::future_status::ready){
                p=AsyncStepResult.get();
            }
        }else {
            AsyncStepResult = std::async(std::launch::async, &IPlayer::NextStep, player);
        }
    }else{
        p=player->NextStep();
    }

    if(p.x==-1&&p.y==-1)return;
    cout<<"POINT AT: "<<p.x<<" "<<p.y<<endl;
    MapData[p.x][p.y]=CurrentPlayer;
    ExchangePlayer();
    StepHistory.push(p);
    if(CheckModel) {
        //region 调试信息
        cout << "-------------Checked Model Count: " << list.size() << "-------------" << endl;
        for (const auto &model: list) {
            cout << "Whose: " << (model.whose == PieceStatus::Black ? "Black" : "White") << endl;
            cout << "Model: " << ChessModel::GetModelName(model.type) << endl;
            cout << "Ava: ";
            for (auto p: model.ava) {
                cout << p.x << " " << p.y << "   ";
            }
            cout << endl;
            cout << "Points: ";
            for (auto p: model.points)
                cout << p.x << " " << p.y << "   ";
            cout << endl;
        }
        //endregion
    }
    if(sleepTime!=0)this_thread::sleep_for(chrono::milliseconds(sleepTime));
}
void BoardDrawer::ExchangePlayer() {
    CurrentPlayer = CurrentPlayer == PieceStatus::Black ? PieceStatus::White : PieceStatus::Black;
}
//endregion