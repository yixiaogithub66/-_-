#include "setting.h"
#include <graphics.h>
#include <tchar.h>

void Setting::menu() {
    setbkcolor(RGB(236, 246, 255));
    cleardevice();
    settextstyle(40, 0, _T("Î¢ÈíÑÅºÚ"));
    settextcolor(RGB(50, 50, 120));
    outtextxy(320, 70, _T("ÉèÖÃ"));

    // »­ÖÊÉèÖÃ
    settextstyle(28, 0, _T("Î¢ÈíÑÅºÚ"));
    settextcolor(RGB(40, 40, 90));
    RECT rQuality = { 250, 160, 550, 210 };
    setfillcolor(RGB(180, 200, 250));
    fillroundrect(rQuality.left, rQuality.top, rQuality.right, rQuality.bottom, 15, 15);
    drawtext(quality == "¸ß" ? _T("»­ÖÊ£º¸ß£¨µã»÷ÇÐ»»£©") : _T("»­ÖÊ£ºµÍ£¨µã»÷ÇÐ»»£©"),
        &rQuality, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    // Ö¡ÂÊÉèÖÃ
    RECT rFps = { 250, 230, 550, 280 };
    setfillcolor(RGB(180, 200, 250));
    fillroundrect(rFps.left, rFps.top, rFps.right, rFps.bottom, 15, 15);
    TCHAR fpsStr[32];
    _stprintf_s(fpsStr, _T("Ö¡ÂÊ£º%d£¨µã»÷ÇÐ»»£©"), fps);
    drawtext(fpsStr, &rFps, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    // ¿ØÖÆ·½Ê½ÉèÖÃ
    RECT rCtrl = { 250, 300, 550, 350 };
    setfillcolor(RGB(180, 200, 250));
    fillroundrect(rCtrl.left, rCtrl.top, rCtrl.right, rCtrl.bottom, 15, 15);
    drawtext(controlMode == CONTROL_MOUSE ? _T("²Ù¿Ø·½Ê½£ºÊó±ê£¨µã»÷ÇÐ»»£©") : _T("²Ù¿Ø·½Ê½£º¼üÅÌ£¨µã»÷ÇÐ»»£©"),
        &rCtrl, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    // ·µ»Ø°´Å¥
    RECT rBack = { 320, 400, 480, 450 };
    setfillcolor(RGB(180, 200, 240));
    fillroundrect(rBack.left, rBack.top, rBack.right, rBack.bottom, 12, 12);
    settextcolor(RGB(20, 20, 80));
    settextstyle(28, 0, _T("Î¢ÈíÑÅºÚ"));
    drawtext(_T("·µ»Ø"), &rBack, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    // ÊÂ¼þÑ­»·
    while (true) {
        ExMessage m;
        while (peekmessage(&m, EM_MOUSE)) {
            if (m.message == WM_LBUTTONDOWN) {
                int x = m.x, y = m.y;
                // »­ÖÊÇÐ»»
                if (x >= rQuality.left && x <= rQuality.right && y >= rQuality.top && y <= rQuality.bottom) {
                    quality = (quality == "¸ß") ? "µÍ" : "¸ß";
                    setfillcolor(RGB(180, 200, 250));
                    fillroundrect(rQuality.left, rQuality.top, rQuality.right, rQuality.bottom, 15, 15);
                    drawtext(quality == "¸ß" ? _T("»­ÖÊ£º¸ß£¨µã»÷ÇÐ»»£©") : _T("»­ÖÊ£ºµÍ£¨µã»÷ÇÐ»»£©"),
                        &rQuality, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                }
                // Ö¡ÂÊÇÐ»»
                if (x >= rFps.left && x <= rFps.right && y >= rFps.top && y <= rFps.bottom) {
                    if (fps == 60) fps = 30;
                    else if (fps == 30) fps = 120;
                    else fps = 60;
                    setfillcolor(RGB(180, 200, 250));
                    fillroundrect(rFps.left, rFps.top, rFps.right, rFps.bottom, 15, 15);
                    _stprintf_s(fpsStr, _T("Ö¡ÂÊ£º%d£¨µã»÷ÇÐ»»£©"), fps);
                    drawtext(fpsStr, &rFps, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                }
                // ¿ØÖÆ·½Ê½ÇÐ»»
                if (x >= rCtrl.left && x <= rCtrl.right && y >= rCtrl.top && y <= rCtrl.bottom) {
                    controlMode = (controlMode == CONTROL_MOUSE) ? CONTROL_KEYBOARD : CONTROL_MOUSE;
                    setfillcolor(RGB(180, 200, 250));
                    fillroundrect(rCtrl.left, rCtrl.top, rCtrl.right, rCtrl.bottom, 15, 15);
                    drawtext(controlMode == CONTROL_MOUSE ? _T("²Ù¿Ø·½Ê½£ºÊó±ê£¨µã»÷ÇÐ»»£©") : _T("²Ù¿Ø·½Ê½£º¼üÅÌ£¨µã»÷ÇÐ»»£©"),
                        &rCtrl, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                }
                // ·µ»Ø
                if (x >= rBack.left && x <= rBack.right && y >= rBack.top && y <= rBack.bottom) {
                    return;
                }
            }
        }
        Sleep(10);
    }
}