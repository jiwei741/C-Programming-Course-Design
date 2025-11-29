#include <graphics.h>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <stack>
#include <vector>
#include <string>
#include <windows.h>
#include <tchar.h>  //_T()宏,用于下面汉字
#include "Show.h"
#include "Chess.h"
#include "Users.h"
#pragma comment(lib,"Winmm.lib")
using namespace std;
constexpr int  Width = 800;
constexpr int  High = 640;
vector<vector<int>> map(18, vector<int>(18, -1)), cop(map);		//map是棋盘，-1空，0电脑，1友方

double myPos = 0, aiPos = 0;		//我方和电脑棋局得分
int aiset;							//记录电脑上一步下的位置


MyStack<pair<int, int>> stk;//用以控制悔棋

void ShowMainMenu() {
    //用户退出界面设计，然后算法：困难模式以及普通模式
    initgraph(Width, High);
    setbkcolor(WHITE);
    cleardevice();
    settextcolor(BLACK);
    settextstyle(30, 0, _T("宋体"));

    outtextxy(300, 100, _T("欢迎来到 AI 五子棋"));

    setfillcolor(LIGHTGRAY);
    solidrectangle(300, 200, 500, 250);
    outtextxy(350, 210, _T("开始游戏"));

    solidrectangle(300, 270, 500, 320);
    outtextxy(350, 280, _T("退出游戏"));

    MOUSEMSG m;
    while (true) {
        m = GetMouseMsg();
        if (m.uMsg == WM_LBUTTONDOWN) {
            int x = m.x, y = m.y;
            if (x >= 300 && x <= 500 && y >= 200 && y <= 250) {
                break; // 开始游戏
            }
            if (x >= 300 && x <= 500 && y >= 270 && y <= 320) {
                closegraph();
                exit(0); // 退出程序
            }
        }
    }
    cleardevice(); // 清空主界面
}

bool ShowContinueMenu() {
    cleardevice();  // 清空当前画面
    setbkcolor(WHITE);
    settextcolor(BLACK);
    settextstyle(30, 0, _T("宋体"));

    outtextxy(300, 100, _T("游戏结束！"));

    setfillcolor(LIGHTGRAY);
    solidrectangle(300, 200, 500, 250);
    outtextxy(340, 210, _T("继续游戏"));

    solidrectangle(300, 270, 500, 320);
    outtextxy(340, 280, _T("结束游戏"));

    MOUSEMSG m;
    while (true) {
        m = GetMouseMsg();
        if (m.uMsg == WM_LBUTTONDOWN) {
            int x = m.x, y = m.y;
            if (x >= 300 && x <= 500 && y >= 200 && y <= 250) {
                return true;  // 继续游戏
            }
            if (x >= 300 && x <= 500 && y >= 270 && y <= 320) {
                return false;  // 退出游戏
            }
        }
    }
}


int main(){
    ShowMainMenu();
    srand((unsigned int)time(NULL)); // 初始化随机数种子

	MyChess mychs;
	Show s;
	AiChess ai;
	Users user;
	std::mutex mt;
	user.Read();
    
    
Start:
    map = cop;
    mychs.steps = 0;
    initgraph(Width, High);

    // ===== 每局游戏前都选择 AI 模式 =====
    cleardevice();
    settextcolor(WHITE); // 设置文字颜色为白色
    setbkmode(TRANSPARENT);
    settextstyle(30, 0, _T("宋体"));

    outtextxy(280, 100, _T("请选择AI模式："));

    // 最优模式按钮
    setfillcolor(LIGHTGRAY);
    solidrectangle(250, 180, 550, 230);
    outtextxy(290, 190, _T("1. 最优模式（困难）"));

    // 普通模式按钮
    solidrectangle(250, 250, 550, 300);
    outtextxy(290, 260, _T("2. 普通模式（简单）"));

    // 地狱模式按钮
    solidrectangle(250, 320, 550, 370);
    outtextxy(290, 330, _T("3. 地狱模式（极难）"));

    //超霸模式按钮
    solidrectangle(250, 390, 550, 440);
    outtextxy(290, 400, _T("4. 智霸模式（最强）"));

    MOUSEMSG modeMsg;
    while (true) {
        modeMsg = GetMouseMsg();
        if (modeMsg.uMsg == WM_LBUTTONDOWN) {
            int x = modeMsg.x, y = modeMsg.y;
            if (x >= 250 && x <= 550) {
                if (y >= 180 && y <= 230) {
                    AiChess::useOptimalMode = true;
                    AiChess::useHellMode = false; // 确保其他模式标志位不冲突
                    AiChess::useZhiBaMode = false; // 关闭智霸模式
                    break;
                }
                if (y >= 250 && y <= 300) {
                    AiChess::useOptimalMode = false;
                    AiChess::useHellMode = false;
                    AiChess::useZhiBaMode = false; // 关闭智霸模式
                    break;
                }
                if (y >= 320 && y <= 370) {
                    AiChess::useOptimalMode = true;
                    AiChess::useHellMode = true; // 激活地狱模式
                    AiChess::useZhiBaMode = false; // 关闭智霸模式
                    break;
                }
                if (y >= 390 && y <= 440) {
                    AiChess::useOptimalMode = false;
                    AiChess::useHellMode = false;
                    AiChess::useZhiBaMode = true; // 智霸模式激活
                    break;
                }
            }
        }
    }

    cleardevice(); // 清屏，进入后续游戏流程


    // 猜拳决定先手
    int user_choice, ai_choice;
    
    while (true) {
        cout << "猜拳决定谁先手：请输入你的选择（0-剪刀，1-石头，2-布，3-退出游戏）：";
        cin >> user_choice;

        if (user_choice == 3) {
            cout << "你选择退出游戏，程序结束。" << endl;
            exit(0);
        }

        if (user_choice < 0 || user_choice > 2) {
            cout << "无效输入，请重新输入 0~2 或 3 退出。" << endl;
            continue;
        }

        ai_choice = rand() % 3;
        cout << "电脑出了：" << ai_choice << endl;

        if (user_choice == ai_choice) {
            cout << "平局，重新猜拳！" << endl;
            continue;
        }
        break;
    }

    bool userFirst = false;
    if ((user_choice == 0 && ai_choice == 2) ||
        (user_choice == 1 && ai_choice == 0) ||
        (user_choice == 2 && ai_choice == 1)) {
        userFirst = true;
        cout << "你赢了，用户先手！" << endl;
    }
    else {
        cout << "你输了，电脑先手！" << endl;
    }

    Sleep(2000);  // 暂停2秒后跳转棋盘界面

	map = cop;
	mychs.steps = 0;
	initgraph(Width,High);
	while(1){		
        if (userFirst) {
            mychs.now = true;
            s.Mouse(mychs, user);
            s.show(mychs, user);
            if (mychs.gameOver(s, user)) {
                user.SaveStepsToFile("record.txt");  // 保存棋谱
                s.ShowRecordViewer(user);              // 显示棋谱查看界面
                goto Start;
            }


            ai.Get_key_Setchess(mychs);
            s.show(mychs, user);
            if (ai.gameOver(s, user)) {
                user.SaveStepsToFile("record.txt");  // 保存棋谱
                s.ShowRecordViewer(user);              // 显示棋谱查看界面
                goto Start;
            }

        }
        else {
            ai.Get_key_Setchess(mychs);
            s.show(mychs, user);
            if (ai.gameOver(s, user)) {
                user.SaveStepsToFile("record.txt");  // 保存棋谱
                s.ShowRecordViewer(user);              // 显示棋谱查看界面
                goto Start;
            }


            mychs.now = true;
            s.Mouse(mychs, user);
            s.show(mychs, user);
            if (mychs.gameOver(s, user)) {
                user.SaveStepsToFile("record.txt");  // 保存棋谱
                s.ShowRecordViewer(user);              // 显示棋谱查看界面
                goto Start;
            }

        }

	}
}

