#include <graphics.h> // EasyX
#include <windows.h>  // For HWND, MessageBox etc.
#include <cstdio>     // For _stprintf_s
#include "Payment.h"  // Our Payment class
// #include "Game.h"  // Assuming you have your Game class for Peg Solitaire

// Define application states
enum class AppState {
    MAIN_MENU,
    IN_GAME,
    RECHARGE_CENTER,
    SETTINGS,
    EXITING
};

// Global or accessible application state
AppState currentAppState = AppState::MAIN_MENU;
HWND g_hwnd = NULL; // Global window handle

// --- Main Menu Button Definitions (Example) ---
const int MM_BTN_START_GAME_X1 = 250, MM_BTN_START_GAME_Y1 = 200;
const int MM_BTN_RECHARGE_X1 = 250, MM_BTN_RECHARGE_Y1 = 270;
const int MM_BTN_SETTINGS_X1 = 250, MM_BTN_SETTINGS_Y1 = 340;
const int MM_BTN_EXIT_X1 = 250, MM_BTN_EXIT_Y1 = 410;
const int MM_BTN_WIDTH = 300, MM_BTN_HEIGHT = 50;
const int MM_BTN_CORNER_RADIUS = 10;

// Re-use or adapt the styled button drawing function
void drawStyledButtonMain(int x1, int y1, int x2, int y2, const TCHAR* text, COLORREF bgColor, COLORREF textColor, int cornerRadius, bool isHovered = false) {
    COLORREF hoverBgColor = RGB(min(255, GetRValue(bgColor) + 20), min(255, GetGValue(bgColor) + 20), min(255, GetBValue(bgColor) + 20));
    COLORREF currentBgColor = isHovered ? hoverBgColor : bgColor;
    COLORREF currentLineColor = isHovered ? RGB(30, 30, 50) : RGB(80, 80, 100);
    setlinestyle(PS_SOLID, isHovered ? 3 : 2); setlinecolor(currentLineColor); setfillcolor(currentBgColor);
    fillroundrect(x1, y1, x2, y2, cornerRadius, cornerRadius);
    settextcolor(textColor); setbkmode(TRANSPARENT);
    LOGFONT f; gettextstyle(&f); f.lfHeight = 24; _tcscpy_s(f.lfFaceName, LF_FACESIZE, _T("微软雅黑")); f.lfQuality = ANTIALIASED_QUALITY; settextstyle(&f);
    int tw = textwidth(text), th = textheight(text);
    outtextxy(x1 + (x2 - x1 - tw) / 2, y1 + (y2 - y1 - th) / 2, text);
}


void drawMainMenu(POINT mousePos) {
    setbkcolor(RGB(210, 220, 230)); // Light blue-grey background
    cleardevice();

    LOGFONT titleFont; gettextstyle(&titleFont); titleFont.lfHeight = 50;
    _tcscpy_s(titleFont.lfFaceName, LF_FACESIZE, _T("Segoe Script")); // A bit playful
    titleFont.lfQuality = ANTIALIASED_QUALITY; settextstyle(&titleFont);
    settextcolor(RGB(50, 70, 90));
    outtextxy((800 - textwidth(_T("孔明棋大挑战"))) / 2, 80, _T("孔明棋大挑战"));

    // Draw buttons
    bool hoverStart = mousePos.x >= MM_BTN_START_GAME_X1 && mousePos.x <= MM_BTN_START_GAME_X1 + MM_BTN_WIDTH && mousePos.y >= MM_BTN_START_GAME_Y1 && mousePos.y <= MM_BTN_START_GAME_Y1 + MM_BTN_HEIGHT;
    drawStyledButtonMain(MM_BTN_START_GAME_X1, MM_BTN_START_GAME_Y1, MM_BTN_START_GAME_X1 + MM_BTN_WIDTH, MM_BTN_START_GAME_Y1 + MM_BTN_HEIGHT, _T("开始游戏"), RGB(100, 180, 100), WHITE, MM_BTN_CORNER_RADIUS, hoverStart);

    bool hoverRecharge = mousePos.x >= MM_BTN_RECHARGE_X1 && mousePos.x <= MM_BTN_RECHARGE_X1 + MM_BTN_WIDTH && mousePos.y >= MM_BTN_RECHARGE_Y1 && mousePos.y <= MM_BTN_RECHARGE_Y1 + MM_BTN_HEIGHT;
    drawStyledButtonMain(MM_BTN_RECHARGE_X1, MM_BTN_RECHARGE_Y1, MM_BTN_RECHARGE_X1 + MM_BTN_WIDTH, MM_BTN_RECHARGE_Y1 + MM_BTN_HEIGHT, _T("充值中心"), RGB(100, 150, 200), WHITE, MM_BTN_CORNER_RADIUS, hoverRecharge);

    bool hoverSettings = mousePos.x >= MM_BTN_SETTINGS_X1 && mousePos.x <= MM_BTN_SETTINGS_X1 + MM_BTN_WIDTH && mousePos.y >= MM_BTN_SETTINGS_Y1 && mousePos.y <= MM_BTN_SETTINGS_Y1 + MM_BTN_HEIGHT;
    drawStyledButtonMain(MM_BTN_SETTINGS_X1, MM_BTN_SETTINGS_Y1, MM_BTN_SETTINGS_X1 + MM_BTN_WIDTH, MM_BTN_SETTINGS_Y1 + MM_BTN_HEIGHT, _T("游戏设置"), RGB(150, 150, 150), WHITE, MM_BTN_CORNER_RADIUS, hoverSettings);

    bool hoverExit = mousePos.x >= MM_BTN_EXIT_X1 && mousePos.x <= MM_BTN_EXIT_X1 + MM_BTN_WIDTH && mousePos.y >= MM_BTN_EXIT_Y1 && mousePos.y <= MM_BTN_EXIT_Y1 + MM_BTN_HEIGHT;
    drawStyledButtonMain(MM_BTN_EXIT_X1, MM_BTN_EXIT_Y1, MM_BTN_EXIT_X1 + MM_BTN_WIDTH, MM_BTN_EXIT_Y1 + MM_BTN_HEIGHT, _T("退出游戏"), RGB(200, 100, 100), WHITE, MM_BTN_CORNER_RADIUS, hoverExit);

    LOGFONT footerFont; gettextstyle(&footerFont); footerFont.lfHeight = 16; _tcscpy_s(footerFont.lfFaceName, LF_FACESIZE, _T("Arial")); settextstyle(&footerFont);
    settextcolor(RGB(100, 100, 100));
    outtextxy(10, 570, _T("同济大学 高级程序设计 yixiaogithub66/-_-"));
}

void handleMainMenuInput(ExMessage& msg) {
    if (msg.message == WM_LBUTTONDOWN) {
        if (msg.x >= MM_BTN_START_GAME_X1 && msg.x <= MM_BTN_START_GAME_X1 + MM_BTN_WIDTH && msg.y >= MM_BTN_START_GAME_Y1 && msg.y <= MM_BTN_START_GAME_Y1 + MM_BTN_HEIGHT) {
            // currentAppState = AppState::IN_GAME; // Placeholder for starting game
            MessageBox(g_hwnd, _T("开始游戏（功能待实现）"), _T("提示"), MB_OK);
        }
        else if (msg.x >= MM_BTN_RECHARGE_X1 && msg.x <= MM_BTN_RECHARGE_X1 + MM_BTN_WIDTH && msg.y >= MM_BTN_RECHARGE_Y1 && msg.y <= MM_BTN_RECHARGE_Y1 + MM_BTN_HEIGHT) {
            currentAppState = AppState::RECHARGE_CENTER;
        }
        else if (msg.x >= MM_BTN_SETTINGS_X1 && msg.x <= MM_BTN_SETTINGS_X1 + MM_BTN_WIDTH && msg.y >= MM_BTN_SETTINGS_Y1 && msg.y <= MM_BTN_SETTINGS_Y1 + MM_BTN_HEIGHT) {
            // currentAppState = AppState::SETTINGS; // Placeholder for settings
            MessageBox(g_hwnd, _T("游戏设置（功能待实现）"), _T("提示"), MB_OK);
        }
        else if (msg.x >= MM_BTN_EXIT_X1 && msg.x <= MM_BTN_EXIT_X1 + MM_BTN_WIDTH && msg.y >= MM_BTN_EXIT_Y1 && msg.y <= MM_BTN_EXIT_Y1 + MM_BTN_HEIGHT) {
            currentAppState = AppState::EXITING;
        }
    }
}


int main() {
    // Initialize graphics window
    g_hwnd = initgraph(800, 600, EW_SHOWCONSOLE); // Show console for debug messages
    if (g_hwnd == NULL) {
        MessageBox(NULL, _T("图形初始化失败！"), _T("错误"), MB_OK | MB_ICONERROR);
        return -1;
    }
    SetWindowText(g_hwnd, _T("孔明棋与充值中心 - yixiaogithub66"));

    Payment paymentModule; // Create an instance of our Payment module
    // Game pegSolitaireGame; // Assuming you have a Game class instance

    ExMessage msg;

    BeginBatchDraw(); // Global batch draw for the main loop

    while (currentAppState != AppState::EXITING) {
        // Clear device at the start of the loop for the current state's drawing
        // However, individual states might also want to cleardevice with their specific bkcolor

        POINT currentMousePos = { 0,0 };
        if (GetActiveWindow() == g_hwnd) { // Only get mouse if our window is active
            GetCursorPos(&currentMousePos);
            ScreenToClient(g_hwnd, &currentMousePos);
        }

        // Process all pending messages for input
        while (peekmessage(&msg, EX_MOUSE | EX_KEY, true)) { // use true to remove message
            if (currentAppState == AppState::MAIN_MENU) {
                handleMainMenuInput(msg);
            }
            // Add input handling for other states (IN_GAME, SETTINGS) if needed directly here
            // or call their respective input handlers.
            // For RECHARGE_CENTER, input is handled within its own showPaymentInterface loop.
        }


        // State-dependent drawing and logic
        switch (currentAppState) {
        case AppState::MAIN_MENU:
            drawMainMenu(currentMousePos);
            break;
        case AppState::IN_GAME:
            // pegSolitaireGame.run(); // Example: run the game loop
            // After game finishes, it should set currentAppState back to MAIN_MENU or other
            cleardevice(); // Placeholder for game screen
            settextstyle(30, 0, _T("Arial"));
            outtextxy(300, 280, _T("游戏进行中..."));
            // For this example, let's add a way to return to menu
            settextstyle(20, 0, _T("Arial"));
            outtextxy(300, 320, _T("(按 ESC 返回主菜单 - 示例)"));
            if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) { // Check if ESC is pressed
                currentAppState = AppState::MAIN_MENU;
                Sleep(150); // Debounce
            }
            break;
        case AppState::RECHARGE_CENTER:
            // The paymentModule.showPaymentInterface has its own internal loop and drawing.
            // It will block until it returns.
            paymentModule.showPaymentInterface(g_hwnd);
            currentAppState = AppState::MAIN_MENU; // After returning from payment, go back to main menu
            break;
        case AppState::SETTINGS:
            cleardevice(); // Placeholder for settings screen
            settextstyle(30, 0, _T("Arial"));
            outtextxy(300, 280, _T("设置界面..."));
            settextstyle(20, 0, _T("Arial"));
            outtextxy(300, 320, _T("(按 ESC 返回主菜单 - 示例)"));
            if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
                currentAppState = AppState::MAIN_MENU;
                Sleep(150); // Debounce
            }
            break;
        default:
            // Should not happen
            break;
        }

        FlushBatchDraw(); // Draw the entire frame
        Sleep(16); // Aim for ~60 FPS
    }

    EndBatchDraw();
    closegraph();
    return 0;
}