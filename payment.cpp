#include "payment.h"
#include <graphics.h>
#include <tchar.h>

int paymentMenu(int balance) {
    setbkcolor(RGB(240, 255, 240));
    cleardevice();

    // 标题
    settextstyle(40, 0, _T("微软雅黑"));
    settextcolor(RGB(50, 120, 80));
    outtextxy(300, 70, _T("充值中心"));

    // 提示语
    settextstyle(24, 0, _T("微软雅黑"));
    settextcolor(RGB(40, 40, 90));
    outtextxy(260, 140, _T("请使用微信扫码付款："));

    // 居中显示付款码图片
    const int imgW = 200, imgH = 272;
    const int winW = 800;
    int imgX = (winW - imgW) / 2;
    int imgY = 180;
    IMAGE wechatImg;
    loadimage(&wechatImg, _T("wechat_pay.png"), imgW, imgH);
    putimage(imgX, imgY, &wechatImg);

    // 等待用户点击任意处返回
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