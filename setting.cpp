#include "setting.h"   // �����Զ����������ͷ�ļ� (setting.h)�����а��� Setting �ṹ���������
#include <graphics.h>  // ���� EasyX ͼ�ο��ͷ�ļ������ڴ����͹������ò˵���ͼ���û����档
#include <tchar.h>     // ����ͨ���ַ�����ͷ�ļ�������֧�� _T() ��� TCHAR ���ͣ���ʵ�� ANSI �� Unicode �ַ����ļ����ԡ�

// Setting �ṹ��ĳ�Ա���� menu()��������ʾ�ʹ�����Ϸ���ý��档
// �����ô˺���ʱ���ᴴ��һ������ʽ�˵��������û��޸� Setting �ṹ���еĸ������á�
void Setting::menu() {
    // ���ò˵�����ı�����ɫ��
    setbkcolor(RGB(236, 246, 255)); // ����Ϊһ�ֵ���ɫ��
    cleardevice(); // ��յ�ǰ��ͼ�豸�����������õı���ɫ����������ڡ�

    // ���Ʋ˵����� "����"��
    settextstyle(40, 0, _T("΢���ź�")); // �����ı���ʽ�������С40���ֿ�0 (����Ӧ)������ "΢���ź�"��
    settextcolor(RGB(50, 50, 120));   // �����ı���ɫ������ɫ��
    outtextxy(320, 70, _T("����"));     // ����Ļ���� (320, 70) ��������⡣

    // ����ѡ�ť��ͨ���ı���ʽ��
    settextstyle(28, 0, _T("΢���ź�")); // �����С28��
    settextcolor(RGB(40, 40, 90));   // �ı���ɫ���������ɫ��

    // --- �������ð�ť ---
    RECT rQuality = { 250, 160, 550, 210 }; // ���廭�����ð�ť�ľ������� (left, top, right, bottom)��
    setfillcolor(RGB(180, 200, 250));      // ���ð�ť�������ɫ (ǳ��ɫ)��
    // ����Բ�Ǿ�����Ϊ��ť������15, 15 ��Բ����Բ�Ŀ�Ⱥ͸߶ȡ�
    fillroundrect(rQuality.left, rQuality.top, rQuality.right, rQuality.bottom, 15, 15);
    // ���ݵ�ǰ `quality` ��Ա������ֵ���ڰ�ť����ʾ��Ӧ���ı���
    // DT_CENTER: ˮƽ����, DT_VCENTER: ��ֱ����, DT_SINGLELINE: ������ʾ��
    drawtext(quality == "��" ? _T("���ʣ��ߣ�����л���") : _T("���ʣ��ͣ�����л���"),
        &rQuality, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    // --- ֡�����ð�ť ---
    RECT rFps = { 250, 230, 550, 280 }; // ����֡�����ð�ť�ľ�������
    setfillcolor(RGB(180, 200, 250));  // ���ð�ť�����ɫ��
    fillroundrect(rFps.left, rFps.top, rFps.right, rFps.bottom, 15, 15); // ���ư�ť������
    TCHAR fpsStr[32]; // ����һ�� TCHAR ������Ϊ�ַ��������������ڸ�ʽ��֡����ʾ��
    // ʹ�� _stprintf_s ��ȫ�ؽ���ǰ֡�� `fps` ��ʽ�����ַ����С�
    _stprintf_s(fpsStr, _T("֡�ʣ�%d������л���"), fps);
    drawtext(fpsStr, &rFps, DT_CENTER | DT_VCENTER | DT_SINGLELINE); // �ڰ�ť����ʾ֡���ı���

    // --- ����ģʽ���ð�ť ---
    RECT rCtrl = { 250, 300, 550, 350 }; // �������ģʽ��ť�ľ�������
    setfillcolor(RGB(180, 200, 250));   // ���ð�ť�����ɫ��
    fillroundrect(rCtrl.left, rCtrl.top, rCtrl.right, rCtrl.bottom, 15, 15); // ���ư�ť������
    // ���ݵ�ǰ `controlMode` ��Ա������ֵ����ʾ��Ӧ�Ĳ���ģʽ�ı���
    drawtext(controlMode == CONTROL_MOUSE ? _T("����ģʽ����꣨����л���") : _T("����ģʽ�����̣�����л���"),
        &rCtrl, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    // --- AI��ʾ���ð�ť ---
    RECT rAI = { 250, 370, 550, 420 }; // ����AI��ʾ���ð�ť�ľ�������
    setfillcolor(RGB(180, 200, 250)); // ���ð�ť�����ɫ��
    fillroundrect(rAI.left, rAI.top, rAI.right, rAI.bottom, 15, 15); // ���ư�ť������
    // ���ݵ�ǰ `enableAI` ��Ա������ֵ����ʾAI��ʾ�ǿ������ǹرա�
    drawtext(enableAI ? _T("AI��ʾ������������л���") : _T("AI��ʾ���رգ�����л���"),
        &rAI, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    // --- ���ذ�ť ---
    RECT rBack = { 320, 470, 480, 520 };  // ���巵�ذ�ť�ľ������� (������λ��)��
    setfillcolor(RGB(180, 200, 240));    // ���÷��ذ�ť�������ɫ (���в�ͬ��ǳ��ɫ)��
    fillroundrect(rBack.left, rBack.top, rBack.right, rBack.bottom, 12, 12); // ���ư�ť������Բ����С��
    settextcolor(RGB(20, 20, 80));       // ���÷��ذ�ť�ϵ��ı���ɫ (������ɫ)��
    settextstyle(28, 0, _T("΢���ź�")); // ȷ���ı���ʽ��ѡ�ťһ�»��������
    drawtext(_T("����"), &rBack, DT_CENTER | DT_VCENTER | DT_SINGLELINE); // �ڰ�ť�ϻ��� "����" �ı���

    // �������¼�ѭ���������û����� (��Ҫ�������)��
    while (true) {
        ExMessage m; // EasyX ��Ϣ�ṹ�壬���ڴ洢��������¼���
        // ʹ�� peekmessage ��������Ϣ (EM_MOUSE)������һ��������������
        // ����������������Ϣ����������
        while (peekmessage(&m, EM_MOUSE)) {
            if (m.message == WM_LBUTTONDOWN) { // ����������������¼���
                int x = m.x; // ��ȡ���������Ļ X ���ꡣ
                int y = m.y; // ��ȡ���������Ļ Y ���ꡣ

                // ����Ƿ����ˡ����ʡ���ť��
                if (x >= rQuality.left && x <= rQuality.right && y >= rQuality.top && y <= rQuality.bottom) {
                    // �л��������ã������ǰ�� "��"�����Ϊ "��"����֮��Ȼ��
                    quality = (quality == "��") ? "��" : "��";
                    // �ػ滭�ʰ�ť�Է�ӳ�µ����á�
                    setfillcolor(RGB(180, 200, 250)); // ��ť����ɫ
                    fillroundrect(rQuality.left, rQuality.top, rQuality.right, rQuality.bottom, 15, 15); // �ػ汳��
                    settextcolor(RGB(40, 40, 90)); // ȷ���ı���ɫ��ȷ
                    settextstyle(28, 0, _T("΢���ź�")); // ȷ���ı���ʽ��ȷ
                    drawtext(quality == "��" ? _T("���ʣ��ߣ�����л���") : _T("���ʣ��ͣ�����л���"),
                        &rQuality, DT_CENTER | DT_VCENTER | DT_SINGLELINE); // �ػ��ı�
                }
                // ����Ƿ����ˡ�֡�ʡ���ť��
                if (x >= rFps.left && x <= rFps.right && y >= rFps.top && y <= rFps.bottom) {
                    // �л�֡�����ã�60 -> 30 -> 120 -> 60 (ѭ��)��
                    if (fps == 60) fps = 30;
                    else if (fps == 30) fps = 120;
                    else fps = 60;
                    // �ػ�֡�ʰ�ť��
                    setfillcolor(RGB(180, 200, 250));
                    fillroundrect(rFps.left, rFps.top, rFps.right, rFps.bottom, 15, 15);
                    settextcolor(RGB(40, 40, 90));
                    settextstyle(28, 0, _T("΢���ź�"));
                    _stprintf_s(fpsStr, _T("֡�ʣ�%d������л���"), fps); // ����֡���ַ�����
                    drawtext(fpsStr, &rFps, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                }
                // ����Ƿ����ˡ�����ģʽ����ť��
                if (x >= rCtrl.left && x <= rCtrl.right && y >= rCtrl.top && y <= rCtrl.bottom) {
                    // �л�����ģʽ����� <-> ���̡�
                    controlMode = (controlMode == CONTROL_MOUSE) ? CONTROL_KEYBOARD : CONTROL_MOUSE;
                    // �ػ����ģʽ��ť��
                    setfillcolor(RGB(180, 200, 250));
                    fillroundrect(rCtrl.left, rCtrl.top, rCtrl.right, rCtrl.bottom, 15, 15);
                    settextcolor(RGB(40, 40, 90));
                    settextstyle(28, 0, _T("΢���ź�"));
                    drawtext(controlMode == CONTROL_MOUSE ? _T("����ģʽ����꣨����л���") : _T("����ģʽ�����̣�����л���"),
                        &rCtrl, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                }
                // ����Ƿ����ˡ�AI��ʾ����ť��
                if (x >= rAI.left && x <= rAI.right && y >= rAI.top && y <= rAI.bottom) {
                    // �л� AI ��ʾ������״̬��
                    enableAI = !enableAI;
                    // �ػ� AI ��ʾ��ť��
                    setfillcolor(RGB(180, 200, 250));
                    fillroundrect(rAI.left, rAI.top, rAI.right, rAI.bottom, 15, 15);
                    settextcolor(RGB(40, 40, 90));
                    settextstyle(28, 0, _T("΢���ź�"));
                    drawtext(enableAI ? _T("AI��ʾ������������л���") : _T("AI��ʾ���رգ�����л���"),
                        &rAI, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                }
                // ����Ƿ����ˡ����ء���ť��
                if (x >= rBack.left && x <= rBack.right && y >= rBack.top && y <= rBack.bottom) {
                    return; // ���������ذ�ť�����˳� menu() ���������ý������� (��Ϊ������ Setting ����ĳ�Ա)��
                }
            }
        }
        Sleep(10); // ��������10���룬�Խ���CPU�ڴ�ѭ���е�ʹ���ʣ����������û������ʱ��ת��
    }
}