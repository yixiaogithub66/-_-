#pragma once
#include <string>

enum ControlMode {
    CONTROL_MOUSE,
    CONTROL_KEYBOARD
};

struct Setting {
    std::string quality = "高";
    int fps = 60;
    ControlMode controlMode = CONTROL_MOUSE; // 默认鼠标
    std::string aiApiKey = "sk-16d270f5ca7546258bf0aeb54534e76f";  // AI API密钥
    bool enableAI = false;      // 是否启用AI助手
    void menu();
};