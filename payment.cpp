#include "payment.h"   // �����Զ���ĳ�ֵ����ͷ�ļ� (payment.h)�����п��ܰ��� paymentMenu ������������
#include <graphics.h>  // ���� EasyX ͼ�ο��ͷ�ļ������ڴ����͹���ͼ�ν��棬������ı���ͼ�����״��
#include <tchar.h>     // ����ͨ���ַ�����ͷ�ļ�������֧�� _T() ��� TCHAR ���ͣ�ʹ�ô���������� ANSI �� Unicode �ַ���֮����ֲ��

// ���尴ť�����Գ��������������޸ġ�
const int RETURN_BUTTON_X = 50;                   // �����ء���ť�����Ͻ� X ���ꡣ
const int RETURN_BUTTON_Y = 550;                  // �����ء���ť�����Ͻ� Y ���� (������Ҫ����Yλ��)��
const int RETURN_BUTTON_WIDTH = 150;              // �����ء���ť�Ŀ�ȡ�
const int RETURN_BUTTON_HEIGHT = 40;              // �����ء���ť�ĸ߶ȡ�
const TCHAR* RETURN_BUTTON_TEXT = _T("����");     // �����ء���ť����ʾ���ı���_T() ��ȷ���ַ����ڲ�ͬ�ַ����µļ����ԡ�

// �������������һ���� (x, y) �Ƿ���һ��ָ���ľ��������ڡ�
// @param x, y: Ҫ���ĵ�����ꡣ
// @param rectX, rectY: �����������Ͻǵ����ꡣ
// @param rectWidth, rectHeight: ��������Ŀ�Ⱥ͸߶ȡ�
// @return bool: ������ھ������򷵻� true�����򷵻� false��
bool isPointInRect(int x, int y, int rectX, int rectY, int rectWidth, int rectHeight) {
    return x >= rectX && x <= rectX + rectWidth && // ����� X �����Ƿ��ھ��ε� X ��Χ�ڡ�
        y >= rectY && y <= rectY + rectHeight;  // ����� Y �����Ƿ��ھ��ε� Y ��Χ�ڡ�
}

// ��ֵ�˵�������
// @param balance: ��ҵ�ǰ��������ִ�к���ܻ᷵�ظ��º����� (�ڴ�ʾ���У�δʵ���޸�����������չʾ)��
// @return int: ������ҵ����ڴ�ʾ���У�������ذ�ťʱ�����ش���� balance ֵ��
int paymentMenu(int balance) {
    // ���ñ�����ɫΪ����ɫ (RGB(240, 255, 240))��
    setbkcolor(RGB(240, 255, 240));
    // ��յ�ǰ��ͼ�豸�����������õı���ɫ����������ڡ�
    cleardevice();

    // ���ñ����ı�����ʽ��
    settextstyle(40, 0, _T("΢���ź�")); // �����С40���ֿ�0 (����Ӧ)���������� "΢���ź�"��
    // ���ñ����ı�����ɫΪһ��ƫ�̵���ɫ (RGB(50, 120, 80))��
    settextcolor(RGB(50, 120, 80));
    // ����Ļָ��λ�� (300, 70) ��������ı� "��ֵ����"��
    outtextxy(300, 70, _T("��ֵ����"));

    // ������ʾ��Ϣ�ı�����ʽ��
    settextstyle(24, 0, _T("΢���ź�")); // �����С24��
    // ������ʾ��Ϣ�ı�����ɫΪ����ɫ (RGB(40, 40, 90))��
    settextcolor(RGB(40, 40, 90));
    // ����Ļָ��λ�� (260, 140) �����ʾ��Ϣ "��ʹ��΢��ɨ���ֵ��"��
    outtextxy(260, 140, _T("��ʹ��΢��ɨ���ֵ��"));

    // ����΢��֧����ά��ͼƬ�ĳߴ硣
    const int imgW = 200, imgH = 272;
    // ��ȡ���ڿ�ȣ����ھ�����ʾͼƬ��
    const int winW = 800; // ���贰�ڿ��Ϊ800��Ӧ�� initgraph �����õĿ��һ�¡�
    // ����ͼƬ���Ͻǵ� X ���꣬ʹ��ˮƽ���С�
    int imgX = (winW - imgW) / 2;
    // ����ͼƬ���Ͻǵ� Y ���ꡣ
    int imgY = 180;

    IMAGE wechatImg; // ����һ�� IMAGE �������ڴ洢���ص�ͼƬ��
    // ����΢��֧���Ķ�ά��ͼƬ "wechat_pay.png"��
    // ��Ҫȷ�� "wechat_pay.png" �ļ����ִ���ļ���ͬһĿ¼�£������ṩ��ȷ�����/����·����
    // imgW, imgH ����ָ���˼���ͼƬʱ�����Ŀ�Ⱥ͸߶ȣ����ͼƬʵ�ʳߴ粻ͬ�����ܻ�������š�
    loadimage(&wechatImg, _T("wechat_pay.png"), imgW, imgH);
    // �����ص�ͼƬ���Ƶ���Ļ��ָ��λ�� (imgX, imgY)��
    putimage(imgX, imgY, &wechatImg);

    // ���ơ����ء���ť��
    setfillcolor(RGB(200, 200, 200)); // ���ð�ť�������ɫΪ��ɫ��
    // ����һ��Բ�Ǿ�����Ϊ��ť�ı�����
    // ����Ϊ�����Ͻ�x, ���Ͻ�y, ���½�x, ���½�y, Բ����Բ�Ŀ��, Բ����Բ�ĸ߶ȡ�
    fillroundrect(RETURN_BUTTON_X, RETURN_BUTTON_Y, RETURN_BUTTON_X + RETURN_BUTTON_WIDTH, RETURN_BUTTON_Y + RETURN_BUTTON_HEIGHT, 10, 10);

    settextcolor(RGB(0, 0, 0)); // ���ð�ť���ı�����ɫΪ��ɫ��
    settextstyle(20, 0, _T("΢���ź�")); // ���ð�ť�ı��������С (�ɸ�����Ҫ����)��

    // �����ı��ڰ�ť�Ͼ�����ʾ�����λ�á�
    int textWidth = textwidth(RETURN_BUTTON_TEXT);   // ��ȡ�ı���ʵ�����ؿ�ȡ�
    int textHeight = textheight(RETURN_BUTTON_TEXT); // ��ȡ�ı���ʵ�����ظ߶ȡ�
    // �ڰ�ť�ڲ���������ı� "����"��
    outtextxy(RETURN_BUTTON_X + (RETURN_BUTTON_WIDTH - textWidth) / 2,
        RETURN_BUTTON_Y + (RETURN_BUTTON_HEIGHT - textHeight) / 2,
        RETURN_BUTTON_TEXT);

    // ��������ѭ�����ȴ��û����� (�����������ء���ť)��
    while (true) {
        ExMessage m; // EasyX ��Ϣ�ṹ�壬���ڴ洢��������¼���
        // ʹ�� peekmessage ��������Ϣ (EM_MOUSE)������һ��������������
        // ���������ֱ������Ϣ������ʹ�� getmessage��
        if (peekmessage(&m, EM_MOUSE)) { // �����鵽�����Ϣ
            if (m.message == WM_LBUTTONDOWN) { // �����Ϣ����������������
                // ��������λ���Ƿ��ڡ����ء���ť�ľ��������ڡ�
                if (isPointInRect(m.x, m.y, RETURN_BUTTON_X, RETURN_BUTTON_Y, RETURN_BUTTON_WIDTH, RETURN_BUTTON_HEIGHT)) {
                    // �������ˡ����ء���ť��ִ����Ӧ�Ĳ�����
                    // ���磬����ѭ���򷵻�һ���ض���ֵ���ı���Ϸ״̬��
                    // �ڴ�ʾ���У�ֱ�ӷ��ش���� balance ֵ��
                    // ע�⣺ʵ�ʵĳ�ֵ�߼� (��ȷ��֧���ɹ����������) ��δ�ڴ�ʵ�֡�
                    return balance;
                }
                // �����Ҫ�����������ﴦ����������ĵ���¼������ߺ������ǡ�
            }
        }
        Sleep(10); // ��������10���룬�Խ���CPU�ڴ�ѭ���е�ʹ���ʣ���������ת��
    }
}