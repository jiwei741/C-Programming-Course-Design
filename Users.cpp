#include "Users.h"
#include <fstream>
#include <iostream>
#include <string>

using namespace std;

extern std::vector<std::vector<int>> map;

void Users::Read()
{
	ifstream in("users.dat", ios::binary | ios::in);
	if (!in.is_open())	return;
	in.read((char*)&wincnt, sizeof(int));
	in.read((char*)&failcnt, sizeof(int));
	in.read((char*)&drawcnt, sizeof(int));
	in.read((char*)&minsteps, sizeof(int));
	in.read((char*)&restcnt, sizeof(int));
	in.close();
}

void Users::Write()
{
	ofstream out("users.dat", ios::binary);
	if (!out.is_open()) exit(0);
	out.write((char*)&wincnt, sizeof(int));
	out.write((char*)&failcnt, sizeof(int));
	out.write((char*)&drawcnt, sizeof(int));
	out.write((char*)&minsteps, sizeof(int));
	out.write((char*)&restcnt, sizeof(int));
	out.close();
}

bool Users::SaveStepsToFile(const std::string& filename)
{
	ofstream ofs(filename);
	if (!ofs.is_open()) return false;

	for (int i = 0; i < currentStep; ++i) {
		const StepInfo& step = stepRecord[i];
		ofs << step.x << ' ' << step.y << ' ' << step.color << ' '
			<< step.scoreAI << ' ' << step.scorePlayer << ' ' << step.comment << '\n';
	}
	ofs.close();
	return true;
}

std::vector<std::pair<int, int>> Users::LoadStepsFromFile(const std::string& filename)
{
	ifstream ifs(filename);
	if (!ifs.is_open()) return {};

	stepRecord.clear();
	currentStep = 0;
	int x, y, color, scoreAI, scorePlayer;
	std::string comment;
	std::vector<std::pair<int, int>> steps;

	while (ifs >> x >> y >> color >> scoreAI >> scorePlayer) {
		getline(ifs, comment);  // 读取剩余文字作为 comment
		if (!comment.empty() && comment[0] == ' ') comment.erase(0, 1);
		stepRecord.push_back({ x, y, color, scoreAI, scorePlayer, comment });
		steps.push_back({ x,y });
		map[x][y] = color;
		++currentStep;
	}
	return steps;
}


