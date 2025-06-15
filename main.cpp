/*
Peg Solitaire 孔明棋（使用 EasyX 图形库开发）
作者：yixiaogithub66
*/

#include <graphics.h>
#include <conio.h>
#include <string>
#include "board.h"
#include "game.h"
#include "setting.h"
#include "payment.h"
#include "save_load.h"  // 新增头文件

enum State { MENU, BOARD_SELECT, GAME, SETTINGS, PAYMENT, SAVE_LOAD, EXIT };  // 添加SAVE_LOAD状态

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
        case SAVE_LOAD:  // 处理存档
            BoardType oldType = boardType;  // 保存原始类型
            boardType = saveLoadMenu(boardType);
            if (boardType != oldType) {
                // 类型变更，可能是加载了存档
                // 这里可以添加日志或提示
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
    settextstyle(60, 0, _T("微软雅黑"));
    setbkmode(TRANSPARENT);
    settextcolor(RGB(50, 50, 120));
    outtextxy(210, 60, _T("Peg Solitaire"));
    settextstyle(38, 0, _T("微软雅黑"));
    // 调整按钮位置，添加存档按钮
    RECT r1 = { 310,160,490,210 };  // 开始游戏
    RECT r2 = { 310,230,490,280 };  // 设置
    RECT r3 = { 310,300,490,350 };  // 充值
    RECT r4 = { 310,370,490,420 };  // 读取存档(修改名称)
    RECT r5 = { 310,440,490,490 };  // 退出

    setfillcolor(RGB(180, 200, 250));
    fillroundrect(r1.left, r1.top, r1.right, r1.bottom, 15, 15);
    fillroundrect(r2.left, r2.top, r2.right, r2.bottom, 15, 15);
    fillroundrect(r3.left, r3.top, r3.right, r3.bottom, 15, 15);
    fillroundrect(r4.left, r4.top, r4.right, r4.bottom, 15, 15);
    fillroundrect(r5.left, r5.top, r5.right, r5.bottom, 15, 15);

    settextcolor(RGB(40, 40, 90));
    drawtext(_T("开始游戏"), &r1, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    drawtext(_T("设置"), &r2, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    drawtext(_T("充值"), &r3, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    drawtext(_T("读取存档"), &r4, DT_CENTER | DT_VCENTER | DT_SINGLELINE); // 修改文本
    drawtext(_T("退出"), &r5, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

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
                if (x >= 310 && x <= 490 && y >= 160 && y <= 210) return BOARD_SELECT;  // 开始游戏
                if (x >= 310 && x <= 490 && y >= 230 && y <= 280) return SETTINGS;      // 设置
                if (x >= 310 && x <= 490 && y >= 300 && y <= 350) return PAYMENT;       // 充值
                if (x >= 310 && x <= 490 && y >= 370 && y <= 420) return SAVE_LOAD;     // 存档(新增)
                if (x >= 310 && x <= 490 && y >= 440 && y <= 490) return EXIT;          // 退出
            }
        }
        Sleep(10);
    }
}