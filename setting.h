#pragma once
#include <string>

enum ControlMode {
    CONTROL_MOUSE,
    CONTROL_KEYBOARD
};

struct Setting {
    std::string quality = "��";
    int fps = 60;
    ControlMode controlMode = CONTROL_MOUSE; // ������Ĭ�����
    void menu();
};