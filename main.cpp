/*
Peg Solitaire �����壨ʹ�� EasyX ͼ�ο⿪����
���ߣ�yixiaogithub66
*/

#include <graphics.h> // ���� EasyX ͼ�ο⣬����ͼ�λ��ƺ��û�����
#include <conio.h>    // ���� conio.h�����ڿ���̨������������� _getch()
#include <string>     // ���� string�������ַ�������
#include "board.h"    // �����Զ��������ͷ�ļ�������������صĹ���
#include "game.h"     // �����Զ������Ϸ�߼�ͷ�ļ���������Ϸ�����淨
#include "setting.h"  // �����Զ��������ͷ�ļ���������Ϸ������ع���
#include "payment.h"  // �����Զ���ĳ�ֵͷ�ļ��������ֵ��ع���
#include "save_load.h"  // ����ͷ�ļ������ڴ�����Ϸ�浵�Ͷ�������
#include "how_to_play.h" // ���������ò���˵��ͷ�ļ�

// ������Ϸ�Ĳ�ͬ״̬���������״̬������л�
enum State {
    MENU,         // ���˵�״̬
    BOARD_SELECT, // ѡ����������״̬
    GAME,         // ��Ϸ������״̬
    SETTINGS,     // ����״̬
    PAYMENT,      // ��ֵ״̬
    SAVE_LOAD,    // �浵/����״̬
    HOW_TO_PLAY,  // ����������˵��״̬
    EXIT          // �˳���Ϸ״̬
};

// �����������������˵����棬��ʾ����ѡ��͵�ǰ���
void drawMainMenu(int balance);
// �����������������˵����߼��������û����������Ӧ����Ϸ״̬
State mainMenu(int balance);
// void showHowToPlayScreen(); // �Ƴ��������������Ϊ���Ѿ��� how_to_play.h ��


int main() {
    // ��ʼ��ͼ�λ��������ڴ�СΪ 800x650 ����
    initgraph(800, 650);
    // ���ñ�����ɫΪ��ɫ
    setbkcolor(WHITE);
    // ��ջ�ͼ�豸��Ӧ�ñ���ɫ
    cleardevice();

    int balance = 0;            // ��ʼ��������Ϊ0
    Setting setting;            // ����һ�� Setting �������ڹ�����Ϸ����
    BoardType boardType = CROSS; // ��ʼ��Ĭ����������Ϊʮ����

    State state = MENU; // ��ʼ����ǰ��Ϸ״̬Ϊ���˵�
    // ����Ϸѭ����ֻҪ״̬���� EXIT����һֱִ��
    while (state != EXIT) {
        // ʹ�� switch �����ݵ�ǰ״ִ̬����Ӧ�Ĳ���
        switch (state) {
        case MENU:
            // �����ǰ�����˵�״̬������� mainMenu ��������������״̬
            state = mainMenu(balance);
            break;
        case BOARD_SELECT: {
            // �����ǰ��ѡ����������״̬
            boardType = boardMenu(); // ���� boardMenu �������û�ѡ����������
            state = GAME;            // ѡ����Ϻ󣬽�״̬�л�����Ϸ������
            break;
        }
        case GAME: {
            // �����ǰ����Ϸ������״̬
            Board board(boardType);       // ����ѡ����������ʹ������̶���
            Game(board, setting).run(); // ������Ϸ����������Ϸ�߼�
            state = MENU;               // ��Ϸ�����󣬷������˵�״̬
            break;
        }
        case SETTINGS:
            // �����ǰ������״̬
            setting.menu(); // ���� setting ����� menu ��������������ѡ��
            state = MENU;   // ������ɺ󣬷������˵�״̬
            break;
        case PAYMENT:
            // �����ǰ�ǳ�ֵ״̬
            balance = paymentMenu(balance); // ���� paymentMenu ���������ֵ�����������
            state = MENU;                  // ��ֵ��ɺ󣬷������˵�״̬
            break;
        case SAVE_LOAD: { // ����浵/����״̬
            BoardType oldType = boardType;  // �������浵/��������ǰ����������
            boardType = saveLoadMenu(boardType); // ���� saveLoadMenu ��������浵������������ܸ�����������
            if (boardType != oldType) {
                // ����������ͷ����˱仯��˵�����ܼ����˴浵
                // ����������һЩ��־��¼���߸��û�����ʾ��Ϣ
            }
            state = MENU; // �����Ǵ浵���Ƕ���������ɣ����������˵�״̬
            break;
        }
        case HOW_TO_PLAY: // �������������˵��״̬
            showHowToPlayScreen(); // ������ how_to_play.cpp �ж���ĺ���
            state = MENU; // �鿴�����˵���󷵻����˵�
            break;
        }
    }
    // �ر�ͼ�λ������ͷ���Դ
    closegraph();
    // ���������˳�
    return 0;
}

// �������˵�����
void drawMainMenu(int balance) {
    // ���ñ�����ɫΪ����ɫ (RGB(236, 246, 255))
    setbkcolor(RGB(236, 246, 255));
    // �����Ļ��Ӧ���µı���ɫ
    cleardevice();
    // �����ı���ʽ�������С60���ֿ�0 (����Ӧ)���������� "΢���ź�"
    settextstyle(60, 0, _T("΢���ź�"));
    // ���ñ������ģʽΪ͸�����������ֱ������Ḳ�����������
    setbkmode(TRANSPARENT);
    // �����ı���ɫΪ����ɫ (RGB(50, 50, 120))
    settextcolor(RGB(50, 50, 120));
    // ��ָ��λ�� (210, 40) �����Ϸ���� "Peg Solitaire" (�����ƶ���һ�����ڳ��ռ�)
    outtextxy(210, 40, _T("Peg Solitaire"));
    // ���ú�����ť���ı���ʽ�������С32���ֿ�0 (����Ӧ)���������� "΢���ź�" (��΢��С����������Ӧ���ఴť)
    settextstyle(32, 0, _T("΢���ź�"));

    // ���������ť�ľ�������λ�úʹ�С (������Y����ͼ��)
    int button_width = 200; // ��΢�ӿ�ť
    int button_height = 45;
    int button_x = (800 - button_width) / 2; // ��ť����
    int button_start_y = 110; // ������ʼY��Ϊ������������ռ�
    int button_spacing = 12; // ��ť֮��Ĵ�ֱ���

    RECT r_start_game = { button_x, button_start_y, button_x + button_width, button_start_y + button_height };
    RECT r_how_to_play = { button_x, r_start_game.bottom + button_spacing, button_x + button_width, r_start_game.bottom + button_spacing + button_height };
    RECT r_settings = { button_x, r_how_to_play.bottom + button_spacing, button_x + button_width, r_how_to_play.bottom + button_spacing + button_height };
    RECT r_payment = { button_x, r_settings.bottom + button_spacing, button_x + button_width, r_settings.bottom + button_spacing + button_height };
    RECT r_save_load = { button_x, r_payment.bottom + button_spacing, button_x + button_width, r_payment.bottom + button_spacing + button_height };
    RECT r_exit = { button_x, r_save_load.bottom + button_spacing, button_x + button_width, r_save_load.bottom + button_spacing + button_height };


    // ���������ɫΪ����ɫ (RGB(180, 200, 250))�����ڻ��ư�ť����
    setfillcolor(RGB(180, 200, 250));
    // ����Բ�Ǿ�����Ϊ��ť����
    fillroundrect(r_start_game.left, r_start_game.top, r_start_game.right, r_start_game.bottom, 15, 15);
    fillroundrect(r_how_to_play.left, r_how_to_play.top, r_how_to_play.right, r_how_to_play.bottom, 15, 15);
    fillroundrect(r_settings.left, r_settings.top, r_settings.right, r_settings.bottom, 15, 15);
    fillroundrect(r_payment.left, r_payment.top, r_payment.right, r_payment.bottom, 15, 15);
    fillroundrect(r_save_load.left, r_save_load.top, r_save_load.right, r_save_load.bottom, 15, 15);
    fillroundrect(r_exit.left, r_exit.top, r_exit.right, r_exit.bottom, 15, 15);

    // ���ð�ť�����ֵ���ɫΪ�������ɫ (RGB(40, 40, 90))
    settextcolor(RGB(40, 40, 90));
    // �ڰ�ť���������ڻ�������
    drawtext(_T("��ʼ��Ϸ"), &r_start_game, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    drawtext(_T("����˵��"), &r_how_to_play, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    drawtext(_T("����"), &r_settings, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    drawtext(_T("��ֵ"), &r_payment, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    drawtext(_T("��ȡ�浵"), &r_save_load, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    drawtext(_T("�˳�"), &r_exit, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    // ׼��������ʾ�����ַ���������
    TCHAR balstr[32];
    _stprintf_s(balstr, _T("��ǰ��%d Ԫ"), balance);
    settextstyle(24, 0, _T("΢���ź�"));
    settextcolor(RGB(80, 130, 80));
    outtextxy(10, 650 - 30, balstr); // �����ʾ�����½�
}

// �������˵����߼�
State mainMenu(int balance) {
    drawMainMenu(balance);
    ExMessage m;
    while (true) {
        // ʹ�� getmessage ��ȡ������Ϣ������CPUռ��
        m = getmessage(EX_MOUSE); // ֻ���������Ϣ�Լ�

        if (m.message == WM_LBUTTONDOWN) {
            int x = m.x;
            int y = m.y;

            // �� drawMainMenu �л�ȡ��ť�ĳߴ��λ����Ϣ
            int button_width = 200;
            int button_x = (800 - button_width) / 2;
            int button_start_y = 110;
            int button_height = 45;
            int button_spacing = 12;

            // ����ʼ��Ϸ����ť
            if (x >= button_x && x <= button_x + button_width && y >= button_start_y && y <= button_start_y + button_height) return BOARD_SELECT;

            // ������˵������ť
            int how_to_play_y_start = button_start_y + button_height + button_spacing;
            if (x >= button_x && x <= button_x + button_width && y >= how_to_play_y_start && y <= how_to_play_y_start + button_height) return HOW_TO_PLAY;

            // �����á���ť
            int settings_y_start = how_to_play_y_start + button_height + button_spacing;
            if (x >= button_x && x <= button_x + button_width && y >= settings_y_start && y <= settings_y_start + button_height) return SETTINGS;

            // ����ֵ����ť
            int payment_y_start = settings_y_start + button_height + button_spacing;
            if (x >= button_x && x <= button_x + button_width && y >= payment_y_start && y <= payment_y_start + button_height) return PAYMENT;

            // ����ȡ�浵����ť
            int save_load_y_start = payment_y_start + button_height + button_spacing;
            if (x >= button_x && x <= button_x + button_width && y >= save_load_y_start && y <= save_load_y_start + button_height) return SAVE_LOAD;

            // ���˳�����ť
            int exit_y_start = save_load_y_start + button_height + button_spacing;
            if (x >= button_x && x <= button_x + button_width && y >= exit_y_start && y <= exit_y_start + button_height) return EXIT;
        }
        // Sleep(10); // ���ʹ�� peekmessage������Ҫ Sleep
    }
}

// �Ƴ� showHowToPlayScreen �������壬������λ�� how_to_play.cpp