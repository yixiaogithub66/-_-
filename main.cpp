/*
Peg Solitaire 单人跳棋 EasyX 图形版主程序（修正版）
作者：yixiaogithub66
*/

#include <graphics.h>
#include <conio.h>
#include <string>
#include "board.h"
#include "game.h"
#include "setting.h"
#include "payment.h"

// 菜单状态
enum State { MENU, BOARD_SELECT, GAME, SETTINGS, PAYMENT, EXIT };

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
        default: state = EXIT;
        }
    }
    closegraph();
    return 0;
}

void drawMainMenu(int balance) {
    setbkcolor(RGB(236, 246, 255));
    cleardevice();
    settextstyle(60, 0, _T("微软雅黑"));
    setbkmode(TRANSPARENT);
    settextcolor(RGB(50, 50, 120));
    outtextxy(210, 60, _T("Peg Solitaire"));
    settextstyle(38, 0, _T("微软雅黑"));
    RECT r1 = { 310,180,490,230 }, r2 = { 310,250,490,300 }, r3 = { 310,320,490,370 }, r4 = { 310,390,490,440 };
    setfillcolor(RGB(180, 200, 250));
    fillroundrect(r1.left, r1.top, r1.right, r1.bottom, 15, 15);
    fillroundrect(r2.left, r2.top, r2.right, r2.bottom, 15, 15);
    fillroundrect(r3.left, r3.top, r3.right, r3.bottom, 15, 15);
    fillroundrect(r4.left, r4.top, r4.right, r4.bottom, 15, 15);
    settextcolor(RGB(40, 40, 90));
    drawtext(_T("开始游戏"), &r1, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    drawtext(_T("设置"), &r2, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    drawtext(_T("充值"), &r3, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    drawtext(_T("退出"), &r4, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    TCHAR balstr[32];
    _stprintf_s(balstr, _T("当前余额：%d 元"), balance);
    settextstyle(24, 0, _T("微软雅黑"));
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
                if (x >= 310 && x <= 490 && y >= 180 && y <= 230) return BOARD_SELECT;
                if (x >= 310 && x <= 490 && y >= 250 && y <= 300) return SETTINGS;
                if (x >= 310 && x <= 490 && y >= 320 && y <= 370) return PAYMENT;
                if (x >= 310 && x <= 490 && y >= 390 && y <= 440) return EXIT;
            }
        }
        Sleep(10);
    }
}