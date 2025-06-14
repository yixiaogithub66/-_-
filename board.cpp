#include "board.h"
#include <tchar.h>
#include <cmath>

Board::Board(BoardType t) : type(t) { reset(); }

void Board::reset() {
    if (type == CROSS) {
        // ʮ�����̴��벻��
        board.assign(7, std::vector<int>(7, 1));
        for (int i : {0, 1, 5, 6})
            for (int j : {0, 1, 5, 6})
                board[i][j] = -1;
        board[3][3] = 0;
    }
    else if (type == DIAMOND) {
        // �������̴��벻��
        board.assign(5, std::vector<int>(5, 0));
        for (int i = 0; i < 5; ++i)
            for (int j = 0; j < 5; ++j)
                if (abs(i - 2) + abs(j - 2) <= 2)
                    board[i][j] = 1;
        board[2][2] = 0;
    }
    else if (type == TRIANGLE) {
        // �������̴��벻��
        board.clear();
        for (int i = 0; i < 5; ++i) {
            std::vector<int> row(5, 0);
            for (int j = 0; j <= i; ++j)
                row[j] = 1;
            board.push_back(row);
        }
        board[0][0] = 0;
    }
    else if (type == TRAPEZOID) {
        // �������� - �Ͽ���խ
        board.assign(5, std::vector<int>(7, -1));
        // ��һ�� (������)
        for (int j = 0; j < 7; j++) {
            board[0][j] = 1;
        }
        // �ڶ���
        for (int j = 0; j < 6; j++) {
            board[1][j] = 1;
        }
        // ������
        for (int j = 0; j < 5; j++) {
            board[2][j] = 1;
        }
        // ������
        for (int j = 0; j < 4; j++) {
            board[3][j] = 1;
        }
        // ������ (������)
        for (int j = 0; j < 3; j++) {
            board[4][j] = 1;
        }
        // ����һ����λ
        board[2][3] = 0;
    }
    else if (type == PYRAMID) {
        // ���������� - ��������
        board.assign(5, std::vector<int>(5, -1));
        for (int i = 0; i < 5; i++) {
            int start = 2 - i / 2;
            int end = 2 + i / 2;
            for (int j = start; j <= end; j++) {
                board[i][j] = 1;
            }
        }
        // ����һ����λ
        board[0][2] = 0;
    }
    else if (type == STAR) {
        // ��������
        board.assign(5, std::vector<int>(5, -1));
        // ����ʮ��
        for (int i = 0; i < 5; i++) {
            board[2][i] = 1;  // ˮƽ����
            board[i][2] = 1;  // ��ֱ����
        }
        // ����ĸ���
        board[1][1] = 1;
        board[1][3] = 1;
        board[3][1] = 1;
        board[3][3] = 1;
        // ��������Ϊ��
        board[2][2] = 0;
    }
}

void Board::draw(int bx, int by, int cell, int selX, int selY, int mx, int my, bool showHint, bool isHighQuality) {
    // ���ݻ��ʵ������Ʋ���
    int shadowOffset = isHighQuality ? 4 : 2;
    int pieceRadius = isHighQuality ? cell / 2 - 8 : cell / 2 - 10;
    bool drawHighlight = isHighQuality;
    // ������͵��׻�ɫ����
    setbkcolor(RGB(252, 240, 220));
    cleardevice();

    // ����Բ�Ǿ���
    setfillcolor(RGB(252, 240, 220));
    setlinecolor(RGB(252, 240, 220));
    fillroundrect(20, 20, 780, 630, 40, 40);

    // ��Ӱ�͸߹���� - ɾ����һ���е� shadowOffset �ض���
    COLORREF shadowColor = RGB(185, 90, 70);
    COLORREF pieceColor = RGB(225, 105, 80);
    COLORREF pieceHoverColor = RGB(240, 115, 90);
    COLORREF highlightColor = RGB(255, 220, 200);
    COLORREF emptyColor = RGB(245, 245, 240);
    COLORREF emptyBorderColor = RGB(230, 220, 200);
    COLORREF validMoveColor = RGB(100, 200, 120);
    COLORREF selectedBorderColor = RGB(80, 180, 100);

    // ������뱣�ֲ���...

    // ���㵱ǰ���ӵĿ��߲�
    std::vector<std::pair<int, int>> validMoves;
    if (showHint && selX >= 0 && selY >= 0 && get(selX, selY) == 1) {
        int dx[] = { 2, -2, 0, 0 }, dy[] = { 0, 0, 2, -2 };
        for (int d = 0; d < 4; ++d) {
            int nx = selX + dx[d], ny = selY + dy[d];
            int mx2 = selX + dx[d] / 2, my2 = selY + dy[d] / 2;
            if (isValid(nx, ny) && get(nx, ny) == 0 && get(mx2, my2) == 1)
                validMoves.emplace_back(nx, ny);
        }
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

    // ���������ϵ����и���
    for (int i = 0; i < board.size(); ++i) {
        for (int j = 0; j < board[i].size(); ++j) {
            int x = bx + j * cell, y = by + i * cell;
            if (board[i][j] == -1) continue;

            bool hovered = false;
            if (mx >= 0 && my >= 0 && board[i][j] == 1) {
                int cx = x + cell / 2, cy = y + cell / 2, r = cell / 2 - 8;
                int dx = mx - cx, dy = my - cy;
                if (dx * dx + dy * dy <= r * r) hovered = true;
            }

            // ����
            if (board[i][j] == 1) {
                // ��Ӱ
                setfillcolor(shadowColor);
                setlinecolor(shadowColor);
                fillcircle(x + cell / 2 + shadowOffset, y + cell / 2 + shadowOffset, cell / 2 - 8);

                // ��������
                COLORREF c = hovered ? pieceHoverColor : pieceColor;
                setfillcolor(c);
                setlinecolor(c);
                fillcircle(x + cell / 2, y + cell / 2, cell / 2 - 8);

                // �߹�Ч�������Ͻǣ�
                setfillcolor(highlightColor);
                setlinecolor(highlightColor);
                fillcircle(x + cell / 2 - 9, y + cell / 2 - 9, (cell / 2 - 16) / 2);

                // ѡ��״̬��Ȧ
                if (showHint && i == selX && j == selY) {
                    // ��ɫ��Ȧ
                    setlinecolor(WHITE);
                    setlinestyle(PS_SOLID, 4);
                    circle(x + cell / 2, y + cell / 2, cell / 2 - 4);

                    // ��ɫ��Ȧ
                    setlinecolor(selectedBorderColor);
                    setlinestyle(PS_SOLID, 2);
                    circle(x + cell / 2, y + cell / 2, cell / 2 - 4);
                    setlinestyle(PS_SOLID, 1);
                }
            }
            // ��λ
            else if (board[i][j] == 0) {
                // ��λ���
                setfillcolor(emptyColor);
                setlinecolor(emptyBorderColor);
                fillcircle(x + cell / 2, y + cell / 2, cell / 2 - 8);
                setlinestyle(PS_SOLID, 2);
                circle(x + cell / 2, y + cell / 2, cell / 2 - 10);
                setlinestyle(PS_SOLID, 1);

                // ����λ�ø���
                if (showHint) {
                    for (auto& mv : validMoves) {
                        if (mv.first == i && mv.second == j) {
                            setfillcolor(validMoveColor);
                            setlinecolor(validMoveColor);
                            fillcircle(x + cell / 2, y + cell / 2, cell / 2 - 10);
                            break;
                        }
                    }
                }
            }
        }
    }
}

bool Board::isValid(int x, int y) const {
    return x >= 0 && y >= 0 && x < board.size() && y < board[x].size() && board[x][y] != -1;
}
int Board::get(int x, int y) const { return board[x][y]; }
void Board::set(int x, int y, int val) { board[x][y] = val; }
BoardType Board::getType() const { return type; }
int Board::sizeX() const { return board.size(); }
int Board::sizeY() const { return board[0].size(); }

BoardType boardMenu() {
    setbkcolor(RGB(220, 240, 255));
    cleardevice();
    settextstyle(45, 0, _T("΢���ź�"));
    settextcolor(RGB(40, 80, 140));
    outtextxy(220, 80, _T("ѡ��������ʽ"));
    settextstyle(32, 0, _T("΢���ź�"));

    // ������������Ӧ����ѡ��
    RECT r1 = { 170,160,630,200 }; // ʮ������
    RECT r2 = { 170,220,630,260 }; // ��������
    RECT r3 = { 170,280,630,320 }; // ��������
    RECT r4 = { 170,340,630,380 }; // �������� (����)
    RECT r5 = { 170,400,630,440 }; // ���������� (����)
    RECT r6 = { 170,460,630,500 }; // �������� (����)

    setfillcolor(RGB(200, 220, 240));
    fillroundrect(r1.left, r1.top, r1.right, r1.bottom, 15, 15);
    fillroundrect(r2.left, r2.top, r2.right, r2.bottom, 15, 15);
    fillroundrect(r3.left, r3.top, r3.right, r3.bottom, 15, 15);
    fillroundrect(r4.left, r4.top, r4.right, r4.bottom, 15, 15);
    fillroundrect(r5.left, r5.top, r5.right, r5.bottom, 15, 15);
    fillroundrect(r6.left, r6.top, r6.right, r6.bottom, 15, 15);

    settextcolor(RGB(20, 40, 80));
    drawtext(_T("ʮ������"), &r1, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    drawtext(_T("��������"), &r2, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    drawtext(_T("��������"), &r3, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    drawtext(_T("��������"), &r4, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    drawtext(_T("����������"), &r5, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    drawtext(_T("��������"), &r6, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    while (true) {
        ExMessage m;
        while (peekmessage(&m, EM_MOUSE)) {
            if (m.message == WM_LBUTTONDOWN) {
                int x = m.x, y = m.y;
                if (x >= r1.left && x <= r1.right && y >= r1.top && y <= r1.bottom) return CROSS;
                if (x >= r2.left && x <= r2.right && y >= r2.top && y <= r2.bottom) return DIAMOND;
                if (x >= r3.left && x <= r3.right && y >= r3.top && y <= r3.bottom) return TRIANGLE;
                if (x >= r4.left && x <= r4.right && y >= r4.top && y <= r4.bottom) return TRAPEZOID;
                if (x >= r5.left && x <= r5.right && y >= r5.top && y <= r5.bottom) return PYRAMID;
                if (x >= r6.left && x <= r6.right && y >= r6.top && y <= r6.bottom) return STAR;
            }
        }
        Sleep(10);
    }
}