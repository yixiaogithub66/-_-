#include "payment.h"
#include <graphics.h>
#include <tchar.h>

int paymentMenu(int balance) {
    setbkcolor(RGB(240, 255, 240));
    cleardevice();
    settextstyle(40, 0, _T("Œ¢»Ì—≈∫⁄"));
    settextcolor(RGB(50, 120, 80));
    outtextxy(300, 70, _T("≥‰÷µΩÁ√Ê"));

    settextstyle(24, 0, _T("Œ¢»Ì—≈∫⁄"));
    settextcolor(RGB(40, 40, 90));
    outtextxy(260, 140, _T("«Î π”√Œ¢–≈…®¬Î≥‰÷µ£∫"));

    const int imgW = 200, imgH = 272;
    const int winW = 800;
    int imgX = (winW - imgW) / 2;
    int imgY = 180;
    IMAGE wechatImg;
    loadimage(&wechatImg, _T("wechat_pay.png"), imgW, imgH);
    putimage(imgX, imgY, &wechatImg);

    while (true) {
        ExMessage m;
        while (peekmessage(&m, EM_MOUSE)) {
            if (m.message == WM_LBUTTONDOWN) {
                return balance;
            }
        }
        Sleep(10);
    }
}