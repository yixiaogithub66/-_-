#include "Payment.h"
#include <graphics.h> // EasyX graphics library
#include <vector>
#include <string>
#include <cstdio> // For _stprintf_s

// --- Button Coordinates and Style Definitions ---
// Return Button for Payment Interface
const int PI_BACK_BUTTON_X1 = 30;
const int PI_BACK_BUTTON_Y1 = 30;
const int PI_BACK_BUTTON_WIDTH = 120;
const int PI_BACK_BUTTON_HEIGHT = 40;
const int PI_BACK_BUTTON_X2 = PI_BACK_BUTTON_X1 + PI_BACK_BUTTON_WIDTH;
const int PI_BACK_BUTTON_Y2 = PI_BACK_BUTTON_Y1 + PI_BACK_BUTTON_HEIGHT;
const int PI_BUTTON_CORNER_RADIUS = 8;

// Constructor
Payment::Payment() {
    // Initialization if needed
}

// Destructor
Payment::~Payment() {
    // Cleanup if needed
}

// Helper function to draw styled buttons (can be part of a UI utility class later)
void drawStyledButtonPI(int x1, int y1, int x2, int y2, const TCHAR* text, COLORREF bgColor, COLORREF textColor, int cornerRadius, bool isHovered = false) {
    COLORREF hoverBgColor = RGB(
        min(255, GetRValue(bgColor) + 20),
        min(255, GetGValue(bgColor) + 20),
        min(255, GetBValue(bgColor) + 20)
    );
    COLORREF currentBgColor = isHovered ? hoverBgColor : bgColor;
    COLORREF currentLineColor = isHovered ? RGB(30, 30, 50) : RGB(80, 80, 100);

    setlinestyle(PS_SOLID, isHovered ? 3 : 2);
    setlinecolor(currentLineColor);
    setfillcolor(currentBgColor);

    fillroundrect(x1, y1, x2, y2, cornerRadius, cornerRadius);

    settextcolor(textColor);
    setbkmode(TRANSPARENT);
    // Choose a suitable text size for buttons
    LOGFONT f;
    gettextstyle(&f); // Get current font settings to preserve them
    f.lfHeight = 20; // Set font height (approximate pixel size)
    _tcscpy_s(f.lfFaceName, LF_FACESIZE, _T("微软雅黑")); // Set font face
    f.lfQuality = ANTIALIASED_QUALITY; // Smoother font
    settextstyle(&f);


    int textWidth = textwidth(text);
    int textHeight = textheight(text); // This might not be accurate for multi-line, but ok for single
    outtextxy(x1 + (x2 - x1 - textWidth) / 2, y1 + (y2 - y1 - textHeight) / 2, text);

    // Restore previous text style if needed, or set a default for other text
    f.lfHeight = 16; // Example: reset to a default size for other text
    settextstyle(&f);
}


void Payment::showPaymentInterface(HWND hwnd) {
    bool runningPaymentInterface = true;
    ExMessage msg;

    struct RechargeOption {
        int id;
        std::wstring description; // Using wstring for TCHAR compatibility
        int amountCents;      // Amount in cents
        RECT rect;
        bool isHovered;
    };

    std::vector<RechargeOption> options = {
        {1, _T("100 游戏币 (￥10)"), 1000, {250, 180, 550, 230}, false},
        {2, _T("550 游戏币 (￥50)"), 5000, {250, 250, 550, 300}, false},
        {3, _T("1200 游戏币 (￥100)"), 10000, {250, 320, 550, 370}, false},
    };

    int currentUserBalanceCents = 50; // Example initial balance in cents

    // BeginBatchDraw(); // Already called in the main loop usually, if this is a sub-screen

    while (runningPaymentInterface) {
        // It's often better to have BeginBatchDraw/EndBatchDraw around the entire game loop in main.
        // If this function is a self-contained loop that takes over, then it's fine here.
        // For now, assuming it's called within a larger Begin/EndBatchDraw block.

        cleardevice();

        // 1. Background & Title
        setbkcolor(RGB(230, 230, 245)); // Light lavender background
        settextcolor(RGB(40, 40, 60));

        LOGFONT titleFont;
        gettextstyle(&titleFont);
        titleFont.lfHeight = 40;
        _tcscpy_s(titleFont.lfFaceName, LF_FACESIZE, _T("Impact"));
        titleFont.lfQuality = ANTIALIASED_QUALITY;
        settextstyle(&titleFont);
        outtextxy((800 - textwidth(_T("充 值 中 心"))) / 2, 50, _T("充 值 中 心"));

        LOGFONT defaultFont; // For other text
        gettextstyle(&defaultFont);
        defaultFont.lfHeight = 22;
        _tcscpy_s(defaultFont.lfFaceName, LF_FACESIZE, _T("微软雅黑"));
        defaultFont.lfQuality = ANTIALIASED_QUALITY;
        settextstyle(&defaultFont);

        TCHAR balanceText[100];
        _stprintf_s(balanceText, _T("当前余额: %.2f 游戏币"), static_cast<double>(currentUserBalanceCents) / 100.0);
        outtextxy((800 - textwidth(balanceText)) / 2, 120, balanceText);

        // 2. Mouse Position for Hover Effects
        POINT mousePos = { 0, 0 }; // Initialize to ensure it has values
        if (GetActiveWindow() == hwnd) { // Only get mouse if our window is active
            GetCursorPos(&mousePos);      // Screen coordinates
            ScreenToClient(hwnd, &mousePos); // Convert to client area coordinates
        }


        // 3. Draw Return Button
        bool isBackButtonHovered = (mousePos.x >= PI_BACK_BUTTON_X1 && mousePos.x <= PI_BACK_BUTTON_X2 &&
            mousePos.y >= PI_BACK_BUTTON_Y1 && mousePos.y <= PI_BACK_BUTTON_Y2);
        drawStyledButtonPI(PI_BACK_BUTTON_X1, PI_BACK_BUTTON_Y1, PI_BACK_BUTTON_X2, PI_BACK_BUTTON_Y2,
            _T("返回"), RGB(190, 190, 210), RGB(20, 20, 20), PI_BUTTON_CORNER_RADIUS, isBackButtonHovered);

        // 4. Draw Recharge Options
        for (auto& option : options) {
            option.isHovered = (mousePos.x >= option.rect.left && mousePos.x <= option.rect.right &&
                mousePos.y >= option.rect.top && mousePos.y <= option.rect.bottom);

            drawStyledButtonPI(option.rect.left, option.rect.top, option.rect.right, option.rect.bottom,
                option.description.c_str(), RGB(120, 180, 230), RGB(255, 255, 255), PI_BUTTON_CORNER_RADIUS, option.isHovered);
        }

        // 5. Other UI Elements (e.g., payment instructions)
        settextstyle(&defaultFont); // Ensure default font for this text
        defaultFont.lfHeight = 18;
        settextstyle(&defaultFont);
        settextcolor(RGB(80, 80, 80));
        outtextxy(200, 420, _T("提示：这是一个模拟充值界面，点击选项不会产生实际扣费。"));
        outtextxy(200, 450, _T("项目仓库: yixiaogithub66/-_-"));


        FlushBatchDraw();

        // 6. Input Handling
        // Use a loop to process all pending messages to avoid unresponsiveness
        while (peekmessage(&msg, EX_MOUSE | EX_KEY, true)) { // peek with remove
            switch (msg.message) {
            case WM_LBUTTONDOWN:
                // Update mousePos for this click event specifically
                mousePos.x = msg.x;
                mousePos.y = msg.y;

                if (mousePos.x >= PI_BACK_BUTTON_X1 && mousePos.x <= PI_BACK_BUTTON_X2 &&
                    mousePos.y >= PI_BACK_BUTTON_Y1 && mousePos.y <= PI_BACK_BUTTON_Y2) {
                    runningPaymentInterface = false; // Exit the loop
                }

                for (const auto& option : options) {
                    if (mousePos.x >= option.rect.left && mousePos.x <= option.rect.right &&
                        mousePos.y >= option.rect.top && mousePos.y <= option.rect.bottom) {

                        TCHAR paymentMsg[256];
                        _stprintf_s(paymentMsg, _T("您选择了: %s\n充值金额: %.2f元\n\n(模拟充值成功！)"),
                            option.description.c_str(), static_cast<double>(option.amountCents) / 100.0);
                        MessageBox(hwnd, paymentMsg, _T("模拟支付确认"), MB_OK | MB_ICONINFORMATION);

                        currentUserBalanceCents += option.amountCents; // Simulate balance update
                        // runningPaymentInterface = false; // Option: exit after payment
                        break;
                    }
                }
                break;

            case WM_KEYDOWN:
                if (msg.vkcode == VK_ESCAPE) {
                    runningPaymentInterface = false;
                }
                break;
            }
        }
        Sleep(10); // Small delay to prevent 100% CPU usage if no messages
    }
    // EndBatchDraw(); // Match BeginBatchDraw if it were here
    // Interface loop ends, function returns
}