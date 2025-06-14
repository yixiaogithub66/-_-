#include "board.h"
#include <tchar.h>
#include <cmath>

Board::Board(BoardType t) : type(t) { reset(); }

void Board::reset() {
    if (type == CROSS) {
        board.assign(7, std::vector<int>(7, 1));
        for (int i : {0, 1, 5, 6})
            for (int j : {0, 1, 5, 6})
                board[i][j] = -1;
        board[3][3] = 0;
    }
    else if (type == DIAMOND) {
        board.assign(5, std::vector<int>(5, 0));
        for (int i = 0; i < 5; ++i)
            for (int j = 0; j < 5; ++j)
                if (abs(i - 2) + abs(j - 2) <= 2)
                    board[i][j] = 1;
        board[2][2] = 0;
    }
    else if (type == TRIANGLE) {
        board.clear();
        for (int i = 0; i < 5; ++i) {
            std::vector<int> row(5, 0);
            for (int j = 0; j <= i; ++j)
                row[j] = 1;
            board.push_back(row);
        }
        board[0][0] = 0;
    }
}

void Board::draw(int bx, int by, int cell, int selX, int selY, int mx, int my, bool showHint) {
    // 棋盘背景
    setbkcolor(RGB(255, 245, 220));
    cleardevice();
    setfillcolor(RGB(255, 245, 220));
    setlinecolor(RGB(255, 245, 220));
    fillroundrect(20, 20, 780, 630, 40, 40);

    int shadowOffset = 6;
    COLORREF shadowColor = RGB(180, 90, 70);
    COLORREF pieceColor = RGB(225, 100, 70);
    COLORREF highlightColor = RGB(255, 220, 200);

    // 计算可落子点
    std::vector<std::pair<int, int>> validMoves;
    if (showHint && selX >= 0 && selY >= 0 && get(selX, selY) == 1) {
        int dx[] = { 2, -2, 0, 0 }, dy[] = { 0, 0, 2, -2 };
        for (int d = 0; d < 4; ++d) {
            int nx = selX + dx[d], ny = selY + dy[d];
            int mx2 = selX + dx[d] / 2, my2 = selY + dy[d] / 2;
            if (isValid(nx, ny) && get(nx, ny) == 0 && get(mx2, my2) == 1)
                validMoves.emplace_back(nx, ny);
        }
        // 三角棋盘斜向
        if (type == TRIANGLE) {
            int tdx[] = { 2, -2, 2, -2 }, tdy[] = { 2, -2, -2, 2 };
            for (int d = 0; d < 4; ++d) {
                int nx = selX + tdx[d], ny = selY + tdy[d];
                int mx2 = selX + tdx[d] / 2, my2 = selY + tdy[d] / 2;
                if (isValid(nx, ny) && get(nx, ny) == 0 && get(mx2, my2) == 1)
                    validMoves.emplace_back(nx, ny);
            }
        }
    }

    for (int i = 0; i < board.size(); ++i) {
        for (int j = 0; j < board[i].size(); ++j) {
            int x = bx + j * cell, y = by + i * cell;
            if (board[i][j] == -1) continue;

            bool hovered = false;
            if (mx >= 0 && my >= 0 && board[i][j] == 1) {
                int cx = x + cell / 2, cy = y + cell / 2, r = cell / 2 - 10;
                int dx = mx - cx, dy = my - cy;
                if (dx * dx + dy * dy <= r * r) hovered = true;
            }

            // 阴影
            if (board[i][j] == 1) {
                setfillcolor(shadowColor);
                setlinecolor(shadowColor);
                fillcircle(x + cell / 2 + shadowOffset, y + cell / 2 + shadowOffset, cell / 2 - 10);
            }

            // 棋子
            if (board[i][j] == 1) {
                COLORREF c = hovered ? RGB(200, 70, 50) : pieceColor;
                int r = hovered ? cell / 2 - 13 : cell / 2 - 10;
                setfillcolor(c);
                setlinecolor(c);
                fillcircle(x + cell / 2, y + cell / 2, r);

                // 高光
                setfillcolor(highlightColor);
                setlinecolor(highlightColor);
                fillcircle(x + cell / 2 - 10, y + cell / 2 - 10, (cell / 2 - 18) / 2);

                // 选中棋子外圈高亮
                if (showHint && i == selX && j == selY) {
                    setlinecolor(WHITE);
                    setlinestyle(PS_SOLID, 5);
                    circle(x + cell / 2, y + cell / 2, r + 4);
                    setlinecolor(RGB(60, 200, 120));
                    setlinestyle(PS_SOLID, 2);
                    circle(x + cell / 2, y + cell / 2, r + 4);
                    setlinestyle(PS_SOLID, 1);
                }
            }
            // 空位
            else if (board[i][j] == 0) {
                setfillcolor(RGB(255, 245, 220));
                setlinecolor(RGB(220, 210, 180));
                fillcircle(x + cell / 2, y + cell / 2, cell / 2 - 10);
                setlinecolor(RGB(220, 210, 180));
                setlinestyle(PS_SOLID, 2);
                circle(x + cell / 2, y + cell / 2, cell / 2 - 12);
                setlinestyle(PS_SOLID, 1);

                // 可落子点绿色提示
                if (showHint) {
                    for (auto& mv : validMoves) {
                        if (mv.first == i && mv.second == j) {
                            setfillcolor(RGB(60, 200, 120));
                            setlinecolor(RGB(60, 200, 120));
                            fillcircle(x + cell / 2, y + cell / 2, cell / 2 - 12);
                            break;
                        }
                    }
                }
            }
        }
    }
}

bool Board::isValid(int x, int y) {
    return x >= 0 && y >= 0 && x < board.size() && y < board[x].size() && board[x][y] != -1;
}
int Board::get(int x, int y) { return board[x][y]; }
void Board::set(int x, int y, int val) { board[x][y] = val; }
BoardType Board::getType() const { return type; }
int Board::sizeX() const { return board.size(); }
int Board::sizeY() const { return board[0].size(); }
std::vector<std::vector<int>>& Board::data() { return board; }

BoardType boardMenu() {
    setbkcolor(RGB(220, 240, 255));
    cleardevice();
    settextstyle(45, 0, _T("微软雅黑"));
    settextcolor(RGB(40, 80, 140));
    outtextxy(220, 100, _T("选择棋盘形状"));
    settextstyle(32, 0, _T("微软雅黑"));
    RECT r1 = { 170,220,630,270 }, r2 = { 170,300,630,350 }, r3 = { 170,380,630,430 };
    setfillcolor(RGB(200, 220, 240));
    fillroundrect(r1.left, r1.top, r1.right, r1.bottom, 15, 15);
    fillroundrect(r2.left, r2.top, r2.right, r2.bottom, 15, 15);
    fillroundrect(r3.left, r3.top, r3.right, r3.bottom, 15, 15);
    settextcolor(RGB(20, 40, 80));
    drawtext(_T("十字棋盘"), &r1, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    drawtext(_T("菱形棋盘"), &r2, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    drawtext(_T("三角棋盘"), &r3, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    while (true) {
        ExMessage m;
        while (peekmessage(&m, EM_MOUSE)) {
            if (m.message == WM_LBUTTONDOWN) {
                int x = m.x, y = m.y;
                if (x >= r1.left && x <= r1.right && y >= r1.top && y <= r1.bottom) return CROSS;
                if (x >= r2.left && x <= r2.right && y >= r2.top && y <= r2.bottom) return DIAMOND;
                if (x >= r3.left && x <= r3.right && y >= r3.top && y <= r3.bottom) return TRIANGLE;
            }
        }
        Sleep(10);
    }
}