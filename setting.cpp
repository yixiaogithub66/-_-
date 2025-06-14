#include "setting.h"
#include <graphics.h>
#include <tchar.h>

void Setting::menu() {
    setbkcolor(RGB(236, 246, 255));
    cleardevice();
    settextstyle(40, 0, _T("微软雅黑"));
    settextcolor(RGB(50, 50, 120));
    outtextxy(320, 70, _T("设置"));

    settextstyle(28, 0, _T("微软雅黑"));
    settextcolor(RGB(40, 40, 90));
    RECT rQuality = { 250, 160, 550, 210 };
    setfillcolor(RGB(180, 200, 250));
    fillroundrect(rQuality.left, rQuality.top, rQuality.right, rQuality.bottom, 15, 15);
    drawtext(quality == "高" ? _T("画质：高（点击切换）") : _T("画质：低（点击切换）"),
        &rQuality, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    RECT rFps = { 250, 230, 550, 280 };
    setfillcolor(RGB(180, 200, 250));
    fillroundrect(rFps.left, rFps.top, rFps.right, rFps.bottom, 15, 15);
    TCHAR fpsStr[32];
    _stprintf_s(fpsStr, _T("帧率：%d（点击切换）"), fps);
    drawtext(fpsStr, &rFps, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    RECT rCtrl = { 250, 300, 550, 350 };
    setfillcolor(RGB(180, 200, 250));
    fillroundrect(rCtrl.left, rCtrl.top, rCtrl.right, rCtrl.bottom, 15, 15);
    drawtext(controlMode == CONTROL_MOUSE ? _T("操作模式：鼠标（点击切换）") : _T("操作模式：键盘（点击切换）"),
        &rCtrl, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    // 添加AI设置选项
    RECT rAI = { 250, 370, 550, 420 };
    setfillcolor(RGB(180, 200, 250));
    fillroundrect(rAI.left, rAI.top, rAI.right, rAI.bottom, 15, 15);
    drawtext(enableAI ? _T("AI提示：开启（点击切换）") : _T("AI提示：关闭（点击切换）"),
        &rAI, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    RECT rBack = { 320, 470, 480, 520 };  // 调整返回按钮位置
    setfillcolor(RGB(180, 200, 240));
    fillroundrect(rBack.left, rBack.top, rBack.right, rBack.bottom, 12, 12);
    settextcolor(RGB(20, 20, 80));
    settextstyle(28, 0, _T("微软雅黑"));
    drawtext(_T("返回"), &rBack, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    while (true) {
        ExMessage m;
        while (peekmessage(&m, EM_MOUSE)) {
            if (m.message == WM_LBUTTONDOWN) {
                int x = m.x, y = m.y;
                // 切换画质
                if (x >= rQuality.left && x <= rQuality.right && y >= rQuality.top && y <= rQuality.bottom) {
                    quality = (quality == "高") ? "低" : "高";
                    setfillcolor(RGB(180, 200, 250));
                    fillroundrect(rQuality.left, rQuality.top, rQuality.right, rQuality.bottom, 15, 15);
                    drawtext(quality == "高" ? _T("画质：高（点击切换）") : _T("画质：低（点击切换）"),
                        &rQuality, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                }
                // 切换帧率
                if (x >= rFps.left && x <= rFps.right && y >= rFps.top && y <= rFps.bottom) {
                    if (fps == 60) fps = 30;
                    else if (fps == 30) fps = 120;
                    else fps = 60;
                    setfillcolor(RGB(180, 200, 250));
                    fillroundrect(rFps.left, rFps.top, rFps.right, rFps.bottom, 15, 15);
                    _stprintf_s(fpsStr, _T("帧率：%d（点击切换）"), fps);
                    drawtext(fpsStr, &rFps, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                }
                // 切换操作
                if (x >= rCtrl.left && x <= rCtrl.right && y >= rCtrl.top && y <= rCtrl.bottom) {
                    controlMode = (controlMode == CONTROL_MOUSE) ? CONTROL_KEYBOARD : CONTROL_MOUSE;
                    setfillcolor(RGB(180, 200, 250));
                    fillroundrect(rCtrl.left, rCtrl.top, rCtrl.right, rCtrl.bottom, 15, 15);
                    drawtext(controlMode == CONTROL_MOUSE ? _T("操作模式：鼠标（点击切换）") : _T("操作模式：键盘（点击切换）"),
                        &rCtrl, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                }
                // 切换AI提示
                if (x >= rAI.left && x <= rAI.right && y >= rAI.top && y <= rAI.bottom) {
                    enableAI = !enableAI;
                    setfillcolor(RGB(180, 200, 250));
                    fillroundrect(rAI.left, rAI.top, rAI.right, rAI.bottom, 15, 15);
                    drawtext(enableAI ? _T("AI提示：开启（点击切换）") : _T("AI提示：关闭（点击切换）"),
                        &rAI, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                }
                // 返回
                if (x >= rBack.left && x <= rBack.right && y >= rBack.top && y <= rBack.bottom) {
                    return;
                }
            }
        }
        Sleep(10);
    }
}