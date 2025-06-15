#include "save_load.h" // �����Զ���Ĵ浵/��������ͷ�ļ�������Ӧ���� SaveManager ��������� saveLoadMenu ������������
#include <graphics.h>  // ���� EasyX ͼ�ο��ͷ�ļ������ڴ����͹���浵�˵���ͼ���û����档
#include <tchar.h>     // ����ͨ���ַ�����ͷ�ļ�������֧�� _T() ��� TCHAR ���ͣ���ʵ�� ANSI �� Unicode �ַ����ļ����ԡ�
#include <fstream>     // �����ļ����⣬���ڽ����ļ��Ķ�д���� (���� std::ofstream, std::ifstream)��
#include <string>      // �����ַ����⣬ȷ�� std::string ���Ϳ��á�
#include <vector>      // ���������⣬ȷ�� std::vector ���Ϳ��ã����ڴ洢�������ݺ���ʷ��¼��
#include <direct.h>    // ����Ŀ¼�����⣬���ڴ���Ŀ¼ (���� _mkdir ����)��
#include <io.h>        // ���� IO �����⣬���ڼ���ļ��Ƿ���� (���� _access ����)��
#include <iomanip>     // ���� IO ���ݷ��⣬���ڸ�ʽ����� (���� std::setw���ڴ˴����е�ǰδʹ�ã�������������Ϊ��δ����չ)��

// SaveManager ��Ĺ��캯����
// ������ SaveManager ����ʱ���á�
SaveManager::SaveManager() {
    // ��ʼ���浵�ļ��Ĵ洢Ŀ¼��
    saveDir = ".\\saves\\"; // ���ô浵Ŀ¼Ϊ��ǰ��ִ���ļ�·���µ� "saves" �ļ��С�
    // ".\\" ��ʾ��ǰĿ¼��
// ȷ���浵Ŀ¼���ڣ�����������򴴽�����
// _access �������ڼ���ļ���Ŀ¼�ķ���Ȩ�ޡ�
// saveDir.c_str() �� std::string ת��Ϊ C ����ַ�����
// 0��Ϊ�ڶ���������ʾ����ļ���Ŀ¼�Ƿ���ڡ�
    if (_access(saveDir.c_str(), 0) == -1) { // ��� _access ���� -1����ʾĿ¼�����ڻ��޷����ʡ�
        _mkdir(saveDir.c_str()); // ʹ�� _mkdir ����Ŀ¼��
    }
}

// ���浱ǰ��Ϸ״̬��ָ���ļ���
// @param board: ��ǰ����Ϸ���̶���
// @param selectedX, selectedY: ��ǰѡ�е����ӵ��߼����ꡣ
// @param history: ����״̬����ʷ��¼��
// @param selXHistory, selYHistory: ѡ�������������ʷ��¼��
// @param filename: �浵�ļ������� (���� "save1.txt")��
// @return bool: �������ɹ��򷵻� true�����򷵻� false��
bool SaveManager::saveGame(const Board& board,
    int selectedX, int selectedY,
    const std::vector<std::vector<std::vector<int>>>& history,
    const std::vector<int>& selXHistory,
    const std::vector<int>& selYHistory,
    const std::string& filename) {
    std::string fullPath = saveDir + filename; // ���������Ĵ浵�ļ�·�� (���� ".\\saves\\save1.txt")��
    std::ofstream file(fullPath); // ���ı�ģʽ (Ĭ��) ���ļ�����д�롣
    // ����ļ��Ѵ��ڣ����ݻᱻ���ǡ�

    if (!file.is_open()) { // ����ļ��Ƿ�ɹ��򿪡�
        return false; // ����ļ���ʧ�ܣ��򷵻� false��
    }

    // 1. �����������͡�
    // static_cast<int>(board.getType()) �� BoardType ö��ת��Ϊ�������д洢��
    file << static_cast<int>(board.getType()) << std::endl; // д���������ͣ������С�

    // 2. �������̵�ʵ�ʳߴ� (����������)��
    int bSizeX = board.sizeX(); // ��ȡ���̵�������
    int bSizeY = board.sizeY(); // ��ȡ���̵����� (ͨ���ǵ�һ�е��������������)��
    file << bSizeX << " " << bSizeY << std::endl; // д���������������ÿո�����������С�

    // 3. ���浱ǰ���̵�״̬���ݡ�
    const auto& boardData = board.data(); // ��ȡ�������ڲ����ݵĳ������ã����⸴�ơ�
    for (int i = 0; i < bSizeX; ++i) { // ����ÿһ�С�
        for (int j = 0; j < bSizeY; ++j) { // ����ÿһ�С�
            file << boardData[i][j] << (j == bSizeY - 1 ? "" : " "); // д�뵥Ԫ���ֵ��
            // ���������β������ֵ���һ���ո񣻷��򲻼ӡ�
        }
        file << std::endl; // ÿ�н������С�
    }

    // 4. ���浱ǰѡ�е�����״̬��
    file << selectedX << " " << selectedY << std::endl; // д��ѡ�����ӵ�X��Y���꣬�����С�

    // 5. ������ʷ��¼��������
    file << history.size() << std::endl; // д����ʷ��¼����Ŀ���������С�

    // 6. ����ÿһ����ʷ��¼�е�����״̬��
    // ������ʷ��¼�е����̳ߴ��뵱ǰ���̵� bSizeX, bSizeY һ�¡�
    for (const auto& histBoardState : history) { // ������ʷ��¼�е�ÿ������״̬��
        for (int i = 0; i < bSizeX; ++i) { // �����С�
            for (int j = 0; j < bSizeY; ++j) { // �����С�
                file << histBoardState[i][j] << (j == bSizeY - 1 ? "" : " "); // д����ʷ�������ݡ�
            }
            file << std::endl; // ÿ����ʷ���ݽ������С�
        }
    }

    // 7. ����ÿһ����ʷ��¼�е�ѡ������״̬��
    // history.size() Ӧ���� selXHistory.size() �� selYHistory.size() ��ȡ�
    for (size_t i = 0; i < history.size(); ++i) {
        file << selXHistory[i] << " " << selYHistory[i] << std::endl; // д����ʷѡ��״̬��
    }

    file.close(); // �ر��ļ���
    return true; // ����ɹ������� true��
}

// ��ָ���ļ�������Ϸ״̬��
// @param board: ���ڽ��ռ��ص�����״̬�� Board ���� (�ᱻ�޸�)��
// @param selectedX, selectedY: ���ڽ��ռ��ص�ѡ����������ı��� (�ᱻ�޸�)��
// @param history: ���ڽ��ռ��ص�������ʷ��¼������ (�ᱻ�޸�)��
// @param selXHistory, selYHistory: ���ڽ��ռ��ص�ѡ����ʷ��¼������ (�ᱻ�޸�)��
// @param boardType: ���ڽ��ռ��ص��������͵ı��� (�ᱻ�޸�)��
// @param filename: �浵�ļ������� (���� "save1.txt")��
// @return bool: ������سɹ��򷵻� true�����򷵻� false��
bool SaveManager::loadGame(Board& board,
    int& selectedX, int& selectedY,
    std::vector<std::vector<std::vector<int>>>& history,
    std::vector<int>& selXHistory,
    std::vector<int>& selYHistory,
    BoardType& boardType,
    const std::string& filename) {
    std::string fullPath = saveDir + filename; // ���������Ĵ浵�ļ�·����
    std::ifstream file(fullPath); // ���ı�ģʽ���ļ����ڶ�ȡ��

    if (!file.is_open()) { // ����ļ��Ƿ�ɹ��򿪡�
        return false; // ����ļ���ʧ�ܣ��򷵻� false��
    }

    // 1. ��ȡ�������͡�
    int typeInt;
    if (!(file >> typeInt)) { file.close(); return false; } // ���ļ���ȡ�����������ȡʧ�ܣ��ر��ļ������� false��
    boardType = static_cast<BoardType>(typeInt); // ����ȡ������ת���� BoardType ö�١�

    // 2. ���ݶ�ȡ�������������´��� Board ����
    // �����Ҫ����Ϊ Board �Ĺ��캯����������ͳ�ʼ�����ڲ��ṹ�ͳߴ硣
    board = Board(boardType);

    // 3. ��ȡ���̳ߴ� (��Щ�ߴ���Ҫ���ں�����ȡ��ʷ��¼ʱȷ��ά��)��
    // �����ϣ���Щ�ߴ�Ӧ�����´����� `board` ����� `board.sizeX()` �� `board.sizeY()` ��ƥ�䡣
    int fileBoardSizeX, fileBoardSizeY;
    if (!(file >> fileBoardSizeX >> fileBoardSizeY)) { file.close(); return false; } // ��ȡ�ߴ�ʧ�ܡ�

    // ��ѡ����֤���裺����ļ��д洢�ĳߴ��Ƿ������ boardType �´��������̶���ĳߴ�һ�¡�
    // if (fileBoardSizeX != board.sizeX() || fileBoardSizeY != board.sizeY()) {
    //     // ����ߴ粻ƥ�䣬������ζ�Ŵ浵�ļ����𻵣������������͵Ķ�����浵ʱ�Ĳ�һ�¡�
    //     file.close();
    //     return false;
    // }

    // 4. ��ȡ��ǰ���̵�״̬���ݡ�
    // ��ȡ�� `board` �����ڲ����ݵ����ã��Ա�ֱ���޸�����
    auto& boardDataRef = board.data();
    // ��ʱ��boardDataRef Ӧ���Ѿ��� Board(boardType) ���캯������������ȷ��ʼ���������˴�С��
    // ʹ�� `board.sizeX()` �� `board.sizeY()` ��ȷ�����Ƕ�ȡ���������뵱ǰ���̶���Ľṹ�����
    for (int i = 0; i < board.sizeX(); ++i) {
        for (int j = 0; j < board.sizeY(); ++j) {
            if (!(file >> boardDataRef[i][j])) { // ��ȡÿ����Ԫ���ֵ��
                file.close(); return false; // ��ȡ��������ʧ�ܡ�
            }
        }
    }

    // 5. ��ȡ��ǰѡ�е�����״̬��
    if (!(file >> selectedX >> selectedY)) { file.close(); return false; } // ��ȡʧ�ܡ�

    // 6. ��ȡ��ʷ��¼��������
    int historyCount;
    if (!(file >> historyCount)) { file.close(); return false; } // ��ȡʧ�ܡ�

    // 7. ��վɵ���ʷ��¼����Ϊ�µ���ʷ��¼Ԥ����ռ������Ч�ʡ�
    history.clear();
    selXHistory.clear();
    selYHistory.clear();
    history.reserve(historyCount);
    selXHistory.reserve(historyCount);
    selYHistory.reserve(historyCount);

    // 8. ��ȡÿһ����ʷ��¼�е�����״̬��
    // ����ʹ�ô��ļ��ж�ȡ�� `fileBoardSizeX` �� `fileBoardSizeY` ��������ʱ�� `histBoardState`��
    // ��Ϊ��ʷ��¼�е����̳ߴ�����뵱ǰ��Ϸ��ѭ���� Board ����Ķ�̬�ߴ粻ͬ������ͨ��Ӧ����ͬ����
    for (int h = 0; h < historyCount; ++h) {
        std::vector<std::vector<int>> histBoardState(fileBoardSizeX, std::vector<int>(fileBoardSizeY)); // ����һ����ʷ����״̬��2D������
        for (int i = 0; i < fileBoardSizeX; ++i) {
            for (int j = 0; j < fileBoardSizeY; ++j) {
                if (!(file >> histBoardState[i][j])) { // ��ȡ��ʷ���̵ĵ�Ԫ�����ݡ�
                    file.close(); return false; // ��ȡʧ�ܡ�
                }
            }
        }
        history.push_back(histBoardState); // ����ȡ������ʷ����״̬��ӵ� history �����С�
    }

    // 9. ��ȡÿһ����ʷ��¼�е�ѡ������״̬��
    for (int i = 0; i < historyCount; ++i) {
        int histSelX, histSelY;
        if (!(file >> histSelX >> histSelY)) { // ��ȡ��ʷѡ��״̬��
            file.close(); return false; // ��ȡʧ�ܡ�
        }
        selXHistory.push_back(histSelX);
        selYHistory.push_back(histSelY);
    }

    file.close(); // �ر��ļ���
    return true; // ���سɹ������� true��
}

// ���ָ���Ĵ浵�ļ��Ƿ���ڡ�
// 'const' ��ʾ�˷��������޸� SaveManager ������κγ�Ա������
// @param filename: Ҫ���Ĵ浵�ļ������� (���� "save1.txt")��
// @return bool: ����ļ������򷵻� true�����򷵻� false��
bool SaveManager::saveExists(const std::string& filename) const {
    std::string fullPath = saveDir + filename; // �����������ļ�·����
    // _access(path, 0) ����ļ��Ƿ���ڡ�����ļ������ҿɷ��ʣ�ģʽ0ֻ�������ԣ�������0�����򷵻�-1��
    return _access(fullPath.c_str(), 0) != -1;
}

// �浵/�����˵�������
// @param currentType: ��ǰ��Ϸ��ѭ���е��������͡��������������Ϸ�������ͻᱻ���¡�
// @return BoardType: ���ز�����ɺ���������ͣ�������ԭʼ���ͣ�Ҳ�����Ǽ��ص����ͣ���
BoardType saveLoadMenu(BoardType currentType) {
    SaveManager saveManager; // ���� SaveManager ������ʹ����浵/�������ܡ�

    // ʹ�þ�̬�������洢���ص���Ϸ״̬��
    // ����������Ϊ saveLoadMenu �������غ�����û�ѡ�������Ϸ��
    // ����Ϸ�߼���Ҫһ�ַ�ʽ����ȡ��Щ���ص����ݡ���̬�����ں������ý�������Ȼ������ֵ��
    // ȱ������Щ������ȫ�ֿɷ��ʵģ�ͨ���ٴε��ô˺��������Ҳ����̰߳�ȫ�ġ�
    // ��һ�ַ����ǽ���Щ������Ϊ���ò������ݸ� saveLoadMenu�������ú�������һ����������״̬�Ľṹ�塣
    static Board savedBoard(CROSS); // ��̬���̶���Ĭ�ϳ�ʼ��Ϊ CROSS ���͡����ڴ洢���ص����̡�
    static int savedX = -1, savedY = -1; // ��̬�������洢���ص�ѡ���������ꡣ
    static std::vector<std::vector<std::vector<int>>> savedHistory; // �洢���ص�������ʷ��
    static std::vector<int> savedXHistory, savedYHistory;       // �洢���ص�ѡ����ʷ��
    static BoardType savedBoardType = CROSS; // ��̬���������ڴ洢���ļ��м��ص��������͡�

    // ���ò˵�����ı�����ɫ��
    setbkcolor(RGB(236, 246, 255)); // ����ɫ������
    cleardevice(); // ������Ӧ�ñ���ɫ��

    // ���Ʋ˵����⡣
    settextstyle(45, 0, _T("΢���ź�")); // ����������ʽ��
    settextcolor(RGB(50, 50, 120));   // ���ñ�����ɫ (����ɫ)��
    outtextxy(300, 60, _T("�浵����")); // ��������ı���

    // ����浵��λ��ť�ľ�������
    settextstyle(26, 0, _T("΢���ź�")); // ���ð�ť�ı����塣
    RECT r1 = { 250, 180, 550, 220 }; // �浵�� 1
    RECT r2 = { 250, 250, 550, 290 }; // �浵�� 2
    RECT r3 = { 250, 320, 550, 360 }; // �浵�� 3

    // ���ƴ浵��λ��ť�ı�����
    setfillcolor(RGB(190, 210, 240)); // ��ť�����ɫ (ǳ����ɫ)��
    fillroundrect(r1.left, r1.top, r1.right, r1.bottom, 15, 15); // Բ�Ǿ��Ρ�
    fillroundrect(r2.left, r2.top, r2.right, r2.bottom, 15, 15);
    fillroundrect(r3.left, r3.top, r3.right, r3.bottom, 15, 15);

    // ���ÿ���浵�۶�Ӧ���ļ��Ƿ���ڡ�
    bool save1Exists = saveManager.saveExists("save1.txt"); // ʹ�� ".txt" ��׺���� saveGame/loadGame ��һ�¡�
    bool save2Exists = saveManager.saveExists("save2.txt");
    bool save3Exists = saveManager.saveExists("save3.txt");

    // ׼���ڰ�ť����ʾ���ı���ָʾ�浵���Ƿ�Ϊ�ա�
    TCHAR slot1[64], slot2[64], slot3[64]; // �ı�����������СӦ�㹻��
    // ʹ�� _stprintf_s ��ȫ�ظ�ʽ���ַ�����
    _stprintf_s(slot1, sizeof(slot1) / sizeof(TCHAR), _T("�浵λ�� 1 %s"), save1Exists ? _T("(�Ѵ���)") : _T("(��)"));
    _stprintf_s(slot2, sizeof(slot2) / sizeof(TCHAR), _T("�浵λ�� 2 %s"), save2Exists ? _T("(�Ѵ���)") : _T("(��)"));
    _stprintf_s(slot3, sizeof(slot3) / sizeof(TCHAR), _T("�浵λ�� 3 %s"), save3Exists ? _T("(�Ѵ���)") : _T("(��)"));

    // �ڰ�ť�ϻ����ı���
    settextcolor(RGB(40, 40, 90)); // �ı���ɫ (����ɫ)��
    drawtext(slot1, &r1, DT_CENTER | DT_VCENTER | DT_SINGLELINE); // �ı�������ʾ��
    drawtext(slot2, &r2, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    drawtext(slot3, &r3, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    // ���ơ��������˵�����ť��
    RECT rBack = { 300, 420, 500, 460 };
    setfillcolor(RGB(180, 190, 230)); // ��ť��ɫ��
    fillroundrect(rBack.left, rBack.top, rBack.right, rBack.bottom, 15, 15);
    drawtext(_T("�������˵�"), &rBack, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    // �������û���ʾ���سɹ�/ʧ�ܵ���Ϣ�ı�����
    TCHAR loadMessage[100] = _T(""); // ��Ϣ�ı�����������ʼΪ�ա�
    bool messageIsError = false;     // �����Ϣ�Ƿ�Ϊ������Ϣ (�������ò�ͬ��ɫ)��

    // ���¼�ѭ���������û����룬ֱ���û�ѡ�񷵻ء�
    while (true) {
        // ��ÿ��ѭ����ʼʱ��������ܴ��ڵľ���Ϣ�����ػ水ť���ı��������Ϣ���������ǣ���
        // ����ͨ�����ض�������䱳��ɫ�������Ϣ��
        setfillcolor(RGB(236, 246, 255)); // ʹ�ñ���ɫ��
        RECT messageRect = { 200, 380, 600, 410 }; // ������Ϣ��ʾ�ľ�������
        bar(messageRect.left, messageRect.top, messageRect.right, messageRect.bottom); // �ñ���ɫ�����������������Ϣ��

        if (_tcslen(loadMessage) > 0) { // ��� loadMessage �������������ݣ�����ʾ��Ϣ��
            settextstyle(20, 0, _T("΢���ź�")); // ������Ϣ�ı����塣
            settextcolor(messageIsError ? RED : RGB(0, 128, 0)); // ������Ϣ�ú�ɫ���ɹ���Ϣ����ɫ��
            // ������Ϣ�ı��� X ������ʹ��ˮƽ���С�
            outtextxy((getwidth() - textwidth(loadMessage)) / 2, 385, loadMessage);
        }

        ExMessage m; // EasyX ��Ϣ�ṹ�塣
        // ������ͼ�����Ϣ��peekmessage �Ƿ������ġ�
        // ����˵�Ӧ����ģ̬�ģ�����ѡ��ǰ��ִ��������������getmessage�����������ܸ����ʡ�
        if (peekmessage(&m, EM_MOUSE | EM_KEY)) {
            if (m.message == WM_LBUTTONDOWN) { // ����������������¼���
                int clickX = m.x; // ��ȡ����� X ���ꡣ
                int clickY = m.y; // ��ȡ����� Y ���ꡣ
                bool actionTaken = false; // ��Ǳ��ε���Ƿ��Ѵ���

                // ����Դ浵�� 1 �ĵ����
                if (clickX >= r1.left && clickX <= r1.right && clickY >= r1.top && clickY <= r1.bottom) {
                    actionTaken = true;
                    if (save1Exists) { // ����浵�ļ����ڣ����Լ��ء�
                        if (saveManager.loadGame(savedBoard, savedX, savedY, savedHistory,
                            savedXHistory, savedYHistory, savedBoardType, "save1.txt")) {
                            _tcscpy_s(loadMessage, _T("�浵 1 ���سɹ���")); // ���óɹ���Ϣ��
                            messageIsError = false;
                            currentType = savedBoardType; // ���µ�ǰ�������ͣ��Ա㺯������ʱ�ⲿ֪��������ʲô���͡�
                            // ��Ϸ״̬�Ѽ��ص���̬���� savedBoard, savedX, savedY, savedHistory �С�
                            // ���ô˲˵����ⲿ������Ҫ����Щ��̬�����л�ȡ���ص���Ϸ���ݡ�
                        }
                        else {
                            _tcscpy_s(loadMessage, _T("�浵 1 ����ʧ�ܣ�")); // ����ʧ����Ϣ��
                            messageIsError = true;
                        }
                    }
                    else {
                        _tcscpy_s(loadMessage, _T("�浵 1 Ϊ�ա�")); // �浵��Ϊ�ա�
                        messageIsError = true; // ������Ϊ true �� false��ȡ�����Ƿ���Ϊ�մ���
                    }
                }
                // ����浵�� 2 �ĵ�� (�߼����λ1����)��
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
                // ����浵�� 3 �ĵ�� (�߼����λ1����)��
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
                // �����������˵�����ť�ĵ����
                else if (clickX >= rBack.left && clickX <= rBack.right && clickY >= rBack.top && clickY <= rBack.bottom) {
                    return currentType; // ֱ�ӷ��ص�ǰ���������� (���֮ǰ���سɹ���currentType ���Ǽ��ص�����)��
                }

                // ����������κε������ (actionTaken Ϊ true)��
                // ��Ϣ�ı� (loadMessage) �Ѿ������£���������һ��ѭ�������Ŀ�ʼ�������ơ�
            } // ���� WM_LBUTTONDOWN ����
        } // ���� peekmessage ����
        Sleep(10); // �������ߣ��Խ��� CPU ʹ���ʡ�
    } // ������ѭ�� (while(true))��
}