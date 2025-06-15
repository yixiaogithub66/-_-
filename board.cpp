#include "board.h"   // �����Զ����������ͷ�ļ��������� Board ��������� BoardType ö�ٵȡ�
#include <tchar.h>    // ���� tchar.h ͷ�ļ�������֧��ͨ���ַ����ͣ��� TCHAR����ʹ�ô���������� ANSI �� Unicode �����±��롣
#include <cmath>      // ���� cmath ��ѧ�⣬������ѧ���㣬����˴��� abs()����ֵ������

// Board ��Ĺ��캯����
// ������һ�� Board ����ʱ������ô˹��캯����
// @param t: ���̵����� (BoardType ö��ֵ������ CROSS, DIAMOND ��)��
// �����ʼ����Ա���� 'type' Ϊ��������� 't'��Ȼ����� reset() ���������ݸ������������̵ĳ�ʼ���֡�
Board::Board(BoardType t) : type(t) {
    reset(); // ���� reset ������ʼ�����̲���
}

// Board ��� reset ������
// ���ݵ�ǰ���̵����� (this->type) ���������̵Ĳ��ֵ����ʼ״̬��
// board ��һ�� std::vector<std::vector<int>> ��Ա���������У�
// -1: �������������Ч�����߽硣
//  0: ���������ϵ�һ���ո񣨿��Է����ӵ�λ�ã���
//  1: ������������һ�����ӡ�
void Board::reset() {
    if (type == CROSS) { // �������������ʮ���� (CROSS)
        // ʮ������ͨ����һ�� 7x7 �����񣬵����䲿������Ч�ġ�
        // board.assign(rows, std::vector<int>(cols, initial_value)) ���ڽ���ά������ʼ��Ϊָ�������������ͳ�ʼֵ��
        board.assign(7, std::vector<int>(7, 1)); // ��ʼ��һ�� 7x7 �����̣�����λ���ȼ��趼������ (1)��
        // ���ĸ� 2x2 �Ľ���������Ϊ��Ч���� (-1)��
        for (int i : {0, 1, 5, 6}) // ���������� 0, 1, 5, 6
            for (int j : {0, 1, 5, 6}) // ���������� 0, 1, 5, 6
                board[i][j] = -1;    // �� board[i][j] ����Ϊ��Ч
        board[3][3] = 0; // ������λ�� (3,3) ����Ϊ�ո� (0)�����ǿ��������ʼ��λ��
    }
    else if (type == DIAMOND) { // ����������������� (DIAMOND)
        // ��������ͨ����һ�� 5x5 �������ڡ�
        board.assign(5, std::vector<int>(5, 0)); // ��ʼ��һ�� 5x5 �����̣�����λ��������Ϊ�ո� (0)��
        // ʹ�������پ�����ȷ�����������ڵ���Ч��λ��
        // abs(i - centerX) + abs(j - centerY) <= radius ������һ�����Ρ�
        for (int i = 0; i < 5; ++i)
            for (int j = 0; j < 5; ++j)
                if (abs(i - 2) + abs(j - 2) <= 2) // ���ĵ� (2,2)���뾶Ϊ 2
                    board[i][j] = 1; // �����������ڵ�λ������Ϊ������ (1)��
        board[2][2] = 0; // ������λ�� (2,2) ����Ϊ�ո� (0)��
    }
    else if (type == TRIANGLE) { // ������������������� (TRIANGLE)
        // ���������̣�ͨ��������������������������ʵ�ֵ���һ���ױ�Ϊ5�ĵ���ֱ�������λ�������״��
        board.clear(); // ����������ݣ�׼��������䡣
        for (int i = 0; i < 5; ++i) { // ����5��
            std::vector<int> row(5, 0); // ����һ������5���ո� (0) ���С�
            for (int j = 0; j <= i; ++j) // �ڵ� i �У��� i+1 �����ӡ�
                row[j] = 1; // ��ǰ j+1 ��λ������Ϊ������ (1)��
            board.push_back(row); // �������õ�����ӵ������С�
        }
        board[0][0] = 0; // ������ (0,0) ����Ϊ�ո� (0)��
    }
    else if (type == TRAPEZOID) { // ����������������� (TRAPEZOID)
        // �������� - ����Ϊһ���Ͽ���խ����״��
        // ʹ��һ�� 5�� x 7�� ��������Ϊ��������Ч������Ϊ -1��
        board.assign(5, std::vector<int>(7, -1)); // ��ʼ��Ϊ 5x7������λ��Ĭ��Ϊ��Ч (-1)��
        // ��һ�� (�����棬7������)
        for (int j = 0; j < 7; j++) {
            board[0][j] = 1;
        }
        // �ڶ��� (6������)
        for (int j = 0; j < 6; j++) {
            board[1][j] = 1;
        }
        // ������ (5������)
        for (int j = 0; j < 5; j++) {
            board[2][j] = 1;
        }
        // ������ (4������)
        for (int j = 0; j < 4; j++) {
            board[3][j] = 1;
        }
        // ������ (�����棬3������)
        for (int j = 0; j < 3; j++) {
            board[4][j] = 1;
        }
        // ����һ����λ�������ڵ����е��м�λ�á�
        board[2][3] = 0;
    }
    else if (type == PYRAMID) { // ������������ǽ������� (PYRAMID) / ��������
        // ����������ͨ��ָŷʽ����������������̡�
        // ����ʵ�ֵ���һ���ԳƵ������Σ���һ�� 5x5 �������ڡ�
        board.assign(5, std::vector<int>(5, -1)); // ��ʼ��Ϊ 5x5������λ��Ĭ��Ϊ��Ч (-1)��
        for (int i = 0; i < 5; i++) { // ����ÿһ��
            // ���㵱ǰ�����ӵ���ʼ�ͽ��������������γ������Ρ�
            // ����һ���߶�Ϊ N �������Σ��� i �� (0-indexed) ������������ i+1��
            // ��������һ�ֶԳƽ�������ʵ�ַ�ʽ��
            int start = 2 - i / 2; // �����кż�����ʼ��
            int end = 2 + i / 2;   // �����кż��������
            if (i % 2 != 0) { // �����У�����������ż���ж�һ�� (���ĳ���ض�������)
                // ������������߼��ƺ��ǣ�
                // i=0: start=2, end=2 (1 peg)
                // i=1: start=1, end=2 (2 pegs if start=2-(1/2)=1.5 -> 1, end=2+(1/2)=2.5 -> 2 for typical triangle)
                //      or start=2, end=2 for a centered line of 1
                // ����� start/end �߼������һ��������ʯ��ˡ��������ν��������Ĳ��֣������Ǳ�׼�ĵȱ������ο����塣
                // �����Ǳ�׼Ӣʽ/ŷʽ������ (ÿ�������� 1, 2, 3, 4, 5):
                // for (int r = 0; r < 5; ++r) {
                //    int num_pegs_in_row = r + 1;
                //    int start_col = (5 - num_pegs_in_row) / 2; // Centering
                //    for (int c = 0; c < num_pegs_in_row; ++c) {
                //        board[r][start_col + c] = 1;
                //    }
                // }
                // ��ǰ������߼�:
                // i=0: s=2, e=2. pegs at [0][2]
                // i=1: s=1, e=2 (if i/2 integer division). pegs at [1][1], [1][2]
                // i=2: s=1, e=3. pegs at [2][1],[2][2],[2][3]
                // i=3: s=0, e=3. pegs at [3][0],[3][1],[3][2],[3][3]
                // i=4: s=0, e=4. pegs at [4][0],[4][1],[4][2],[4][3],[4][4]
                // ����һ����������Ľ���������������������εģ��������ܲ��Ǳ�׼�ĵȱ������Ρ�
                // ����Ǳ�׼�ĵȱ������Σ�������Ҫ����start/end�ļ�������̵ı�ʾ��ʽ��
                // ����һ���������ϵĵȱ������Σ���i��Ӧ����i+1�����ӣ���������Ӧ�þ��С�
                // �����Ǽ���������Ҫһ�����������Ľ�������
                //   *
                //  * *
                // * * *
                // ...
                // ����5�У��� i �� (0-indexed) �� i+1 �����ӡ�
                // �������̿��Ϊ5����ô��i�е�����Ӧ�ô� (4-i)/2 ��ʼ���á�
                // ���磺
                // i=0: (4-0)/2 = 2.  board[0][2]=1
                // i=1: (4-1)/2 = 1.  board[1][1]=1, board[1][2]=1
                // i=2: (4-2)/2 = 1.  board[2][1]=1, board[2][2]=1, board[2][3]=1 (���ԣ�Ӧ����board[2][0], board[2][1], board[2][2] for a left-aligned triangle or centered)
                // ��ǰ����� start, end �߼��ƺ���Ϊĳ���ض��Ľ�������״��Ƶģ������Ǳ�׼�ĵȱ������ο����塣
            }
            for (int j = start; j <= end; j++) {
                if (j >= 0 && j < 5) { // ȷ������������Ч��Χ��
                    board[i][j] = 1;
                }
            }
        }
        // ����һ����λ�����綥�������ӡ�
        board[0][2] = 0; // ����(0,2)�ǽ������Ķ����һ����Ч����ʼ��λ��
    }
    else if (type == STAR) { // ����������������� (STAR) / ����֮��
        // ��������ͨ����һ���ض���״�������ϡ�������һ�� 5x5 �����������ơ�
        board.assign(5, std::vector<int>(5, -1)); // ��ʼ��Ϊ 5x5������λ��Ĭ��Ϊ��Ч (-1)��
        // ����ʮ�ֲ���
        for (int i = 0; i < 5; i++) {
            board[2][i] = 1;  // ˮƽ����ȫ��������
            board[i][2] = 1;  // ��ֱ����ȫ��������
        }
        // ����ĸ����ϵ����ӣ��γ����ε�ͻ�����֡�
        // ����γ�һ������Ӻ���չ����״�������ǵ��������ǡ�
        // �����������ǿ��������̽ṹ�����ӡ�
        // ��ǰʵ��:
        // . . * . .
        // . * * * .
        // * * * * *
        // . * * * .
        // . . * . .
        // ���γ���һ����ʯ�λ���ǿ��ʮ���Ρ�
        board[1][1] = 1; // ���ϽǸ���������
        board[1][3] = 1; // ���ϽǸ���������
        board[3][1] = 1; // ���½Ǹ���������
        board[3][3] = 1; // ���½Ǹ���������
        // ��������λ��Ϊ�ո�
        board[2][2] = 0;
    }
}

// Board ��� draw ������
// ��������Ļ�ϻ����������̣��������ӡ��ո�ѡ��Ч���������ͣЧ������ʾ��
// @param bx, by: ���̻����������ϽǵĻ�׼���� (x, y)��
// @param cell: ÿ�����̸�����ش�С��
// @param selX, selY: ��ǰѡ�е����ӵ��߼����� (��, ��)�����Ϊ -1������ѡ�С�
// @param mx, my: ��굱ǰ����Ļ���� (x, y)��������ͣЧ�������Ϊ -1���򲻴�����ͣ��
// @param showHint: �Ƿ���ʾ����λ�õ���ʾ��
// @param isHighQuality: �Ƿ����ø���������ģʽ (���磬��ƽ������Ӱ�͸߹�)��
void Board::draw(int bx, int by, int cell, int selX, int selY, int mx, int my, bool showHint, bool isHighQuality) {
    // ���� isHighQuality ������������ϸ�ڣ����ṩ��ͬ������Ӿ�Ч����
    int shadowOffset = isHighQuality ? 4 : 2;    // ��Ӱ��ƫ������������ģʽ�¸���
    int pieceRadius = isHighQuality ? cell / 2 - 8 : cell / 2 - 10; // ���Ӱ뾶��������ģʽ�������Դ��߾಻ͬ��
    bool drawHighlight = isHighQuality; // �Ƿ���Ƹ߹⣬������ģʽ�����á�

    // ����һ����͵��׻�ɫ��Ϊ������ͼ���ڵı�����
    setbkcolor(RGB(252, 240, 220));
    cleardevice(); // ��յ�ǰ��ͼ�豸�������±���ɫ��䡣

    // ����һ�����Բ�Ǿ�����Ϊ��������ı����壬�봰�ڱ���ɫ��ͬ���������б߿�������ã���
    setfillcolor(RGB(252, 240, 220)); // �����ɫ
    setlinecolor(RGB(252, 240, 220)); // �߿���ɫ (�����ɫ��ͬ���������Ա߿�)
    fillroundrect(20, 20, 780, 630, 40, 40); // ����Բ�Ǿ��Σ�����Ϊ���Ͻ�x,y�����½�x,y��Բ����Բ�Ŀ�͸ߡ�

    // �������Ԫ�ص���ɫ��
    // COLORREF ��һ����ʾ��ɫ������ (ͨ���� DWORD)��RGB(r,g,b)�����ڴ�����ɫֵ��
    COLORREF shadowColor = RGB(185, 90, 70);      // ������Ӱ����ɫ (ƫ���ĳȺ�ɫ)
    COLORREF pieceColor = RGB(225, 105, 80);     // ���ӵ�����ɫ (���޵ĳȺ�ɫ)
    COLORREF pieceHoverColor = RGB(240, 115, 90); // �����ͣ��������ʱ����ɫ (�����ĳȺ�ɫ)
    COLORREF highlightColor = RGB(255, 220, 200); // ���Ӹ߹����ɫ (�ǳ�ǳ�ĳ�ɫ/��ɫ)
    COLORREF emptyColor = RGB(245, 245, 240);    // �ո�������ɫ (�ǳ�ǳ�Ļ�ɫ/��ɫ)
    COLORREF emptyBorderColor = RGB(230, 220, 200); // �ո�ı߿���ɫ (�����ɫ�������ɫ)
    COLORREF validMoveColor = RGB(100, 200, 120); // ��Ч�ƶ�λ����ʾ����ɫ (��͵���ɫ)
    COLORREF selectedBorderColor = RGB(80, 180, 100); // ѡ��������Ȧ��ʾ����ɫ (�������ɫ)

    // ... ����������̺����ӵĴ��� ...

    // ���㵱ǰѡ�����ӵ�������Ч���߲�����
    std::vector<std::pair<int, int>> validMoves; // ���ڴ洢���ƶ�����Ŀ��λ�� (��, ��)��
    // ֻ������ʾ��ʾ (showHint)�������ӱ�ѡ�� (selX, selY >= 0)����ѡ�е�ȷʵ������ (get(selX, selY) == 1) ʱ�ż��㡣
    if (showHint && selX >= 0 && selY >= 0 && get(selX, selY) == 1) {
        // ��׼�����������ĸ��������Ծ��
        int dx[] = { 2, -2, 0, 0 }; // �з����λ�� (������)
        int dy[] = { 0, 0, 2, -2 }; // �з����λ�� (������)
        for (int d = 0; d < 4; ++d) { // �����ĸ�����
            int nx = selX + dx[d];      // Ŀ��λ�õ�������
            int ny = selY + dy[d];      // Ŀ��λ�õ�������
            int mx2 = selX + dx[d] / 2; // �м䱻�������ӵ�������
            int my2 = selY + dy[d] / 2; // �м䱻�������ӵ�������
            // ����ƶ��Ƿ���Ч��Ŀ��λ��������������Ϊ�գ��м�λ�������������������ӡ�
            if (isValid(nx, ny) && get(nx, ny) == 0 && isValid(mx2, my2) && get(mx2, my2) == 1)
                validMoves.emplace_back(nx, ny); // ����Ч��Ŀ��λ����ӵ��б��С�
        }
        // �������������� (TRIANGLE)�����ܴ��ڶ���ĶԽ�����Ծ����
        // ע�⣺��׼�Ŀ��������ͨ�������������������ı������Ծ���򣬳������������̡�
        // ����ĶԽ��߹���������ض��ڴ���Ŀ�� TRIANGLE ���̵���չ��
        if (type == TRIANGLE) { // �������������
            int tdx[] = { 2, -2, 2, -2 }; // �Խ�����Ծ���з���λ��
            int tdy[] = { 2, -2, -2, 2 }; // �Խ�����Ծ���з���λ�� (ע������ dy �� dx ��Ӧ���γ� (2,2), (-2,-2), (2,-2), (-2,2) �ĸ�����)
            for (int d = 0; d < 4; ++d) { // �����ĸ��Խ��߷���
                int nx = selX + tdx[d];
                int ny = selY + tdy[d];
                int mx2 = selX + tdx[d] / 2;
                int my2 = selY + tdy[d] / 2;
                if (isValid(nx, ny) && get(nx, ny) == 0 && isValid(mx2, my2) && get(mx2, my2) == 1)
                    validMoves.emplace_back(nx, ny);
            }
        }
    }

    // ���������ϵ����и��Ӳ��������ǡ�
    // board.size() �����̵�������
    for (int i = 0; i < board.size(); ++i) {
        // board[i].size() �ǵ� i �е�������
        for (int j = 0; j < board[i].size(); ++j) {
            // ���㵱ǰ�������Ͻǵ���Ļ���ꡣ
            int x = bx + j * cell;
            int y = by + i * cell;
            // �����ǰ��������Ч���� (-1)���򲻻����κζ�����ֱ��������һ�����ӡ�
            if (board[i][j] == -1) continue;

            bool hovered = false; // ��ǵ�ǰ����Ƿ���ͣ�ڴ������ϡ�
            // ��������ͣ�����������Ч (mx,my >=0)����ǰ���������� (board[i][j] == 1)��
            if (mx >= 0 && my >= 0 && board[i][j] == 1) {
                int cx = x + cell / 2; // �������ĵ���Ļ X ����
                int cy = y + cell / 2; // �������ĵ���Ļ Y ����
                int r = pieceRadius;   // ���ӵİ뾶 (ʹ��������ݻ��ʵ�����İ뾶)
                int dist_x = mx - cx;  // ������������ĵ� X �������
                int dist_y = my - cy;  // ������������ĵ� Y �������
                // �����굽�������ĵľ���С�ڵ��ڰ뾶 (���ɶ���)������Ϊ�����ͣ�������ϡ�
                if (dist_x * dist_x + dist_y * dist_y <= r * r) hovered = true;
            }

            // �����ǰ���������� (board[i][j] == 1)
            if (board[i][j] == 1) {
                // 1. ������Ӱ (Ϊ�������)
                setfillcolor(shadowColor); // �������ɫΪ��Ӱ��ɫ
                setlinecolor(shadowColor); // ���ñ߿�ɫΪ��Ӱ��ɫ (ʵ��Բ�ޱ߿�Ч��)
                // ����һ��ʵ��Բ��Ϊ��Ӱ��λ�ñ�����������΢����ƫ�ơ�
                fillcircle(x + cell / 2 + shadowOffset, y + cell / 2 + shadowOffset, pieceRadius);

                // 2. ������������
                COLORREF c = hovered ? pieceHoverColor : pieceColor; // �����Ƿ���ͣѡ��������ɫ��
                setfillcolor(c); // �������ɫ
                setlinecolor(c); // ���ñ߿�ɫ
                fillcircle(x + cell / 2, y + cell / 2, pieceRadius); // ������������Բ��

                // 3. ���Ƹ߹�Ч�� (ģ���Դ���䣬���������)
                if (drawHighlight) { // ���ڸ�����ģʽ�»��Ƹ߹�
                    setfillcolor(highlightColor); // �������ɫΪ�߹���ɫ
                    setlinecolor(highlightColor); // ���ñ߿�ɫΪ�߹���ɫ
                    // �����ӵ����Ͻǻ���һ��Сһ�����ɫԲ��Ϊ�߹⡣
                    fillcircle(x + cell / 2 - pieceRadius / 2.5, y + cell / 2 - pieceRadius / 2.5, pieceRadius / 3.5);
                }


                // 4. ������ӱ�ѡ�У�����������ʾ��ʾ�������ѡ��״̬����Ȧ��
                if (showHint && i == selX && j == selY) {
                    // ����һ���ϴֵİ�ɫ��Ȧ��Ϊ������
                    setlinecolor(WHITE);        // ������ɫΪ��ɫ
                    setlinestyle(PS_SOLID, 4);  // ʵ�ߣ���ϸΪ4
                    circle(x + cell / 2, y + cell / 2, pieceRadius + 3); // �뾶�������Դ�

                    // �ڰ�ɫ��Ȧ�ڻ���һ����ϸ����ɫ��Ȧ��Ϊѡ�б�ǡ�
                    setlinecolor(selectedBorderColor); // ������ɫΪѡ�еı߿���ɫ
                    setlinestyle(PS_SOLID, 2);         // ʵ�ߣ���ϸΪ2
                    circle(x + cell / 2, y + cell / 2, pieceRadius + 3); // ���ɫ��Ȧͬ�뾶
                    setlinestyle(PS_SOLID, 1); // �ָ�Ĭ��������ϸΪ1���Է�Ӱ��������ơ�
                }
            }
            // �����ǰ�����ǿո� (board[i][j] == 0)
            else if (board[i][j] == 0) {
                // 1. ���ƿո�ĵ�����ʽ��
                setfillcolor(emptyColor);       // �������ɫΪ�ո���ɫ
                setlinecolor(emptyBorderColor); // ���ñ߿�ɫΪ�ո�߿���ɫ
                // ����һ���ϴ��Բ��Ϊ��ɫ��
                fillcircle(x + cell / 2, y + cell / 2, pieceRadius + 2); // �뾶�������Դ�һ��
                // �����������ٻ�һ����С��Բ���γɰ��ݸл�߿�Ч����
                // setlinestyle(PS_SOLID, 2); // ����������ϸ
                // circle(x + cell / 2, y + cell / 2, cell / 2 - 10); // ������Ȧ
                // setlinestyle(PS_SOLID, 1); // �ָ�Ĭ��������ϸ

                // 2. ���������ʾ��ʾ�����Ҵ˿ո��ǵ�ǰѡ�����ӵ���Ч����λ�ã��������ʾ��
                if (showHint) {
                    for (auto& mv : validMoves) { // ����������Ч�ƶ�Ŀ��λ��
                        if (mv.first == i && mv.second == j) { // �����ǰ�ո�������֮һ
                            setfillcolor(validMoveColor); // �������ɫΪ��Ч�ƶ���ʾ��ɫ
                            setlinecolor(validMoveColor); // ���ñ߿�ɫ
                            // ����ʾɫ���ո�ʹ��ͻ����ʾ��
                            fillcircle(x + cell / 2, y + cell / 2, pieceRadius); // ��С��������ͬ����С
                            break; // �ҵ��󼴿�����ѭ��
                        }
                    }
                }
            }
        }
    }
}

// ���������߼����� (x, y) �Ƿ������̵���Ч�����ڡ�
// 'const' ��ʾ�˷��������޸��κ� Board ����ĳ�Ա������
// @param x: Ҫ���������ꡣ
// @param y: Ҫ���������ꡣ
// @return bool: ���������Ч (�����̱߽����Ҳ��Ǳ��Ϊ -1 ����Ч��) �򷵻� true�����򷵻� false��
bool Board::isValid(int x, int y) const {
    return x >= 0 && y >= 0 &&                 // �������Ǹ�
        x < board.size() &&                 //���������������������Χ��
        y < board[x].size() &&              // ��������������̶�Ӧ�е�������Χ�� (֧�ַǾ�������)
        board[x][y] != -1;                  // ��λ�õ�ֵ������ -1 (��Ч����)
}

// ��ȡָ���߼����� (x, y) ����λ��ֵ (1:����, 0:�ո�, -1:��Ч)��
// 'const' ��ʾ�˷��������޸��κ� Board ����ĳ�Ա������
// @param x: Ҫ��ȡֵ�������ꡣ
// @param y: Ҫ��ȡֵ�������ꡣ
// @return int: ��Ӧ��λ��ֵ�����������Ч����Ϊδ���壨Ӧ�ȵ��� isValid����
int Board::get(int x, int y) const { return board[x][y]; }

// ����ָ���߼����� (x, y) ����λ��ֵ��
// @param x: Ҫ����ֵ�������ꡣ
// @param y: Ҫ����ֵ�������ꡣ
// @param val: Ҫ���õ���ֵ (1, 0, or -1)��
void Board::set(int x, int y, int val) { board[x][y] = val; }

// ��ȡ��ǰ���̵����͡�
// 'const' ��ʾ�˷��������޸��κ� Board ����ĳ�Ա������
// @return BoardType: ��ǰ���̵� BoardType ö��ֵ��
BoardType Board::getType() const { return type; }

// ��ȡ������ x ����ͨ�����������ϵĴ�С��
// 'const' ��ʾ�˷��������޸��κ� Board ����ĳ�Ա������
// @return int: ���̵�������
int Board::sizeX() const { return board.size(); }

// ��ȡ������ y ����ͨ���ǵ�һ�е�������������������ϵĴ�С��
// ע�⣺���ڷǾ������̣���ͬ�е��������ܲ�ͬ���˺������ص�һ�е�������
// �������Ϊ�գ�board[0] �ᵼ�´���Ӧȷ�� board ��Ϊ�ա�
// 'const' ��ʾ�˷��������޸��κ� Board ����ĳ�Ա������
// @return int: ���̵�һ�е��������������û���У�����Ϊδ���塣
int Board::sizeY() const {
    if (board.empty()) return 0; // �������û���У�����0��
    return board[0].size(); // ���ص�һ�е�����
}

// ��������ʾ��������ѡ��˵������ȴ��û�ѡ��
// @return BoardType: �û�ѡ����������͡�
BoardType boardMenu() {
    // ���ò˵�����ɫΪ����ɫ��
    setbkcolor(RGB(220, 240, 255));
    cleardevice(); // ������Ӧ�ñ���ɫ��

    // ���ñ����ı���ʽ��
    settextstyle(45, 0, _T("΢���ź�")); // �����С45��"΢���ź�"���塣
    settextcolor(RGB(40, 80, 140));   // �ı���ɫ (����ɫ)��
    outtextxy(220, 80, _T("ѡ��������ʽ")); // ��ָ��λ����ʾ���⡣

    // ����ѡ�ť���ı���ʽ��
    settextstyle(32, 0, _T("΢���ź�")); // �����С32��

    // ���������������ѡ�ť�ľ������� (RECT �ṹ���� left, top, right, bottom)��
    // �����˲�������Ӧ����ѡ���ť����
    RECT r1 = { 170,160,630,200 }; // ʮ������ (CROSS)
    RECT r2 = { 170,220,630,260 }; // �������� (DIAMOND)
    RECT r3 = { 170,280,630,320 }; // �������� (TRIANGLE)
    RECT r4 = { 170,340,630,380 }; // �������� (TRAPEZOID) (����)
    RECT r5 = { 170,400,630,440 }; // ���������� (PYRAMID) (����)
    RECT r6 = { 170,460,630,500 }; // �������� (STAR) (����)

    // ���ð�ť�������ɫ (��ǳ������ɫ)��
    setfillcolor(RGB(200, 220, 240));
    // �������а�ť��Բ�Ǿ��α�����
    fillroundrect(r1.left, r1.top, r1.right, r1.bottom, 15, 15); // Բ�ǰ뾶15x15
    fillroundrect(r2.left, r2.top, r2.right, r2.bottom, 15, 15);
    fillroundrect(r3.left, r3.top, r3.right, r3.bottom, 15, 15);
    fillroundrect(r4.left, r4.top, r4.right, r4.bottom, 15, 15);
    fillroundrect(r5.left, r5.top, r5.right, r5.bottom, 15, 15);
    fillroundrect(r6.left, r6.top, r6.right, r6.bottom, 15, 15);

    // ���ð�ť�����ֵ���ɫ (���������ɫ)��
    settextcolor(RGB(20, 40, 80));
    // ��ÿ����ť�����ھ��л��ƶ�Ӧ�������������ơ�
    // DT_CENTER: ˮƽ����, DT_VCENTER: ��ֱ����, DT_SINGLELINE: ������ʾ��
    drawtext(_T("ʮ������"), &r1, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    drawtext(_T("��������"), &r2, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    drawtext(_T("��������"), &r3, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    drawtext(_T("��������"), &r4, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    drawtext(_T("����������"), &r5, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    drawtext(_T("��������"), &r6, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    // ��������ѭ�����ȴ��û�ͨ�������ѡ��һ���������͡�
    while (true) {
        ExMessage m; // ���ڴ洢�����Ϣ�Ľṹ�塣
        // peekmessage ��������Ƿ��������Ϣ (EM_MOUSE)�������������� m ������Ϣ�������Ƴ���
        // ʹ�� while(peekmessage) ��Ϊ�˴�������ۻ��Ķ�������Ϣ����ͨ��һ�ε��ֻ����һ�� WM_LBUTTONDOWN��
        while (peekmessage(&m, EM_MOUSE)) {
            if (m.message == WM_LBUTTONDOWN) { // ��������������µ���Ϣ
                int x = m.x; // ��ȡ���������Ļ X ���ꡣ
                int y = m.y; // ��ȡ���������Ļ Y ���ꡣ
                // ��������λ���Ƿ���ĳ����ť�ľ��������ڡ�
                if (x >= r1.left && x <= r1.right && y >= r1.top && y <= r1.bottom) return CROSS;
                if (x >= r2.left && x <= r2.right && y >= r2.top && y <= r2.bottom) return DIAMOND;
                if (x >= r3.left && x <= r3.right && y >= r3.top && y <= r3.bottom) return TRIANGLE;
                if (x >= r4.left && x <= r4.right && y >= r4.top && y <= r4.bottom) return TRAPEZOID;
                if (x >= r5.left && x <= r5.right && y >= r5.top && y <= r5.bottom) return PYRAMID;
                if (x >= r6.left && x <= r6.right && y >= r6.top && y <= r6.bottom) return STAR;
            }
        }
        Sleep(10); // ��������10���룬�Խ���CPUʹ���ʣ�����æ�ȴ���
    }
}