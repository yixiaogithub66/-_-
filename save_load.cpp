#include "save_load.h"
#include <graphics.h>
#include <tchar.h>
#include <fstream>
#include <direct.h> // ���ڴ���Ŀ¼
#include <io.h>     // ���ڼ���ļ��Ƿ����

SaveManager::SaveManager() {
    // ��ʼ���浵Ŀ¼
    saveDir = ".\\saves\\";
    // ȷ��Ŀ¼����
    if (_access(saveDir.c_str(), 0) == -1) {
        _mkdir(saveDir.c_str());
    }
}

bool SaveManager::saveGame(const Board& board,
    int selectedX, int selectedY,
    const std::vector<std::vector<std::vector<int>>>& history,
    const std::vector<int>& selXHistory,
    const std::vector<int>& selYHistory,
    const std::string& filename) {
    std::string fullPath = saveDir + filename;
    std::ofstream file(fullPath, std::ios::binary);

    if (!file.is_open()) {
        return false;
    }

    // ������������
    BoardType type = board.getType();
    file.write(reinterpret_cast<const char*>(&type), sizeof(type));

    // �������̳ߴ�
    int sizeX = board.sizeX();
    int sizeY = board.sizeY();
    file.write(reinterpret_cast<const char*>(&sizeX), sizeof(sizeX));
    file.write(reinterpret_cast<const char*>(&sizeY), sizeof(sizeY));

    // ��������״̬
    const auto& data = board.data();
    for (int i = 0; i < sizeX; i++) {
        for (int j = 0; j < sizeY; j++) {
            int val = data[i][j];
            file.write(reinterpret_cast<const char*>(&val), sizeof(val));
        }
    }

    // ����ѡ��״̬
    file.write(reinterpret_cast<const char*>(&selectedX), sizeof(selectedX));
    file.write(reinterpret_cast<const char*>(&selectedY), sizeof(selectedY));

    // ������ʷ��¼����
    int historyCount = (int)history.size();
    file.write(reinterpret_cast<const char*>(&historyCount), sizeof(historyCount));

    // ������ʷ��¼
    for (const auto& histBoard : history) {
        for (int i = 0; i < sizeX; i++) {
            for (int j = 0; j < sizeY; j++) {
                int val = histBoard[i][j];
                file.write(reinterpret_cast<const char*>(&val), sizeof(val));
            }
        }
    }

    // ������ʷѡ��״̬
    for (int i = 0; i < historyCount; i++) {
        file.write(reinterpret_cast<const char*>(&selXHistory[i]), sizeof(int));
        file.write(reinterpret_cast<const char*>(&selYHistory[i]), sizeof(int));
    }

    file.close();
    return true;
}

bool SaveManager::loadGame(Board& board,
    int& selectedX, int& selectedY,
    std::vector<std::vector<std::vector<int>>>& history,
    std::vector<int>& selXHistory,
    std::vector<int>& selYHistory,
    BoardType& boardType,
    const std::string& filename) {
    std::string fullPath = saveDir + filename;
    std::ifstream file(fullPath, std::ios::binary);

    if (!file.is_open()) {
        return false;
    }

    // ��ȡ��������
    BoardType type;
    file.read(reinterpret_cast<char*>(&type), sizeof(type));
    boardType = type;

    // ����������
    board = Board(type);

    // ��ȡ���̳ߴ�
    int sizeX, sizeY;
    file.read(reinterpret_cast<char*>(&sizeX), sizeof(sizeX));
    file.read(reinterpret_cast<char*>(&sizeY), sizeof(sizeY));

    // ��ȡ����״̬
    auto& data = board.data();
    for (int i = 0; i < sizeX; i++) {
        for (int j = 0; j < sizeY; j++) {
            int val;
            file.read(reinterpret_cast<char*>(&val), sizeof(val));
            data[i][j] = val;
        }
    }

    // ��ȡѡ��״̬
    file.read(reinterpret_cast<char*>(&selectedX), sizeof(selectedX));
    file.read(reinterpret_cast<char*>(&selectedY), sizeof(selectedY));

    // ��ȡ��ʷ��¼����
    int historyCount;
    file.read(reinterpret_cast<char*>(&historyCount), sizeof(historyCount));

    // ��վ���ʷ
    history.clear();
    selXHistory.clear();
    selYHistory.clear();

    // ��ȡ��ʷ��¼
    for (int h = 0; h < historyCount; h++) {
        std::vector<std::vector<int>> histBoard(sizeX, std::vector<int>(sizeY));
        for (int i = 0; i < sizeX; i++) {
            for (int j = 0; j < sizeY; j++) {
                int val;
                file.read(reinterpret_cast<char*>(&val), sizeof(val));
                histBoard[i][j] = val;
            }
        }
        history.push_back(histBoard);
    }

    // ��ȡ��ʷѡ��״̬
    for (int i = 0; i < historyCount; i++) {
        int x, y;
        file.read(reinterpret_cast<char*>(&x), sizeof(int));
        file.read(reinterpret_cast<char*>(&y), sizeof(int));
        selXHistory.push_back(x);
        selYHistory.push_back(y);
    }

    file.close();
    return true;
}

bool SaveManager::saveExists(const std::string& filename) const {
    std::string fullPath = saveDir + filename;
    return _access(fullPath.c_str(), 0) != -1;
}

BoardType saveLoadMenu(BoardType currentType) {
    SaveManager saveManager;
    static Board savedBoard(CROSS);  // Ĭ�ϴ���һ������
    static int savedX = -1, savedY = -1;
    static std::vector<std::vector<std::vector<int>>> savedHistory;
    static std::vector<int> savedXHistory, savedYHistory;
    static BoardType savedBoardType = CROSS;

    // ɾ��isSaveMode������ʼ��Ϊ��ȡģʽ

    setbkcolor(RGB(236, 246, 255));
    cleardevice();

    settextstyle(45, 0, _T("΢���ź�"));
    settextcolor(RGB(50, 50, 120));
    outtextxy(300, 60, _T("�浵����"));

    // ɾ��ģʽ�л���ť��ֱ�ӻ��ƴ浵��λ
    settextstyle(26, 0, _T("΢���ź�"));
    RECT r1 = { 250, 180, 550, 220 };  // ��λ������
    RECT r2 = { 250, 250, 550, 290 };  // ��λ������
    RECT r3 = { 250, 320, 550, 360 };  // ��λ������

    setfillcolor(RGB(190, 210, 240));
    fillroundrect(r1.left, r1.top, r1.right, r1.bottom, 15, 15);
    fillroundrect(r2.left, r2.top, r2.right, r2.bottom, 15, 15);
    fillroundrect(r3.left, r3.top, r3.right, r3.bottom, 15, 15);

    // ���浵�Ƿ����
    bool save1Exists = saveManager.saveExists("save1.dat");
    bool save2Exists = saveManager.saveExists("save2.dat");
    bool save3Exists = saveManager.saveExists("save3.dat");

    TCHAR slot1[32], slot2[32], slot3[32];
    _stprintf_s(slot1, _T("�浵λ�� 1 %s"), save1Exists ? _T("(�Ѵ���)") : _T("(��)"));
    _stprintf_s(slot2, _T("�浵λ�� 2 %s"), save2Exists ? _T("(�Ѵ���)") : _T("(��)"));
    _stprintf_s(slot3, _T("�浵λ�� 3 %s"), save3Exists ? _T("(�Ѵ���)") : _T("(��)"));

    settextcolor(RGB(40, 40, 90));
    drawtext(slot1, &r1, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    drawtext(slot2, &r2, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    drawtext(slot3, &r3, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    // ���ذ�ť
    RECT rBack = { 300, 420, 500, 460 };  // ��λ������
    setfillcolor(RGB(180, 190, 230));
    fillroundrect(rBack.left, rBack.top, rBack.right, rBack.bottom, 15, 15);
    drawtext(_T("�������˵�"), &rBack, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    bool loadedGame = false;

    while (true) {
        ExMessage m;
        while (peekmessage(&m, EM_MOUSE)) {
            if (m.message == WM_LBUTTONDOWN) {
                int x = m.x, y = m.y;

                // ɾ���л�ģʽ�Ĵ���

                // ����浵��λ1
                if (x >= r1.left && x <= r1.right && y >= r1.top && y <= r1.bottom) {
                    if (save1Exists) {
                        // ��ȡ��Ϸ
                        if (saveManager.loadGame(savedBoard, savedX, savedY, savedHistory,
                            savedXHistory, savedYHistory, savedBoardType, "save1.dat")) {
                            outtextxy(300, 380, _T("��ȡ�ɹ���"));  // ����λ��
                            loadedGame = true;
                            currentType = savedBoardType;
                        }
                    }
                }
                // ����浵��λ2
                else if (x >= r2.left && x <= r2.right && y >= r2.top && y <= r2.bottom) {
                    if (save2Exists) {
                        // ��ȡ��Ϸ
                        if (saveManager.loadGame(savedBoard, savedX, savedY, savedHistory,
                            savedXHistory, savedYHistory, savedBoardType, "save2.dat")) {
                            outtextxy(300, 380, _T("��ȡ�ɹ���"));  // ����λ��
                            loadedGame = true;
                            currentType = savedBoardType;
                        }
                    }
                }
                // ����浵��λ3
                else if (x >= r3.left && x <= r3.right && y >= r3.top && y <= r3.bottom) {
                    if (save3Exists) {
                        // ��ȡ��Ϸ
                        if (saveManager.loadGame(savedBoard, savedX, savedY, savedHistory,
                            savedXHistory, savedYHistory, savedBoardType, "save3.dat")) {
                            outtextxy(300, 380, _T("��ȡ�ɹ���"));  // ����λ��
                            loadedGame = true;
                            currentType = savedBoardType;
                        }
                    }
                }
                // �������˵�
                else if (x >= rBack.left && x <= rBack.right && y >= rBack.top && y <= rBack.bottom) {
                    return currentType;
                }
            }
        }
        Sleep(10);
    }
}