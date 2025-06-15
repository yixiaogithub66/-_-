// 在文件顶部添加
#pragma comment(lib, "winmm.lib")
#include "game.h"
#include <graphics.h>
#include <conio.h>
#include <tchar.h>
#include <cmath>
#include "save_load.h"  // 添加存档头文件
#include "ai_helper.h"  // 添加AI助手头文件
// 自定义RGBA颜色函数
COLORREF RGBA(int r, int g, int b, int a) {
    return RGB(r, g, b) | (a << 24);
}

// 判断鼠标是否在格子内
bool isInCell(int mx, int my, int gx, int gy, int cell) {
    return mx >= gx && mx < gx + cell && my >= gy && my < gy + cell;
}

Game::Game(Board b, const Setting& s)
    : board(b), setting(s), selectedX(-1), selectedY(-1),
    animating(false), animStartTime(0), animDuration(400), lastFrameTime(0),
    showAIHint(s.enableAI), lastAIUpdateTime(0) {
    initialBoard = board.data();

    // 设置AI助手
    if (!s.aiApiKey.empty()) {
        aiHelper.setApiKey(s.aiApiKey);
    }
    else {
        // 使用默认API密钥（实际应用中应从配置文件读取）
        aiHelper.setApiKey("sk-16d270f5ca7546258bf0aeb54534e76f");
    }

    // 初始化AI提示
    aiSuggestion = { -1, -1, -1, -1, "尚未获取AI建议" };
}

void Game::toggleAIHint() {
    showAIHint = !showAIHint;
    if (showAIHint && aiHelper.isReady()) {
        aiSuggestion = aiHelper.getBestMove(board);
        lastAIUpdateTime = GetTickCount();
    }
}

void Game::saveHistory() {
    boardHistory.push_back(board.data());
    selectedXHistory.push_back(selectedX);
    selectedYHistory.push_back(selectedY);
}

void Game::undoMove() {
    if (!boardHistory.empty()) {
        board.data() = boardHistory.back(); boardHistory.pop_back();
        selectedX = selectedXHistory.back(); selectedXHistory.pop_back();
        selectedY = selectedYHistory.back(); selectedYHistory.pop_back();
        animating = false;

        // 重新获取AI建议
        if (showAIHint && aiHelper.isReady()) {
            aiSuggestion = aiHelper.getBestMove(board);
            lastAIUpdateTime = GetTickCount();
        }
    }
}

void Game::restoreInitialBoard() {
    board.data() = initialBoard;
    selectedX = selectedY = -1;
    animating = false;
    boardHistory.clear();
    selectedXHistory.clear();
    selectedYHistory.clear();

    // 重新获取AI建议
    if (showAIHint && aiHelper.isReady()) {
        aiSuggestion = aiHelper.getBestMove(board);
        lastAIUpdateTime = GetTickCount();
    }
}

void Game::run() {
    int baseX = 80, baseY = 80, cell = 62;
    bool running = true;
    bool gameEnded = false;  // 添加标志跟踪游戏是否结束

    // 使用双缓冲绘图，减少闪烁
    BeginBatchDraw();

    // 根据设置调整动画持续时间
    if (setting.quality == "高") {
        // 高画质模式下，动画更精细
        animDuration = 400;
    }
    else {
        // 低画质模式下，动画简化
        animDuration = 300;
    }

    // 根据帧率设置计算每帧延迟时间
    int frameDelay = 1000 / setting.fps;  // 毫秒/帧

    // 保存成功消息显示时间控制
    DWORD saveMessageTime = 0;
    bool showSaveMessage = false;

    // 游戏结束消息绘制控制
    DWORD gameOverTime = 0;

    // 在主循环前，初始化获取AI提示
    if (showAIHint && aiHelper.isReady()) {
        aiSuggestion = aiHelper.getBestMove(board);
        lastAIUpdateTime = GetTickCount();
    }

    // 主循环
    while (running) {
        DWORD frameStartTime = GetTickCount();

        // 如果在动画过程中
        if (animating) {
            // 获取当前时间
            DWORD currentTime = timeGetTime();
            double elapsedTime = currentTime - animStartTime;

            // 绘制动画帧
            drawGameWithAnim();
            FlushBatchDraw();

            // 检查动画是否完成
            if (elapsedTime >= animDuration) {
                // 动画完成，更新棋盘状态
                board.set(animToX, animToY, 1);
                board.set(animMidX, animMidY, 0);
                board.set(animFromX, animFromY, 0);
                animating = false;

                // 检查游戏是否结束
                if (!hasMoves()) {
                    gameEnded = true;
                    gameOverTime = GetTickCount();
                }

                // 动画完成后更新AI提示
                if (showAIHint && aiHelper.isReady() && !gameEnded) {
                    aiSuggestion = aiHelper.getBestMove(board);
                    lastAIUpdateTime = GetTickCount();
                }
            }
            else {
                // 确保高帧率渲染，但最小等待1ms避免过度占用CPU
                Sleep(1);
            }

            // 动画期间不处理输入
            continue;
        }

        // 非动画状态：正常游戏逻辑
        drawGame();

        // 如果需要显示保存成功消息
        if (showSaveMessage) {
            // 显示保存成功消息
            settextcolor(RGB(40, 160, 40));
            settextstyle(22, 0, _T("微软雅黑"));
            outtextxy(615, 420, _T("保存成功！"));

            // 检查是否应该停止显示消息
            if (GetTickCount() - saveMessageTime > 2000) { // 显示2秒
                showSaveMessage = false;
            }
        }

        // 如果游戏结束，显示结束信息
        if (gameEnded) {
            // 创建半透明遮罩
            setfillcolor(RGBA(0, 0, 0, 80));
            fillrectangle(0, 0, 800, 650);

            // 结束消息背景
            setfillcolor(RGB(240, 240, 250));
            fillroundrect(200, 220, 600, 380, 20, 20);

            // 显示结束消息
            bool win = (countPegs() == 1);
            settextstyle(36, 0, _T("微软雅黑"));
            settextcolor(win ? RGB(60, 180, 60) : RGB(180, 40, 40));
            RECT r = { 200, 240, 600, 300 };
            drawtext(win ? _T("恭喜！只剩1枚棋子！") : _T("无可走棋步，游戏结束！"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

            // 提示点击返回
            settextstyle(22, 0, _T("微软雅黑"));
            RECT r2 = { 200, 320, 600, 350 };
            drawtext(_T("点击任意处或按钮返回..."), &r2, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        }

        FlushBatchDraw();

        // 处理用户输入
        ExMessage m;
        while (peekmessage(&m, EX_MOUSE | EX_KEY)) {
            // 根据操作模式处理输入
            if (setting.controlMode == CONTROL_MOUSE) {
                // 鼠标操作模式
                if (m.message == WM_LBUTTONDOWN) {
                    int mx = m.x, my = m.y;
                    bool handled = false;

                    // 检查右侧按钮 - 即使游戏结束也应该可以点击
                    if (mx >= 600 && mx <= 740) {
                        if (my >= 170 && my <= 210) { // 返回菜单
                            running = false;
                            handled = true;
                        }
                        else if (my >= 230 && my <= 270) { // 撤销
                            undoMove();
                            gameEnded = false; // 撤销可能让游戏继续
                            handled = true;
                        }
                        else if (my >= 290 && my <= 330) { // 还原
                            restoreInitialBoard();
                            gameEnded = false; // 还原后游戏可以继续
                            handled = true;
                        }
                        else if (my >= 350 && my <= 390) { // 保存游戏
                            SaveManager saveManager;
                            if (saveManager.saveGame(board, selectedX, selectedY,
                                boardHistory, selectedXHistory, selectedYHistory, "save1.dat")) {
                                saveMessageTime = GetTickCount();
                                showSaveMessage = true;
                            }
                            handled = true;
                        }
                        else if (my >= 430 && my <= 470) { // AI提示按钮
                            toggleAIHint();
                            handled = true;
                        }
                    }
                    if (handled) continue;

                    // 如果游戏结束，任何点击都返回主菜单
                    if (gameEnded) {
                        running = false;
                        continue;
                    }

                    // 检查是否点击了AI建议的移动
                    if (showAIHint && aiHelper.isReady() && aiSuggestion.fromX >= 0 && aiSuggestion.toX >= 0) {
                        int fromX = aiSuggestion.fromX, fromY = aiSuggestion.fromY;
                        int toX = aiSuggestion.toX, toY = aiSuggestion.toY;
                        int fromGx = baseX + fromY * cell, fromGy = baseY + fromX * cell;
                        int toGx = baseX + toY * cell, toGy = baseY + toX * cell;

                        // 检查是否点击了AI建议的起点或终点
                        bool clickedFromPos = isInCell(mx, my, fromGx, fromGy, cell);
                        bool clickedToPos = isInCell(mx, my, toGx, toGy, cell);

                        if (clickedFromPos && selectedX == -1) {
                            // 选中AI建议的起点
                            selectedX = fromX;
                            selectedY = fromY;
                            handled = true;
                        }
                        else if (clickedToPos && selectedX == fromX && selectedY == fromY) {
                            // 执行AI建议的移动
                            saveHistory();
                            playMoveAnimation(selectedX, selectedY, toX, toY);
                            selectedX = selectedY = -1;
                            handled = true;
                        }
                    }
                    if (handled) continue;

                    // 检查棋盘点击 (游戏未结束时处理)
                    int hitRow = -1, hitCol = -1;
                    for (int i = 0; i < board.sizeX(); ++i) {
                        for (int j = 0; j < board.sizeY(); ++j) {
                            if (board.get(i, j) == -1) continue;
                            int gx = baseX + j * cell;
                            int gy = baseY + i * cell;
                            if (isInCell(mx, my, gx, gy, cell)) {
                                hitRow = i; hitCol = j;
                            }
                        }
                    }
                    if (hitRow != -1 && hitCol != -1) {
                        if (selectedX == -1 && board.get(hitRow, hitCol) == 1) {
                            // 第一次点击，选中棋子
                            selectedX = hitRow; selectedY = hitCol;
                        }
                        else if (selectedX != -1 && board.get(hitRow, hitCol) == 0
                            && isValidMove(selectedX, selectedY, hitRow, hitCol)) {
                            // 第二次点击，目标为"空"且为合法走法
                            saveHistory();
                            playMoveAnimation(selectedX, selectedY, hitRow, hitCol);
                            selectedX = selectedY = -1;
                        }
                        else {
                            // 其他情况取消选中
                            selectedX = selectedY = -1;
                        }
                    }
                }
            }
            else {
                // 键盘操作模式
                if (m.message == WM_KEYDOWN) {
                    // 游戏结束状态下，任何按键都返回主菜单
                    if (gameEnded) {
                        running = false;
                        continue;
                    }

                    // 键盘操作逻辑
                    if (m.ctrl && (m.vkcode == 'Z' || m.vkcode == 'z')) {
                        undoMove();
                        gameEnded = false; // 撤销可能让游戏继续
                    }
                    else if (m.ctrl && (m.vkcode == 'R' || m.vkcode == 'r')) {
                        restoreInitialBoard();
                        gameEnded = false; // 还原后游戏可以继续
                    }
                    else if (m.ctrl && (m.vkcode == 'S' || m.vkcode == 's')) {
                        SaveManager saveManager;
                        if (saveManager.saveGame(board, selectedX, selectedY,
                            boardHistory, selectedXHistory, selectedYHistory, "save1.dat")) {
                            saveMessageTime = GetTickCount();
                            showSaveMessage = true;
                        }
                    }
                    else if (m.vkcode == 'H' || m.vkcode == 'h') {
                        // 切换AI提示
                        toggleAIHint();
                    }
                    else if (m.vkcode == 'A' || m.vkcode == 'a') {
                        // 自动执行AI建议的移动
                        if (showAIHint && aiHelper.isReady() &&
                            aiSuggestion.fromX >= 0 && aiSuggestion.toX >= 0) {
                            saveHistory();
                            playMoveAnimation(aiSuggestion.fromX, aiSuggestion.fromY,
                                aiSuggestion.toX, aiSuggestion.toY);
                            selectedX = selectedY = -1;
                        }
                    }
                    else if (m.vkcode == VK_ESCAPE) {
                        running = false;
                    }
                    // 方向键支持
                    else if (selectedX != -1 && selectedY != -1) {
                        int dx = 0, dy = 0;
                        if (m.vkcode == VK_UP) dx = -2;
                        if (m.vkcode == VK_DOWN) dx = 2;
                        if (m.vkcode == VK_LEFT) dy = -2;
                        if (m.vkcode == VK_RIGHT) dy = 2;
                        int nx = selectedX + dx, ny = selectedY + dy;
                        if ((dx != 0 || dy != 0) && board.isValid(nx, ny)) {
                            if (isValidMove(selectedX, selectedY, nx, ny)) {
                                saveHistory();
                                playMoveAnimation(selectedX, selectedY, nx, ny);
                                selectedX = selectedY = -1;
                            }
                        }
                    }
                }
            }
        }

        // 帧率控制
        DWORD frameTime = GetTickCount() - frameStartTime;
        if (frameTime < frameDelay) {
            Sleep(frameDelay - frameTime);
        }
        else {
            Sleep(1); // 最小延迟，确保系统响应
        }
    }
    EndBatchDraw();
}

void Game::playMoveAnimation(int x1, int y1, int x2, int y2) {
    animating = true;
    animFromX = x1; animFromY = y1;
    animToX = x2; animToY = y2;
    animMidX = (x1 + x2) / 2; animMidY = (y1 + y2) / 2;

    // 根据帧率调整动画持续时间，使体验一致
    switch (setting.fps) {
    case 30:  animDuration = 500; break; // 低帧率，延长动画时间
    case 60:  animDuration = 400; break; // 中等帧率
    case 120: animDuration = 300; break; // 高帧率，缩短动画时间
    default:  animDuration = 400;        // 默认值
    }

    animStartTime = timeGetTime();
    lastFrameTime = 0;

    // 先从棋盘上移除起点和中间点的棋子
    board.set(x1, y1, 0);
    board.set(animMidX, animMidY, 0);
}

void Game::drawGameWithAnim() {
    // 背景设置
    setbkcolor(RGB(252, 240, 220));
    cleardevice();

    // 获取当前时间
    DWORD currentTime = timeGetTime();
    double elapsedTime = currentTime - animStartTime;
    double progress = elapsedTime / animDuration;

    // 确保动画在范围内
    if (progress > 1.0) progress = 1.0;

    // 获取鼠标位置
    POINT pt;
    GetCursorPos(&pt);
    ScreenToClient(GetHWnd(), &pt);
    int mx = pt.x, my = pt.y;

    // 绘制棋盘（不带选中/提示）
    // 将画质设置传递给board.draw
    bool isHighQuality = (setting.quality == "高");
    board.draw(80, 80, 62, -1, -1, mx, my, false, isHighQuality);

    // 绘制界面元素
    setfillcolor(RGB(210, 230, 250));
    fillroundrect(600, 170, 740, 210, 12, 12);
    fillroundrect(600, 230, 740, 270, 12, 12);
    fillroundrect(600, 290, 740, 330, 12, 12);
    fillroundrect(600, 350, 740, 390, 12, 12); // 添加保存按钮

    settextstyle(26, 0, _T("微软雅黑"));
    settextcolor(RGB(30, 40, 80));
    outtextxy(615, 182, _T("返回主菜单"));
    outtextxy(635, 242, _T("撤销"));
    outtextxy(635, 302, _T("还原"));
    outtextxy(635, 362, _T("保存")); // 添加保存按钮文字

    TCHAR s[48];
    _stprintf_s(s, _T("剩余棋子: %d"), countPegs());
    settextcolor(RGB(40, 130, 40));
    settextstyle(22, 0, _T("微软雅黑"));
    outtextxy(610, 45, s);

    // 其他动画绘制代码保持不变...

    // 绘制动画中的棋子，根据画质调整参数
    int baseX = 80, baseY = 80, cell = 62;
    int shadowOffset = isHighQuality ? 4 : 2;
    int pieceRadius = isHighQuality ? cell / 2 - 8 : cell / 2 - 10;
    int highlightSize = isHighQuality ? (cell / 2 - 16) / 2 : (cell / 2 - 18) / 3;

    // 计算起点和终点坐标
    float px1 = baseX + animFromY * cell + cell / 2.0f;
    float py1 = baseY + animFromX * cell + cell / 2.0f;
    float px2 = baseX + animToY * cell + cell / 2.0f;
    float py2 = baseY + animToX * cell + cell / 2.0f;

    // 根据画质选择缓动函数
    double t = progress;
    if (isHighQuality) {
        // 高画质：iOS风格的平滑缓动函数
        t = t < 0.2 ? t * 5 * t / 2 :
            t < 0.6 ? 1.5 * t - 0.15 :
            1.0 - pow(1.0 - t, 2) * 2;
    }
    else {
        // 低画质：简单的缓入缓出函数
        t = t * t * (3 - 2 * t);
    }

    // 计算当前位置，包括自然的弧线路径
    float nowx = px1 + (px2 - px1) * t;
    float nowy = py1 + (py2 - py1) * t;

    // 根据画质调整弧度
    if (isHighQuality) {
        // 高画质：精细的抛物线轨迹
        double arcHeight = 20.0; // 弧高
        double arcFactor = sin(t * 3.14159);
        nowy -= arcFactor * arcHeight;
    }
    else {
        // 低画质：简单的上升效果
        double arcHeight = 10.0; // 弧高
        nowy -= t * (1 - t) * 4 * arcHeight;
    }

    // 绘制棋子阴影
    if (isHighQuality) {
        setfillcolor(RGB(185, 90, 70));
        setlinecolor(RGB(185, 90, 70));
        fillcircle(int(nowx + shadowOffset), int(nowy + shadowOffset), pieceRadius);
    }

    // 绘制棋子主体
    setfillcolor(RGB(225, 105, 80));
    setlinecolor(RGB(225, 105, 80));
    fillcircle(int(nowx), int(nowy), pieceRadius);

    // 棋子高光 - 高画质才绘制
    if (isHighQuality) {
        setfillcolor(RGB(255, 220, 200));
        setlinecolor(RGB(255, 220, 200));
        fillcircle(int(nowx - 9), int(nowy - 9), highlightSize);
    }
}

void Game::drawGame() {
    setbkcolor(RGB(252, 240, 220));
    cleardevice();

    // 获取鼠标位置而不消耗消息
    POINT pt;
    GetCursorPos(&pt);
    ScreenToClient(GetHWnd(), &pt);
    int mx = pt.x, my = pt.y;

    // 将画质设置传递给board.draw
    bool isHighQuality = (setting.quality == "高");
    board.draw(80, 80, 62, selectedX, selectedY, mx, my, true, isHighQuality);

    // 增加AI提示按钮
    setfillcolor(RGB(210, 230, 250));
    fillroundrect(600, 170, 740, 210, 12, 12);
    fillroundrect(600, 230, 740, 270, 12, 12);
    fillroundrect(600, 290, 740, 330, 12, 12);
    fillroundrect(600, 350, 740, 390, 12, 12);
    fillroundrect(600, 430, 740, 470, 12, 12); // 新增：AI提示按钮

    settextstyle(26, 0, _T("微软雅黑"));
    settextcolor(RGB(30, 40, 80));
    outtextxy(615, 182, _T("返回主菜单"));
    outtextxy(635, 242, _T("撤销"));
    outtextxy(635, 302, _T("还原"));
    outtextxy(635, 362, _T("保存"));
    outtextxy(635, 442, _T("AI提示")); // 新增：AI提示按钮文字

    TCHAR s[48];
    _stprintf_s(s, _T("剩余棋子: %d"), countPegs());
    settextcolor(RGB(40, 130, 40));
    settextstyle(22, 0, _T("微软雅黑"));
    outtextxy(610, 45, s);
    // 如果启用了AI提示，绘制提示
    if (showAIHint && aiHelper.isReady()) {
        aiHelper.drawHint(aiSuggestion, 80, 80, 62);
    }
}

bool Game::isValidMove(int x1, int y1, int x2, int y2) {
    if (board.get(x1, y1) != 1 || board.get(x2, y2) != 0) return false;

    int dx = x2 - x1, dy = y2 - y1;

    // 水平和垂直走法 - 所有棋盘都适用
    if (std::abs(dx) == 2 && dy == 0 && board.get(x1 + dx / 2, y1) == 1) return true;
    if (std::abs(dy) == 2 && dx == 0 && board.get(x1, y1 + dy / 2) == 1) return true;

    // 对角线走法 - 对特定棋盘类型启用
    BoardType type = board.getType();
    if ((type == TRIANGLE || type == PYRAMID || type == STAR) &&
        std::abs(dx) == 2 && std::abs(dy) == 2 &&
        board.get(x1 + dx / 2, y1 + dy / 2) == 1) return true;

    return false;
}

void Game::move(int x1, int y1, int x2, int y2) {
    board.set(x1, y1, 0);
    board.set(x2, y2, 1);
    board.set(x1 + (x2 - x1) / 2, y1 + (y2 - y1) / 2, 0);
}

bool Game::hasMoves() {
    for (int i = 0; i < board.sizeX(); ++i)
        for (int j = 0; j < board.sizeY(); ++j)
            if (board.get(i, j) == 1) {
                int dx[] = { 2,-2,0,0,2,-2,2,-2 }, dy[] = { 0,0,2,-2,2,-2,-2,2 };
                for (int d = 0; d < (board.getType() == TRIANGLE ? 8 : 4); ++d) {
                    int ni = i + dx[d], nj = j + dy[d];
                    if (board.isValid(ni, nj) && isValidMove(i, j, ni, nj)) return true;
                }
            }
    return false;
}

int Game::countPegs() {
    int cnt = 0;
    for (auto& row : board.data()) for (int v : row) if (v == 1) ++cnt;
    return cnt;
}