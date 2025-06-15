#include "setting.h"   // 引用自定义的设置类头文件 (setting.h)，其中包含 Setting 结构体的声明。
#include <graphics.h>  // 引用 EasyX 图形库的头文件，用于创建和管理设置菜单的图形用户界面。
#include <tchar.h>     // 引用通用字符类型头文件，用于支持 _T() 宏和 TCHAR 类型，以实现 ANSI 和 Unicode 字符集的兼容性。

// Setting 结构体的成员函数 menu()，用于显示和处理游戏设置界面。
// 当调用此函数时，会创建一个交互式菜单，允许用户修改 Setting 结构体中的各项设置。
void Setting::menu() {
    // 设置菜单界面的背景颜色。
    setbkcolor(RGB(236, 246, 255)); // 设置为一种淡蓝色。
    cleardevice(); // 清空当前绘图设备，并用新设置的背景色填充整个窗口。

    // 绘制菜单标题 "设置"。
    settextstyle(40, 0, _T("微软雅黑")); // 设置文本样式：字体大小40，字宽0 (自适应)，字体 "微软雅黑"。
    settextcolor(RGB(50, 50, 120));   // 设置文本颜色：深蓝色。
    outtextxy(320, 70, _T("设置"));     // 在屏幕坐标 (320, 70) 处输出标题。

    // 设置选项按钮的通用文本样式。
    settextstyle(28, 0, _T("微软雅黑")); // 字体大小28。
    settextcolor(RGB(40, 40, 90));   // 文本颜色：较深的蓝色。

    // --- 画质设置按钮 ---
    RECT rQuality = { 250, 160, 550, 210 }; // 定义画质设置按钮的矩形区域 (left, top, right, bottom)。
    setfillcolor(RGB(180, 200, 250));      // 设置按钮的填充颜色 (浅蓝色)。
    // 绘制圆角矩形作为按钮背景。15, 15 是圆角椭圆的宽度和高度。
    fillroundrect(rQuality.left, rQuality.top, rQuality.right, rQuality.bottom, 15, 15);
    // 根据当前 `quality` 成员变量的值，在按钮上显示相应的文本。
    // DT_CENTER: 水平居中, DT_VCENTER: 垂直居中, DT_SINGLELINE: 单行显示。
    drawtext(quality == "高" ? _T("画质：高（点击切换）") : _T("画质：低（点击切换）"),
        &rQuality, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    // --- 帧率设置按钮 ---
    RECT rFps = { 250, 230, 550, 280 }; // 定义帧率设置按钮的矩形区域。
    setfillcolor(RGB(180, 200, 250));  // 设置按钮填充颜色。
    fillroundrect(rFps.left, rFps.top, rFps.right, rFps.bottom, 15, 15); // 绘制按钮背景。
    TCHAR fpsStr[32]; // 创建一个 TCHAR 数组作为字符串缓冲区，用于格式化帧率显示。
    // 使用 _stprintf_s 安全地将当前帧率 `fps` 格式化到字符串中。
    _stprintf_s(fpsStr, _T("帧率：%d（点击切换）"), fps);
    drawtext(fpsStr, &rFps, DT_CENTER | DT_VCENTER | DT_SINGLELINE); // 在按钮上显示帧率文本。

    // --- 操作模式设置按钮 ---
    RECT rCtrl = { 250, 300, 550, 350 }; // 定义操作模式按钮的矩形区域。
    setfillcolor(RGB(180, 200, 250));   // 设置按钮填充颜色。
    fillroundrect(rCtrl.left, rCtrl.top, rCtrl.right, rCtrl.bottom, 15, 15); // 绘制按钮背景。
    // 根据当前 `controlMode` 成员变量的值，显示相应的操作模式文本。
    drawtext(controlMode == CONTROL_MOUSE ? _T("操作模式：鼠标（点击切换）") : _T("操作模式：键盘（点击切换）"),
        &rCtrl, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    // --- AI提示设置按钮 ---
    RECT rAI = { 250, 370, 550, 420 }; // 定义AI提示设置按钮的矩形区域。
    setfillcolor(RGB(180, 200, 250)); // 设置按钮填充颜色。
    fillroundrect(rAI.left, rAI.top, rAI.right, rAI.bottom, 15, 15); // 绘制按钮背景。
    // 根据当前 `enableAI` 成员变量的值，显示AI提示是开启还是关闭。
    drawtext(enableAI ? _T("AI提示：开启（点击切换）") : _T("AI提示：关闭（点击切换）"),
        &rAI, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    // --- 返回按钮 ---
    RECT rBack = { 320, 470, 480, 520 };  // 定义返回按钮的矩形区域 (调整了位置)。
    setfillcolor(RGB(180, 200, 240));    // 设置返回按钮的填充颜色 (略有不同的浅蓝色)。
    fillroundrect(rBack.left, rBack.top, rBack.right, rBack.bottom, 12, 12); // 绘制按钮背景，圆角略小。
    settextcolor(RGB(20, 20, 80));       // 设置返回按钮上的文本颜色 (深蓝黑色)。
    settextstyle(28, 0, _T("微软雅黑")); // 确保文本样式与选项按钮一致或按需调整。
    drawtext(_T("返回"), &rBack, DT_CENTER | DT_VCENTER | DT_SINGLELINE); // 在按钮上绘制 "返回" 文本。

    // 进入主事件循环，处理用户输入 (主要是鼠标点击)。
    while (true) {
        ExMessage m; // EasyX 消息结构体，用于存储鼠标或键盘事件。
        // 使用 peekmessage 检查鼠标消息 (EM_MOUSE)，这是一个非阻塞函数。
        // 如果队列中有鼠标消息，则处理它。
        while (peekmessage(&m, EM_MOUSE)) {
            if (m.message == WM_LBUTTONDOWN) { // 如果是鼠标左键按下事件。
                int x = m.x; // 获取鼠标点击的屏幕 X 坐标。
                int y = m.y; // 获取鼠标点击的屏幕 Y 坐标。

                // 检查是否点击了“画质”按钮。
                if (x >= rQuality.left && x <= rQuality.right && y >= rQuality.top && y <= rQuality.bottom) {
                    // 切换画质设置：如果当前是 "高"，则变为 "低"；反之亦然。
                    quality = (quality == "高") ? "低" : "高";
                    // 重绘画质按钮以反映新的设置。
                    setfillcolor(RGB(180, 200, 250)); // 按钮背景色
                    fillroundrect(rQuality.left, rQuality.top, rQuality.right, rQuality.bottom, 15, 15); // 重绘背景
                    settextcolor(RGB(40, 40, 90)); // 确保文本颜色正确
                    settextstyle(28, 0, _T("微软雅黑")); // 确保文本样式正确
                    drawtext(quality == "高" ? _T("画质：高（点击切换）") : _T("画质：低（点击切换）"),
                        &rQuality, DT_CENTER | DT_VCENTER | DT_SINGLELINE); // 重绘文本
                }
                // 检查是否点击了“帧率”按钮。
                if (x >= rFps.left && x <= rFps.right && y >= rFps.top && y <= rFps.bottom) {
                    // 切换帧率设置：60 -> 30 -> 120 -> 60 (循环)。
                    if (fps == 60) fps = 30;
                    else if (fps == 30) fps = 120;
                    else fps = 60;
                    // 重绘帧率按钮。
                    setfillcolor(RGB(180, 200, 250));
                    fillroundrect(rFps.left, rFps.top, rFps.right, rFps.bottom, 15, 15);
                    settextcolor(RGB(40, 40, 90));
                    settextstyle(28, 0, _T("微软雅黑"));
                    _stprintf_s(fpsStr, _T("帧率：%d（点击切换）"), fps); // 更新帧率字符串。
                    drawtext(fpsStr, &rFps, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                }
                // 检查是否点击了“操作模式”按钮。
                if (x >= rCtrl.left && x <= rCtrl.right && y >= rCtrl.top && y <= rCtrl.bottom) {
                    // 切换操作模式：鼠标 <-> 键盘。
                    controlMode = (controlMode == CONTROL_MOUSE) ? CONTROL_KEYBOARD : CONTROL_MOUSE;
                    // 重绘操作模式按钮。
                    setfillcolor(RGB(180, 200, 250));
                    fillroundrect(rCtrl.left, rCtrl.top, rCtrl.right, rCtrl.bottom, 15, 15);
                    settextcolor(RGB(40, 40, 90));
                    settextstyle(28, 0, _T("微软雅黑"));
                    drawtext(controlMode == CONTROL_MOUSE ? _T("操作模式：鼠标（点击切换）") : _T("操作模式：键盘（点击切换）"),
                        &rCtrl, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                }
                // 检查是否点击了“AI提示”按钮。
                if (x >= rAI.left && x <= rAI.right && y >= rAI.top && y <= rAI.bottom) {
                    // 切换 AI 提示的启用状态。
                    enableAI = !enableAI;
                    // 重绘 AI 提示按钮。
                    setfillcolor(RGB(180, 200, 250));
                    fillroundrect(rAI.left, rAI.top, rAI.right, rAI.bottom, 15, 15);
                    settextcolor(RGB(40, 40, 90));
                    settextstyle(28, 0, _T("微软雅黑"));
                    drawtext(enableAI ? _T("AI提示：开启（点击切换）") : _T("AI提示：关闭（点击切换）"),
                        &rAI, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                }
                // 检查是否点击了“返回”按钮。
                if (x >= rBack.left && x <= rBack.right && y >= rBack.top && y <= rBack.bottom) {
                    return; // 如果点击返回按钮，则退出 menu() 函数，设置将被保存 (因为它们是 Setting 对象的成员)。
                }
            }
        }
        Sleep(10); // 短暂休眠10毫秒，以降低CPU在此循环中的使用率，避免程序在没有输入时空转。
    }
}