#include "payment.h"
#include <graphics.h>
#include <tchar.h>

// Define button properties
const int RETURN_BUTTON_X = 50;
const int RETURN_BUTTON_Y = 550; // Adjust Y position as needed
const int RETURN_BUTTON_WIDTH = 150;
const int RETURN_BUTTON_HEIGHT = 40;
const TCHAR* RETURN_BUTTON_TEXT = _T("·µ»Ø");

// Helper function to check if a point is within a rectangle
bool isPointInRect(int x, int y, int rectX, int rectY, int rectWidth, int rectHeight) {
    return x >= rectX && x <= rectX + rectWidth && y >= rectY && y <= rectY + rectHeight;
}

int paymentMenu(int balance) {
    setbkcolor(RGB(240, 255, 240));
    cleardevice();
    settextstyle(40, 0, _T("Î¢ÈíÑÅºÚ"));
    settextcolor(RGB(50, 120, 80));
    outtextxy(300, 70, _T("³äÖµ½çÃæ"));

    settextstyle(24, 0, _T("Î¢ÈíÑÅºÚ"));
    settextcolor(RGB(40, 40, 90));
    outtextxy(260, 140, _T("ÇëÊ¹ÓÃÎ¢ÐÅÉ¨Âë³äÖµ£º"));

    const int imgW = 200, imgH = 272;
    const int winW = 800;
    int imgX = (winW - imgW) / 2;
    int imgY = 180;
    IMAGE wechatImg;
    loadimage(&wechatImg, _T("wechat_pay.png"), imgW, imgH); // Ensure wechat_pay.png is in the correct path
    putimage(imgX, imgY, &wechatImg);

    // Draw the return button
    setfillcolor(RGB(200, 200, 200)); // Button color
    fillroundrect(RETURN_BUTTON_X, RETURN_BUTTON_Y, RETURN_BUTTON_X + RETURN_BUTTON_WIDTH, RETURN_BUTTON_Y + RETURN_BUTTON_HEIGHT, 10, 10);
    settextcolor(RGB(0, 0, 0)); // Text color
    settextstyle(20, 0, _T("Î¢ÈíÑÅºÚ")); // Adjust text size if needed
    // Calculate text position to center it on the button
    int textWidth = textwidth(RETURN_BUTTON_TEXT);
    int textHeight = textheight(RETURN_BUTTON_TEXT);
    outtextxy(RETURN_BUTTON_X + (RETURN_BUTTON_WIDTH - textWidth) / 2, RETURN_BUTTON_Y + (RETURN_BUTTON_HEIGHT - textHeight) / 2, RETURN_BUTTON_TEXT);


    while (true) {
        ExMessage m;
        // Use getmessage to block until a message, or peekmessage for non-blocking
        if (peekmessage(&m, EM_MOUSE)) { // Check for mouse messages
            if (m.message == WM_LBUTTONDOWN) {
                // Check if the click was on the return button
                if (isPointInRect(m.x, m.y, RETURN_BUTTON_X, RETURN_BUTTON_Y, RETURN_BUTTON_WIDTH, RETURN_BUTTON_HEIGHT)) {
                    // Perform action for return button, e.g., break loop or return a specific value
                    return balance; // Or a different value to indicate return, or change state
                }
                // Potentially handle other clicks here if needed, or ignore them
            }
        }
        Sleep(10); // Reduce CPU usage
    }
}