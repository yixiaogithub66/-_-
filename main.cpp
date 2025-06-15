/*
Peg Solitaire 孔明棋（使用 EasyX 图形库开发）
作者：yixiaogithub66
*/

#include <graphics.h> // 引用 EasyX 图形库，用于图形绘制和用户界面
#include <conio.h>    // 引用 conio.h，用于控制台输入输出，例如 _getch()
#include <string>     // 引用 string，用于字符串操作
#include "board.h"    // 引用自定义的棋盘头文件，定义棋盘相关的功能
#include "game.h"     // 引用自定义的游戏逻辑头文件，定义游戏核心玩法
#include "setting.h"  // 引用自定义的设置头文件，定义游戏设置相关功能
#include "payment.h"  // 引用自定义的充值头文件，定义充值相关功能
#include "save_load.h"  // 新增头文件，用于处理游戏存档和读档功能
#include "how_to_play.h" // 新增：引用操作说明头文件

// 定义游戏的不同状态，方便进行状态管理和切换
enum State {
    MENU,         // 主菜单状态
    BOARD_SELECT, // 选择棋盘类型状态
    GAME,         // 游戏进行中状态
    SETTINGS,     // 设置状态
    PAYMENT,      // 充值状态
    SAVE_LOAD,    // 存档/读档状态
    HOW_TO_PLAY,  // 新增：操作说明状态
    EXIT          // 退出游戏状态
};

// 函数声明：绘制主菜单界面，显示各个选项和当前余额
void drawMainMenu(int balance);
// 函数声明：处理主菜单的逻辑，根据用户点击返回相应的游戏状态
State mainMenu(int balance);
// void showHowToPlayScreen(); // 移除这里的声明，因为它已经在 how_to_play.h 中


int main() {
    // 初始化图形环境，窗口大小为 800x650 像素
    initgraph(800, 650);
    // 设置背景颜色为白色
    setbkcolor(WHITE);
    // 清空绘图设备，应用背景色
    cleardevice();

    int balance = 0;            // 初始化玩家余额为0
    Setting setting;            // 创建一个 Setting 对象，用于管理游戏设置
    BoardType boardType = CROSS; // 初始化默认棋盘类型为十字形

    State state = MENU; // 初始化当前游戏状态为主菜单
    // 主游戏循环，只要状态不是 EXIT，就一直执行
    while (state != EXIT) {
        // 使用 switch 语句根据当前状态执行相应的操作
        switch (state) {
        case MENU:
            // 如果当前是主菜单状态，则调用 mainMenu 函数处理，并更新状态
            state = mainMenu(balance);
            break;
        case BOARD_SELECT: {
            // 如果当前是选择棋盘类型状态
            boardType = boardMenu(); // 调用 boardMenu 函数让用户选择棋盘类型
            state = GAME;            // 选择完毕后，将状态切换到游戏进行中
            break;
        }
        case GAME: {
            // 如果当前是游戏进行中状态
            Board board(boardType);       // 根据选择的棋盘类型创建棋盘对象
            Game(board, setting).run(); // 创建游戏对象并运行游戏逻辑
            state = MENU;               // 游戏结束后，返回主菜单状态
            break;
        }
        case SETTINGS:
            // 如果当前是设置状态
            setting.menu(); // 调用 setting 对象的 menu 方法，处理设置选项
            state = MENU;   // 设置完成后，返回主菜单状态
            break;
        case PAYMENT:
            // 如果当前是充值状态
            balance = paymentMenu(balance); // 调用 paymentMenu 函数处理充值，并更新余额
            state = MENU;                  // 充值完成后，返回主菜单状态
            break;
        case SAVE_LOAD: { // 处理存档/读档状态
            BoardType oldType = boardType;  // 保存进入存档/读档界面前的棋盘类型
            boardType = saveLoadMenu(boardType); // 调用 saveLoadMenu 函数处理存档或读档，并可能更新棋盘类型
            if (boardType != oldType) {
                // 如果棋盘类型发生了变化，说明可能加载了存档
                // 这里可以添加一些日志记录或者给用户的提示信息
            }
            state = MENU; // 无论是存档还是读档操作完成，都返回主菜单状态
            break;
        }
        case HOW_TO_PLAY: // 新增：处理操作说明状态
            showHowToPlayScreen(); // 调用在 how_to_play.cpp 中定义的函数
            state = MENU; // 查看完操作说明后返回主菜单
            break;
        }
    }
    // 关闭图形环境，释放资源
    closegraph();
    // 程序正常退出
    return 0;
}

// 绘制主菜单界面
void drawMainMenu(int balance) {
    // 设置背景颜色为淡蓝色 (RGB(236, 246, 255))
    setbkcolor(RGB(236, 246, 255));
    // 清空屏幕，应用新的背景色
    cleardevice();
    // 设置文本样式：字体大小60，字宽0 (自适应)，字体名称 "微软雅黑"
    settextstyle(60, 0, _T("微软雅黑"));
    // 设置背景混合模式为透明，这样文字背景不会覆盖下面的内容
    setbkmode(TRANSPARENT);
    // 设置文本颜色为深蓝色 (RGB(50, 50, 120))
    settextcolor(RGB(50, 50, 120));
    // 在指定位置 (210, 40) 输出游戏标题 "Peg Solitaire" (向上移动了一点以腾出空间)
    outtextxy(210, 40, _T("Peg Solitaire"));
    // 设置后续按钮的文本样式：字体大小32，字宽0 (自适应)，字体名称 "微软雅黑" (稍微调小了字体以适应更多按钮)
    settextstyle(32, 0, _T("微软雅黑"));

    // 定义各个按钮的矩形区域位置和大小 (调整了Y坐标和间距)
    int button_width = 200; // 稍微加宽按钮
    int button_height = 45;
    int button_x = (800 - button_width) / 2; // 按钮居中
    int button_start_y = 110; // 调整起始Y，为标题留出更多空间
    int button_spacing = 12; // 按钮之间的垂直间距

    RECT r_start_game = { button_x, button_start_y, button_x + button_width, button_start_y + button_height };
    RECT r_how_to_play = { button_x, r_start_game.bottom + button_spacing, button_x + button_width, r_start_game.bottom + button_spacing + button_height };
    RECT r_settings = { button_x, r_how_to_play.bottom + button_spacing, button_x + button_width, r_how_to_play.bottom + button_spacing + button_height };
    RECT r_payment = { button_x, r_settings.bottom + button_spacing, button_x + button_width, r_settings.bottom + button_spacing + button_height };
    RECT r_save_load = { button_x, r_payment.bottom + button_spacing, button_x + button_width, r_payment.bottom + button_spacing + button_height };
    RECT r_exit = { button_x, r_save_load.bottom + button_spacing, button_x + button_width, r_save_load.bottom + button_spacing + button_height };


    // 设置填充颜色为淡紫色 (RGB(180, 200, 250))，用于绘制按钮背景
    setfillcolor(RGB(180, 200, 250));
    // 绘制圆角矩形作为按钮背景
    fillroundrect(r_start_game.left, r_start_game.top, r_start_game.right, r_start_game.bottom, 15, 15);
    fillroundrect(r_how_to_play.left, r_how_to_play.top, r_how_to_play.right, r_how_to_play.bottom, 15, 15);
    fillroundrect(r_settings.left, r_settings.top, r_settings.right, r_settings.bottom, 15, 15);
    fillroundrect(r_payment.left, r_payment.top, r_payment.right, r_payment.bottom, 15, 15);
    fillroundrect(r_save_load.left, r_save_load.top, r_save_load.right, r_save_load.bottom, 15, 15);
    fillroundrect(r_exit.left, r_exit.top, r_exit.right, r_exit.bottom, 15, 15);

    // 设置按钮上文字的颜色为较深的蓝色 (RGB(40, 40, 90))
    settextcolor(RGB(40, 40, 90));
    // 在按钮矩形区域内绘制文字
    drawtext(_T("开始游戏"), &r_start_game, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    drawtext(_T("操作说明"), &r_how_to_play, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    drawtext(_T("设置"), &r_settings, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    drawtext(_T("充值"), &r_payment, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    drawtext(_T("读取存档"), &r_save_load, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    drawtext(_T("退出"), &r_exit, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    // 准备用于显示余额的字符串缓冲区
    TCHAR balstr[32];
    _stprintf_s(balstr, _T("当前余额：%d 元"), balance);
    settextstyle(24, 0, _T("微软雅黑"));
    settextcolor(RGB(80, 130, 80));
    outtextxy(10, 650 - 30, balstr); // 余额显示在左下角
}

// 处理主菜单的逻辑
State mainMenu(int balance) {
    drawMainMenu(balance);
    ExMessage m;
    while (true) {
        // 使用 getmessage 获取阻塞消息，减少CPU占用
        m = getmessage(EX_MOUSE); // 只关心鼠标消息以简化

        if (m.message == WM_LBUTTONDOWN) {
            int x = m.x;
            int y = m.y;

            // 从 drawMainMenu 中获取按钮的尺寸和位置信息
            int button_width = 200;
            int button_x = (800 - button_width) / 2;
            int button_start_y = 110;
            int button_height = 45;
            int button_spacing = 12;

            // “开始游戏”按钮
            if (x >= button_x && x <= button_x + button_width && y >= button_start_y && y <= button_start_y + button_height) return BOARD_SELECT;

            // “操作说明”按钮
            int how_to_play_y_start = button_start_y + button_height + button_spacing;
            if (x >= button_x && x <= button_x + button_width && y >= how_to_play_y_start && y <= how_to_play_y_start + button_height) return HOW_TO_PLAY;

            // “设置”按钮
            int settings_y_start = how_to_play_y_start + button_height + button_spacing;
            if (x >= button_x && x <= button_x + button_width && y >= settings_y_start && y <= settings_y_start + button_height) return SETTINGS;

            // “充值”按钮
            int payment_y_start = settings_y_start + button_height + button_spacing;
            if (x >= button_x && x <= button_x + button_width && y >= payment_y_start && y <= payment_y_start + button_height) return PAYMENT;

            // “读取存档”按钮
            int save_load_y_start = payment_y_start + button_height + button_spacing;
            if (x >= button_x && x <= button_x + button_width && y >= save_load_y_start && y <= save_load_y_start + button_height) return SAVE_LOAD;

            // “退出”按钮
            int exit_y_start = save_load_y_start + button_height + button_spacing;
            if (x >= button_x && x <= button_x + button_width && y >= exit_y_start && y <= exit_y_start + button_height) return EXIT;
        }
        // Sleep(10); // 如果使用 peekmessage，则需要 Sleep
    }
}

// 移除 showHowToPlayScreen 函数定义，它现在位于 how_to_play.cpp