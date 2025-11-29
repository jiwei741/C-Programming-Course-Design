# AI 五子棋（基于 happysnaker/Gobang 二次开发）

本项目在 [happysnaker/Gobang](https://github.com/happysnaker/Gobang) 的基础上继续演进，使用 C++ + EasyX 图形库实现的 Windows 版五子棋，提供多种 AI 难度、落子记录与简易复盘统计，并附带音效与基础数据持久化。

## 主要特性
- **图形化界面**：基于 EasyX 的 18×18 棋盘绘制、坐标标注与落子高亮。
- **多种 AI 模式**：普通评估、加强（Optimal）、强化阻断（Hell）以及实验性的 Alpha-Beta “直霸”模式。
- **先手决策**：落子前可通过石头剪刀布决定先手。
- **对局评估**：实时显示双方估值分（`myPos`、`aiPos`），结束后展示胜负提示与音效。
- **悔棋与存档**：内置悔棋次数（`restcnt`，默认 10，胜利增加 10 次），胜负与最短胜局步数等数据持久化到 `users.dat`。
- **落子记录与复盘**：对局结束后保存 `record.txt`，可在复盘视图中统计黑白落子数、活三活四与胜负判断。

## 目录说明
- `AI五子棋.sln` / `AI五子棋.vcxproj`：Visual Studio 解决方案与工程文件（v143 工具集，Unicode）。
- `Chess.h/.cpp`：核心棋盘逻辑、评分函数与 AI 决策实现。
- `Show.h/.cpp`：界面绘制、鼠标事件、结束页与复盘统计。
- `Users.h/.cpp`：用户胜负统计、最短胜局步数、悔棋次数与落子记录的读写。
- `Stack.h`：简易单链表栈，用于悔棋记录。
- `setChs.wav`、`win.wav`、`defet.mp3`：落子/胜/负音效资源（运行时需与可执行文件同目录）。
- `users.dat`（运行生成）：保存用户数据；`record.txt`（对局结束生成）：保存当局落子记录。

## 环境依赖
- **操作系统**：Windows。
- **编译器**：Visual Studio 2022 及以上（v143 工具集），x86/x64 均可。
- **图形与音频**：已安装 EasyX（提供 `graphics.h`）；`winmm.lib` 随 Windows SDK 自带。

## 本地构建
1. 安装并配置 EasyX 的包含目录与库目录，确保 `graphics.h` 可被编译器找到。
2. 用 Visual Studio 打开 `AI五子棋.sln`，选择 `Debug/Release` 与 `Win32/x64`。
3. 直接生成并运行，或在“开发者命令提示符”中执行：
   ```powershell
   MSBuild "AI五子棋.sln" /p:Configuration=Release /p:Platform=x64
   ```
4. 运行时保证音频文件与可执行文件在同一目录。

## 运行与玩法
1. 启动程序后，在主菜单选择“开始游戏”（或“退出”）。
2. 选择 AI 模式：
   - `1` 加强模式（默认）：基于模式匹配评分，兼顾自身扩张与防守。
   - `2` 普通模式：简单连珠长度评估，适合新手。
   - `3` 地狱模式：更侧重阻断对方威胁（防守权重更高）。
   - `4` 直霸模式：实验性的 Alpha-Beta 搜索（深度 3，局部 15×15 评分）。
3. 与 AI 进行“石头剪刀布”决定先手；随后进入棋盘界面。
4. 鼠标移动可查看候选坐标提示，左键落子；右侧面板显示当前步数、上一手坐标、估值分与战力称号。
5. 悔棋：点击右侧“悔棋”按钮，消耗 1 次悔棋机会（次数不足或无棋可悔会提示）。
6. 胜负判定后会播放对应音效，更新 `users.dat`，并保存当局 `record.txt`；随后自动打开复盘统计视图。

## 代码要点
- 棋盘数据：`map[18][18]`，`-1` 表示空位，`0` 为 AI，`1` 为玩家。
- 评分体系：
  - `Get_Points`/`x_Points`：模式匹配打分（活四、冲四、活三等）。
  - `EvaluatePoint`：局部模式检测，用于阻断/进攻。
  - `AlphaBetaAI`：小深度 negamax + alpha-beta 剪枝，配合 `EvaluateBoard` 粗略评分。
- 数据持久化：`Users::Read/Write` 读写 `users.dat`，`SaveStepsToFile` 输出 `record.txt`，`ShowRecordViewer` 提供复盘统计。

## 与原项目的差异/扩展
- 增加 EasyX 图形界面、主菜单与多 AI 难度选择。
- 新增石头剪刀布决定先手、实时估值显示、音效反馈。
- 引入悔棋次数管理与用户胜负/最短胜局步数持久化。
- 提供对局记录保存与复盘统计视图。

## 注意事项
- 工程名与部分文件名包含中文，建议使用支持 UTF-8/GBK 路径的环境运行。
- “直霸”模式目前为实验性质，深度有限，主要用于演示搜索思路。
- `users.dat`、`record.txt` 位于可执行文件同目录，可删除后重置统计数据。

## 致谢
感谢 [happysnaker/Gobang](https://github.com/happysnaker/Gobang) 项目提供的启发与基础思路，本项目在其上进行了界面、AI 模式和用户体验的扩展。***
Test SSH signing at 2025年11月30日  0:16:39
