#include "save_load.h"
#include <graphics.h>
#include <tchar.h>
#include <fstream>
#include <string> // ȷ������ string �� vector
#include <vector>
#include <direct.h> // ���ڴ���Ŀ¼
#include <io.h>     // ���ڼ���ļ��Ƿ����
#include <iomanip>  // ���� std::setw ��������ʽ���������Ҫ����ǰδʹ�ã�

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
    const std::string& filename) { // ���� "save1.txt"
    std::string fullPath = saveDir + filename;
    std::ofstream file(fullPath); // ���ı�ģʽ��

    if (!file.is_open()) {
        return false;
    }

    // ������������
    file << static_cast<int>(board.getType()) << std::endl;

    // �������̳ߴ�
    int bSizeX = board.sizeX();
    int bSizeY = board.sizeY();
    file << bSizeX << " " << bSizeY << std::endl;

    // ��������״̬
    const auto& boardData = board.data();
    for (int i = 0; i < bSizeX; ++i) {
        for (int j = 0; j < bSizeY; ++j) {
            file << boardData[i][j] << (j == bSizeY - 1 ? "" : " ");
        }
        file << std::endl;
    }

    // ����ѡ��״̬
    file << selectedX << " " << selectedY << std::endl;

    // ������ʷ��¼����
    file << history.size() << std::endl;

    // ������ʷ��¼
    // ������ʷ��¼�е����̳ߴ��뵱ǰ���̳ߴ�һ��
    for (const auto& histBoardState : history) {
        for (int i = 0; i < bSizeX; ++i) {
            for (int j = 0; j < bSizeY; ++j) {
                file << histBoardState[i][j] << (j == bSizeY - 1 ? "" : " ");
            }
            file << std::endl;
        }
    }

    // ������ʷѡ��״̬
    for (size_t i = 0; i < history.size(); ++i) {
        file << selXHistory[i] << " " << selYHistory[i] << std::endl;
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
    const std::string& filename) { // ���� "save1.txt"
    std::string fullPath = saveDir + filename;
    std::ifstream file(fullPath); // ���ı�ģʽ��

    if (!file.is_open()) {
        return false;
    }

    // ��ȡ��������
    int typeInt;
    if (!(file >> typeInt)) return false; // ��ȡʧ��
    boardType = static_cast<BoardType>(typeInt);

    // ���������� (Board���캯��Ӧ���������������ڲ��ṹ�ͳߴ�)
    board = Board(boardType);

    // ��ȡ���̳ߴ� (��Щ�ߴ�������ʷ��¼����Ӧ��board.sizeX/Y()ƥ��)
    int fileBoardSizeX, fileBoardSizeY;
    if (!(file >> fileBoardSizeX >> fileBoardSizeY)) return false;

    // ��֤��ȡ�ĳߴ��Ƿ������̶���ĳߴ�ƥ�� (��ѡ��������)
    // if (fileBoardSizeX != board.sizeX() || fileBoardSizeY != board.sizeY()) {
    //     // �ߴ粻ƥ�䣬�����Ǵ浵�ļ��𻵻����������Ͷ��岻һ��
    //     file.close();
    //     return false;
    // }

    // ��ȡ����״̬ (ʹ��board����ĳߴ�)
    auto& boardDataRef = board.data(); // ��ȡ�������ڲ����ݵ�����
    // ȷ�� boardDataRef ���� Board(boardType) ���캯����ȷ��ʼ���͵�����С
    for (int i = 0; i < board.sizeX(); ++i) {
        for (int j = 0; j < board.sizeY(); ++j) {
            if (!(file >> boardDataRef[i][j])) {
                file.close();
                return false; // ��ȡ��������ʧ��
            }
        }
    }

    // ��ȡѡ��״̬
    if (!(file >> selectedX >> selectedY)) return false;

    // ��ȡ��ʷ��¼����
    int historyCount;
    if (!(file >> historyCount)) return false;

    // ��վ���ʷ
    history.clear();
    selXHistory.clear();
    selYHistory.clear();
    history.reserve(historyCount);
    selXHistory.reserve(historyCount);
    selYHistory.reserve(historyCount);

    // ��ȡ��ʷ��¼ (ʹ�ô��ļ��ж�ȡ��fileBoardSizeX/Y)
    for (int h = 0; h < historyCount; ++h) {
        std::vector<std::vector<int>> histBoardState(fileBoardSizeX, std::vector<int>(fileBoardSizeY));
        for (int i = 0; i < fileBoardSizeX; ++i) {
            for (int j = 0; j < fileBoardSizeY; ++j) {
                if (!(file >> histBoardState[i][j])) {
                    file.close();
                    return false; // ��ȡ��ʷ��������ʧ��
                }
            }
        }
        history.push_back(histBoardState);
    }

    // ��ȡ��ʷѡ��״̬
    for (int i = 0; i < historyCount; ++i) {
        int histSelX, histSelY;
        if (!(file >> histSelX >> histSelY)) {
            file.close();
            return false; // ��ȡ��ʷѡ��״̬ʧ��
        }
        selXHistory.push_back(histSelX);
        selYHistory.push_back(histSelY);
    }

    file.close();
    return true;
}

bool SaveManager::saveExists(const std::string& filename) const { // ���� "save1.txt"
    std::string fullPath = saveDir + filename;
    return _access(fullPath.c_str(), 0) != -1;
}

BoardType saveLoadMenu(BoardType currentType) {
    SaveManager saveManager;
    // ��Щ��̬���������ڼ�����Ϸ������û���������ʼ��Ϸ���Ƿ������˵���
    // ��Ȼ�ܹ�����Щ�����л�ȡ���ص���Ϸ״̬��
    // ���ߣ�����˵�������Ҫ��ʾ���ص�������Ϣ������Ҳ�����á�
    // ���loadGameֱ���޸�������Ϸѭ���еı�������Щ���ܿ��Լ򻯡�
    // ����ǰ�ṹ�У�����������loadGame��ʵ����Ϸ��ʼ֮�䴫�����ݡ�
    static Board savedBoard(CROSS);
    static int savedX = -1, savedY = -1;
    static std::vector<std::vector<std::vector<int>>> savedHistory;
    static std::vector<int> savedXHistory, savedYHistory;
    static BoardType savedBoardType = CROSS; // ���ڴ洢���ص���������

    setbkcolor(RGB(236, 246, 255));
    cleardevice();

    settextstyle(45, 0, _T("΢���ź�"));
    settextcolor(RGB(50, 50, 120));
    outtextxy(300, 60, _T("�浵����"));

    settextstyle(26, 0, _T("΢���ź�"));
    RECT r1 = { 250, 180, 550, 220 };
    RECT r2 = { 250, 250, 550, 290 };
    RECT r3 = { 250, 320, 550, 360 };

    setfillcolor(RGB(190, 210, 240));
    fillroundrect(r1.left, r1.top, r1.right, r1.bottom, 15, 15);
    fillroundrect(r2.left, r2.top, r2.right, r2.bottom, 15, 15);
    fillroundrect(r3.left, r3.top, r3.right, r3.bottom, 15, 15);

    // ���浵�Ƿ���� (ʹ�� .txt)
    bool save1Exists = saveManager.saveExists("save1.txt");
    bool save2Exists = saveManager.saveExists("save2.txt");
    bool save3Exists = saveManager.saveExists("save3.txt");

    TCHAR slot1[64], slot2[64], slot3[64]; // ���ӻ�������С�Է���һ
    _stprintf_s(slot1, sizeof(slot1) / sizeof(TCHAR), _T("�浵λ�� 1 %s"), save1Exists ? _T("(�Ѵ���)") : _T("(��)"));
    _stprintf_s(slot2, sizeof(slot2) / sizeof(TCHAR), _T("�浵λ�� 2 %s"), save2Exists ? _T("(�Ѵ���)") : _T("(��)"));
    _stprintf_s(slot3, sizeof(slot3) / sizeof(TCHAR), _T("�浵λ�� 3 %s"), save3Exists ? _T("(�Ѵ���)") : _T("(��)"));

    settextcolor(RGB(40, 40, 90));
    drawtext(slot1, &r1, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    drawtext(slot2, &r2, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    drawtext(slot3, &r3, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    RECT rBack = { 300, 420, 500, 460 };
    setfillcolor(RGB(180, 190, 230));
    fillroundrect(rBack.left, rBack.top, rBack.right, rBack.bottom, 15, 15);
    drawtext(_T("�������˵�"), &rBack, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    // �������û���ʾ���سɹ�/ʧ�ܵ���Ϣ
    TCHAR loadMessage[100] = _T("");
    bool messageIsError = false;

    // ��ѭ����ֱ���û�ѡ�񷵻�
    // ע�⣺������������������Ϸ�����᷵�ؼ��ص��������� (currentType = savedBoardType)
    // ʵ�ʵ���Ϸ״̬ (savedBoard, savedX, etc.) ���洢�ھ�̬�����У�
    // ���ô˺����Ĵ��루����������Ϸѭ������Ϸ���ò��֣���Ҫ����Щ��̬�����л�ȡ���ص����ݡ�
    while (true) {
        // �������Ϣ���ػ水ť���ı����Է���Ϣ��������
        setfillcolor(RGB(236, 246, 255)); // ����ɫ
        RECT messageRect = { 200, 380, 600, 410 }; // ��Ϣ����
        bar(messageRect.left, messageRect.top, messageRect.right, messageRect.bottom); // �����Ϣ����

        if (_tcslen(loadMessage) > 0) { // �������Ϣ����ʾ
            settextstyle(20, 0, _T("΢���ź�"));
            settextcolor(messageIsError ? RED : RGB(0, 128, 0)); // �����ú�ɫ���ɹ�����ɫ
            outtextxy((getwidth() - textwidth(loadMessage)) / 2, 385, loadMessage);
        }

        ExMessage m;
        // ʹ�� getmessage ��ȡ������Ϣ�������� peekmessage ��æ�ȴ�
        // ��Ϊ�˱���ԭ���߼��ķ��������ԣ������������̨���񣩣����ﱣ�� peekmessage
        // ����˵���ģ̬�ģ�getmessage ������
        if (peekmessage(&m, EM_MOUSE | EM_KEY)) { // ������ͼ�����Ϣ
            if (m.message == WM_LBUTTONDOWN) {
                int clickX = m.x;
                int clickY = m.y;
                bool actionTaken = false; // ����Ƿ����˵��

                // ����浵��λ1
                if (clickX >= r1.left && clickX <= r1.right && clickY >= r1.top && clickY <= r1.bottom) {
                    actionTaken = true;
                    if (save1Exists) {
                        if (saveManager.loadGame(savedBoard, savedX, savedY, savedHistory,
                            savedXHistory, savedYHistory, savedBoardType, "save1.txt")) {
                            _tcscpy_s(loadMessage, _T("�浵 1 ���سɹ���"));
                            messageIsError = false;
                            currentType = savedBoardType; // ���µ�ǰ���������Ա�����
                            // ��Ϸ״̬�Ѽ��ص� savedBoard, savedX, savedY, savedHistory �Ⱦ�̬������
                            // ����Ϸ�߼���Ҫʹ����Щ�������ָ���Ϸ
                        }
                        else {
                            _tcscpy_s(loadMessage, _T("�浵 1 ����ʧ�ܣ�"));
                            messageIsError = true;
                        }
                    }
                    else {
                        _tcscpy_s(loadMessage, _T("�浵 1 Ϊ�ա�"));
                        messageIsError = true; // �� false��ȡ�����Ƿ���Ϊ�մ���
                    }
                }
                // ����浵��λ2
                else if (clickX >= r2.left && clickX <= r2.right && clickY >= r2.top && clickY <= r2.bottom) {
                    actionTaken = true;
                    if (save2Exists) {
                        if (saveManager.loadGame(savedBoard, savedX, savedY, savedHistory,
                            savedXHistory, savedYHistory, savedBoardType, "save2.txt")) {
                            _tcscpy_s(loadMessage, _T("�浵 2 ���سɹ���"));
                            messageIsError = false;
                            currentType = savedBoardType;
                        }
                        else {
                            _tcscpy_s(loadMessage, _T("�浵 2 ����ʧ�ܣ�"));
                            messageIsError = true;
                        }
                    }
                    else {
                        _tcscpy_s(loadMessage, _T("�浵 2 Ϊ�ա�"));
                        messageIsError = true;
                    }
                }
                // ����浵��λ3
                else if (clickX >= r3.left && clickX <= r3.right && clickY >= r3.top && clickY <= r3.bottom) {
                    actionTaken = true;
                    if (save3Exists) {
                        if (saveManager.loadGame(savedBoard, savedX, savedY, savedHistory,
                            savedXHistory, savedYHistory, savedBoardType, "save3.txt")) {
                            _tcscpy_s(loadMessage, _T("�浵 3 ���سɹ���"));
                            messageIsError = false;
                            currentType = savedBoardType;
                        }
                        else {
                            _tcscpy_s(loadMessage, _T("�浵 3 ����ʧ�ܣ�"));
                            messageIsError = true;
                        }
                    }
                    else {
                        _tcscpy_s(loadMessage, _T("�浵 3 Ϊ�ա�"));
                        messageIsError = true;
                    }
                }
                // �������˵�
                else if (clickX >= rBack.left && clickX <= rBack.right && clickY >= rBack.top && clickY <= rBack.bottom) {
                    return currentType; // ���أ������Ѹ��µģ���������
                }

                if (actionTaken) { // ����в������������Ϣ��׼����ʾ����Ϣ
                    // ��Ϣ������һ��ѭ��������ʼʱ����
                }
            } // end WM_LBUTTONDOWN
        } // end peekmessage
        Sleep(10); // ����CPUռ��
    } // end while(true)
}