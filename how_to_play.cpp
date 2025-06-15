#include "how_to_play.h" // 引用对应的头文件
#include <graphics.h>    // 引用 EasyX 图形库
#include <tchar.h>       // 引用通用字符类型头文件 (TCHAR, _T() 等)
#include <conio.h>       // _getch() 等 (如果需要按任意键退出)

// 显示操作说明界面
void showHowToPlayScreen() {
    // 设置背景颜色
    setbkcolor(RGB(245, 245, 245)); // 使用一个柔和的背景色
    cleardevice();

    // 绘制标题
    settextstyle(40, 0, _T("微软雅黑"));
    settextcolor(RGB(30, 30, 100)); // 深蓝色标题
    outtextxy(280, 40, _T("操作说明"));

    // 设置说明文本的样式
    settextstyle(20, 0, _T("宋体"));
    settextcolor(BLACK);
    setbkmode(TRANSPARENT); // 确保文本背景透明

    int text_x = 50;
    int text_y_start = 100;
    int line_height = 28; // 增加行高以便阅读

    // 说明内容
    outtextxy(text_x, text_y_start, _T("【游戏目标】"));
    outtextxy(text_x + 20, text_y_start + line_height, _T("通过棋子之间的跳跃，消除棋子，最终目标是使棋盘上只剩下"));
    outtextxy(text_x + 20, text_y_start + line_height * 2, _T("一个棋子，并且该棋子最好位于棋盘的中心位置（经典玩法）。"));

    outtextxy(text_x, text_y_start + line_height * 4, _T("【基本规则】"));
    outtextxy(text_x + 20, text_y_start + line_height * 5, _T("1. 棋子只能跳过相邻的棋子，落到该棋子后方的空位上。"));
    outtextxy(text_x + 20, text_y_start + line_height * 6, _T("2. 跳跃方向可以是水平或垂直。部分特殊棋盘（如星形）支持斜向跳跃。"));
    outtextxy(text_x + 20, text_y_start + line_height * 7, _T("3. 被跳过的棋子将从棋盘上移除。"));
    outtextxy(text_x + 20, text_y_start + line_height * 8, _T("4. 棋子不能斜向跳跃（除非棋盘规则特殊说明）。"));
    outtextxy(text_x + 20, text_y_start + line_height * 9, _T("5. 不能跳到有棋子的位置，也不能跳过两个或更多棋子。"));


    outtextxy(text_x, text_y_start + line_height * 11, _T("【鼠标操作】"));
    outtextxy(text_x + 20, text_y_start + line_height * 12, _T("- 点击一个棋子：选中该棋子（棋子会高亮）。"));
    outtextxy(text_x + 20, text_y_start + line_height * 13, _T("- 再次点击已选中的棋子：取消选中。"));
    outtextxy(text_x + 20, text_y_start + line_height * 14, _T("- 选中棋子后，点击一个符合规则的空位：完成跳跃。"));

    outtextxy(text_x, text_y_start + line_height * 16, _T("【键盘快捷键】"));
    outtextxy(text_x + 20, text_y_start + line_height * 17, _T("- H 键：显示/隐藏 AI 提供的最佳走法提示。"));
    outtextxy(text_x + 20, text_y_start + line_height * 18, _T("- A 键：如果 AI 提示已显示，则自动执行该提示的走法。"));
    outtextxy(text_x + 20, text_y_start + line_height * 19, _T("- Ctrl + Z：撤销上一步操作。"));
    outtextxy(text_x + 20, text_y_start + line_height * 20, _T("- Ctrl + R：将当前棋盘恢复到初始布局。"));
    outtextxy(text_x + 20, text_y_start + line_height * 21, _T("- ESC 键：在游戏界面中，返回主菜单。"));


    // 绘制返回按钮
    settextstyle(24, 0, _T("微软雅黑"));
    RECT r_back = { 300, 650 - 70, 500, 650 - 30 }; // 按钮位置调整到底部
    setfillcolor(RGB(200, 220, 240)); // 淡蓝色按钮
    fillroundrect(r_back.left, r_back.top, r_back.right, r_back.bottom, 10, 10);
    settextcolor(RGB(30, 30, 80)); // 深蓝色文字
    drawtext(_T("返回主菜单"), &r_back, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    FlushBatchDraw(); // 确保所有内容都绘制到屏幕上

    // 等待用户操作返回
    ExMessage msg;
    while (true) {
        // 获取消息，如果没有消息则阻塞
        msg = getmessage(EX_MOUSE | EX_KEY); // 使用 getmessage 替代 peekmessage + Sleep

        if (msg.message == WM_LBUTTONDOWN) { // 鼠标左键按下
            if (msg.x >= r_back.left && msg.x <= r_back.right && msg.y >= r_back.top && msg.y <= r_back.bottom) {
                break; // 点击了返回按钮
            }
        }
        else if (msg.message == WM_KEYDOWN) { // 任意键盘按下
            if (msg.vkcode == VK_ESCAPE) { // 如果是 ESC 键，也返回
                break;
            }
            // 如果需要其他按键也触发展开，可以在这里添加判断
            // break; // 例如，任意键按下都返回
        }
    }
}