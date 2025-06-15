#include "save_load.h"
#include <graphics.h>
#include <tchar.h>
#include <fstream>
#include <string> // 确保包含 string 和 vector
#include <vector>
#include <direct.h> // 用于创建目录
#include <io.h>     // 用于检查文件是否存在
#include <iomanip>  // 用于 std::setw 或其他格式化（如果需要，当前未使用）

SaveManager::SaveManager() {
    // 初始化存档目录
    saveDir = ".\\saves\\";
    // 确保目录存在
    if (_access(saveDir.c_str(), 0) == -1) {
        _mkdir(saveDir.c_str());
    }
}

bool SaveManager::saveGame(const Board& board,
    int selectedX, int selectedY,
    const std::vector<std::vector<std::vector<int>>>& history,
    const std::vector<int>& selXHistory,
    const std::vector<int>& selYHistory,
    const std::string& filename) { // 例如 "save1.txt"
    std::string fullPath = saveDir + filename;
    std::ofstream file(fullPath); // 以文本模式打开

    if (!file.is_open()) {
        return false;
    }

    // 保存棋盘类型
    file << static_cast<int>(board.getType()) << std::endl;

    // 保存棋盘尺寸
    int bSizeX = board.sizeX();
    int bSizeY = board.sizeY();
    file << bSizeX << " " << bSizeY << std::endl;

    // 保存棋盘状态
    const auto& boardData = board.data();
    for (int i = 0; i < bSizeX; ++i) {
        for (int j = 0; j < bSizeY; ++j) {
            file << boardData[i][j] << (j == bSizeY - 1 ? "" : " ");
        }
        file << std::endl;
    }

    // 保存选中状态
    file << selectedX << " " << selectedY << std::endl;

    // 保存历史记录数量
    file << history.size() << std::endl;

    // 保存历史记录
    // 假设历史记录中的棋盘尺寸与当前棋盘尺寸一致
    for (const auto& histBoardState : history) {
        for (int i = 0; i < bSizeX; ++i) {
            for (int j = 0; j < bSizeY; ++j) {
                file << histBoardState[i][j] << (j == bSizeY - 1 ? "" : " ");
            }
            file << std::endl;
        }
    }

    // 保存历史选中状态
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
    const std::string& filename) { // 例如 "save1.txt"
    std::string fullPath = saveDir + filename;
    std::ifstream file(fullPath); // 以文本模式打开

    if (!file.is_open()) {
        return false;
    }

    // 读取棋盘类型
    int typeInt;
    if (!(file >> typeInt)) return false; // 读取失败
    boardType = static_cast<BoardType>(typeInt);

    // 创建新棋盘 (Board构造函数应根据类型设置其内部结构和尺寸)
    board = Board(boardType);

    // 读取棋盘尺寸 (这些尺寸用于历史记录，并应与board.sizeX/Y()匹配)
    int fileBoardSizeX, fileBoardSizeY;
    if (!(file >> fileBoardSizeX >> fileBoardSizeY)) return false;

    // 验证读取的尺寸是否与棋盘对象的尺寸匹配 (可选，但建议)
    // if (fileBoardSizeX != board.sizeX() || fileBoardSizeY != board.sizeY()) {
    //     // 尺寸不匹配，可能是存档文件损坏或与棋盘类型定义不一致
    //     file.close();
    //     return false;
    // }

    // 读取棋盘状态 (使用board对象的尺寸)
    auto& boardDataRef = board.data(); // 获取对棋盘内部数据的引用
    // 确保 boardDataRef 已由 Board(boardType) 构造函数正确初始化和调整大小
    for (int i = 0; i < board.sizeX(); ++i) {
        for (int j = 0; j < board.sizeY(); ++j) {
            if (!(file >> boardDataRef[i][j])) {
                file.close();
                return false; // 读取棋盘数据失败
            }
        }
    }

    // 读取选中状态
    if (!(file >> selectedX >> selectedY)) return false;

    // 读取历史记录数量
    int historyCount;
    if (!(file >> historyCount)) return false;

    // 清空旧历史
    history.clear();
    selXHistory.clear();
    selYHistory.clear();
    history.reserve(historyCount);
    selXHistory.reserve(historyCount);
    selYHistory.reserve(historyCount);

    // 读取历史记录 (使用从文件中读取的fileBoardSizeX/Y)
    for (int h = 0; h < historyCount; ++h) {
        std::vector<std::vector<int>> histBoardState(fileBoardSizeX, std::vector<int>(fileBoardSizeY));
        for (int i = 0; i < fileBoardSizeX; ++i) {
            for (int j = 0; j < fileBoardSizeY; ++j) {
                if (!(file >> histBoardState[i][j])) {
                    file.close();
                    return false; // 读取历史棋盘数据失败
                }
            }
        }
        history.push_back(histBoardState);
    }

    // 读取历史选中状态
    for (int i = 0; i < historyCount; ++i) {
        int histSelX, histSelY;
        if (!(file >> histSelX >> histSelY)) {
            file.close();
            return false; // 读取历史选中状态失败
        }
        selXHistory.push_back(histSelX);
        selYHistory.push_back(histSelY);
    }

    file.close();
    return true;
}

bool SaveManager::saveExists(const std::string& filename) const { // 例如 "save1.txt"
    std::string fullPath = saveDir + filename;
    return _access(fullPath.c_str(), 0) != -1;
}

BoardType saveLoadMenu(BoardType currentType) {
    SaveManager saveManager;
    // 这些静态变量用于在加载游戏后，如果用户不立即开始游戏而是返回主菜单，
    // 仍然能够从这些变量中获取加载的游戏状态。
    // 或者，如果菜单本身需要显示加载的棋盘信息，它们也很有用。
    // 如果loadGame直接修改了主游戏循环中的变量，这些可能可以简化。
    // 但当前结构中，它们用于在loadGame和实际游戏开始之间传递数据。
    static Board savedBoard(CROSS);
    static int savedX = -1, savedY = -1;
    static std::vector<std::vector<std::vector<int>>> savedHistory;
    static std::vector<int> savedXHistory, savedYHistory;
    static BoardType savedBoardType = CROSS; // 用于存储加载的棋盘类型

    setbkcolor(RGB(236, 246, 255));
    cleardevice();

    settextstyle(45, 0, _T("微软雅黑"));
    settextcolor(RGB(50, 50, 120));
    outtextxy(300, 60, _T("存档管理"));

    settextstyle(26, 0, _T("微软雅黑"));
    RECT r1 = { 250, 180, 550, 220 };
    RECT r2 = { 250, 250, 550, 290 };
    RECT r3 = { 250, 320, 550, 360 };

    setfillcolor(RGB(190, 210, 240));
    fillroundrect(r1.left, r1.top, r1.right, r1.bottom, 15, 15);
    fillroundrect(r2.left, r2.top, r2.right, r2.bottom, 15, 15);
    fillroundrect(r3.left, r3.top, r3.right, r3.bottom, 15, 15);

    // 检查存档是否存在 (使用 .txt)
    bool save1Exists = saveManager.saveExists("save1.txt");
    bool save2Exists = saveManager.saveExists("save2.txt");
    bool save3Exists = saveManager.saveExists("save3.txt");

    TCHAR slot1[64], slot2[64], slot3[64]; // 增加缓冲区大小以防万一
    _stprintf_s(slot1, sizeof(slot1) / sizeof(TCHAR), _T("存档位置 1 %s"), save1Exists ? _T("(已存在)") : _T("(空)"));
    _stprintf_s(slot2, sizeof(slot2) / sizeof(TCHAR), _T("存档位置 2 %s"), save2Exists ? _T("(已存在)") : _T("(空)"));
    _stprintf_s(slot3, sizeof(slot3) / sizeof(TCHAR), _T("存档位置 3 %s"), save3Exists ? _T("(已存在)") : _T("(空)"));

    settextcolor(RGB(40, 40, 90));
    drawtext(slot1, &r1, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    drawtext(slot2, &r2, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    drawtext(slot3, &r3, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    RECT rBack = { 300, 420, 500, 460 };
    setfillcolor(RGB(180, 190, 230));
    fillroundrect(rBack.left, rBack.top, rBack.right, rBack.bottom, 15, 15);
    drawtext(_T("返回主菜单"), &rBack, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    // 用于向用户显示加载成功/失败的消息
    TCHAR loadMessage[100] = _T("");
    bool messageIsError = false;

    // 主循环，直到用户选择返回
    // 注意：这个函数如果加载了游戏，它会返回加载的棋盘类型 (currentType = savedBoardType)
    // 实际的游戏状态 (savedBoard, savedX, etc.) 被存储在静态变量中，
    // 调用此函数的代码（可能是主游戏循环或游戏设置部分）需要从这些静态变量中获取加载的数据。
    while (true) {
        // 清除旧消息并重绘按钮和文本，以防消息覆盖它们
        setfillcolor(RGB(236, 246, 255)); // 背景色
        RECT messageRect = { 200, 380, 600, 410 }; // 消息区域
        bar(messageRect.left, messageRect.top, messageRect.right, messageRect.bottom); // 清除消息区域

        if (_tcslen(loadMessage) > 0) { // 如果有消息则显示
            settextstyle(20, 0, _T("微软雅黑"));
            settextcolor(messageIsError ? RED : RGB(0, 128, 0)); // 错误用红色，成功用绿色
            outtextxy((getwidth() - textwidth(loadMessage)) / 2, 385, loadMessage);
        }

        ExMessage m;
        // 使用 getmessage 获取阻塞消息，而不是 peekmessage 的忙等待
        // 但为了保持原有逻辑的非阻塞特性（如果有其他后台任务），这里保留 peekmessage
        // 如果菜单是模态的，getmessage 更合适
        if (peekmessage(&m, EM_MOUSE | EM_KEY)) { // 检查鼠标和键盘消息
            if (m.message == WM_LBUTTONDOWN) {
                int clickX = m.x;
                int clickY = m.y;
                bool actionTaken = false; // 标记是否处理了点击

                // 处理存档槽位1
                if (clickX >= r1.left && clickX <= r1.right && clickY >= r1.top && clickY <= r1.bottom) {
                    actionTaken = true;
                    if (save1Exists) {
                        if (saveManager.loadGame(savedBoard, savedX, savedY, savedHistory,
                            savedXHistory, savedYHistory, savedBoardType, "save1.txt")) {
                            _tcscpy_s(loadMessage, _T("存档 1 加载成功！"));
                            messageIsError = false;
                            currentType = savedBoardType; // 更新当前棋盘类型以备返回
                            // 游戏状态已加载到 savedBoard, savedX, savedY, savedHistory 等静态变量中
                            // 主游戏逻辑需要使用这些变量来恢复游戏
                        }
                        else {
                            _tcscpy_s(loadMessage, _T("存档 1 加载失败！"));
                            messageIsError = true;
                        }
                    }
                    else {
                        _tcscpy_s(loadMessage, _T("存档 1 为空。"));
                        messageIsError = true; // 或 false，取决于是否视为空错误
                    }
                }
                // 处理存档槽位2
                else if (clickX >= r2.left && clickX <= r2.right && clickY >= r2.top && clickY <= r2.bottom) {
                    actionTaken = true;
                    if (save2Exists) {
                        if (saveManager.loadGame(savedBoard, savedX, savedY, savedHistory,
                            savedXHistory, savedYHistory, savedBoardType, "save2.txt")) {
                            _tcscpy_s(loadMessage, _T("存档 2 加载成功！"));
                            messageIsError = false;
                            currentType = savedBoardType;
                        }
                        else {
                            _tcscpy_s(loadMessage, _T("存档 2 加载失败！"));
                            messageIsError = true;
                        }
                    }
                    else {
                        _tcscpy_s(loadMessage, _T("存档 2 为空。"));
                        messageIsError = true;
                    }
                }
                // 处理存档槽位3
                else if (clickX >= r3.left && clickX <= r3.right && clickY >= r3.top && clickY <= r3.bottom) {
                    actionTaken = true;
                    if (save3Exists) {
                        if (saveManager.loadGame(savedBoard, savedX, savedY, savedHistory,
                            savedXHistory, savedYHistory, savedBoardType, "save3.txt")) {
                            _tcscpy_s(loadMessage, _T("存档 3 加载成功！"));
                            messageIsError = false;
                            currentType = savedBoardType;
                        }
                        else {
                            _tcscpy_s(loadMessage, _T("存档 3 加载失败！"));
                            messageIsError = true;
                        }
                    }
                    else {
                        _tcscpy_s(loadMessage, _T("存档 3 为空。"));
                        messageIsError = true;
                    }
                }
                // 返回主菜单
                else if (clickX >= rBack.left && clickX <= rBack.right && clickY >= rBack.top && clickY <= rBack.bottom) {
                    return currentType; // 返回（可能已更新的）棋盘类型
                }

                if (actionTaken) { // 如果有操作，清除旧消息并准备显示新消息
                    // 消息将在下一次循环迭代开始时绘制
                }
            } // end WM_LBUTTONDOWN
        } // end peekmessage
        Sleep(10); // 减少CPU占用
    } // end while(true)
}