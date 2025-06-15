#include "payment.h"   // 引用自定义的充值功能头文件 (payment.h)，其中可能包含 paymentMenu 函数的声明。
#include <graphics.h>  // 引用 EasyX 图形库的头文件，用于创建和管理图形界面，如绘制文本、图像和形状。
#include <tchar.h>     // 引用通用字符类型头文件，用于支持 _T() 宏和 TCHAR 类型，使得代码更容易在 ANSI 和 Unicode 字符集之间移植。

// 定义按钮的属性常量，方便管理和修改。
const int RETURN_BUTTON_X = 50;                   // “返回”按钮的左上角 X 坐标。
const int RETURN_BUTTON_Y = 550;                  // “返回”按钮的左上角 Y 坐标 (根据需要调整Y位置)。
const int RETURN_BUTTON_WIDTH = 150;              // “返回”按钮的宽度。
const int RETURN_BUTTON_HEIGHT = 40;              // “返回”按钮的高度。
const TCHAR* RETURN_BUTTON_TEXT = _T("返回");     // “返回”按钮上显示的文本。_T() 宏确保字符串在不同字符集下的兼容性。

// 辅助函数：检查一个点 (x, y) 是否在一个指定的矩形区域内。
// @param x, y: 要检查的点的坐标。
// @param rectX, rectY: 矩形区域左上角的坐标。
// @param rectWidth, rectHeight: 矩形区域的宽度和高度。
// @return bool: 如果点在矩形内则返回 true，否则返回 false。
bool isPointInRect(int x, int y, int rectX, int rectY, int rectWidth, int rectHeight) {
    return x >= rectX && x <= rectX + rectWidth && // 检查点的 X 坐标是否在矩形的 X 范围内。
        y >= rectY && y <= rectY + rectHeight;  // 检查点的 Y 坐标是否在矩形的 Y 范围内。
}

// 充值菜单函数。
// @param balance: 玩家当前的余额，函数执行后可能会返回更新后的余额 (在此示例中，未实际修改余额，仅作界面展示)。
// @return int: 返回玩家的余额。在此示例中，点击返回按钮时，返回传入的 balance 值。
int paymentMenu(int balance) {
    // 设置背景颜色为淡绿色 (RGB(240, 255, 240))。
    setbkcolor(RGB(240, 255, 240));
    // 清空当前绘图设备，并用新设置的背景色填充整个窗口。
    cleardevice();

    // 设置标题文本的样式。
    settextstyle(40, 0, _T("微软雅黑")); // 字体大小40，字宽0 (自适应)，字体名称 "微软雅黑"。
    // 设置标题文本的颜色为一种偏绿的深色 (RGB(50, 120, 80))。
    settextcolor(RGB(50, 120, 80));
    // 在屏幕指定位置 (300, 70) 输出标题文本 "充值界面"。
    outtextxy(300, 70, _T("充值界面"));

    // 设置提示信息文本的样式。
    settextstyle(24, 0, _T("微软雅黑")); // 字体大小24。
    // 设置提示信息文本的颜色为深蓝色 (RGB(40, 40, 90))。
    settextcolor(RGB(40, 40, 90));
    // 在屏幕指定位置 (260, 140) 输出提示信息 "请使用微信扫码充值："。
    outtextxy(260, 140, _T("请使用微信扫码充值："));

    // 定义微信支付二维码图片的尺寸。
    const int imgW = 200, imgH = 272;
    // 获取窗口宽度，用于居中显示图片。
    const int winW = 800; // 假设窗口宽度为800，应与 initgraph 中设置的宽度一致。
    // 计算图片左上角的 X 坐标，使其水平居中。
    int imgX = (winW - imgW) / 2;
    // 设置图片左上角的 Y 坐标。
    int imgY = 180;

    IMAGE wechatImg; // 创建一个 IMAGE 对象，用于存储加载的图片。
    // 加载微信支付的二维码图片 "wechat_pay.png"。
    // 需要确保 "wechat_pay.png" 文件与可执行文件在同一目录下，或者提供正确的相对/绝对路径。
    // imgW, imgH 参数指定了加载图片时期望的宽度和高度，如果图片实际尺寸不同，可能会进行缩放。
    loadimage(&wechatImg, _T("wechat_pay.png"), imgW, imgH);
    // 将加载的图片绘制到屏幕的指定位置 (imgX, imgY)。
    putimage(imgX, imgY, &wechatImg);

    // 绘制“返回”按钮。
    setfillcolor(RGB(200, 200, 200)); // 设置按钮的填充颜色为灰色。
    // 绘制一个圆角矩形作为按钮的背景。
    // 参数为：左上角x, 左上角y, 右下角x, 右下角y, 圆角椭圆的宽度, 圆角椭圆的高度。
    fillroundrect(RETURN_BUTTON_X, RETURN_BUTTON_Y, RETURN_BUTTON_X + RETURN_BUTTON_WIDTH, RETURN_BUTTON_Y + RETURN_BUTTON_HEIGHT, 10, 10);

    settextcolor(RGB(0, 0, 0)); // 设置按钮上文本的颜色为黑色。
    settextstyle(20, 0, _T("微软雅黑")); // 设置按钮文本的字体大小 (可根据需要调整)。

    // 计算文本在按钮上居中显示所需的位置。
    int textWidth = textwidth(RETURN_BUTTON_TEXT);   // 获取文本的实际像素宽度。
    int textHeight = textheight(RETURN_BUTTON_TEXT); // 获取文本的实际像素高度。
    // 在按钮内部居中输出文本 "返回"。
    outtextxy(RETURN_BUTTON_X + (RETURN_BUTTON_WIDTH - textWidth) / 2,
        RETURN_BUTTON_Y + (RETURN_BUTTON_HEIGHT - textHeight) / 2,
        RETURN_BUTTON_TEXT);

    // 进入无限循环，等待用户操作 (例如点击“返回”按钮)。
    while (true) {
        ExMessage m; // EasyX 消息结构体，用于存储鼠标或键盘事件。
        // 使用 peekmessage 检查鼠标消息 (EM_MOUSE)，这是一个非阻塞函数。
        // 如果想阻塞直到有消息，可以使用 getmessage。
        if (peekmessage(&m, EM_MOUSE)) { // 如果检查到鼠标消息
            if (m.message == WM_LBUTTONDOWN) { // 如果消息类型是鼠标左键按下
                // 检查鼠标点击位置是否在“返回”按钮的矩形区域内。
                if (isPointInRect(m.x, m.y, RETURN_BUTTON_X, RETURN_BUTTON_Y, RETURN_BUTTON_WIDTH, RETURN_BUTTON_HEIGHT)) {
                    // 如果点击了“返回”按钮，执行相应的操作。
                    // 例如，跳出循环或返回一个特定的值来改变游戏状态。
                    // 在此示例中，直接返回传入的 balance 值。
                    // 注意：实际的充值逻辑 (如确认支付成功并增加余额) 并未在此实现。
                    return balance;
                }
                // 如果需要，可以在这里处理其他区域的点击事件，或者忽略它们。
            }
        }
        Sleep(10); // 短暂休眠10毫秒，以降低CPU在此循环中的使用率，避免程序空转。
    }
}