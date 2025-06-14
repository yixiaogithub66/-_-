/*
Peg Solitaire �����壨ʹ�� EasyX ͼ�ο⿪����
���ߣ�yixiaogithub66
*/

#include <graphics.h>
#include <conio.h>
#include <string>
#include "board.h"
#include "game.h"
#include "setting.h"
#include "payment.h"
#include "save_load.h"  // ����ͷ�ļ�

enum State { MENU, BOARD_SELECT, GAME, SETTINGS, PAYMENT, SAVE_LOAD, EXIT };  // ���SAVE_LOAD״̬

void drawMainMenu(int balance);
State mainMenu(int balance);

int main() {
    initgraph(800, 650);
    setbkcolor(WHITE);
    cleardevice();

    int balance = 0;
    Setting setting;
    BoardType boardType = CROSS;

    State state = MENU;
    while (state != EXIT) {
        switch (state) {
        case MENU:
            state = mainMenu(balance);
            break;
        case BOARD_SELECT: {
            boardType = boardMenu();
            state = GAME;
            break;
        }
        case GAME: {
            Board board(boardType);
            Game(board, setting).run();
            state = MENU;
            break;
        }
        case SETTINGS:
            setting.menu();
            state = MENU;
            break;
        case PAYMENT:
            balance = paymentMenu(balance);
            state = MENU;
            break;
        case SAVE_LOAD:  // ����浵
            BoardType oldType = boardType;  // ����ԭʼ����
            boardType = saveLoadMenu(boardType);
            if (boardType != oldType) {
                // ���ͱ���������Ǽ����˴浵
                // ������������־����ʾ
            }
            state = MENU;
            break;
        }
    }
    closegraph();
    return 0;
}

void drawMainMenu(int balance) {
    setbkcolor(RGB(236, 246, 255));
    cleardevice();
    settextstyle(60, 0, _T("΢���ź�"));
    setbkmode(TRANSPARENT);
    settextcolor(RGB(50, 50, 120));
    outtextxy(210, 60, _T("Peg Solitaire"));
    settextstyle(38, 0, _T("΢���ź�"));
    // ������ťλ�ã���Ӵ浵��ť
    RECT r1 = { 310,160,490,210 };  // ��ʼ��Ϸ
    RECT r2 = { 310,230,490,280 };  // ����
    RECT r3 = { 310,300,490,350 };  // ��ֵ
    RECT r4 = { 310,370,490,420 };  // ��ȡ�浵(�޸�����)
    RECT r5 = { 310,440,490,490 };  // �˳�

    setfillcolor(RGB(180, 200, 250));
    fillroundrect(r1.left, r1.top, r1.right, r1.bottom, 15, 15);
    fillroundrect(r2.left, r2.top, r2.right, r2.bottom, 15, 15);
    fillroundrect(r3.left, r3.top, r3.right, r3.bottom, 15, 15);
    fillroundrect(r4.left, r4.top, r4.right, r4.bottom, 15, 15);
    fillroundrect(r5.left, r5.top, r5.right, r5.bottom, 15, 15);

    settextcolor(RGB(40, 40, 90));
    drawtext(_T("��ʼ��Ϸ"), &r1, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    drawtext(_T("����"), &r2, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    drawtext(_T("��ֵ"), &r3, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    drawtext(_T("��ȡ�浵"), &r4, DT_CENTER | DT_VCENTER | DT_SINGLELINE); // �޸��ı�
    drawtext(_T("�˳�"), &r5, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    TCHAR balstr[32];
    _stprintf_s(balstr, _T("��ǰ��%d Ԫ"), balance);
    settextstyle(24, 0, _T("΢���ź�"));
    settextcolor(RGB(80, 130, 80));
    outtextxy(10, 600, balstr);
}

State mainMenu(int balance) {
    drawMainMenu(balance);
    while (true) {
        ExMessage m;
        while (peekmessage(&m, EM_MOUSE)) {
            if (m.message == WM_LBUTTONDOWN) {
                int x = m.x, y = m.y;
                if (x >= 310 && x <= 490 && y >= 160 && y <= 210) return BOARD_SELECT;  // ��ʼ��Ϸ
                if (x >= 310 && x <= 490 && y >= 230 && y <= 280) return SETTINGS;      // ����
                if (x >= 310 && x <= 490 && y >= 300 && y <= 350) return PAYMENT;       // ��ֵ
                if (x >= 310 && x <= 490 && y >= 370 && y <= 420) return SAVE_LOAD;     // �浵(����)
                if (x >= 310 && x <= 490 && y >= 440 && y <= 490) return EXIT;          // �˳�
            }
        }
        Sleep(10);
    }
}