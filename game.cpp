// 在文件顶部添加
#pragma comment(lib, "winmm.lib") // 链接 Windows Multimedia API 库 (winmm.dll)。
                                 // 如果使用了 timeGetTime 函数（用于高精度计时），则需要此库。
                                 // GetTickCount 通常是首选，因为它不需要额外链接库，并且在大多数情况下精度足够。
#include "game.h"      // 引用游戏类 (Game) 的头文件，其中包含 Game 类的声明。
#include <graphics.h>  // 引用 EasyX 图形库的头文件，用于所有图形绘制操作。
#include <conio.h>     // 引用控制台输入输出库。如果使用 _kbhit (检查键盘是否有按键) 或 getch (获取按键) 进行键盘输入，则需要此库。
                       // 对于 peekmessage 来处理键盘消息，此库不是必需的。
#include <tchar.h>     // 引用通用字符类型头文件，用于支持 _T() 宏、TCHAR 等，方便代码在 ANSI 和 Unicode 字符集之间移植。
#include <cmath>       // 引用 C++ 数学库，用于数学函数，例如 std::abs (绝对值) 和三角函数 sin (正弦) 等。
#include "save_load.h" // 引用存档/读档功能的头文件 (可能包含 SaveManager 类或相关函数)。
#include "ai_helper.h" // 引用 AI 助手类的头文件，包含 AI 逻辑。

// 自定义 RGBA 颜色函数。
// EasyX 的 RGB 宏只处理红、绿、蓝三个分量。这个函数试图通过位操作将 Alpha (透明度) 分量合并到颜色值中。
// 注意：EasyX 本身对 Alpha 通道的直接支持有限，此处的 Alpha 可能不会按预期在所有绘图函数中生效，
// 或者可能需要特定的绘图模式 (如使用带 Alpha 通道的图片)。
// 对于简单的颜色填充，Alpha 通常被忽略。
// @param r, g, b: 红、绿、蓝颜色分量 (0-255)。
// @param a: Alpha (透明度) 分量 (0-255)。
// @return COLORREF: 合成的颜色值。
COLORREF RGBA(int r, int g, int b, int a) {
    return RGB(r, g, b) | (a << 24); // 将 Alpha 值左移24位并与 RGB 值进行位或操作。
}

// 判断鼠标坐标是否在指定的棋盘格子内。
// @param mx, my: 鼠标的屏幕 x, y 坐标。
// @param gx, gy: 格子左上角的屏幕 x, y 坐标。
// @param cell: 格子的边长（像素）。
// @return bool: 如果鼠标在格子内则返回 true，否则返回 false。
bool isInCell(int mx, int my, int gx, int gy, int cell) {
    return mx >= gx && mx < gx + cell && my >= gy && my < gy + cell;
}

// Game 类的构造函数。
// @param b: 游戏使用的棋盘对象 (Board)。
// @param s: 游戏的设置对象 (Setting)。
Game::Game(Board b, const Setting& s)
    : board(b),                // 初始化棋盘成员变量，使用传入的棋盘对象 b。
    setting(s),              // 初始化设置成员变量，使用传入的设置对象 s。
    selectedX(-1), selectedY(-1), // 初始化当前选中的棋子坐标为 -1 (表示没有选中任何棋子)。
    animating(false),          // 初始化动画状态为 false (没有正在播放的动画)。
    animStartTime(0),          // 初始化动画开始时间为 0。
    animDuration(400),         // 初始化默认动画持续时间为 400 毫秒。
    lastAIUpdateTime(0),       // 初始化上次 AI 更新时间为 0。
    aiHelper(),                // 初始化 AI 助手对象 (调用 AIHelper 的默认构造函数)。
    aiHintCurrentState(AIHintState::IDLE), // 初始化 AI 提示状态为 IDLE (空闲)。
    currentAIHintMove({ -1,-1,-1,-1, "" }) // 初始化当前 AI 提示的移动为一个无效移动。
{
    initialBoard = board.data(); // 保存棋盘的初始状态，用于后续的“还原棋盘”功能。
    // .data() 方法返回棋盘内部数据的引用或副本。
// AI 已被初始化，但在用户点击提示按钮之前不会计算提示。
}

// 切换 AI 提示按钮的动作和状态。
// 当用户点击 AI 提示按钮时调用此函数。
void Game::toggleAIButtonAction() {
    switch (aiHintCurrentState) {
    case AIHintState::IDLE: // 如果 AI 当前是空闲状态
        aiHintCurrentState = AIHintState::WORKING; // 将状态切换到“工作中”
        currentAIHintMove = { -1, -1, -1, -1, "" }; // 立即清除任何旧的视觉提示，防止残留。
        // 实际的 AI 计算将在游戏主循环 (run 方法) 的下一次迭代中，在绘制完 "WORKING" 状态（例如绿色按钮）之后进行。
        // 这样用户可以立即看到按钮状态的变化。
        break;
    case AIHintState::WORKING: // 如果 AI 已经在“工作中”
        // 用户再次点击。可以选择在此实现“取消计算”的逻辑。
        // 目前的实现是：当 AI 正在工作时再次点击按钮，不做任何操作。
        break;
    case AIHintState::SHOWING_HINT: // 如果 AI 提示正在显示中
        // 再次点击按钮意味着用户想要隐藏提示。
        aiHintCurrentState = AIHintState::IDLE; // 将状态切换回“空闲”。
        currentAIHintMove = { -1, -1, -1, -1, "" }; // 清除视觉提示。
        break;
    }
}

// 保存当前棋盘状态到历史记录中，用于实现撤销 (undo) 功能。
void Game::saveHistory() {
    boardHistory.push_back(board.data()); // 将当前棋盘的布局数据存入 boardHistory 向量。
    selectedXHistory.push_back(selectedX); // 保存当前选中的棋子 X 坐标。
    selectedYHistory.push_back(selectedY); // 保存当前选中的棋子 Y 坐标。
}

// 撤销上一步移动。
void Game::undoMove() {
    if (!boardHistory.empty()) { // 确保历史记录不为空
        // 从历史记录中恢复棋盘状态。
        board.data() = boardHistory.back(); // 获取最后保存的棋盘数据并赋值给当前棋盘。
        boardHistory.pop_back();           // 从历史记录中移除已恢复的状态。

        // 恢复选中棋子的状态。
        selectedX = selectedXHistory.back();
        selectedXHistory.pop_back();
        selectedY = selectedYHistory.back();
        selectedYHistory.pop_back();

        animating = false; // 如果正在播放动画，则停止动画。

        // 撤销后，重置 AI 提示状态为空闲，并清除任何显示的提示。
        aiHintCurrentState = AIHintState::IDLE;
        currentAIHintMove = { -1, -1, -1, -1, "" };
    }
}

// 将棋盘恢复到初始状态。
void Game::restoreInitialBoard() {
    board.data() = initialBoard; // 将当前棋盘数据恢复为游戏开始时保存的 initialBoard。
    selectedX = selectedY = -1;   // 清除选中的棋子。
    animating = false;            // 停止任何正在播放的动画。

    // 清空所有历史记录。
    boardHistory.clear();
    selectedXHistory.clear();
    selectedYHistory.clear();

    // 恢复棋盘后，同样重置 AI 提示状态。
    aiHintCurrentState = AIHintState::IDLE;
    currentAIHintMove = { -1, -1, -1, -1, "" };
}

// 游戏的主运行逻辑。包含游戏循环，处理输入、更新状态、绘制界面等。
void Game::run() {
    // 定义棋盘绘制的基准坐标和单元格大小。
    int baseX = 80, baseY = 80, cell = 62;
    bool running = true;   // 控制游戏主循环是否继续运行。
    bool gameEnded = false; // 标记游戏是否已结束 (例如，无路可走或胜利)。

    BeginBatchDraw(); // 开始批量绘图模式。这可以减少闪烁，提高绘制效率。
    // 在此调用之后的所有绘图操作都先在内存缓冲区进行，直到 FlushBatchDraw() 或 EndBatchDraw()。

// 根据设置中的画面质量来设定基础动画时长。
    if (setting.quality == "高") animDuration = 400;      // 高画质，动画慢一些 (400ms)。
    else if (setting.quality == "中") animDuration = 300; // 中画质 (300ms)。
    else animDuration = 200;                             // 低画质，动画快一些 (200ms)。

    int frameDelay = 1000 / setting.fps; // 计算每帧之间的延迟时间 (毫秒)，以达到目标 FPS。
    DWORD saveMessageTime = 0;           // 用于记录“保存成功”消息开始显示的时间。
    bool showSaveMessage = false;        // 控制是否显示“保存成功”消息。

    // 标记本帧迭代是否需要执行 AI 计算。
    // 这个标志确保 AI 计算在当前帧的绘制完成后进行，以避免阻塞UI更新。
    bool performAICalculationThisIteration = false;

    while (running) { // 游戏主循环
        DWORD frameStartTime = GetTickCount(); // 获取当前帧开始的时间戳，用于FPS控制。
        // GetTickCount() 返回系统启动后经过的毫秒数。
        performAICalculationThisIteration = false; // 每帧开始时重置 AI 计算标志。

        // 1. 输入处理阶段 (处理鼠标点击、键盘按键，并可能将 aiHintCurrentState 设置为 WORKING)
        ExMessage m; // EasyX 消息结构体，用于存储鼠标或键盘事件。
        // 使用 peekmessage 检查并获取鼠标 (EX_MOUSE) 或键盘 (EX_KEY) 消息，非阻塞。
        while (peekmessage(&m, EX_MOUSE | EX_KEY)) {
            if (setting.controlMode == CONTROL_MOUSE) { // 如果是鼠标控制模式
                if (m.message == WM_LBUTTONDOWN) { // 如果是鼠标左键按下事件
                    int mx = m.x, my = m.y;     // 获取鼠标点击的屏幕坐标。
                    bool inputHandled = false;   // 标记输入是否已被处理，避免后续逻辑重复处理。

                    // 检查是否点击了右侧的 UI 按钮。
                    if (mx >= 600 && mx <= 740) { // 按钮区域的 X 坐标范围
                        if (my >= 170 && my <= 210) { running = false; inputHandled = true; } // "返回主菜单"按钮
                        else if (my >= 230 && my <= 270) { // "撤销"按钮
                            // 只有在游戏未结束，或者虽然结束但不是因为只剩一个棋子（即输了）的情况下才允许撤销。
                            if (!gameEnded || countPegs() != 1) {
                                undoMove();
                                gameEnded = !hasMoves() && !(countPegs() == 1); // 撤销后重新判断游戏是否结束
                            }
                            inputHandled = true;
                        }
                        else if (my >= 290 && my <= 330) { // "还原"按钮
                            restoreInitialBoard();
                            gameEnded = !hasMoves() && !(countPegs() == 1); // 还原后重新判断游戏是否结束
                            inputHandled = true;
                        }
                        else if (my >= 350 && my <= 390) { // "保存"按钮
                            SaveManager sv; // 创建存档管理器对象。
                            // 调用 saveGame 保存当前游戏状态到 "save1.dat"。
                            if (sv.saveGame(board, selectedX, selectedY, boardHistory, selectedXHistory, selectedYHistory, "save1.dat")) {
                                saveMessageTime = GetTickCount(); // 记录保存成功的时间。
                                showSaveMessage = true;          // 设置显示“保存成功”消息。
                            }
                            inputHandled = true;
                        }
                        else if (my >= 430 && my <= 470) { // "AI 提示"按钮
                            toggleAIButtonAction(); // 调用切换 AI 按钮状态的函数。
                            // 这个函数会将 aiHintCurrentState 设置为 WORKING 或 IDLE。
                            inputHandled = true;
                        }
                    }
                    if (inputHandled) continue; // 如果点击的是UI按钮，则处理完毕，跳过后续棋盘点击逻辑。

                    if (gameEnded) { running = false; continue; } // 如果游戏已结束，任何棋盘点击都视为返回主菜单。

                    // 检查是否点击了 AI 提示的棋子或目标位置。
                    if (aiHintCurrentState == AIHintState::SHOWING_HINT && currentAIHintMove.fromX >= 0) {
                        // 计算 AI 提示的起始棋子和目标位置在屏幕上的格子区域。
                        int fromGx_ai = baseX + currentAIHintMove.fromY * cell, fromGy_ai = baseY + currentAIHintMove.fromX * cell;
                        int toGx_ai = baseX + currentAIHintMove.toY * cell, toGy_ai = baseY + currentAIHintMove.toX * cell;
                        // 判断鼠标是否点在 AI 提示的起始棋子上。
                        bool clickedAIPeg = isInCell(mx, my, fromGx_ai, fromGy_ai, cell);
                        // 判断鼠标是否点在 AI 提示的目标位置上。
                        bool clickedAITarget = isInCell(mx, my, toGx_ai, toGy_ai, cell);

                        if (clickedAIPeg && selectedX == -1) { // 如果点击了AI提示的起始棋子，且当前没有选中棋子
                            selectedX = currentAIHintMove.fromX; // 选中该棋子。
                            selectedY = currentAIHintMove.fromY;
                            inputHandled = true;
                        }
                        else if (selectedX == currentAIHintMove.fromX && selectedY == currentAIHintMove.fromY && clickedAITarget) {
                            // 如果已选中AI提示的起始棋子，并且点击了AI提示的目标位置
                            saveHistory(); // 保存当前状态。
                            playMoveAnimation(selectedX, selectedY, currentAIHintMove.toX, currentAIHintMove.toY); // 执行移动并播放动画。
                            selectedX = selectedY = -1; // 清除选中。
                            inputHandled = true;
                        }
                    }
                    if (inputHandled) continue; // 如果处理了AI提示的点击，则跳过。

                    // 标准的棋盘点击逻辑。
                    int hitRow = -1, hitCol = -1; // 用于存储点击到的棋盘格子的逻辑坐标。
                    // 遍历棋盘，找到鼠标点击的格子。
                    for (int i = 0; i < board.sizeX(); ++i) {
                        for (int j = 0; j < board.sizeY(); ++j) {
                            if (board.get(i, j) == -1) continue; // 跳过无效格子。
                            int gx = baseX + j * cell, gy = baseY + i * cell; // 格子左上角屏幕坐标。
                            if (isInCell(mx, my, gx, gy, cell)) { // 如果鼠标在当前格内
                                hitRow = i; hitCol = j; break; // 记录行列并跳出内层循环。
                            }
                        }
                        if (hitRow != -1) break; // 如果已找到，跳出外层循环。
                    }

                    if (hitRow != -1) { // 如果确实点击了棋盘上的一个有效格子
                        if (selectedX == -1 && board.get(hitRow, hitCol) == 1) { // 如果没有棋子被选中，且点击的是一个棋子
                            selectedX = hitRow; selectedY = hitCol; // 选中该棋子。
                        }
                        else if (selectedX != -1 && board.get(hitRow, hitCol) == 0 && isValidMove(selectedX, selectedY, hitRow, hitCol)) {
                            // 如果已有棋子被选中，点击的是一个空格，并且这是一个有效的移动
                            saveHistory(); // 保存当前状态。
                            playMoveAnimation(selectedX, selectedY, hitRow, hitCol); // 执行移动并播放动画。
                            selectedX = selectedY = -1; // 清除选中。
                        }
                        else { // 其他情况 (例如，点击已选中的棋子，或点击无效目标)
                            selectedX = selectedY = -1; // 取消选中。
                        }
                    }
                }
            } // 结束鼠标控制模式处理
            else { // 键盘控制模式
                if (m.message == WM_KEYDOWN) { // 如果是键盘按下事件
                    if (gameEnded) { running = false; continue; } // 游戏结束时按任意键返回。

                    if (m.ctrl && (m.vkcode == 'Z' || m.vkcode == 'z')) { // Ctrl+Z 撤销
                        if (!gameEnded || countPegs() != 1) { undoMove(); gameEnded = !hasMoves() && !(countPegs() == 1); }
                    }
                    else if (m.ctrl && (m.vkcode == 'R' || m.vkcode == 'r')) { // Ctrl+R 还原
                        restoreInitialBoard(); gameEnded = !hasMoves() && !(countPegs() == 1);
                    }
                    else if (m.ctrl && (m.vkcode == 'S' || m.vkcode == 's')) { /*键盘保存逻辑待实现*/ }
                    else if (m.vkcode == 'H' || m.vkcode == 'h') { // H键 AI提示
                        toggleAIButtonAction();
                    }
                    else if (m.vkcode == 'A' || m.vkcode == 'a') { // A键 执行AI提示的移动
                        if (aiHintCurrentState == AIHintState::SHOWING_HINT && currentAIHintMove.fromX >= 0) {
                            saveHistory();
                            playMoveAnimation(currentAIHintMove.fromX, currentAIHintMove.fromY, currentAIHintMove.toX, currentAIHintMove.toY);
                            selectedX = selectedY = -1;
                        }
                    }
                    else if (m.vkcode == VK_ESCAPE) { // ESC键 退出到主菜单
                        running = false;
                    }
                    // 此处可以添加更多键盘控制逻辑，如使用方向键选择棋子和目标位置。
                }
            }
        } // 结束输入处理循环 (while peekmessage)

        // 2. 逻辑与状态更新阶段 (在本帧绘制之前)
        if (animating) { // 如果正在播放动画
            DWORD currentTime = GetTickCount();
            double elapsedTime = currentTime - animStartTime; // 计算动画已播放的时间。
            if (elapsedTime >= animDuration) { // 如果动画播放时间达到或超过预设的持续时间
                // 动画结束的逻辑处理：
                board.set(animToX, animToY, 1); // 将棋子正式放置在目标位置。
                // (注意：在 playMoveAnimation 中，起始和中间位置已被清空)
                animating = false; // 标记动画已结束。
                if (!hasMoves()) gameEnded = true; // 动画结束后，检查游戏是否结束 (无路可走)。
                aiHintCurrentState = AIHintState::IDLE; // 一次移动完成后，重置 AI 提示状态。
                currentAIHintMove = { -1, -1, -1, -1, "" }; // 清除 AI 提示。
            }
        }
        else { // 如果没有在播放动画
            // 如果 AI 按钮被点击 (状态变为 WORKING)，则设置标志，以便在本帧绘制后进行 AI 计算。
            if (aiHintCurrentState == AIHintState::WORKING) {
                performAICalculationThisIteration = true;
            }
        }

        // 3. 绘制阶段
        // 这个绘制调用会根据当前的 aiHintCurrentState 来渲染按钮。
        // 如果状态是 WORKING (由输入处理设置)，按钮会在这里被画成绿色。
        if (animating) {
            drawGameWithAnim(); // 如果正在动画，调用带动画效果的绘制函数。
        }
        else {
            drawGame(); // 否则，调用常规的游戏绘制函数。
        }

        // 绘制其他 UI 元素，如“保存成功”消息、游戏结束画面 (如果不在动画中)。
        if (showSaveMessage && !animating) { // 显示“保存成功”
            settextcolor(RGB(40, 160, 40)); settextstyle(22, 0, _T("微软雅黑"));
            outtextxy(615, 420, _T("保存成功！"));
            if (GetTickCount() - saveMessageTime > 2000) showSaveMessage = false; // 2秒后自动隐藏。
        }
        if (gameEnded && !animating) { // 显示游戏结束画面
            // 半透明黑色遮罩
            setfillcolor(RGBA(0, 0, 0, 120)); fillrectangle(0, 0, 800, 650);
            // 结束信息框
            setfillcolor(RGB(240, 240, 250)); fillroundrect(200, 220, 600, 380, 20, 20);
            bool win = (countPegs() == 1); // 判断是赢还是输
            settextstyle(36, 0, _T("微软雅黑"));
            settextcolor(win ? RGB(60, 180, 60) : RGB(180, 40, 40)); // 赢则绿色，输则红色
            RECT r_msg = { 200,240,600,300 };
            drawtext(win ? _T("恭喜！只剩1枚棋子！") : _T("无可走棋步，游戏结束！"), &r_msg, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            settextstyle(22, 0, _T("微软雅黑")); settextcolor(RGB(50, 50, 50));
            RECT r2_msg = { 200,320,600,350 };
            drawtext(_T("点击任意处或按钮返回..."), &r2_msg, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        }

        FlushBatchDraw(); // 重要：将当前帧（可能包含绿色的 "WORKING" 按钮）的内容从缓冲区实际绘制到屏幕上。
        // 这样用户能立刻看到状态变化，AI计算则在之后进行。

// 4. AI 计算阶段 (阻塞操作)
// 这个阶段发生在 "WORKING" 状态 (例如绿色按钮) 已经被绘制并刷新到屏幕之后。
// 用户在 AI 进行计算的这段阻塞时间内，看到的是按钮的 "WORKING" 状态。
        if (performAICalculationThisIteration) {
            currentAIHintMove = aiHelper.getBestMove(board); // AI 计算最佳移动 (这可能是耗时操作)。
            aiHintCurrentState = AIHintState::SHOWING_HINT;  // 计算完成后，设置状态为“显示提示”，下一帧按钮会变红。
            lastAIUpdateTime = GetTickCount();              // 记录提示准备好的时间。
        }

        // 5. FPS 控制阶段
        DWORD currentLoopFrameTime = GetTickCount() - frameStartTime; // 计算本帧的实际执行时间。
        if (currentLoopFrameTime < frameDelay) { // 如果执行时间小于目标帧延迟
            Sleep(frameDelay - currentLoopFrameTime); // 则休眠剩余时间，以维持设定的 FPS。
        }
        else {
            Sleep(1); // 如果执行时间已超过，至少休眠1毫秒，让出CPU给其他进程。
        }
    } // 结束主游戏循环 (while running)
    EndBatchDraw(); // 结束批量绘图模式，释放相关资源。
}

// 播放棋子移动的动画。
// @param x1, y1: 棋子移动的起始逻辑坐标。
// @param x2, y2: 棋子移动的目标逻辑坐标。
void Game::playMoveAnimation(int x1, int y1, int x2, int y2) {
    animating = true;    // 设置动画状态为 true。
    animFromX = x1; animFromY = y1; // 记录动画起始点。
    animToX = x2; animToY = y2;   // 记录动画目标点。
    animMidX = (x1 + x2) / 2;     // 计算被跳过的中间棋子的逻辑坐标。
    animMidY = (y1 + y2) / 2;

    // animDuration (动画总时长) 已在 run() 方法开始时根据画质设置。
    animStartTime = GetTickCount(); // 记录动画开始的时间戳。

    // 在动画开始前就更新棋盘的逻辑状态，以保证绘制一致性：
    // 棋子移动的逻辑是：起始位置变空，中间被跳过的棋子被移除（变空），目标位置在动画结束后才变为有棋子。
    // 这里提前将起始和中间位置设为0，动画过程中只绘制移动的那个棋子。
    board.set(x1, y1, 0);             // 起始位置 (x1,y1) 的棋子被“拿起”，变为空格 (0)。
    board.set(animMidX, animMidY, 0); // 中间位置 (animMidX,animMidY) 的棋子被“移除”，变为空格 (0)。
    // 目标位置 (animToX, animToY) 的棋子将在动画结束时 (run循环的逻辑更新部分) 才被设置为1。
}

// 绘制带有动画效果的游戏界面。
// 在棋子移动动画过程中被调用。
void Game::drawGameWithAnim() {
    setbkcolor(RGB(252, 240, 220)); // 设置背景色
    cleardevice();                   // 清屏

    DWORD currentTime = GetTickCount();
    double elapsedTime = currentTime - animStartTime; // 计算动画已播放时间
    double progress = elapsedTime / animDuration;    // 计算动画进度 (0.0 到 1.0)
    // animDuration 是棋子移动的总时长
    if (progress > 1.0) progress = 1.0; //确保进度不超过1

    // 获取鼠标当前位置，用于可能的悬停效果 (虽然在动画中棋盘部分不响应悬停)
    POINT pt_mouse; GetCursorPos(&pt_mouse); ScreenToClient(GetHWnd(), &pt_mouse);
    int mx = pt_mouse.x, my = pt_mouse.y;
    bool isHighQuality = (setting.quality == "高"); // 判断是否为高质量模式

    // 绘制棋盘背景和所有非动画棋子。
    // 在动画期间，不显示选中棋子的特殊高亮 (selX, selY 传 -1)，也不显示常规的可走步提示 (showHint 传 false)。
    board.draw(80, 80, 62, -1, -1, mx, my, false, isHighQuality);

    // --- 绘制 UI 按钮 (包括 AI 按钮，其颜色/文本根据当前 aiHintCurrentState 动态变化) ---
    COLORREF aiButtonColorFill; // AI 按钮的填充颜色
    LPCTSTR aiButtonText;       // AI 按钮上显示的文本 (LPCTSTR 用于 _T() 宏的字符串字面量)
    switch (aiHintCurrentState) {
    case AIHintState::IDLE:
        aiButtonColorFill = RGB(210, 230, 250); // 默认按钮颜色 (淡蓝)
        aiButtonText = _T("AI 提示");
        break;
    case AIHintState::WORKING:
        aiButtonColorFill = RGB(100, 220, 100); // AI 工作时为绿色
        aiButtonText = _T("AI 计算中...");
        break;
    case AIHintState::SHOWING_HINT:
        aiButtonColorFill = RGB(220, 100, 100); // AI 提示准备好时为红色
        aiButtonText = _T("隐藏提示");
        break;
    default: // 理论上不应发生
        aiButtonColorFill = RGB(150, 150, 150); // 错误状态为灰色
        aiButtonText = _T("AI 状态错误");
        break;
    }
    // 绘制其他固定颜色的按钮
    setfillcolor(RGB(210, 230, 250)); // 默认按钮填充色
    fillroundrect(600, 170, 740, 210, 12, 12); // 返回按钮
    fillroundrect(600, 230, 740, 270, 12, 12); // 撤销按钮
    fillroundrect(600, 290, 740, 330, 12, 12); // 还原按钮
    fillroundrect(600, 350, 740, 390, 12, 12); // 保存按钮
    // 使用动态颜色绘制 AI 提示按钮
    setfillcolor(aiButtonColorFill);
    fillroundrect(600, 430, 740, 470, 12, 12); // AI 提示按钮

    // 绘制按钮上的文字
    settextstyle(26, 0, _T("微软雅黑"));      // 设置按钮文字字体和大小
    settextcolor(RGB(30, 40, 80));          // 设置按钮文字颜色 (深色)
    outtextxy(615, 182, _T("返回主菜单"));
    outtextxy(635, 242, _T("撤销"));
    outtextxy(635, 302, _T("还原"));
    outtextxy(635, 362, _T("保存"));
    outtextxy(635, 442, aiButtonText);        // 绘制 AI 按钮上的动态文本

    // 绘制剩余棋子数量显示
    TCHAR s_pegs_anim[48]; // 存储棋子数量的字符串缓冲区
    // 注意：此时 countPegs() 反映的是棋子刚被拿起、中间棋子刚被移除的状态。
    _stprintf_s(s_pegs_anim, _T("剩余棋子: %d"), countPegs());
    settextcolor(RGB(40, 130, 40));        // 设置棋子数量文本颜色 (绿色)
    settextstyle(22, 0, _T("微软雅黑"));   // 设置字体
    outtextxy(610, 45, s_pegs_anim);        // 显示文本

    // 如果 AI 提示正在显示，则绘制提示线 (即使在其他棋子移动动画期间也显示)
    if (aiHintCurrentState == AIHintState::SHOWING_HINT && currentAIHintMove.fromX >= 0) {
        aiHelper.drawHint(currentAIHintMove, 80, 80, 62); // 调用 AIHelper 的绘制提示函数
    }

    // --- 绘制正在移动的棋子 ---
    int baseX_anim = 80, baseY_anim = 80, cell_anim = 62; // 动画相关的棋盘参数 (避免与外部变量名冲突)
    int shadowOffset_anim = isHighQuality ? 4 : 2;         // 动画棋子的阴影偏移
    int pieceRadius_anim = isHighQuality ? cell_anim / 2 - 8 : cell_anim / 2 - 10; // 动画棋子的半径
    // 动画棋子高光内部圆的大小
    int highlightSizeInner_anim = isHighQuality ? (cell_anim / 2 - 16) / 2 : (cell_anim / 2 - 18) / 3;

    // 计算动画起始点和结束点的屏幕中心坐标 (浮点数以实现平滑插值)
    float px1_center = baseX_anim + animFromY * cell_anim + cell_anim / 2.0f;
    float py1_center = baseY_anim + animFromX * cell_anim + cell_anim / 2.0f;
    float px2_center = baseX_anim + animToY * cell_anim + cell_anim / 2.0f;
    float py2_center = baseY_anim + animToX * cell_anim + cell_anim / 2.0f;

    double t_anim_progress = progress; // 当前动画的原始进度 (0.0 到 1.0)

    // 应用缓动函数 (Easing function) 使动画更平滑自然
    if (isHighQuality) { // 高质量模式下使用二次缓入缓出
        t_anim_progress = t_anim_progress < 0.5 ? 2 * t_anim_progress * t_anim_progress : -1 + (4 - 2 * t_anim_progress) * t_anim_progress;
    }
    else { // 其他模式下使用三次缓入缓出 (更简单)
        t_anim_progress = t_anim_progress * t_anim_progress * (3 - 2 * t_anim_progress);
    }

    // 根据缓动后的进度计算棋子当前的屏幕坐标 (线性插值)
    float currentX = px1_center + (px2_center - px1_center) * t_anim_progress;
    float currentY = py1_center + (py2_center - py1_center) * t_anim_progress;

    // 为跳跃的棋子添加弧线轨迹视觉效果
    double arcHeight = isHighQuality ? 20.0 : 10.0; // 弧线的高度
    if (t_anim_progress > 0.0 && t_anim_progress < 1.0) { //只在跳跃过程中应用弧线
        // 使用原始的线性进度 `progress` (0到1) 来计算完整的 sin 弧线 (0到PI)
        // 这样可以确保棋子在起点和终点时 Y 坐标不受弧线影响。
        currentY -= sin(progress * 3.1415926535) * arcHeight;
    }

    // 绘制移动棋子的阴影 (如果高质量模式开启)
    if (isHighQuality) {
        setfillcolor(RGB(185, 90, 70)); // 阴影颜色
        setlinecolor(RGB(185, 90, 70));
        fillcircle(int(currentX + shadowOffset_anim), int(currentY + shadowOffset_anim), pieceRadius_anim);
    }
    // 绘制移动棋子的主体
    setfillcolor(RGB(225, 105, 80)); // 棋子颜色
    setlinecolor(RGB(225, 105, 80));
    fillcircle(int(currentX), int(currentY), pieceRadius_anim);
    // 绘制移动棋子的高光 (如果高质量模式开启)
    if (isHighQuality) {
        setfillcolor(RGB(255, 220, 200)); // 高光颜色
        setlinecolor(RGB(255, 220, 200));
        // 高光位置通常在棋子左上角偏移一点
        fillcircle(int(currentX - pieceRadius_anim * 0.3f), int(currentY - pieceRadius_anim * 0.3f), highlightSizeInner_anim);
    }
}

// 绘制静态的游戏界面 (当没有动画播放时调用)。
void Game::drawGame() {
    setbkcolor(RGB(252, 240, 220)); // 设置背景色
    cleardevice();                   // 清屏

    POINT pt_mouse; GetCursorPos(&pt_mouse); ScreenToClient(GetHWnd(), &pt_mouse); // 获取鼠标在窗口内的坐标
    int mx = pt_mouse.x, my = pt_mouse.y;
    bool isHighQuality = (setting.quality == "高"); // 判断是否高质量模式

    // 调用 Board 对象的 draw 方法绘制棋盘。
    // 传入当前选中的棋子 (selectedX, selectedY)，鼠标位置 (mx, my) 以显示悬停效果，
    // showHint 为 true 以显示可走步提示。
    board.draw(80, 80, 62, selectedX, selectedY, mx, my, true, isHighQuality);

    // --- 绘制 UI 按钮 (与 drawGameWithAnim 中的逻辑相同) ---
    COLORREF aiButtonColorFill;
    LPCTSTR aiButtonText;
    switch (aiHintCurrentState) {
    case AIHintState::IDLE:
        aiButtonColorFill = RGB(210, 230, 250); aiButtonText = _T("AI 提示"); break;
    case AIHintState::WORKING:
        aiButtonColorFill = RGB(100, 220, 100); aiButtonText = _T("AI 计算中..."); break;
    case AIHintState::SHOWING_HINT:
        aiButtonColorFill = RGB(220, 100, 100); aiButtonText = _T("隐藏提示"); break;
    default:
        aiButtonColorFill = RGB(150, 150, 150); aiButtonText = _T("AI 状态错误"); break;
    }
    // 绘制其他按钮
    setfillcolor(RGB(210, 230, 250));
    fillroundrect(600, 170, 740, 210, 12, 12); // 返回
    fillroundrect(600, 230, 740, 270, 12, 12); // 撤销
    fillroundrect(600, 290, 740, 330, 12, 12); // 还原
    fillroundrect(600, 350, 740, 390, 12, 12); // 保存
    // 绘制AI按钮
    setfillcolor(aiButtonColorFill);
    fillroundrect(600, 430, 740, 470, 12, 12); // AI 提示

    // 绘制按钮文字
    settextstyle(26, 0, _T("微软雅黑")); settextcolor(RGB(30, 40, 80));
    outtextxy(615, 182, _T("返回主菜单")); outtextxy(635, 242, _T("撤销"));
    outtextxy(635, 302, _T("还原"));     outtextxy(635, 362, _T("保存"));
    outtextxy(635, 442, aiButtonText); // AI 按钮文字

    // 绘制剩余棋子数量
    TCHAR s_pegs_static[48];
    _stprintf_s(s_pegs_static, _T("剩余棋子: %d"), countPegs());
    settextcolor(RGB(40, 130, 40)); settextstyle(22, 0, _T("微软雅黑"));
    outtextxy(610, 45, s_pegs_static);

    // 如果 AI 提示正在显示，则绘制提示。
    if (aiHintCurrentState == AIHintState::SHOWING_HINT && currentAIHintMove.fromX >= 0) {
        aiHelper.drawHint(currentAIHintMove, 80, 80, 62); // aiHelper.drawHint 负责绘制提示线和可能的文本。
    }
}

// 检查从 (x1, y1) 到 (x2, y2) 是否为一次有效的孔明棋移动。
// @param x1, y1: 起始棋子的逻辑坐标。
// @param x2, y2: 目标空位的逻辑坐标。
// @return bool: 如果移动有效则返回 true，否则返回 false。
bool Game::isValidMove(int x1, int y1, int x2, int y2) {
    // 1. 检查起始点和目标点是否都在棋盘的有效区域内。
    if (!board.isValid(x1, y1) || !board.isValid(x2, y2)) return false;

    // 2. 计算中间点的逻辑坐标。
    int mX = x1 + (x2 - x1) / 2; // 中间点行坐标
    int mY = y1 + (y2 - y1) / 2; // 中间点列坐标
    // 检查中间点是否也在棋盘的有效区域内。
    if (!board.isValid(mX, mY)) return false;

    // 3. 检查棋子和空位条件：起始点必须有棋子 (1)，目标点必须是空格 (0)。
    if (board.get(x1, y1) != 1 || board.get(x2, y2) != 0) return false;

    // 4. 检查跳跃距离和方向。
    int dx_abs = std::abs(x2 - x1); // 行方向的距离绝对值
    int dy_abs = std::abs(y2 - y1); // 列方向的距离绝对值

    // 标准跳跃：水平或垂直跳两格。
    if (((dx_abs == 2 && dy_abs == 0) || (dx_abs == 0 && dy_abs == 2)) && board.get(mX, mY) == 1) {
        // 同时要求中间点必须有棋子 (1)。
        return true;
    }

    // 特定棋盘类型的对角线跳跃规则。
    BoardType type = board.getType();
    if ((type == TRIANGLE || type == PYRAMID || type == STAR) && // 如果是三角、金字塔或星形棋盘
        dx_abs == 2 && dy_abs == 2 && board.get(mX, mY) == 1) { // 并且是严格的对角线跳两格 (行、列都差2)
        // 且中间点有棋子。
        return true;
    }
    return false; // 其他情况均为无效移动。
}

// 概念上的移动函数 - 更新棋盘状态。
// 注意：在当前带有动画的实现中，此函数可能不被直接用于玩家的移动，
// 因为 playMoveAnimation 会处理棋盘状态的更新。
// 此函数可能用于 AI 内部模拟移动或无动画的游戏版本。
void Game::move(int x1, int y1, int x2, int y2) {
    /*
    // (如果直接使用此函数进行移动，逻辑如下)
    // int mX = x1 + (x2 - x1) / 2;
    // int mY = y1 + (y2 - y1) / 2;
    // board.set(x1, y1, 0);      // 拿起起始棋子
    // board.set(mX, mY, 0);      // 移除中间棋子
    // board.set(x2, y2, 1);      // 落子到目标位置
    */
}

// 检查当前棋盘状态下是否还存在任何有效的移动。
// @return bool: 如果至少存在一步有效移动则返回 true，否则返回 false (游戏可能结束)。
bool Game::hasMoves() {
    for (int i = 0; i < board.sizeX(); ++i) { // 遍历棋盘的每一行
        for (int j = 0; j < board.sizeY(); ++j) { // 遍历棋盘的每一列
            if (board.get(i, j) == 1) { // 如果当前位置有棋子
                // 检查标准跳跃 (上下左右)
                int std_dx[] = { 0,0,2,-2 }, std_dy[] = { 2,-2,0,0 };
                for (int d = 0; d < 4; ++d) {
                    if (isValidMove(i, j, i + std_dx[d], j + std_dy[d])) return true; // 如果找到一个有效移动，立即返回 true
                }
                // 检查特定棋盘类型的对角线跳跃
                BoardType type = board.getType();
                if (type == TRIANGLE || type == PYRAMID || type == STAR) {
                    int diag_dx[] = { 2,2,-2,-2 }, diag_dy[] = { 2,-2,2,-2 };
                    for (int d = 0; d < 4; ++d) {
                        if (isValidMove(i, j, i + diag_dx[d], j + diag_dy[d])) return true; // 如果找到一个有效移动，立即返回 true
                    }
                }
            }
        }
    }
    return false; // 遍历完所有棋子都找不到有效移动，则返回 false
}

// 计算当前棋盘上剩余棋子的数量。
// @return int: 棋盘上值为 1 (代表棋子) 的单元格数量。
int Game::countPegs() {
    int cnt = 0;
    const auto& data = board.data(); // 使用常量引用以提高效率，避免复制棋盘数据。
    for (const auto& row : data) {   // 遍历每一行
        for (int val : row) {        // 遍历行中的每一个单元格值
            if (val == 1) ++cnt;     // 如果值为1，则棋子计数器加一。
        }
    }
    return cnt; // 返回总棋子数。
}