#pragma once
#include <string>

enum ControlMode {
    CONTROL_MOUSE,
    CONTROL_KEYBOARD
};

struct Setting {
    std::string quality = "��";
    int fps = 60;
    ControlMode controlMode = CONTROL_MOUSE; // Ĭ�����
    std::string aiApiKey = "sk-16d270f5ca7546258bf0aeb54534e76f";  // AI API��Կ
    bool enableAI = false;      // �Ƿ�����AI����
    void menu();
};