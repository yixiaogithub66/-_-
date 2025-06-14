#include "game.h"
#include <graphics.h>
#include <conio.h>
#include <tchar.h>
#include <cmath>

// 判断鼠标是否在格子内
bool isInCell(int mx, int my, int gx, int gy, int cell) {
    return mx >= gx && mx < gx + cell && my >= gy && my < gy + cell;
}

Game::Game(Board b, const Setting& s)
    : board(b), setting(s), selectedX(-1), selectedY(-1),
    animating(false), animProgress(0), animFrames(20), animCurrentFrame(0) {
    initialBoard = board.data();
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
    }
}

void Game::restoreInitialBoard() {
    board.data() = initialBoard;
    selectedX = selectedY = -1;
    animating = false;
    boardHistory.clear();
    selectedXHistory.clear();
    selectedYHistory.clear();
}

void Game::run() {
    int baseX = 80, baseY = 80, cell = 62;
    bool running = true;
    BeginBatchDraw();
    while (running) {
        if (animating)
            drawGameWithAnim();
        else
            drawGame();
        FlushBatchDraw();
        if (animating) {
            Sleep(16);
            animCurrentFrame++;
            animProgress = (float)animCurrentFrame / animFrames;
            if (animCurrentFrame > animFrames) {
                board.set(animToX, animToY, 1);
                board.set(animMidX, animMidY, 0);
                board.set(animFromX, animFromY, 0);
                animating = false;
                if (!hasMoves()) {
                    drawGame();
                    FlushBatchDraw();
                    showEnd(countPegs() == 1);
                    running = false;
                }
            }
            continue;
        }

        if (setting.controlMode == CONTROL_MOUSE) {
            ExMessage m;
            while (peekmessage(&m, EM_MOUSE)) {
                if (m.message == WM_LBUTTONDOWN) {
                    int hitRow = -1, hitCol = -1;
                    for (int i = 0; i < board.sizeX(); ++i) {
                        for (int j = 0; j < board.sizeY(); ++j) {
                            if (board.get(i, j) == -1) continue;
                            int gx = baseX + j * cell;
                            int gy = baseY + i * cell;
                            if (isInCell(m.x, m.y, gx, gy, cell)) {
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
                            // 第二次点击，目标为“空”且为合法走法
                            saveHistory();
                            playMoveAnimation(selectedX, selectedY, hitRow, hitCol);
                            selectedX = selectedY = -1;
                        }
                        else {
                            // 其他情况取消选中
                            selectedX = selectedY = -1;
                        }
                    }
                    if (m.x >= 600 && m.x <= 740 && m.y >= 170 && m.y <= 210) {
                        running = false;
                    }
                    if (m.x >= 600 && m.x <= 740 && m.y >= 230 && m.y <= 270) {
                        undoMove();
                    }
                    if (m.x >= 600 && m.x <= 740 && m.y >= 290 && m.y <= 330) {
                        restoreInitialBoard();
                    }
                }
            }
        }
        else if (setting.controlMode == CONTROL_KEYBOARD) {
            ExMessage m;
            while (peekmessage(&m, EM_MOUSE)) {
                if (m.message == WM_LBUTTONDOWN) {
                    int hitRow = -1, hitCol = -1;
                    for (int i = 0; i < board.sizeX(); ++i) {
                        for (int j = 0; j < board.sizeY(); ++j) {
                            if (board.get(i, j) == -1) continue;
                            int gx = baseX + j * cell;
                            int gy = baseY + i * cell;
                            if (isInCell(m.x, m.y, gx, gy, cell)) {
                                hitRow = i; hitCol = j;
                            }
                        }
                    }
                    if (hitRow != -1 && hitCol != -1 && board.get(hitRow, hitCol) == 1) {
                        selectedX = hitRow;
                        selectedY = hitCol;
                    }
                    if (m.x >= 600 && m.x <= 740 && m.y >= 170 && m.y <= 210) {
                        running = false;
                    }
                    if (m.x >= 600 && m.x <= 740 && m.y >= 230 && m.y <= 270) {
                        undoMove();
                    }
                    if (m.x >= 600 && m.x <= 740 && m.y >= 290 && m.y <= 330) {
                        restoreInitialBoard();
                    }
                }
            }
            while (peekmessage(&m, EX_KEY)) {
                if (selectedX != -1 && selectedY != -1 && m.message == WM_KEYDOWN) {
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

        Sleep(10);
    }
    EndBatchDraw();
}

void Game::playMoveAnimation(int x1, int y1, int x2, int y2) {
    animating = true;
    animFromX = x1; animFromY = y1;
    animToX = x2; animToY = y2;
    animMidX = (x1 + x2) / 2; animMidY = (y1 + y2) / 2;
    animFrames = 16;
    animCurrentFrame = 0;
    animProgress = 0.0f;
    board.set(x1, y1, 0);
    board.set(animMidX, animMidY, 0);
}

void Game::drawGameWithAnim() {
    setbkcolor(RGB(255, 245, 220));
    cleardevice();
    // 动画帧不显示选中和落子提示
    MOUSEMSG m = GetMouseMsg();
    board.draw(80, 80, 62, -1, -1, m.x, m.y, false);

    setfillcolor(RGB(210, 230, 250));
    fillroundrect(600, 170, 740, 210, 12, 12);
    fillroundrect(600, 230, 740, 270, 12, 12);
    fillroundrect(600, 290, 740, 330, 12, 12);
    settextstyle(26, 0, _T("微软雅黑"));
    settextcolor(RGB(30, 40, 80));
    outtextxy(615, 182, _T("返回菜单"));
    outtextxy(635, 242, _T("悔棋"));
    outtextxy(635, 302, _T("复原"));
    TCHAR s[48];
    _stprintf_s(s, _T("剩余棋子: %d"), countPegs());
    settextcolor(RGB(40, 130, 40));
    settextstyle(22, 0, _T("微软雅黑"));
    outtextxy(610, 45, s);

    // 动画棋子
    int baseX = 80, baseY = 80, cell = 62;
    float px1 = baseX + animFromY * cell + cell / 2;
    float py1 = baseY + animFromX * cell + cell / 2;
    float px2 = baseX + animToY * cell + cell / 2;
    float py2 = baseY + animToX * cell + cell / 2;
    float t = animProgress;
    t = t * t * (3 - 2 * t);
    float nowx = px1 + (px2 - px1) * t;
    float nowy = py1 + (py2 - py1) * t;

    // 阴影
    setfillcolor(RGB(180, 90, 70));
    setlinecolor(RGB(180, 90, 70));
    fillcircle((int)nowx + 6, (int)nowy + 6, cell / 2 - 10);
    // 棋子
    setfillcolor(RGB(225, 100, 70));
    setlinecolor(RGB(225, 100, 70));
    fillcircle((int)nowx, (int)nowy, cell / 2 - 10);
    // 高光
    setfillcolor(RGB(255, 220, 200));
    setlinecolor(RGB(255, 220, 200));
    fillcircle((int)nowx - 10, (int)nowy - 10, (cell / 2 - 18) / 2);
}

void Game::drawGame() {
    setbkcolor(RGB(255, 245, 220));
    cleardevice();
    MOUSEMSG m = GetMouseMsg(); // 用GetMouseMsg防止闪烁
    int mx = m.x, my = m.y;
    board.draw(80, 80, 62, selectedX, selectedY, mx, my, true);

    setfillcolor(RGB(210, 230, 250));
    fillroundrect(600, 170, 740, 210, 12, 12);
    fillroundrect(600, 230, 740, 270, 12, 12);
    fillroundrect(600, 290, 740, 330, 12, 12);
    settextstyle(26, 0, _T("微软雅黑"));
    settextcolor(RGB(30, 40, 80));
    outtextxy(615, 182, _T("返回菜单"));
    outtextxy(635, 242, _T("悔棋"));
    outtextxy(635, 302, _T("复原"));
    TCHAR s[48];
    _stprintf_s(s, _T("剩余棋子: %d"), countPegs());
    settextcolor(RGB(40, 130, 40));
    settextstyle(22, 0, _T("微软雅黑"));
    outtextxy(610, 45, s);
}

void Game::showEnd(bool win) {
    settextstyle(36, 0, _T("微软雅黑"));
    settextcolor(win ? RGB(60, 180, 60) : RGB(180, 40, 40));
    RECT r = { 170,260,630,340 };
    drawtext(win ? _T("恭喜！只剩1枚棋子！") : _T("无可走棋步，游戏结束！"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    settextstyle(22, 0, _T("微软雅黑"));
    outtextxy(280, 370, _T("点击任意处返回菜单..."));
    while (!_kbhit()) Sleep(10);
    _getch();
}

bool Game::isValidMove(int x1, int y1, int x2, int y2) {
    if (board.get(x1, y1) != 1 || board.get(x2, y2) != 0) return false;
    int dx = x2 - x1, dy = y2 - y1;
    if (std::abs(dx) == 2 && dy == 0 && board.get(x1 + dx / 2, y1) == 1) return true;
    if (std::abs(dy) == 2 && dx == 0 && board.get(x1, y1 + dy / 2) == 1) return true;
    if (board.getType() == TRIANGLE && std::abs(dx) == 2 && std::abs(dy) == 2 && board.get(x1 + dx / 2, y1 + dy / 2) == 1) return true;
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