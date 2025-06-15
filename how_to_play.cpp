#include "how_to_play.h" // ���ö�Ӧ��ͷ�ļ�
#include <graphics.h>    // ���� EasyX ͼ�ο�
#include <tchar.h>       // ����ͨ���ַ�����ͷ�ļ� (TCHAR, _T() ��)
#include <conio.h>       // _getch() �� (�����Ҫ��������˳�)

// ��ʾ����˵������
void showHowToPlayScreen() {
    // ���ñ�����ɫ
    setbkcolor(RGB(245, 245, 245)); // ʹ��һ����͵ı���ɫ
    cleardevice();

    // ���Ʊ���
    settextstyle(40, 0, _T("΢���ź�"));
    settextcolor(RGB(30, 30, 100)); // ����ɫ����
    outtextxy(280, 40, _T("����˵��"));

    // ����˵���ı�����ʽ
    settextstyle(20, 0, _T("����"));
    settextcolor(BLACK);
    setbkmode(TRANSPARENT); // ȷ���ı�����͸��

    int text_x = 50;
    int text_y_start = 100;
    int line_height = 28; // �����и��Ա��Ķ�

    // ˵������
    outtextxy(text_x, text_y_start, _T("����ϷĿ�꡿"));
    outtextxy(text_x + 20, text_y_start + line_height, _T("ͨ������֮�����Ծ���������ӣ�����Ŀ����ʹ������ֻʣ��"));
    outtextxy(text_x + 20, text_y_start + line_height * 2, _T("һ�����ӣ����Ҹ��������λ�����̵�����λ�ã������淨����"));

    outtextxy(text_x, text_y_start + line_height * 4, _T("����������"));
    outtextxy(text_x + 20, text_y_start + line_height * 5, _T("1. ����ֻ���������ڵ����ӣ��䵽�����Ӻ󷽵Ŀ�λ�ϡ�"));
    outtextxy(text_x + 20, text_y_start + line_height * 6, _T("2. ��Ծ���������ˮƽ��ֱ�������������̣������Σ�֧��б����Ծ��"));
    outtextxy(text_x + 20, text_y_start + line_height * 7, _T("3. �����������ӽ����������Ƴ���"));
    outtextxy(text_x + 20, text_y_start + line_height * 8, _T("4. ���Ӳ���б����Ծ���������̹�������˵������"));
    outtextxy(text_x + 20, text_y_start + line_height * 9, _T("5. �������������ӵ�λ�ã�Ҳ��������������������ӡ�"));


    outtextxy(text_x, text_y_start + line_height * 11, _T("����������"));
    outtextxy(text_x + 20, text_y_start + line_height * 12, _T("- ���һ�����ӣ�ѡ�и����ӣ����ӻ��������"));
    outtextxy(text_x + 20, text_y_start + line_height * 13, _T("- �ٴε����ѡ�е����ӣ�ȡ��ѡ�С�"));
    outtextxy(text_x + 20, text_y_start + line_height * 14, _T("- ѡ�����Ӻ󣬵��һ�����Ϲ���Ŀ�λ�������Ծ��"));

    outtextxy(text_x, text_y_start + line_height * 16, _T("�����̿�ݼ���"));
    outtextxy(text_x + 20, text_y_start + line_height * 17, _T("- H ������ʾ/���� AI �ṩ������߷���ʾ��"));
    outtextxy(text_x + 20, text_y_start + line_height * 18, _T("- A ������� AI ��ʾ����ʾ�����Զ�ִ�и���ʾ���߷���"));
    outtextxy(text_x + 20, text_y_start + line_height * 19, _T("- Ctrl + Z��������һ��������"));
    outtextxy(text_x + 20, text_y_start + line_height * 20, _T("- Ctrl + R������ǰ���ָ̻�����ʼ���֡�"));
    outtextxy(text_x + 20, text_y_start + line_height * 21, _T("- ESC ��������Ϸ�����У��������˵���"));


    // ���Ʒ��ذ�ť
    settextstyle(24, 0, _T("΢���ź�"));
    RECT r_back = { 300, 650 - 70, 500, 650 - 30 }; // ��ťλ�õ������ײ�
    setfillcolor(RGB(200, 220, 240)); // ����ɫ��ť
    fillroundrect(r_back.left, r_back.top, r_back.right, r_back.bottom, 10, 10);
    settextcolor(RGB(30, 30, 80)); // ����ɫ����
    drawtext(_T("�������˵�"), &r_back, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    FlushBatchDraw(); // ȷ���������ݶ����Ƶ���Ļ��

    // �ȴ��û���������
    ExMessage msg;
    while (true) {
        // ��ȡ��Ϣ�����û����Ϣ������
        msg = getmessage(EX_MOUSE | EX_KEY); // ʹ�� getmessage ��� peekmessage + Sleep

        if (msg.message == WM_LBUTTONDOWN) { // ����������
            if (msg.x >= r_back.left && msg.x <= r_back.right && msg.y >= r_back.top && msg.y <= r_back.bottom) {
                break; // ����˷��ذ�ť
            }
        }
        else if (msg.message == WM_KEYDOWN) { // ������̰���
            if (msg.vkcode == VK_ESCAPE) { // ����� ESC ����Ҳ����
                break;
            }
            // �����Ҫ��������Ҳ����չ������������������ж�
            // break; // ���磬��������¶�����
        }
    }
}