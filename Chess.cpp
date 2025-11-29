#include "Chess.h"
#include "Show.h"
#include "Users.h"
#include <vector>
#include <string>

#pragma comment(lib,"Winmm.lib")
extern vector<vector<int>> map;
extern int aiset;
extern double aiPos, myPos;
bool AiChess::useOptimalMode = true;
bool AiChess::useHellMode = false;
bool AiChess::useZhiBaMode = false;
std::vector<StepInfo> stepRecord;

std::string GenerateComment(int aiScore, int playerScore) {
	if (aiScore >= 10000) return "AI即将胜利";
	if (aiScore >= 5000) return "AI已形成活四";
	if (aiScore >= 1000) return "AI形成活三";
	if (playerScore >= 10000) return "玩家威胁极大";
	if (playerScore >= 5000) return "玩家形成活四";
	return "局势胶着";
}


int AiChess::EvaluateBoard(vector<vector<int>>& board, int player) {
	// 简单启发式评分函数（可替换为更复杂的）
	int score = 0;
	// 可遍历行、列、对角线，计算某方形成的连子数量，如：
	for (int i = 0; i < 15; i++) {
		for (int j = 0; j < 11; j++) {
			int count = 0;
			for (int k = 0; k < 5; k++) {
				if (board[i][j + k] == player) count++;
				else if (board[i][j + k] != 0) { count = 0; break; }
			}
			if (count > 0) score += count * count;
		}
	}
	return score;
}

int AiChess::AlphaBetaAI(vector<vector<int>>& board, int player, int depth, int alpha, int beta) {
	if (depth == 0) return EvaluateBoard(board, player);

	int maxEval = -1e9;
	for (int i = 0; i < 15; i++) {
		for (int j = 0; j < 15; j++) {
			if (board[i][j] == 0) {
				board[i][j] = player;
				int eval = -AlphaBetaAI(board, 3 - player, depth - 1, -beta, -alpha);
				board[i][j] = 0;
				if (eval > maxEval) maxEval = eval;
				if (eval > alpha) alpha = eval;
				if (alpha >= beta) return alpha; // 剪枝
			}
		}
	}
	return maxEval;
}

POINT AiChess::FindBestMove_ZhiBa(vector<vector<int>>& board, int player) {
	int bestScore = -1e9;
	POINT bestMove = { -1, -1 };
	for (int i = 0; i < 15; i++) {
		for (int j = 0; j < 15; j++) {
			if (board[i][j] == 0) {
				board[i][j] = player;
				int score = -AlphaBetaAI(board, 3 - player, 3, -1e9, 1e9); // depth = 3
				board[i][j] = 0;
				if (score > bestScore) {
					bestScore = score;
					bestMove = { j, i };
				}
			}
		}
	}
	return bestMove;
}


int MyChess::Get_xy(int& x, int& y)//判断鼠标是否落入空交点内
{
	for (int i = 0; i < 18; i++) {
		for (int j = 0; j < 18; j++) {
			if ((x - switch_xy(i)) * (x - switch_xy(i)) + (y - switch_xy(j)) * (y - switch_xy(j)) <= 36) {
				x = switch_xy(i);
				y = switch_xy(j);
				return 1;
			}
		}
	}
	return 0;
}



int AiChess::EvaluatePoint(int x, int y, int color) {
	const int dx[4] = { 1, 0, 1, 1 };
	const int dy[4] = { 0, 1, 1, -1 };

	int score = 0;
	for (int d = 0; d < 4; ++d) {
		int count = 1, blocks = 0, empty = 0;

		// 正方向
		int i = x + dx[d], j = y + dy[d];
		while (i >= 0 && i < 18 && j >= 0 && j < 18 && map[i][j] == color) {
			++count; i += dx[d]; j += dy[d];
		}
		if (i < 0 || i >= 18 || j < 0 || j >= 18 || (map[i][j] != -1 && map[i][j] != color)) blocks++;

		// 反方向
		i = x - dx[d], j = y - dy[d];
		while (i >= 0 && i < 18 && j >= 0 && j < 18 && map[i][j] == color) {
			++count; i -= dx[d]; j -= dy[d];
		}
		if (i < 0 || i >= 18 || j < 0 || j >= 18 || (map[i][j] != -1 && map[i][j] != color)) blocks++;

		// 按照棋形打分（可调参数）
		if (count >= 5) score += 100000;
		else if (count == 4 && blocks == 0) score += 10000;       // 活四
		else if (count == 4 && blocks == 1) score += 1000;        // 冲四
		else if (count == 3 && blocks == 0) score += 500;         // 活三
		else if (count == 3 && blocks == 1) score += 100;         // 眠三
		else if (count == 2 && blocks == 0) score += 50;          // 活二
		else if (count == 2 && blocks == 1) score += 10;          // 眠二
	}
	return score;
}


void AiChess::Get_key_Setchess(MyChess& mychs)
{
	if (mychs.steps == 0) {
		map[8][8] = 0;
		aiset = 0x108;
	}
	else {
		int bestScore = -1;
		int bestX = -1, bestY = -1;

		if (useHellMode) {
			// ----------- 地狱模式 ------------
			for (int i = 0; i < 18; ++i) {
				for (int j = 0; j < 18; ++j) {
					if (map[i][j] != -1) continue;

					map[i][j] = 0;
					int aiScore = EvaluatePoint(i, j, 0);
					map[i][j] = 1;
					int playerScore = EvaluatePoint(i, j, 1);
					map[i][j] = -1;

					int totalScore = aiScore * 1.2+ playerScore * 2.5; // 更强调防守和均衡
					if (totalScore > bestScore) {
						bestScore = totalScore;
						bestX = i;
						bestY = j;
					}
				}
			}
		}
		else if (!useOptimalMode) {
			// ----------- 普通模式 ------------
			int dx[8] = { 1, 1, 0, -1, -1, -1, 0, 1 };
			int dy[8] = { 0, 1, 1, 1, 0, -1, -1, -1 };

			auto dfs = [&](int x, int y, int color) {
				int maxLine = 1;
				for (int d = 0; d < 4; ++d) {
					int count = 1;
					for (int dir = 0; dir < 2; ++dir) {
						int nx = x, ny = y;
						while (true) {
							nx += dx[d + dir * 4];
							ny += dy[d + dir * 4];
							if (nx < 0 || ny < 0 || nx >= 18 || ny >= 18) break;
							if (map[nx][ny] == color) ++count;
							else break;
						}
					}
					maxLine = max(maxLine, count);
				}
				return maxLine;
				};

			for (int i = 0; i < 18; ++i) {
				for (int j = 0; j < 18; ++j) {
					if (map[i][j] != -1) continue;

					map[i][j] = 0;
					int aiLine = dfs(i, j, 0);
					map[i][j] = 1;
					int playerLine = dfs(i, j, 1);
					map[i][j] = -1;

					int score = aiLine * 10 + playerLine * 12;
					if (score > bestScore) {
						bestScore = score;
						bestX = i;
						bestY = j;
					}
				}
			}
		}
		else if (useOptimalMode) {
			// ----------- 最优模式 ------------
			vector<pair<int, int>> candidates;

			for (int i = 0; i < 18; ++i) {
				for (int j = 0; j < 18; ++j) {
					if (map[i][j] != -1) continue;
					bool ner = false;
					for (int dx = -2; dx <= 2 && !ner; ++dx) {
						for (int dy = -2; dy <= 2 && !ner; ++dy) {
							int ni = i + dx, nj = j + dy;
							if (ni >= 0 && ni < 18 && nj >= 0 && nj < 18 && map[ni][nj] != -1) {
								ner = true;
							}
						}
					}
					if (ner) candidates.emplace_back(i, j);
				}
			}

			for (auto& p : candidates) {
				int i = p.first, j = p.second;
				int aiScore = Get_Points(i, j, 0);
				int playerScore = Get_Points(i, j, 1);
				int totalScore = aiScore + int(playerScore * 1.5);

				if (totalScore > bestScore) {
					bestScore = totalScore;
					bestX = i;
					bestY = j;
				}
			}
		}else if (useZhiBaMode) {
			//超霸模式
			POINT p = FindBestMove_ZhiBa(map, 0);
			bestX = p.y;
			bestY = p.x;
		}


		// 若未找到最佳位置，随机选一个合法点
		if (bestX == -1 || bestY == -1) {
			srand((unsigned)time(0));
			do {
				bestX = rand() % 18;
				bestY = rand() % 18;
			} while (map[bestX][bestY] != -1);
		}

		aiset = (bestX << 5) | bestY;
		map[bestX][bestY] = 0;
		// ---- 记录本步 ----
		StepInfo step;
		step.x = bestX;
		step.y = bestY;

		// 获取评分（按当前模式获取 AI/玩家估值）
		if (useHellMode)
		{
			step.scoreAI = EvaluatePoint(bestX, bestY, 0);
			step.scorePlayer = EvaluatePoint(bestX, bestY, 1);
		}
		else if (useOptimalMode)
		{
			step.scoreAI = Get_Points(bestX, bestY, 0);
			step.scorePlayer = Get_Points(bestX, bestY, 1);
		}
		else if (useZhiBaMode)
		{
			step.scoreAI = EvaluatePoint(bestX, bestY, 0);  // 或更复杂方式
			step.scorePlayer = EvaluatePoint(bestX, bestY, 1);
		}
		else
		{
			// 普通模式用 dfs 简化评估
			auto dfs = [&](int x, int y, int color) {
				int dx[8] = { 1, 1, 0, -1, -1, -1, 0, 1 };
				int dy[8] = { 0, 1, 1, 1, 0, -1, -1, -1 };
				int maxLine = 1;
				for (int d = 0; d < 4; ++d) {
					int count = 1;
					for (int dir = 0; dir < 2; ++dir) {
						int nx = x, ny = y;
						while (true) {
							nx += dx[d + dir * 4];
							ny += dy[d + dir * 4];
							if (nx < 0 || ny < 0 || nx >= 18 || ny >= 18) break;
							if (map[nx][ny] == color) ++count;
							else break;
						}
					}
					maxLine = max(maxLine, count);
				}
				return maxLine;
				};
			step.scoreAI = dfs(bestX, bestY, 0) * 10;
			step.scorePlayer = dfs(bestX, bestY, 1) * 12;
		}

		step.comment = GenerateComment(step.scoreAI, step.scorePlayer);
		stepRecord.push_back(step);

	}

	myPos = mychs.GetNowPoints(1);
	aiPos = mychs.GetNowPoints(0);
	Sleep(500);
	mciSendString(_T("close chs"), NULL, 0, NULL);
	mciSendString(_T("open setChs.wav alias chs"), NULL, 0, NULL);
	mciSendString(_T("play chs"), NULL, 0, NULL);
}



int Chess::Get_Points(int x, int y, int ch)
{
	int ret = 0;
	int x1(x), y1(y), x2(x), y2(y), x3(x), y3(y), x4(x), y4(y);
	for (int i = 0; i < 5; i++) {
		if (is_Ok(x1, y1)) {
			ret += x_Points(Get_Nums1(x1, y1), ch);
			y1++;
		}
		if (is_Ok(x2, y2)) {
			ret += x_Points(Get_Nums2(x2, y2), ch);
			x2++;
		}
		if (is_Ok(x3, y3)) {
			ret += x_Points(Get_Nums3(x3, y3), ch);
			x3--, y3++;
		}
		if (is_Ok(x4, y4)) {
			ret += x_Points(Get_Nums4(x4, y4), ch);
			x4++, y4++;
		}
	}
	return ret;
}

vector<int> Chess::Get_Nums1(int x, int y)
{
	int k = 0, t = 0, cnt = 5;
	while (cnt-- && is_Ok(x,y)) {
		if (map[x][y] == 0) k++;
		if (map[x][y] == 1)	t++;
		y--;
	}
	if (cnt >= 0)	return {};
	return vector<int>({ k, t });
}

vector<int> Chess::Get_Nums2(int x, int y)
{
	int k = 0, t = 0, cnt = 5;
	while (cnt-- && is_Ok(x, y)) {
		if (map[x][y] == 0) k++;
		if (map[x][y] == 1)	t++;
		x--;
	}
	if (cnt >= 0 )	return {};
	return vector<int>({ k, t });
}

vector<int> Chess::Get_Nums3(int x, int y)
{
	int k = 0, t = 0, cnt = 5;
	while (cnt-- && is_Ok(x, y)) {
		if (map[x][y] == 0) k++;
		if (map[x][y] == 1)	t++;
		x++, y--;
	}
	if (cnt >= 0)	return {};
	return vector<int>({k,t});
}

vector<int> Chess::Get_Nums4(int x, int y) {
	int k = 0, t = 0, cnt = 5;
	while (cnt-- && is_Ok(x, y)) {
		if (map[x][y] == 0) k++;
		if (map[x][y] == 1)	t++;
		x--, y--;
	}
	if (cnt >= 0)	return {};
	return vector<int>({ k,t });
}

int Chess::x_Points(vector<int> nums, int ch)
{
	if (nums.empty())	return 0;
	int g, b;
	if (ch)		 g = nums[0], b = nums[1];/*g是我方，b是电脑，ch = 1则获取电脑得分*/
	else         g = nums[1], b = nums[0];/*g是电脑，b是我方，ch = 0则获取我方得分*/
	if (g == 0 && b == 0)		return 10;
	else if (g == 1 && b == 0)	return 35;
	else if (g == 2 && b == 0)	return 1500;
	else if (g == 3 && b == 0)	return 18000;
	else if (g == 4 && b == 0)	return 1000000;
	else if (g == 0 && b == 1)	return 15;
	else if (g == 0 && b == 2)	return 400;
	else if (g == 0 && b == 3)	return 6000;
	else if (g == 0 && b == 4)	return 150000;
	else if (g != 0 && b != 0)	return 0;
	return 0;
}

double Chess::GetNowPoints(int ch)
{
	double ret = 0;
	for (int i = 0; i < 18; i++) {
		for (int j = 0; j < 18; j++) {
			if (map[i][j] == ch) {
				ret += Get_Points(i, j, ch) / 100.0;
			}
		}
	}
	return ret;
}

bool AiChess::gameOver(Show s, Users& user)
{
	for (int i = 0; i < 18; i++) {
		for (int j = 0; j < 18; j++) {
			if (map[i][j] == 0 && win(i, j)) {
				s.End(-1, user, 0);
				return true;
			}
		}
	}
	return false;
}

bool MyChess::gameOver(Show s, Users& user)
{
	for (int i = 0; i < 18; i++) {
		for (int j = 0; j < 18; j++) {
			if (map[i][j] == 1 && win(i, j)) {
				s.End(1, user, steps);
				return true;
			}
		}
	}
	return false;
}

void MyChess::get_grade(Users user, wchar_t*& wstr)
{
	int w = user.wincnt, f = user.failcnt;
	if (w < 10) {
		if (f < 10)			wstr = const_cast<wchar_t*>(L"默默无闻");
		else if (f < 30)	wstr = const_cast<wchar_t*>(L"逆境之旅");
		else if (f < 60)	wstr = const_cast<wchar_t*>(L"饥寒交迫");
		else if (f < 100)	wstr = const_cast<wchar_t*>(L"露宿街头");
		else				wstr = const_cast<wchar_t*>(L"心如死灰");
	}
	else if (w < 30) {
		if (f < 10)			wstr = const_cast<wchar_t*>(L"小有成色");
		else if (f < 30)	wstr = const_cast<wchar_t*>(L"英勇之士");
		else if (f < 50)	wstr = const_cast<wchar_t*>(L"心怀希望");
		else				wstr = const_cast<wchar_t*>(L"不屈不饶");
	}
	else if (w < 50) {
		if (f < 10)			wstr = const_cast<wchar_t*>(L"黎明之日");
		else if (f < 50)	wstr = const_cast<wchar_t*>(L"年少有为");
		else				wstr = const_cast<wchar_t*>(L"探索者");
	}
	else if (w < 100) {
		if (f < 10)			wstr = const_cast<wchar_t*>(L"闪耀之星");
		else if (f < 50)	wstr = const_cast<wchar_t*>(L"大放光彩");
		else				wstr = const_cast<wchar_t*>(L"才华横溢");
	}
	else if (w < 180) {
		if (f < 10)			wstr = const_cast<wchar_t*>(L"常胜将军");
		else if (f < 50)	wstr = const_cast<wchar_t*>(L"登峰造极");
		else if (f < 200)	wstr = const_cast<wchar_t*>(L"春风得意");
		else				wstr = const_cast<wchar_t*>(L"万众瞩目");
	}
	else
		wstr = const_cast<wchar_t*>(L"群星之巅");
}

void MyChess::get_assuse(wchar_t*& wstr)
{
	double ai = aiPos;
	double my = myPos;
	if (ai > my + 8000) {
		wstr = const_cast<wchar_t*>(L"毫无胜算");
	}
	else if (ai > my + 5000) {
		wstr = const_cast<wchar_t*>(L"危在旦夕");
	}
	else if (ai > my + 2000) {
		wstr = const_cast<wchar_t*>(L"措不及防");
	}
	else if (ai > my + 600) {
		wstr = const_cast<wchar_t*>(L"略显被动");
	}
	else if (ai > my) {
		wstr = const_cast<wchar_t*>(L"均势");
	}
	else if (my > ai + 8000) {
		wstr = const_cast<wchar_t*>(L"锁定胜局");
	}
	else if (my > ai + 5000) {
		wstr = const_cast<wchar_t*>(L"胜券在握");
	}
	else if (my > ai + 2000) {
		wstr = const_cast<wchar_t*>(L"遥遥领先");
	}
	else if (my > ai + 600) {
		wstr = const_cast<wchar_t*>(L"小有优势");
	}
	else
		wstr = const_cast<wchar_t*>(L"均势");
	return;
}

