
#ifndef USERS_H
#define USERS_H

#include <vector>
#include <utility>
#include <string>
#include "Chess.h"

struct StepInfo {
    int x, y;             // 下棋位置
    int color;            // 棋子颜色
    int scoreAI;          // AI分数估计
    int scorePlayer;      // 玩家分数估计
    std::string comment;  // 文字解说
};

class Users {
public:
    int wincnt = 0;     // 胜利场数
    int failcnt = 0;    // 失败场数
    int drawcnt = 0;    // 平局场数
    int minsteps = 0;   // 胜利最短步数
    int restcnt = 10;   // 剩余悔棋步数

    std::vector<StepInfo> stepRecord;  // 棋谱记录
    int currentStep = 0;               // 当前查看步骤（用于回放）

    void Read();   // 读取文档
    void Write();  // 存档

    bool SaveStepsToFile(const std::string& filename); // 保存棋谱
    std::vector<std::pair<int, int>> LoadStepsFromFile(const std::string& filename); // 加载棋谱
};

#endif
