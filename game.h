#pragma once // Ԥ����ָ�ȷ����ͷ�ļ���һ�α����н�������һ�Σ���ֹ�ظ�����ͱ������

#include "board.h"     // ����������ͷ�ļ���Game ����Ҫʹ�� Board ��������������״̬��
#include "setting.h"   // ����������ͷ�ļ���Game ����Ҫʹ�� Setting ��������ȡ��Ϸ���á�
#include <vector>      // ���� C++ ��׼���е� vector ���������ڴ洢��ʷ��¼�ȶ�̬���顣
#include "ai_helper.h" // ���� AI ������ͷ�ļ���ȷ�� ai_helper.h Ҳ�Ѹ�����֧�ֵ���������㷨��

// AI ��ʾ״̬��ö������
// ���ڹ��� AI ��ʾ��ť�͹��ܵĵ�ǰ״̬��
enum class AIHintState {
    IDLE,         // AI δ����״̬��AI ��ǰû���ڼ��㣬��ʾ��ť��ʾΪ����״̬��
    WORKING,      // AI ����״̬��AI ���ڼ�������ƶ�����ʾ��ť������ʾΪ������ɫ����ʾ�������С���
    SHOWING_HINT  // AI ��ʾ��ʾ״̬��AI �Ѿ��������ʾ����������ʾ����ʾ��ť������ʾΪ�����ɫ����ʾ����ʾ�Ѹ�������������ʾ�������Ͽɼ���
};

// Game �࣬��װ�˿�������Ϸ����Ҫ�߼������̡�
class Game {
public:
    // ���캯�������ڳ�ʼ��һ���µ���Ϸʵ����
    // @param b: ��Ϸ��ʹ�õ����̶���
    // @param s: ��Ϸ�����ö��󣬰������Ѷȡ��Ƿ�������ʾ�����á�
    Game(Board b, const Setting& s);

    // ������Ϸ����ѭ����
    // �˺����������û����롢��Ϸ�߼����¡�������Ƶȣ�ֱ����Ϸ�������û��˳���
    void run();

    // �л� AI ��ʾ��ť�Ķ�����
    // ���û���� AI ��ʾ��ťʱ���ô˺�����
    // ������ݵ�ǰ�� AIHintState �����ǿ�ʼ������ʾ��ȡ�����㻹��������ʾ��
    void toggleAIButtonAction();

    // --- ���� Getter ���� ---
    // ��Щ�����ṩ�˶���Ϸ�ڲ�״̬��ֻ�����ʡ�

    // ��ȡ�Ե�ǰ��Ϸ���̶���ĳ������á�
    // @return const Board&: ��ǰ���̶����ֻ�����á�
    const Board& getBoard() const { return board; }

    // ��ȡ��ǰѡ�е����ӵ� X ���ꡣ
    // @return int: ѡ�����ӵ� X ���ꣻ���û�����ӱ�ѡ�У�ͨ��Ϊ -1��
    int getSelectedX() const { return selectedX; }

    // ��ȡ��ǰѡ�е����ӵ� Y ���ꡣ
    // @return int: ѡ�����ӵ� Y ���ꣻ���û�����ӱ�ѡ�У�ͨ��Ϊ -1��
    int getSelectedY() const { return selectedY; }

    // ��ȡ������ʷ��¼�ĳ������� (���ڳ������ܻ�ط�)��
    // boardHistory �洢��ÿһ���ƶ��������״̬��
    // @return const std::vector<std::vector<std::vector<int>>>&: ����״̬��ʷ��¼��ֻ�����á�
    const std::vector<std::vector<std::vector<int>>>& getBoardHistory() const { return boardHistory; }

    // ��ȡѡ������ X ������ʷ��¼�ĳ������á�
    // @return const std::vector<int>&: ѡ������ X ������ʷ��ֻ�����á�
    const std::vector<int>& getSelectedXHistory() const { return selectedXHistory; }

    // ��ȡѡ������ Y ������ʷ��¼�ĳ������á�
    // @return const std::vector<int>&: ѡ������ Y ������ʷ��ֻ�����á�
    const std::vector<int>& getSelectedYHistory() const { return selectedYHistory; }

private:
    // --- ˽�г�Ա���� ---
    // ��Щ�����洢����Ϸ�ĺ���״̬�͸�����Ϣ��

    Board board;        // ��ǰ��Ϸ�����̶��󣬹������ӵĲ��ֺ���Ч�ԡ�
    Setting setting;    // ��ǰ��Ϸ�����ö��󣬴洢�û����á�
    int selectedX, selectedY; // ��ǰѡ�е����ӵ����ꡣ���Ϊ -1����ʾû�����ӱ�ѡ�С�

    // ��ʷ��¼������ʵ�ֳ��� (undo) ���ܡ�
    std::vector<std::vector<std::vector<int>>> boardHistory; // �洢ÿһ����������״̬ (�������ݵĶ�ά�����ļ���)��
    std::vector<int> selectedXHistory, selectedYHistory;   // �ֱ�洢ÿһ����ʱ selectedX �� selectedY ��ֵ��

    std::vector<std::vector<int>> initialBoard; // �洢���̵ĳ�ʼ״̬�����ڡ����¿�ʼ���򡰻ָ���ʼ���̡����ܡ�

    DWORD lastAIUpdateTime; // ��һ�� AI ���»�����ʱ��� (DWORD �� Windows API �е��������ͣ�ͨ�����޷���32λ����)��
    // �������ڿ��� AI �����Ƶ�ʻ�ʱ��

// ������صĳ�Ա����������ʵ�������ƶ���ƽ������Ч����
    bool animating;        // ��ǵ�ǰ�Ƿ����ڲ��������ƶ�������
    int animFromX, animFromY; // ������ʼ��λ�����ꡣ
    int animToX, animToY;   // ����Ŀ����λ�����ꡣ
    int animMidX, animMidY; // �����м�㣨�����������ӣ������ꡣ
    DWORD animStartTime;   // ������ʼ��ʱ�����
    int animDuration;      // �����ƶ��������ܳ���ʱ�䣨���룩��

    AIHelper aiHelper;     // AI ���ֶ������ڼ�������ƶ����ṩ��ʾ��

    AIHintState aiHintCurrentState; // ��ǰ AI ��ʾ���ܵ�״̬ (IDLE, WORKING, SHOWING_HINT)��
    AIMove currentAIHintMove;       // �洢�� AI ������ĵ�ǰ��ʾ�ƶ���

    // --- ˽�г�Ա���� ---
    // ��Щ������ Game ����ڲ�������������ֱ�ӱ�¶���ⲿ��

    // ���Ƶ�ǰ��Ϸ���棨�������������е�״̬����
    // �������̡����ӡ��÷֡���ť�Ⱦ�̬Ԫ�ء�
    void drawGame();

    // ���ƴ��ж���Ч������Ϸ���档
    // ���������ƶ������ж�̬��������λ�ã�ʵ��ƽ�����ɡ�
    void drawGameWithAnim();

    // �������Ӵ� (x1, y1) �ƶ��� (x2, y2) �Ķ�����
    // �˺��������ö�����صĳ�Ա���������ں����� drawGameWithAnim ������ʵ�ʻ��ƶ���֡��
    void playMoveAnimation(int x1, int y1, int x2, int y2);

    // ���浱ǰ����״̬��ѡ������״̬����ʷ��¼�С�
    // ͨ����ÿ����Ч�ƶ�����á�
    void saveHistory();

    // ������һ���ƶ���
    // ����ʷ��¼�лָ���һ������״̬��ѡ��״̬��
    void undoMove();

    // �����ָ̻�����ʼ���֡�
    void restoreInitialBoard();

    // ���� (x1, y1) �� (x2, y2) �Ƿ�Ϊһ����Ч�Ŀ������ƶ���
    // @param x1, y1: ��ʼλ�á�
    // @param x2, y2: Ŀ��λ�á�
    // @return bool: ����ƶ���Ч�򷵻� true�����򷵻� false��
    bool isValidMove(int x1, int y1, int x2, int y2);

    // ִ��һ�θ����ϵ��ƶ� (��������״̬����������������ʷ��¼)��
    // ʵ�ʵ��ƶ������������õ��м�����ӣ�������ʼλ�õ������ƶ���Ŀ��λ�á�
    // @param x1, y1: ��ʼλ�á�
    // @param x2, y2: Ŀ��λ�á�
    void move(int x1, int y1, int x2, int y2);

    // ��鵱ǰ����״̬���Ƿ񻹴����κ���Ч���ƶ���
    // @return bool: ������ٴ���һ����Ч�ƶ��򷵻� true�����򷵻� false (��Ϸ���ܽ���)��
    bool hasMoves();

    // ���㵱ǰ������ʣ�����ӵ�������
    // @return int: ���������ӵ�������
    int countPegs();
};