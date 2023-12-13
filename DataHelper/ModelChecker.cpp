//
// Created by cz241 on 11/21/2023.
//
#include <iostream>
#include <algorithm>
#include "ModelChecker.h"
#include "CountingEvaluator.h"

//可走点生成器
vector<Point> ModelChecker::GetAvaPoints(const ChessMap& map,int MaxCount){
    auto list = ModelChecker::CheckModel(map);
    vector<Point> avaPoints;
    int count=0;
    for (const auto &item: list) {
        //该层对局玩家可能走的点
        int cot=0;
        for (const auto &p: item.ava) {
            cot++;
            //P.S. 在GetAvaPoints中限制单个模型点数不影响Cube模型的生成
            //限制单个模型的可走点数
            int max=(item.type==ModelType::H2||item.type==ModelType::M2)?1:2;
            if(cot>max)break;
            //如果该点已经存在，则不重复添加
            bool found = false;
            for (const auto &point: avaPoints)
                if (point.x == p.x && point.y == p.y) {
                    found = true;
                    break;
                }
            if (found)continue;
            avaPoints.push_back(p);
            count++;
        }
        //限制一次最多添加的点数
        if(count>=MaxCount)break;
    }
    return avaPoints;
}
//评估局面分数
int ModelChecker::Evaluate(PieceStatus player,const ChessMap& map){
    auto mlist=CheckModel(map);
    int score=0;
    for(const auto& model:mlist){
        int p=model.whose==player?1:-1;
        switch (model.type) {
            case ModelType::Win:
                score+=100000000*p;
                break;
            case ModelType::H4:
                score+=1000000*p;
                break;
                //不应该为Cube模型计分，因为Cube模型是由其它模型组成的，分数已经计算过了
            case ModelType::Cube4:
                score+=100000*p;
                break;
            case ModelType::C4:
                score+=10000*p;
                break;
            case ModelType::Cube3:
                score+=1500*p;
                break;
            case ModelType::H3:
                score+=1000*p;
                break;
            case ModelType::M3:
                score+=500*p;
                break;
            case ModelType::H2:
                score+=200*p;
                break;
            case ModelType::M2:
                score+=100*p;
                break;
            default:
                break;
        }
    }
    return score;
}
//对模型进行价值排序
void ShellSort(vector<ChessModel>& list) {
    ChessModel win;
    bool foundWin=false;
    for(auto item:list) {
        if (item.type == ModelType::Win) {
            //如果有五子连珠，则只在list中保留Win模型，删除其它模型
            win = item;
            foundWin = true;
        }
    }
    if(foundWin){
        list.clear();
        list.push_back(win);
        return;
    }

    int gap,count=0,length=list.size();
    ChessModel temp;
    for (gap = length >> 1; gap > 0; gap >>= 1)
        for (int i = gap; i < length; i++) {
            temp = list[i];
            int j = i - gap;
            for (; j >= 0 && list[j].type < temp.type; j -= gap)
                list[j + gap] = list[j];
            list[j + gap] = temp;
            count++;
        }
}
long long ModelChecker::CheckerTime=0;

//检索双方符合的模型，预判可能的步骤
vector<ChessModel> ModelChecker::CheckModel(const ChessMap& map){
    auto now=chrono::high_resolution_clock ::now();
    vector<ChessModel> result;
    //region 检索模型
    auto CheckEmpty=[](vector<Point>& p,const ChessMap& map)->bool{
        bool empty=true;
        for(const auto& point:p)
            if(!point.EmptyInMap(map))
                empty=false;
        return empty;
    };
    auto Check=[&](vector<Point>& p,ModelType type,const vector<vector<int>>& rules){
        int pointCount=rules[0].size();
        ChessModel Model;
        Model.type=type;
        vector<Point> ava,points;

        int rulesCount=rules.size();
        //匹配模型类型
        //规则示例：-101113
        // 其中0表示空子，1表示有子且相等
        // -1表示有子且不相等
        //-2表示空但不为ava
        // 2表示不匹配任何子
        // 3表示优先落子

        //相等子是谁的
        PieceStatus owner=PieceStatus::None;
        auto detect=[&]()->bool {
            bool hasMatched = false;
            for (int i = 0; i < rulesCount; i++) {
                bool match = true;
                for (int j = 0; j < pointCount; j++) {
                    if(rules[i].size()<=j)break;
                    if (rules[i][j] == 1) {
                        if (p[j].EmptyInMap(map)) {
                            match = false;
                            break;
                        }
                        if (owner == PieceStatus::None) {
                            owner = map[p[j].x][p[j].y];
                        }
                        if (!p[j].EqualInMap(owner, map)) {
                            match = false;
                            break;
                        }
                    }
                    else if(rules[i][j]==-1){
                        if (p[j].EmptyInMap(map)) {
                            match = false;
                            break;
                        }
                        if (owner == PieceStatus::None) {
                            owner = Opponent(map[p[j].x][p[j].y]);
                        }
                        if (p[j].EqualInMap(owner, map)) {
                            match = false;
                            break;
                        }
                    }
                    else if (rules[i][j] == 0||rules[i][j]==3||rules[i][j]==-2) {
                        if (!p[j].EmptyInMap(map)) {
                            match = false;
                            break;
                        }
                    }
                }

                if (match) {
                    hasMatched=true;
                    for (int j = 0; j < pointCount; j++) {
                        if (rules[i][j] == 0||rules[i][j]==3||rules[i][j]==-2) {
                            //如果已经存在则不重复添加
                            bool found = false;
                            for(const auto& point:ava)
                                if(point.x==p[j].x&&point.y==p[j].y){
                                    found=true;
                                    break;
                                }
                            if(found)continue;
                            if(rules[i][j]==3)
                                ava.insert(ava.begin(),p[j]);
                            else if(rules[i][j]!=-2) ava.push_back(p[j]);
                        }/*Win模型不应该添加任何东西
                        else if(type==ModelType::Win){
                            ava.push_back(p[j]);
                        }*/
                    }
                }
            }
            return hasMatched;
        };
        auto checked=detect();

        if(checked){
            for(int i=0;i<pointCount;i++)
                points.push_back(p[i]);
            Model.whose=owner;
            Model.ava=ava;
            Model.points=points;
            result.push_back(Model);
        }

    };
    auto CheckWin=[&](vector<Point>& plist){
        //匹配五子连珠
        vector<vector<int>> ruleWin={{1,1,1,1,1}};
        Check(plist,ModelType::Win,ruleWin);
    };
    auto CheckH4=[&](vector<Point>& plist){
        vector<vector<int>> ruleH4={{1,1,1,1,0}};
        Check(plist,ModelType::H4,ruleH4);
    };
    auto CheckC4=[&](vector<Point>& plist){
        //匹配冲四
        vector<vector<int>> ruleM4={{1,0,1,1,1},{0,1,1,1,1,-1},{1,1,0,1,1}};
        Check(plist, ModelType::C4, ruleM4);
    };
    auto CheckH3=[&](vector<Point>& plist){
        //匹配活三
        vector<vector<int>> ruleH3={{0,1,1,1,0},
                                    {0,1,3,1,1}};
        Check(plist,ModelType::H3,ruleH3);
    };
    auto CheckM3=[&](vector<Point>& plist){
        //匹配眠三
        vector<vector<int>> ruleM3={{0,3,1,1,1,-1},
                                    {0,1,0,1,1,-1},
                                    {3,1,1,0,1,-1},
                                    {1,0,0,1,1,2},
                                    {1,0,1,0,1,2},
                                    {-1,0,1,1,1,0,-1}};
        Check(plist,ModelType::M3,ruleM3);
    };
    auto CheckH2=[&](vector<Point>& plist){
        //匹配活二
        vector<vector<int>> ruleH2={{0,3,1,1,3,0},
                                    {0,1,0,1,0,2},
                                    {1,0,0,1,2,2}};
        Check(plist,ModelType::H2,ruleH2);
    };
    auto CheckM2=[&](vector<Point>& plist){
        //匹配眠二
        vector<vector<int>> ruleM2={
                {0,0,3,1,1,-1},
                {0,0,1,3,1,0},
                {0,1,0,0,1,-1},
                {1,0,0,0,1,2}};
        Check(plist,ModelType::M2,ruleM2);
    };
    //endregion
    for(int x=0;x<15;x++) {
        for (int y = 0; y < 15; y++) {
            vector<vector<Point>> region;
            //region 遍历所有可能的五个点
            if(y<=10) {
                //取纵向+
                vector<Point> p = { {x, y}, {x, y + 1},  {x, y + 2}, {x, y + 3},  {x, y + 4}};
                if(!CheckEmpty(p,map)) {
                    if (y <= 9)
                        p.push_back({x, y + 5});
                    else p.push_back({-1, -1});
                    region.push_back(p);
                }
            }
            if(y>=4){
                //取纵向-
                vector<Point> p = { {x, y}, {x, y - 1},  {x, y - 2}, {x, y - 3},  {x, y - 4}};
                if(!CheckEmpty(p,map)) {
                    if (y >= 5)
                        p.push_back({x, y - 5});
                    else p.push_back({-1, -1});
                    region.push_back(p);
                }
            }
            if(x<=10) {
                //取横向+
                vector<Point> p = { {x, y}, {x+1, y},  {x+2, y}, {x+3, y},  {x+4, y}};
                if(!CheckEmpty(p,map)) {
                    if (x <= 9)
                        p.push_back({x + 5, y});
                    else p.push_back({-1, -1});
                    region.push_back(p);
                }
            }
            if(x>=4) {
                //取横向-
                vector<Point> p = { {x, y}, {x-1, y},  {x-2, y}, {x-3, y},  {x-4, y}};
                if(!CheckEmpty(p,map)) {
                    if (x >= 5)
                        p.push_back({x - 5, y});
                    else p.push_back({-1, -1});
                    region.push_back(p);
                }
            }
            if(x<=10&&y<=10) {
                //取左上到右下
                vector<Point> p = { {x, y}, {x+1, y + 1},  {x+2, y + 2}, {x+3, y + 3},  {x+4, y + 4}};
                if(!CheckEmpty(p,map)) {
                    if (x <= 9 && y <= 9)
                        p.push_back({x + 5, y + 5});
                    else p.push_back({-1, -1});
                    region.push_back(p);
                }
            }
            if(x>=4&&y>=4) {
                //取左上到右下
                vector<Point> p = { {x, y}, {x-1, y - 1},  {x-2, y - 2}, {x-3, y - 3},  {x-4, y - 4}};
                if(!CheckEmpty(p,map)) {
                    if (x >= 5 && y >= 5)
                        p.push_back({x - 5, y - 5});
                    else p.push_back({-1, -1});
                    region.push_back(p);
                }
            }
            if(x>=4&&y<=10) {
                //取右上到左下
                vector<Point> p = { {x, y}, {x-1, y + 1},  {x-2, y + 2}, {x-3, y + 3},  {x-4, y + 4}};
                if(!CheckEmpty(p,map)) {
                    if (x >= 5 && y <= 9)
                        p.push_back({x - 5, y + 5});
                    else p.push_back({-1, -1});
                    region.push_back(p);
                }
            }
            if(x<=10&&y>=4) {
                //取右上到左下
                vector<Point> p = { {x, y}, {x+1, y - 1},  {x+2, y - 2}, {x+3, y - 3},  {x+4, y - 4}};
                if(!CheckEmpty(p,map)) {
                    if (x <= 9 && y >= 5)
                        p.push_back({x + 5, y - 5});
                    else p.push_back({-1, -1});
                    region.push_back(p);
                }
            }
            //endregion
            for (auto &item: region) {
                CheckWin(item);
                CheckH4(item);
                CheckC4(item);
                CheckH3(item);
                CheckM3(item);
                CheckH2(item);
                CheckM2(item);
            }
        }
    }
    //检查二维模型(Cube)
    vector<ChessModel_Single> Checked;
    vector<Point> Added;
    int OrdCount=result.size();
    for(int i=0;i<OrdCount;i++){
        auto model=result[i];
        if(model.type==ModelType::H3||model.type==ModelType::H2||model.type==ModelType::H4||model.type==ModelType::M3) {
            for (const auto &p: model.ava) {
                bool found = false;
                ChessModel_Single foundModel;
                for (const auto &item: Checked) {
                    //平行的重复点不是Cube模型
                    int vec_x = item.ava.x - item.point.x, vec_y = item.ava.y - item.point.y;
                    int inc_x = p.x - model.points[0].x, inc_y = p.y - model.points[0].y;
                    if (item.whose == model.whose && item.ava.Equal(p) && (vec_x * inc_y != vec_y * inc_x)) {
                        found = true;
                        foundModel = item;
                        break;
                    }
                }
                if (found) {
                    bool added = false;
                    for (const auto &o: Added) {
                        if (o.Equal(foundModel.ava)) {
                            added = true;
                            break;
                        }
                    }
                    if (!added) {
                        ChessModel m;
                        ModelType most = model.type > foundModel.type ? model.type : foundModel.type;
                        if (most == ModelType::H2)
                            m.type = ModelType::Cube3;
                        else
                            m.type = ModelType::Cube4;
                        m.ava.push_back(foundModel.ava);
                        m.points = model.points;
                        m.whose = foundModel.whose;
                        result.push_back(m);
                        Added.push_back(foundModel.ava);
                    }
                } else {
                    Checked.push_back(ChessModel_Single{model.type, p, model.points[0], model.whose});
                }
            }
        }
    }
    ShellSort(result);
    CheckerTime+= chrono::duration_cast<chrono::milliseconds>((chrono::high_resolution_clock::now()-now)).count();
    return result;
}


string ChessModel::GetModelName(ModelType type){
    switch (type){
        case ModelType::M2:
            return "M2";
        case ModelType::H2:
            return "H2";
        case ModelType::M3:
            return "M3";
        case ModelType::H3:
            return "H3";
        case ModelType::Cube3:
            return "Cube3";
        case ModelType::C4:
            return "C4";
        case ModelType::Cube4:
            return "Cube4";
        case ModelType::H4:
            return "H4";
        case ModelType::Win:
            return "Win";
        default:
            return "Unknown";
    }
}

vector<ChessModel> ModelChecker::PrintMapModel(const ChessMap &map,string ptr) {
    //region 调试信息
    vector<ChessModel> list = ModelChecker::CheckModel(MapData);
    cout << "-----"<<ptr<<"--------Checked Model Count: " << list.size() << "-------------" << endl;
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
    return list;
    //endregion
}
