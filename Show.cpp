#include "Show.h"
#include "Chess.h"
#include "Users.h"
#include <graphics.h>
#include <vector>
#include <conio.h>
#include <string>
#include <iostream>
#include <fstream>

using namespace std;

#pragma comment(lib,"Winmm.lib")
using namespace std;
extern int aiset;
extern MyStack<pair<int, int>> stk;
extern vector<vector<int>> map;
extern double myPos, aiPos;
extern vector<StepInfo> stepRecord;

const int  Width = 800;
const int  High = 640;

static const char dir1[] = { "ABCDEFGHIJKLMNOPQR" };
static const char dir2[] = { "123456789ABCDEFGHI" };
void Show::show(MyChess& mychs, Users& user)
{
	BeginBatchDraw();
	setbkcolor(YELLOW);
	cleardevice();
	setcolor(BLACK);
	TCHAR row, col;
	for(int i = 20; i <= 530; i += 30){
		line(20, i, 530, i);
		line(i, 20, i, 530);
		row = dir1[reswitch_xy(i)];
		col = dir2[reswitch_xy(i)];
		outtextxy(5, i - 7, row);
		outtextxy(i - 5, 545, col);
	}
	setfillcolor(BLACK);
	//棋盘的4个小黑点
	fillcircle(170, 170, 3);
	fillcircle(170, 380, 3);
	fillcircle(380, 170, 3);
	fillcircle(380, 380, 3);
	for(int i = 0; i < 18; i++){
		for(int j = 0; j < 18; j++){
			if (map[i][j] != -1) {
				setChess(switch_xy(i), switch_xy(j), map[i][j]);
			}
		}
	}
	TCHAR str[250];
	wchar_t* wstr = new wchar_t[100];
	//double nowTime = clock(), sec = 20 - ((nowTime - startTime) / CLOCKS_PER_SEC);
	mychs.get_grade(user, wstr);
	setcolor(BLACK);
	//显示用户信息
	outtextxy(600, 5, _T("用户：学生"));
	_stprintf_s(str, _T("胜场：%d"), user.wincnt);
	outtextxy(600, 25, str);
	_stprintf_s(str, _T("负场：%d"), user.failcnt);
	outtextxy(600, 45, str);
	_stprintf_s(str, _T("平局：%d"), user.drawcnt);
	outtextxy(600, 65, str);
	_stprintf_s(str, _T("胜率：%.2f%%"), user.failcnt ? (double)user.wincnt / (double)(user.wincnt + user.failcnt) * 100: 0);
	outtextxy(600, 85, str);
	_stprintf_s(str, _T("段位：%s"), wstr);
	outtextxy(600, 105, str);
	_stprintf_s(str, _T("胜场最短步数: %d"), user.minsteps);
	outtextxy(600, 125, str);
	_stprintf_s(str, _T("剩余悔棋步数: %d"), user.restcnt);
	outtextxy(600, 145, str);

	setcolor(BLUE);
	line(580, 2, 580, 170);
	line(580, 170, 735, 170);
	line(735, 170, 735, 2);
	line(580, 2, 735, 2);
	setcolor(BLACK);

	_stprintf_s(str, _T("您当前走了 %d 步"), mychs.steps);
	outtextxy (600, 195, str);
	outtextxy(600, 225, _T("当前鼠标行列："));

	setcolor(RED);
	rectangle(620, 265, 680, 305);
	outtextxy(634, 276, _T("悔棋"));
	setcolor(BLACK);

	//注意绘图时的坐标是逆的，它的y轴是向下的
	_stprintf_s(str, _T("电脑刚才所下位置：%c行 %c列"),
		(aiset & 0x1f) < 15 ? dir1[aiset & 0x1f] : _T('?'),  //检查是否越界
		(aiset >> 5) < 15 ? dir2[aiset >> 5] : _T('?'));     //检查是否越界
	outtextxy(570, 330, str);

	
	_stprintf_s(str, _T("您的棋局得分: %.1f"), myPos);
	outtextxy(570, 370, str);
	_stprintf_s(str, _T("对方棋局得分: %.1f"), aiPos);
	outtextxy(570, 410, str);
	mychs.get_assuse(wstr);
	_stprintf_s(str, _T("当前您的棋局评估：%s"), wstr);
	outtextxy(570, 450, str);

	if(mychs.now == true){
		outtextxy(180, 595, _T("现在是您的回合，请下棋..."));
	}
	else
		outtextxy(160, 595, _T("现在是对方的回合，请您等待..."));
	FlushBatchDraw();
	EndBatchDraw();
	return;
}

void Show::setChess(int x, int y, int choice)//0为电脑下棋，1为自己下棋
{
	setfillcolor(RED);
	if (!choice) 
		setfillcolor(BLACK);
	fillcircle(x, y, 14);
	return;
}

void Show::Mouse(MyChess& mychs, Users& user)
{
	MOUSEMSG m;
	while(true){
		TCHAR str[250];
		m = GetMouseMsg();
		int x = m.x, y = m.y;
		switch (m.uMsg)
		{
		case WM_MOUSEMOVE:	
			show(mychs, user);
			if(mychs.Get_xy(x, y) && map[reswitch_xy(x)][reswitch_xy(y)] == -1){
				int x1 = reswitch_xy(x), y1 = reswitch_xy(y);
				_stprintf_s(str, _T("%c行 %c列"), dir1[y1], dir2[x1]);
				outtextxy(705, 225, str);
				circle(x, y, 14);		
			}
			else if(m.x > 620 && m.x < 680 && m.y > 265 && m.y < 305){
				setcolor(BLUE);
				rectangle(620, 265, 680, 305);
				outtextxy(634, 276, _T("悔棋"));
				setcolor(BLACK); 
			}
			break;

		case WM_LBUTTONUP:
			//下棋过程
			if(mychs.Get_xy(x, y) && map[reswitch_xy(x)][reswitch_xy(y)] == -1){
				int x1 = reswitch_xy(x), y1 = reswitch_xy(y);
				stk.push({ aiset, (x1 << 5) | y1 });
				map[x1][y1] = 1;
				mychs.now = false;
				mychs.steps++;
				myPos = mychs.GetNowPoints(1);
				aiPos = mychs.GetNowPoints(0);

				//记录落子信息
				StepInfo step;
				step.x = x1;
				step.y = y1;
				step.color = 1;  //用户颜色
				step.scoreAI = aiPos;
				step.scorePlayer = myPos;
				step.comment = "用户落子";
				user.stepRecord.push_back(step);
				user.currentStep++;

				mciSendString(_T("close chs"), NULL, 0, NULL);
				mciSendString(_T("open setChs.wav alias chs"), NULL, 0, NULL);
				mciSendString(_T("play chs"), NULL, 0, NULL);
				return;
			}
			//悔棋
			else if(m.x > 620 && m.x < 680 && m.y > 265 && m.y < 305){
				if(!user.restcnt){
					MessageBox(0, _T("您没有悔棋机会了，加油赢棋以获得更多机会吧！"), _T("NO"), 0);
					continue;
				}
				if(stk.empty()){
					MessageBox(0, _T("现在无法悔棋！"), _T("NO"), 0);
					continue;
				}				
				auto it = stk.top();
				stk.pop();
				int ai = it.first, self = it.second;			
				map[self >> 5][self & 0x1f] = -1;
				map[aiset >> 5][aiset & 0x1f] = -1;
				aiset = ai;

				myPos=mychs.GetNowPoints(1);
				aiPos=mychs.GetNowPoints(0);
				user.restcnt--;
				user.Write();
				show(mychs,user);
				return Mouse(mychs,user);
			}
		}
	}
	return;
}

void Show::End(int who, Users& user, int steps){
	if(!who) return;
	LOGFONT f;
	gettextstyle(&f);						//获取当前字体设置
	f.lfHeight = 20;						//设置字体高度为 48
	_tcscpy_s(f.lfFaceName, _T("黑体"));		//设置字体为“黑体”(高版本 VC 推荐使用 _tcscpy_s 函数)
	f.lfQuality = ANTIALIASED_QUALITY;		//设置输出效果为抗锯齿  
	settextstyle(&f);						//设置字体样式
	outtextxy(615, 500, _T("游戏结束"));
	if(who == 1){
		MessageBox(0, _T("恭喜您胜利！"), _T("YES"), 0);
		setbkcolor(GREEN);
		cleardevice();
		outtextxy(250, 200, _T("恭喜您胜利！"));	
		outtextxy(250, 300, _T("请按任意键继续游戏..."));
		user.wincnt++;
		user.restcnt += 10;
		if(user.minsteps == 0)
			user.minsteps = steps;
		else
			user.minsteps = min(user.minsteps, steps);
		mciSendString(_T("close win"), NULL, 0, NULL);
		mciSendString(_T("open win.wav alias win"), NULL, 0, NULL);
		mciSendString(_T("play win"), NULL, 0, NULL);
	}
	else {
		MessageBox(0, _T("您失败了，不要灰心下次再来吧！！"), _T("NO"), 0);
		setbkcolor(YELLOW);
		cleardevice();
		outtextxy(250, 200, _T("您失败了，不要灰心下次再来吧！"));
		outtextxy(250, 300, _T("请按任意键继续游戏..."));
		mciSendString(_T("close det"), NULL, 0, NULL);
		mciSendString(_T("open defet.mp3 alias det"), NULL, 0, NULL);
		mciSendString(_T("play det"), NULL, 0, NULL);
		user.failcnt++;
	}
	user.Write();
	int ch = _gettch();
	while (!stk.empty()) stk.pop();
	aiPos = 0;
	myPos = 0;
	closegraph();
	return;
}

void Show::DrawChessRecord(const vector<pair<int, int>>& steps) {
	for (int i = 0; i < steps.size(); ++i) {
		int x = steps[i].first;
		int y = steps[i].second;
		cout << "绘制第 " << i + 1 << " 步：" << x << ", " << y << endl;

		setfillcolor((i % 2 == 0) ? BLACK : WHITE); // 轮流绘制黑白
		fillcircle(20 + x * 30, 20 + y * 30, 14);
	}
}

void Show::DrawChessBoard() {
	for (int i = 0; i <= 18; ++i) {
		// 横线
		line(20, 20 + i * 30, 530, 20 + i * 30);
		// 竖线
		line(20 + i * 30, 20, 20 + i * 30, 530);
	}
}

void DrawChessMan(int row, int col, int color) {
	const int Radius = 12; // 棋子半径，你可以调大点
	int x = switch_xy(col);
	int y = switch_xy(row);

	setfillcolor(color); // BLACK or WHITE
	setlinecolor(BLACK);
	solidcircle(x, y, Radius);
}


void Show::ShowRecordViewer(Users& user) {
	initgraph(800, 300); // 宽一些展示更多评价
	setbkcolor(WHITE);
	cleardevice();

	// 加载棋谱
	vector<pair<int, int>> steps = user.LoadStepsFromFile("record.txt");

	// 初始化一个虚拟棋盘（0空，1黑，2白）
	const int SIZE = 15;
	int board[SIZE][SIZE] = { 0 };

	// 分析统计
	int blackCount = 0, whiteCount = 0;
	int blackLive3 = 0, whiteLive3 = 0;
	int blackLive4 = 0, whiteLive4 = 0;
	int blackWin = 0, whiteWin = 0;

	for (int i = 0; i < steps.size(); ++i) {
		int x = steps[i].first;
		int y = steps[i].second;
		int color = (i % 2 == 0) ? 1 : 2; // 1黑 2白
		board[x][y] = color;

		// 简单评分函数
		auto check_dir = [&](int dx, int dy) {
			int cnt = 1;
			int left = 0, right = 0;

			for (int d = 1; d < 5; ++d) {
				int nx = x + dx * d, ny = y + dy * d;
				if (nx < 0 || ny < 0 || nx >= SIZE || ny >= SIZE) break;
				if (board[nx][ny] == color) cnt++;
				else { right = board[nx][ny] == 0; break; }
			}
			for (int d = 1; d < 5; ++d) {
				int nx = x - dx * d, ny = y - dy * d;
				if (nx < 0 || ny < 0 || nx >= SIZE || ny >= SIZE) break;
				if (board[nx][ny] == color) cnt++;
				else { left = board[nx][ny] == 0; break; }
			}
			return std::make_tuple(cnt, left, right);
			};

		int live3 = 0, live4 = 0, win = 0;
		// 替换结构化绑定的语法
		for (int i = 0; i < 4; ++i) {
			int dx = 0, dy = 0;
			switch (i) {
			case 0: dx = 1; dy = 0; break;  // 水平
			case 1: dx = 0; dy = 1; break;  // 垂直
			case 2: dx = 1; dy = 1; break;  // 正对角
			case 3: dx = 1; dy = -1; break; // 反对角
			}

			std::tuple<int, int, int> result = check_dir(dx, dy);
			int cnt = std::get<0>(result);
			int left = std::get<1>(result);
			int right = std::get<2>(result);

			if (cnt >= 5) win++;  // 如果有五连
			else if (cnt == 4 && left && right) live4++;  // 如果是活四
			else if (cnt == 3 && left && right) live3++;  // 如果是活三
		}


		if (color == 1) {
			blackCount++;
			blackLive3 += live3;
			blackLive4 += live4;
			blackWin += win;
		}
		else {
			whiteCount++;
			whiteLive3 += live3;
			whiteLive4 += live4;
			whiteWin += win;
		}
	}

	// 设置字体样式
	settextstyle(20, 0, _T("黑体"));
	settextcolor(BLACK);

	int y = 30;
	outtextxy(20, y, _T("棋谱智能分析报告："));
	y += 30;

	outtextxy(20, y, _T("总步数："));
	outtextxy(150, y, to_wstring(steps.size()).c_str());
	y += 25;

	outtextxy(20, y, _T("黑子数量："));
	outtextxy(150, y, to_wstring(blackCount).c_str());
	outtextxy(300, y, _T("白子数量："));
	outtextxy(430, y, to_wstring(whiteCount).c_str());
	y += 25;

	outtextxy(20, y, _T("黑方活三："));
	outtextxy(150, y, to_wstring(blackLive3).c_str());
	outtextxy(300, y, _T("白方活三："));
	outtextxy(430, y, to_wstring(whiteLive3).c_str());
	y += 25;

	outtextxy(20, y, _T("黑方活四："));
	outtextxy(150, y, to_wstring(blackLive4).c_str());
	outtextxy(300, y, _T("白方活四："));
	outtextxy(430, y, to_wstring(whiteLive4).c_str());
	y += 25;

	outtextxy(20, y, _T("黑方五连次数："));
	outtextxy(220, y, to_wstring(blackWin).c_str());
	outtextxy(400, y, _T("白方五连次数："));
	outtextxy(600, y, to_wstring(whiteWin).c_str());
	y += 40;

	// 综合评价
	std::wstring comment;
	if (blackWin > 0 && whiteWin == 0)
		comment = L"黑方胜势明显，完成五连！";
	else if (whiteWin > 0 && blackWin == 0)
		comment = L"白方胜势明显，完成五连！";
	else if (blackLive4 > whiteLive4 + 1)
		comment = L"黑棋多次形成活四，控局能力强。";
	else if (whiteLive4 > blackLive4 + 1)
		comment = L"白棋多次形成活四，进攻凌厉。";
	else if (blackLive3 > whiteLive3 + 2)
		comment = L"黑棋频繁制造活三，布局更紧凑。";
	else if (whiteLive3 > blackLive3 + 2)
		comment = L"白棋频繁制造活三，积极寻机。";
	else
		comment = L"局势胶着，胜负难料。";

	outtextxy(20, y, comment.c_str());

	y += 40;
	outtextxy(20, y, _T("3秒后自动关闭..."));
	Sleep(3000);
	closegraph();
}


