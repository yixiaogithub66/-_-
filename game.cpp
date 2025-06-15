// 在文件顶部添加
#pragma comment(lib, "winmm.lib") // For timeGetTime, if used. GetTickCount is generally preferred.
#include "game.h"
#include <graphics.h>
#include <conio.h> // For _kbhit, getch if used for keyboard input, not essential for peekmessage
#include <tchar.h> // For _T, TCHAR, etc.
#include <cmath>   // For std::abs, sin
#include "save_load.h"
#include "ai_helper.h"

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
    animating(false), animStartTime(0), animDuration(400),
    lastAIUpdateTime(0),
    aiHelper(),
    aiHintCurrentState(AIHintState::IDLE),
    currentAIHintMove({ -1,-1,-1,-1, "" }) {
    initialBoard = board.data();
    // AI is initialized but does not calculate a hint until button is pressed.
}

void Game::toggleAIButtonAction() {
    switch (aiHintCurrentState) {
    case AIHintState::IDLE:
        aiHintCurrentState = AIHintState::WORKING;
        currentAIHintMove = { -1, -1, -1, -1, "" }; // Clear any old visual hint immediately
        // The actual AI calculation will happen in the run loop's next iteration, after drawing the "WORKING" state.
        break;
    case AIHintState::WORKING:
        // AI is already working. Optionally, could implement a 'cancel' here.
        // For now, clicking again while working does nothing.
        break;
    case AIHintState::SHOWING_HINT:
        // If hint is showing, button press means hide it and go to IDLE.
        aiHintCurrentState = AIHintState::IDLE;
        currentAIHintMove = { -1, -1, -1, -1, "" }; // Clear visual hint
        break;
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
        animating = false; // Stop any ongoing animation

        // After undo, reset AI hint state to IDLE and clear any displayed hint
        aiHintCurrentState = AIHintState::IDLE;
        currentAIHintMove = { -1, -1, -1, -1, "" };
    }
}

void Game::restoreInitialBoard() {
    board.data() = initialBoard;
    selectedX = selectedY = -1;
    animating = false; // Stop any ongoing animation
    boardHistory.clear();
    selectedXHistory.clear();
    selectedYHistory.clear();

    // After restore, reset AI hint state
    aiHintCurrentState = AIHintState::IDLE;
    currentAIHintMove = { -1, -1, -1, -1, "" };
}

void Game::run() {
    int baseX = 80, baseY = 80, cell = 62;
    bool running = true;
    bool gameEnded = false;

    BeginBatchDraw();

    // Set base animation duration based on settings
    if (setting.quality == "高") animDuration = 400;
    else if (setting.quality == "中") animDuration = 300;
    else animDuration = 200; // Low

    int frameDelay = 1000 / setting.fps;
    DWORD saveMessageTime = 0;
    bool showSaveMessage = false;

    bool performAICalculationThisIteration = false; // Flag to trigger AI calculation after drawing

    while (running) {
        DWORD frameStartTime = GetTickCount();
        performAICalculationThisIteration = false; // Reset flag each iteration

        // 1. INPUT PROCESSING (Handles clicks, key presses, and sets aiHintCurrentState to WORKING if button clicked)
        ExMessage m;
        while (peekmessage(&m, EX_MOUSE | EX_KEY)) {
            if (setting.controlMode == CONTROL_MOUSE) {
                if (m.message == WM_LBUTTONDOWN) {
                    int mx = m.x, my = m.y;
                    bool inputHandled = false;
                    if (mx >= 600 && mx <= 740) { // Button column
                        if (my >= 170 && my <= 210) { running = false; inputHandled = true; } // Return to Menu
                        else if (my >= 230 && my <= 270) {
                            if (!gameEnded || countPegs() != 1) { undoMove(); gameEnded = !hasMoves() && !(countPegs() == 1); }
                            inputHandled = true;
                        }
                        else if (my >= 290 && my <= 330) {
                            restoreInitialBoard(); gameEnded = !hasMoves() && !(countPegs() == 1);
                            inputHandled = true;
                        }
                        else if (my >= 350 && my <= 390) {
                            SaveManager sv; if (sv.saveGame(board, selectedX, selectedY, boardHistory, selectedXHistory, selectedYHistory, "save1.dat")) { saveMessageTime = GetTickCount(); showSaveMessage = true; }
                            inputHandled = true;
                        }
                        else if (my >= 430 && my <= 470) { // AI Hint Button
                            toggleAIButtonAction(); // This sets aiHintCurrentState to WORKING or IDLE
                            inputHandled = true;
                        }
                    }
                    if (inputHandled) continue;
                    if (gameEnded) { running = false; continue; }
                    // Clicking on AI suggested move
                    if (aiHintCurrentState == AIHintState::SHOWING_HINT && currentAIHintMove.fromX >= 0) {
                        int fromGx_ai = baseX + currentAIHintMove.fromY * cell, fromGy_ai = baseY + currentAIHintMove.fromX * cell;
                        int toGx_ai = baseX + currentAIHintMove.toY * cell, toGy_ai = baseY + currentAIHintMove.toX * cell;
                        bool clickedAIPeg = isInCell(mx, my, fromGx_ai, fromGy_ai, cell);
                        bool clickedAITarget = isInCell(mx, my, toGx_ai, toGy_ai, cell);
                        if (clickedAIPeg && selectedX == -1) { selectedX = currentAIHintMove.fromX; selectedY = currentAIHintMove.fromY; inputHandled = true; }
                        else if (selectedX == currentAIHintMove.fromX && selectedY == currentAIHintMove.fromY && clickedAITarget) {
                            saveHistory(); playMoveAnimation(selectedX, selectedY, currentAIHintMove.toX, currentAIHintMove.toY); selectedX = selectedY = -1; inputHandled = true;
                        }
                    }
                    if (inputHandled) continue;
                    // Standard board click
                    int hitRow = -1, hitCol = -1;
                    for (int i = 0; i < board.sizeX(); ++i) { for (int j = 0; j < board.sizeY(); ++j) { if (board.get(i, j) == -1) continue; int gx = baseX + j * cell, gy = baseY + i * cell; if (isInCell(mx, my, gx, gy, cell)) { hitRow = i; hitCol = j; break; } } if (hitRow != -1)break; }
                    if (hitRow != -1) { if (selectedX == -1 && board.get(hitRow, hitCol) == 1) { selectedX = hitRow; selectedY = hitCol; } else if (selectedX != -1 && board.get(hitRow, hitCol) == 0 && isValidMove(selectedX, selectedY, hitRow, hitCol)) { saveHistory(); playMoveAnimation(selectedX, selectedY, hitRow, hitCol); selectedX = selectedY = -1; } else { selectedX = selectedY = -1; } }
                }
            }
            else { // Keyboard Control Mode
                if (m.message == WM_KEYDOWN) {
                    if (gameEnded) { running = false; continue; }
                    if (m.ctrl && (m.vkcode == 'Z' || m.vkcode == 'z')) { if (!gameEnded || countPegs() != 1) { undoMove(); gameEnded = !hasMoves() && !(countPegs() == 1); } }
                    else if (m.ctrl && (m.vkcode == 'R' || m.vkcode == 'r')) { restoreInitialBoard(); gameEnded = !hasMoves() && !(countPegs() == 1); }
                    else if (m.ctrl && (m.vkcode == 'S' || m.vkcode == 's')) { /*Save logic for keyboard*/ }
                    else if (m.vkcode == 'H' || m.vkcode == 'h') { toggleAIButtonAction(); }
                    else if (m.vkcode == 'A' || m.vkcode == 'a') { if (aiHintCurrentState == AIHintState::SHOWING_HINT && currentAIHintMove.fromX >= 0) { saveHistory(); playMoveAnimation(currentAIHintMove.fromX, currentAIHintMove.fromY, currentAIHintMove.toX, currentAIHintMove.toY); selectedX = selectedY = -1; } }
                    else if (m.vkcode == VK_ESCAPE) { running = false; }
                }
            }
        } // End input processing loop

        // 2. LOGIC & STATE UPDATES (before drawing this frame)
        if (animating) {
            DWORD currentTime = GetTickCount();
            double elapsedTime = currentTime - animStartTime;
            if (elapsedTime >= animDuration) { // animDuration is for piece movement
                board.set(animToX, animToY, 1); // Peg lands at destination
                animating = false; // Animation finished
                if (!hasMoves()) gameEnded = true; // Check for game end condition
                aiHintCurrentState = AIHintState::IDLE; // After a move, reset AI hint
                currentAIHintMove = { -1, -1, -1, -1, "" }; // Clear the hint
            }
        }
        else { // Not animating
            // If AI button was clicked (state is WORKING), set flag to calculate after drawing this frame
            if (aiHintCurrentState == AIHintState::WORKING) {
                performAICalculationThisIteration = true;
            }
        }

        // 3. DRAWING PHASE
        // This draw call will render the button based on the current aiHintCurrentState.
        // If state is WORKING (set by input processing), the button will be drawn green here.
        if (animating) {
            drawGameWithAnim();
        }
        else {
            drawGame();
        }
        // Draw other UI elements like save message, game over screen (if not animating)
        if (showSaveMessage && !animating) {
            settextcolor(RGB(40, 160, 40)); settextstyle(22, 0, _T("微软雅黑"));
            outtextxy(615, 420, _T("保存成功！"));
            if (GetTickCount() - saveMessageTime > 2000) showSaveMessage = false;
        }
        if (gameEnded && !animating) {
            setfillcolor(RGBA(0, 0, 0, 120)); fillrectangle(0, 0, 800, 650);
            setfillcolor(RGB(240, 240, 250)); fillroundrect(200, 220, 600, 380, 20, 20);
            bool win = (countPegs() == 1); settextstyle(36, 0, _T("微软雅黑"));
            settextcolor(win ? RGB(60, 180, 60) : RGB(180, 40, 40)); RECT r_msg = { 200,240,600,300 };
            drawtext(win ? _T("恭喜！只剩1枚棋子！") : _T("无可走棋步，游戏结束！"), &r_msg, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            settextstyle(22, 0, _T("微软雅黑")); settextcolor(RGB(50, 50, 50)); RECT r2_msg = { 200,320,600,350 };
            drawtext(_T("点击任意处或按钮返回..."), &r2_msg, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        }
        FlushBatchDraw(); // IMPORTANT: Make the current frame (potentially with green "WORKING" button) visible NOW

        // 4. AI CALCULATION (BLOCKING)
        // This happens *after* the "WORKING" state (green button) has been drawn and flushed.
        // The user sees the green button during this blocking call.
        if (performAICalculationThisIteration) {
            currentAIHintMove = aiHelper.getBestMove(board); // AI calculates the move
            aiHintCurrentState = AIHintState::SHOWING_HINT; // Set for next frame's draw (red button)
            lastAIUpdateTime = GetTickCount(); // Record when the hint was ready
        }

        // 5. FPS CONTROL
        DWORD currentLoopFrameTime = GetTickCount() - frameStartTime;
        if (currentLoopFrameTime < frameDelay) {
            Sleep(frameDelay - currentLoopFrameTime);
        }
        else {
            Sleep(1); // Minimum sleep to yield CPU
        }
    } // End main game loop (while running)
    EndBatchDraw();
}

void Game::playMoveAnimation(int x1, int y1, int x2, int y2) {
    animating = true;
    animFromX = x1; animFromY = y1;
    animToX = x2; animToY = y2;
    animMidX = (x1 + x2) / 2; animMidY = (y1 + y2) / 2;

    // animDuration is already set in run() based on setting.quality
    animStartTime = GetTickCount(); // Use GetTickCount consistently

    // Update board state *before* animation for drawing consistency:
    board.set(x1, y1, 0); // Peg at (x1,y1) is "lifted" (becomes 0).
    board.set(animMidX, animMidY, 0); // Peg at (midX,midY) is "removed" (becomes 0).
}

void Game::drawGameWithAnim() {
    setbkcolor(RGB(252, 240, 220));
    cleardevice();
    DWORD currentTime = GetTickCount();
    double elapsedTime = currentTime - animStartTime;
    double progress = elapsedTime / animDuration; // animDuration for piece movement
    if (progress > 1.0) progress = 1.0;

    POINT pt_mouse; GetCursorPos(&pt_mouse); ScreenToClient(GetHWnd(), &pt_mouse);
    int mx = pt_mouse.x, my = pt_mouse.y;
    bool isHighQuality = (setting.quality == "高");

    board.draw(80, 80, 62, -1, -1, mx, my, false, isHighQuality); // No selection highlights during move animation

    // --- Draw UI Buttons (including AI button with its current state color/text) ---
    COLORREF aiButtonColorFill;
    LPCTSTR aiButtonText; // Use LPCTSTR for string literals with _T()
    switch (aiHintCurrentState) {
    case AIHintState::IDLE:
        aiButtonColorFill = RGB(210, 230, 250); // Default button color
        aiButtonText = _T("AI 提示");
        break;
    case AIHintState::WORKING:
        aiButtonColorFill = RGB(100, 220, 100); // Green for working
        aiButtonText = _T("AI 计算中...");
        break;
    case AIHintState::SHOWING_HINT:
        aiButtonColorFill = RGB(220, 100, 100); // Red for hint ready
        aiButtonText = _T("隐藏提示");
        break;
    default:
        aiButtonColorFill = RGB(150, 150, 150); // Grey for error/unknown
        aiButtonText = _T("AI 状态错误"); // Should not happen
        break;
    }
    // Draw other buttons with default color
    setfillcolor(RGB(210, 230, 250));
    fillroundrect(600, 170, 740, 210, 12, 12); // Return
    fillroundrect(600, 230, 740, 270, 12, 12); // Undo
    fillroundrect(600, 290, 740, 330, 12, 12); // Restore
    fillroundrect(600, 350, 740, 390, 12, 12); // Save
    // Draw AI Hint Button with its dynamic color
    setfillcolor(aiButtonColorFill);
    fillroundrect(600, 430, 740, 470, 12, 12);

    // Draw Button Text
    settextstyle(26, 0, _T("微软雅黑")); // Font for button text
    settextcolor(RGB(30, 40, 80)); // Dark text for buttons
    outtextxy(615, 182, _T("返回主菜单"));
    outtextxy(635, 242, _T("撤销"));
    outtextxy(635, 302, _T("还原"));
    outtextxy(635, 362, _T("保存"));
    outtextxy(635, 442, aiButtonText); // Dynamic AI Button Text

    // Draw Peg Count Display
    TCHAR s_pegs_anim[48];
    _stprintf_s(s_pegs_anim, _T("剩余棋子: %d"), countPegs()); // Reflects mid-move state
    settextcolor(RGB(40, 130, 40)); // Green text for peg count
    settextstyle(22, 0, _T("微软雅黑")); // Font for peg count
    outtextxy(610, 45, s_pegs_anim);

    // Draw actual AI hint lines on board if state is SHOWING_HINT (even during other animations)
    if (aiHintCurrentState == AIHintState::SHOWING_HINT && currentAIHintMove.fromX >= 0) {
        aiHelper.drawHint(currentAIHintMove, 80, 80, 62);
    }

    // --- Draw the animated peg ---
    int baseX_anim = 80, baseY_anim = 80, cell_anim = 62; // Use distinct names or ensure no conflict
    int shadowOffset_anim = isHighQuality ? 4 : 2;
    int pieceRadius_anim = isHighQuality ? cell_anim / 2 - 8 : cell_anim / 2 - 10;
    int highlightSizeInner_anim = isHighQuality ? (cell_anim / 2 - 16) / 2 : (cell_anim / 2 - 18) / 3;

    float px1_center = baseX_anim + animFromY * cell_anim + cell_anim / 2.0f;
    float py1_center = baseY_anim + animFromX * cell_anim + cell_anim / 2.0f;
    float px2_center = baseX_anim + animToY * cell_anim + cell_anim / 2.0f;
    float py2_center = baseY_anim + animToX * cell_anim + cell_anim / 2.0f;

    double t_anim_progress = progress; // progress is 0 to 1 for the current animation
    // Easing function for smoother animation
    if (isHighQuality) t_anim_progress = t_anim_progress < 0.5 ? 2 * t_anim_progress * t_anim_progress : -1 + (4 - 2 * t_anim_progress) * t_anim_progress; // Ease in-out quadratic
    else t_anim_progress = t_anim_progress * t_anim_progress * (3 - 2 * t_anim_progress); // Simpler ease in-out cubic

    float currentX = px1_center + (px2_center - px1_center) * t_anim_progress;
    float currentY = py1_center + (py2_center - py1_center) * t_anim_progress;

    // Arc for jump visual
    double arcHeight = isHighQuality ? 20.0 : 10.0;
    if (t_anim_progress > 0.0 && t_anim_progress < 1.0) { // Apply arc only during the jump
        currentY -= sin(progress * 3.1415926535) * arcHeight; // Use raw 'progress' for full arc cycle (0 to PI)
    }

    // Draw shadow for animated peg
    if (isHighQuality) {
        setfillcolor(RGB(185, 90, 70));
        setlinecolor(RGB(185, 90, 70));
        fillcircle(int(currentX + shadowOffset_anim), int(currentY + shadowOffset_anim), pieceRadius_anim);
    }
    // Draw animated peg body
    setfillcolor(RGB(225, 105, 80));
    setlinecolor(RGB(225, 105, 80));
    fillcircle(int(currentX), int(currentY), pieceRadius_anim);
    // Draw highlight on animated peg
    if (isHighQuality) {
        setfillcolor(RGB(255, 220, 200));
        setlinecolor(RGB(255, 220, 200));
        fillcircle(int(currentX - pieceRadius_anim * 0.3f), int(currentY - pieceRadius_anim * 0.3f), highlightSizeInner_anim);
    }
}


void Game::drawGame() { // Called when not animating
    setbkcolor(RGB(252, 240, 220));
    cleardevice();
    POINT pt_mouse; GetCursorPos(&pt_mouse); ScreenToClient(GetHWnd(), &pt_mouse);
    int mx = pt_mouse.x, my = pt_mouse.y;
    bool isHighQuality = (setting.quality == "高");

    board.draw(80, 80, 62, selectedX, selectedY, mx, my, true, isHighQuality); // Draw board with selection/hover

    // --- Draw UI Buttons (including AI button with its current state color/text) ---
    COLORREF aiButtonColorFill;
    LPCTSTR aiButtonText;
    switch (aiHintCurrentState) {
    case AIHintState::IDLE:
        aiButtonColorFill = RGB(210, 230, 250);
        aiButtonText = _T("AI 提示");
        break;
    case AIHintState::WORKING:
        aiButtonColorFill = RGB(100, 220, 100);
        aiButtonText = _T("AI 计算中...");
        break;
    case AIHintState::SHOWING_HINT:
        aiButtonColorFill = RGB(220, 100, 100);
        aiButtonText = _T("隐藏提示");
        break;
    default:
        aiButtonColorFill = RGB(150, 150, 150);
        aiButtonText = _T("AI 状态错误");
        break;
    }
    setfillcolor(RGB(210, 230, 250));
    fillroundrect(600, 170, 740, 210, 12, 12); fillroundrect(600, 230, 740, 270, 12, 12);
    fillroundrect(600, 290, 740, 330, 12, 12); fillroundrect(600, 350, 740, 390, 12, 12);
    setfillcolor(aiButtonColorFill); fillroundrect(600, 430, 740, 470, 12, 12);

    settextstyle(26, 0, _T("微软雅黑")); settextcolor(RGB(30, 40, 80));
    outtextxy(615, 182, _T("返回主菜单")); outtextxy(635, 242, _T("撤销"));
    outtextxy(635, 302, _T("还原")); outtextxy(635, 362, _T("保存"));
    outtextxy(635, 442, aiButtonText);

    TCHAR s_pegs_static[48]; _stprintf_s(s_pegs_static, _T("剩余棋子: %d"), countPegs());
    settextcolor(RGB(40, 130, 40)); settextstyle(22, 0, _T("微软雅黑")); outtextxy(610, 45, s_pegs_static);

    if (aiHintCurrentState == AIHintState::SHOWING_HINT && currentAIHintMove.fromX >= 0) {
        aiHelper.drawHint(currentAIHintMove, 80, 80, 62); // aiHelper.drawHint handles drawing lines and text
    }
}

bool Game::isValidMove(int x1, int y1, int x2, int y2) {
    if (!board.isValid(x1, y1) || !board.isValid(x2, y2)) return false;
    int mX = x1 + (x2 - x1) / 2, mY = y1 + (y2 - y1) / 2;
    if (!board.isValid(mX, mY)) return false;
    if (board.get(x1, y1) != 1 || board.get(x2, y2) != 0) return false;
    int dx_abs = std::abs(x2 - x1), dy_abs = std::abs(y2 - y1);
    if (((dx_abs == 2 && dy_abs == 0) || (dx_abs == 0 && dy_abs == 2)) && board.get(mX, mY) == 1) return true;
    BoardType type = board.getType();
    if ((type == TRIANGLE || type == PYRAMID || type == STAR) && dx_abs == 2 && dy_abs == 2 && board.get(mX, mY) == 1) return true;
    return false;
}

void Game::move(int x1, int y1, int x2, int y2) { /* Conceptual - not directly used for animated moves */ }

bool Game::hasMoves() {
    for (int i = 0; i < board.sizeX(); ++i) {
        for (int j = 0; j < board.sizeY(); ++j) {
            if (board.get(i, j) == 1) {
                int std_dx[] = { 0,0,2,-2 }, std_dy[] = { 2,-2,0,0 };
                for (int d = 0; d < 4; ++d) if (isValidMove(i, j, i + std_dx[d], j + std_dy[d])) return true;
                BoardType type = board.getType();
                if (type == TRIANGLE || type == PYRAMID || type == STAR) {
                    int diag_dx[] = { 2,2,-2,-2 }, diag_dy[] = { 2,-2,2,-2 };
                    for (int d = 0; d < 4; ++d)if (isValidMove(i, j, i + diag_dx[d], j + diag_dy[d]))return true;
                }
            }
        }
    }
    return false;
}

int Game::countPegs() {
    int cnt = 0;
    const auto& data = board.data(); // Use const auto& for efficiency
    for (const auto& row : data) {
        for (int val : row) {
            if (val == 1) ++cnt;
        }
    }
    return cnt;
}