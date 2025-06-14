#pragma once
#include <string>

enum ControlMode {
    CONTROL_MOUSE,
    CONTROL_KEYBOARD
};

struct Setting {
    std::string quality = "高";
    int fps = 60;
    ControlMode controlMode = CONTROL_MOUSE; // 新增，默认鼠标
    void menu();
};