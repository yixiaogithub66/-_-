#include "save_load.h" // 引用自定义的存档/读档功能头文件，其中应包含 SaveManager 类的声明和 saveLoadMenu 函数的声明。
#include <graphics.h>  // 引用 EasyX 图形库的头文件，用于创建和管理存档菜单的图形用户界面。
#include <tchar.h>     // 引用通用字符类型头文件，用于支持 _T() 宏和 TCHAR 类型，以实现 ANSI 和 Unicode 字符集的兼容性。
#include <fstream>     // 引用文件流库，用于进行文件的读写操作 (例如 std::ofstream, std::ifstream)。
#include <string>      // 引用字符串库，确保 std::string 类型可用。
#include <vector>      // 引用向量库，确保 std::vector 类型可用，用于存储棋盘数据和历史记录。
#include <direct.h>    // 引用目录操作库，用于创建目录 (例如 _mkdir 函数)。
#include <io.h>        // 引用 IO 操作库，用于检查文件是否存在 (例如 _access 函数)。
#include <iomanip>     // 引用 IO 操纵符库，用于格式化输出 (例如 std::setw，在此代码中当前未使用，但包含可能是为了未来扩展)。

// SaveManager 类的构造函数。
// 当创建 SaveManager 对象时调用。
SaveManager::SaveManager() {
    // 初始化存档文件的存储目录。
    saveDir = ".\\saves\\"; // 设置存档目录为当前可执行文件路径下的 "saves" 文件夹。
    // ".\\" 表示当前目录。
// 确保存档目录存在，如果不存在则创建它。
// _access 函数用于检查文件或目录的访问权限。
// saveDir.c_str() 将 std::string 转换为 C 风格字符串。
// 0作为第二个参数表示检查文件或目录是否存在。
    if (_access(saveDir.c_str(), 0) == -1) { // 如果 _access 返回 -1，表示目录不存在或无法访问。
        _mkdir(saveDir.c_str()); // 使用 _mkdir 创建目录。
    }
}

// 保存当前游戏状态到指定文件。
// @param board: 当前的游戏棋盘对象。
// @param selectedX, selectedY: 当前选中的棋子的逻辑坐标。
// @param history: 棋盘状态的历史记录。
// @param selXHistory, selYHistory: 选中棋子坐标的历史记录。
// @param filename: 存档文件的名称 (例如 "save1.txt")。
// @return bool: 如果保存成功则返回 true，否则返回 false。
bool SaveManager::saveGame(const Board& board,
    int selectedX, int selectedY,
    const std::vector<std::vector<std::vector<int>>>& history,
    const std::vector<int>& selXHistory,
    const std::vector<int>& selYHistory,
    const std::string& filename) {
    std::string fullPath = saveDir + filename; // 构造完整的存档文件路径 (例如 ".\\saves\\save1.txt")。
    std::ofstream file(fullPath); // 以文本模式 (默认) 打开文件用于写入。
    // 如果文件已存在，内容会被覆盖。

    if (!file.is_open()) { // 检查文件是否成功打开。
        return false; // 如果文件打开失败，则返回 false。
    }

    // 1. 保存棋盘类型。
    // static_cast<int>(board.getType()) 将 BoardType 枚举转换为整数进行存储。
    file << static_cast<int>(board.getType()) << std::endl; // 写入棋盘类型，并换行。

    // 2. 保存棋盘的实际尺寸 (行数和列数)。
    int bSizeX = board.sizeX(); // 获取棋盘的行数。
    int bSizeY = board.sizeY(); // 获取棋盘的列数 (通常是第一行的列数或最大列数)。
    file << bSizeX << " " << bSizeY << std::endl; // 写入行数和列数，用空格隔开，并换行。

    // 3. 保存当前棋盘的状态数据。
    const auto& boardData = board.data(); // 获取对棋盘内部数据的常量引用，避免复制。
    for (int i = 0; i < bSizeX; ++i) { // 遍历每一行。
        for (int j = 0; j < bSizeY; ++j) { // 遍历每一列。
            file << boardData[i][j] << (j == bSizeY - 1 ? "" : " "); // 写入单元格的值。
            // 如果不是行尾，则在值后加一个空格；否则不加。
        }
        file << std::endl; // 每行结束后换行。
    }

    // 4. 保存当前选中的棋子状态。
    file << selectedX << " " << selectedY << std::endl; // 写入选中棋子的X和Y坐标，并换行。

    // 5. 保存历史记录的数量。
    file << history.size() << std::endl; // 写入历史记录的条目数，并换行。

    // 6. 保存每一条历史记录中的棋盘状态。
    // 假设历史记录中的棋盘尺寸与当前棋盘的 bSizeX, bSizeY 一致。
    for (const auto& histBoardState : history) { // 遍历历史记录中的每个棋盘状态。
        for (int i = 0; i < bSizeX; ++i) { // 遍历行。
            for (int j = 0; j < bSizeY; ++j) { // 遍历列。
                file << histBoardState[i][j] << (j == bSizeY - 1 ? "" : " "); // 写入历史棋盘数据。
            }
            file << std::endl; // 每行历史数据结束后换行。
        }
    }

    // 7. 保存每一条历史记录中的选中棋子状态。
    // history.size() 应该与 selXHistory.size() 和 selYHistory.size() 相等。
    for (size_t i = 0; i < history.size(); ++i) {
        file << selXHistory[i] << " " << selYHistory[i] << std::endl; // 写入历史选中状态。
    }

    file.close(); // 关闭文件。
    return true; // 保存成功，返回 true。
}

// 从指定文件加载游戏状态。
// @param board: 用于接收加载的棋盘状态的 Board 对象 (会被修改)。
// @param selectedX, selectedY: 用于接收加载的选中棋子坐标的变量 (会被修改)。
// @param history: 用于接收加载的棋盘历史记录的向量 (会被修改)。
// @param selXHistory, selYHistory: 用于接收加载的选中历史记录的向量 (会被修改)。
// @param boardType: 用于接收加载的棋盘类型的变量 (会被修改)。
// @param filename: 存档文件的名称 (例如 "save1.txt")。
// @return bool: 如果加载成功则返回 true，否则返回 false。
bool SaveManager::loadGame(Board& board,
    int& selectedX, int& selectedY,
    std::vector<std::vector<std::vector<int>>>& history,
    std::vector<int>& selXHistory,
    std::vector<int>& selYHistory,
    BoardType& boardType,
    const std::string& filename) {
    std::string fullPath = saveDir + filename; // 构造完整的存档文件路径。
    std::ifstream file(fullPath); // 以文本模式打开文件用于读取。

    if (!file.is_open()) { // 检查文件是否成功打开。
        return false; // 如果文件打开失败，则返回 false。
    }

    // 1. 读取棋盘类型。
    int typeInt;
    if (!(file >> typeInt)) { file.close(); return false; } // 从文件读取整数。如果读取失败，关闭文件并返回 false。
    boardType = static_cast<BoardType>(typeInt); // 将读取的整数转换回 BoardType 枚举。

    // 2. 根据读取的棋盘类型重新创建 Board 对象。
    // 这很重要，因为 Board 的构造函数会根据类型初始化其内部结构和尺寸。
    board = Board(boardType);

    // 3. 读取棋盘尺寸 (这些尺寸主要用于后续读取历史记录时确认维度)。
    // 理论上，这些尺寸应该与新创建的 `board` 对象的 `board.sizeX()` 和 `board.sizeY()` 相匹配。
    int fileBoardSizeX, fileBoardSizeY;
    if (!(file >> fileBoardSizeX >> fileBoardSizeY)) { file.close(); return false; } // 读取尺寸失败。

    // 可选的验证步骤：检查文件中存储的尺寸是否与根据 boardType 新创建的棋盘对象的尺寸一致。
    // if (fileBoardSizeX != board.sizeX() || fileBoardSizeY != board.sizeY()) {
    //     // 如果尺寸不匹配，可能意味着存档文件已损坏，或者棋盘类型的定义与存档时的不一致。
    //     file.close();
    //     return false;
    // }

    // 4. 读取当前棋盘的状态数据。
    // 获取对 `board` 对象内部数据的引用，以便直接修改它。
    auto& boardDataRef = board.data();
    // 此时，boardDataRef 应该已经被 Board(boardType) 构造函数根据类型正确初始化并调整了大小。
    // 使用 `board.sizeX()` 和 `board.sizeY()` 来确保我们读取的数据量与当前棋盘对象的结构相符。
    for (int i = 0; i < board.sizeX(); ++i) {
        for (int j = 0; j < board.sizeY(); ++j) {
            if (!(file >> boardDataRef[i][j])) { // 读取每个单元格的值。
                file.close(); return false; // 读取棋盘数据失败。
            }
        }
    }

    // 5. 读取当前选中的棋子状态。
    if (!(file >> selectedX >> selectedY)) { file.close(); return false; } // 读取失败。

    // 6. 读取历史记录的数量。
    int historyCount;
    if (!(file >> historyCount)) { file.close(); return false; } // 读取失败。

    // 7. 清空旧的历史记录，并为新的历史记录预分配空间以提高效率。
    history.clear();
    selXHistory.clear();
    selYHistory.clear();
    history.reserve(historyCount);
    selXHistory.reserve(historyCount);
    selYHistory.reserve(historyCount);

    // 8. 读取每一条历史记录中的棋盘状态。
    // 这里使用从文件中读取的 `fileBoardSizeX` 和 `fileBoardSizeY` 来创建临时的 `histBoardState`，
    // 因为历史记录中的棋盘尺寸可能与当前游戏主循环中 Board 对象的动态尺寸不同（尽管通常应该相同）。
    for (int h = 0; h < historyCount; ++h) {
        std::vector<std::vector<int>> histBoardState(fileBoardSizeX, std::vector<int>(fileBoardSizeY)); // 创建一个历史棋盘状态的2D向量。
        for (int i = 0; i < fileBoardSizeX; ++i) {
            for (int j = 0; j < fileBoardSizeY; ++j) {
                if (!(file >> histBoardState[i][j])) { // 读取历史棋盘的单元格数据。
                    file.close(); return false; // 读取失败。
                }
            }
        }
        history.push_back(histBoardState); // 将读取到的历史棋盘状态添加到 history 向量中。
    }

    // 9. 读取每一条历史记录中的选中棋子状态。
    for (int i = 0; i < historyCount; ++i) {
        int histSelX, histSelY;
        if (!(file >> histSelX >> histSelY)) { // 读取历史选中状态。
            file.close(); return false; // 读取失败。
        }
        selXHistory.push_back(histSelX);
        selYHistory.push_back(histSelY);
    }

    file.close(); // 关闭文件。
    return true; // 加载成功，返回 true。
}

// 检查指定的存档文件是否存在。
// 'const' 表示此方法不会修改 SaveManager 对象的任何成员变量。
// @param filename: 要检查的存档文件的名称 (例如 "save1.txt")。
// @return bool: 如果文件存在则返回 true，否则返回 false。
bool SaveManager::saveExists(const std::string& filename) const {
    std::string fullPath = saveDir + filename; // 构造完整的文件路径。
    // _access(path, 0) 检查文件是否存在。如果文件存在且可访问（模式0只检查存在性），返回0；否则返回-1。
    return _access(fullPath.c_str(), 0) != -1;
}

// 存档/读档菜单函数。
// @param currentType: 当前游戏主循环中的棋盘类型。如果加载了新游戏，此类型会被更新。
// @return BoardType: 返回操作完成后的棋盘类型（可能是原始类型，也可能是加载的类型）。
BoardType saveLoadMenu(BoardType currentType) {
    SaveManager saveManager; // 创建 SaveManager 对象以使用其存档/读档功能。

    // 使用静态变量来存储加载的游戏状态。
    // 这样做是因为 saveLoadMenu 函数返回后，如果用户选择加载游戏，
    // 主游戏逻辑需要一种方式来获取这些加载的数据。静态变量在函数调用结束后仍然保持其值。
    // 缺点是这些数据是全局可访问的（通过再次调用此函数），且不是线程安全的。
    // 另一种方法是将这些变量作为引用参数传递给 saveLoadMenu，或者让函数返回一个包含所有状态的结构体。
    static Board savedBoard(CROSS); // 静态棋盘对象，默认初始化为 CROSS 类型。用于存储加载的棋盘。
    static int savedX = -1, savedY = -1; // 静态变量，存储加载的选中棋子坐标。
    static std::vector<std::vector<std::vector<int>>> savedHistory; // 存储加载的棋盘历史。
    static std::vector<int> savedXHistory, savedYHistory;       // 存储加载的选中历史。
    static BoardType savedBoardType = CROSS; // 静态变量，用于存储从文件中加载的棋盘类型。

    // 设置菜单界面的背景颜色。
    setbkcolor(RGB(236, 246, 255)); // 淡蓝色背景。
    cleardevice(); // 清屏并应用背景色。

    // 绘制菜单标题。
    settextstyle(45, 0, _T("微软雅黑")); // 设置字体样式。
    settextcolor(RGB(50, 50, 120));   // 设置标题颜色 (深蓝色)。
    outtextxy(300, 60, _T("存档管理")); // 输出标题文本。

    // 定义存档槽位按钮的矩形区域。
    settextstyle(26, 0, _T("微软雅黑")); // 设置按钮文本字体。
    RECT r1 = { 250, 180, 550, 220 }; // 存档槽 1
    RECT r2 = { 250, 250, 550, 290 }; // 存档槽 2
    RECT r3 = { 250, 320, 550, 360 }; // 存档槽 3

    // 绘制存档槽位按钮的背景。
    setfillcolor(RGB(190, 210, 240)); // 按钮填充颜色 (浅蓝灰色)。
    fillroundrect(r1.left, r1.top, r1.right, r1.bottom, 15, 15); // 圆角矩形。
    fillroundrect(r2.left, r2.top, r2.right, r2.bottom, 15, 15);
    fillroundrect(r3.left, r3.top, r3.right, r3.bottom, 15, 15);

    // 检查每个存档槽对应的文件是否存在。
    bool save1Exists = saveManager.saveExists("save1.txt"); // 使用 ".txt" 后缀，与 saveGame/loadGame 中一致。
    bool save2Exists = saveManager.saveExists("save2.txt");
    bool save3Exists = saveManager.saveExists("save3.txt");

    // 准备在按钮上显示的文本，指示存档槽是否为空。
    TCHAR slot1[64], slot2[64], slot3[64]; // 文本缓冲区，大小应足够。
    // 使用 _stprintf_s 安全地格式化字符串。
    _stprintf_s(slot1, sizeof(slot1) / sizeof(TCHAR), _T("存档位置 1 %s"), save1Exists ? _T("(已存在)") : _T("(空)"));
    _stprintf_s(slot2, sizeof(slot2) / sizeof(TCHAR), _T("存档位置 2 %s"), save2Exists ? _T("(已存在)") : _T("(空)"));
    _stprintf_s(slot3, sizeof(slot3) / sizeof(TCHAR), _T("存档位置 3 %s"), save3Exists ? _T("(已存在)") : _T("(空)"));

    // 在按钮上绘制文本。
    settextcolor(RGB(40, 40, 90)); // 文本颜色 (深蓝色)。
    drawtext(slot1, &r1, DT_CENTER | DT_VCENTER | DT_SINGLELINE); // 文本居中显示。
    drawtext(slot2, &r2, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    drawtext(slot3, &r3, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    // 绘制“返回主菜单”按钮。
    RECT rBack = { 300, 420, 500, 460 };
    setfillcolor(RGB(180, 190, 230)); // 按钮颜色。
    fillroundrect(rBack.left, rBack.top, rBack.right, rBack.bottom, 15, 15);
    drawtext(_T("返回主菜单"), &rBack, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    // 用于向用户显示加载成功/失败的消息的变量。
    TCHAR loadMessage[100] = _T(""); // 消息文本缓冲区，初始为空。
    bool messageIsError = false;     // 标记消息是否为错误消息 (用于设置不同颜色)。

    // 主事件循环，处理用户输入，直到用户选择返回。
    while (true) {
        // 在每次循环开始时，清除可能存在的旧消息，并重绘按钮和文本（如果消息覆盖了它们）。
        // 这里通过在特定区域填充背景色来清除消息。
        setfillcolor(RGB(236, 246, 255)); // 使用背景色。
        RECT messageRect = { 200, 380, 600, 410 }; // 定义消息显示的矩形区域。
        bar(messageRect.left, messageRect.top, messageRect.right, messageRect.bottom); // 用背景色填充该区域以清除旧消息。

        if (_tcslen(loadMessage) > 0) { // 如果 loadMessage 缓冲区中有内容，则显示消息。
            settextstyle(20, 0, _T("微软雅黑")); // 设置消息文本字体。
            settextcolor(messageIsError ? RED : RGB(0, 128, 0)); // 错误消息用红色，成功消息用绿色。
            // 计算消息文本的 X 坐标以使其水平居中。
            outtextxy((getwidth() - textwidth(loadMessage)) / 2, 385, loadMessage);
        }

        ExMessage m; // EasyX 消息结构体。
        // 检查鼠标和键盘消息。peekmessage 是非阻塞的。
        // 如果菜单应该是模态的（即在选择前不执行其他操作），getmessage（阻塞）可能更合适。
        if (peekmessage(&m, EM_MOUSE | EM_KEY)) {
            if (m.message == WM_LBUTTONDOWN) { // 如果是鼠标左键按下事件。
                int clickX = m.x; // 获取点击的 X 坐标。
                int clickY = m.y; // 获取点击的 Y 坐标。
                bool actionTaken = false; // 标记本次点击是否已处理。

                // 处理对存档槽 1 的点击。
                if (clickX >= r1.left && clickX <= r1.right && clickY >= r1.top && clickY <= r1.bottom) {
                    actionTaken = true;
                    if (save1Exists) { // 如果存档文件存在，尝试加载。
                        if (saveManager.loadGame(savedBoard, savedX, savedY, savedHistory,
                            savedXHistory, savedYHistory, savedBoardType, "save1.txt")) {
                            _tcscpy_s(loadMessage, _T("存档 1 加载成功！")); // 设置成功消息。
                            messageIsError = false;
                            currentType = savedBoardType; // 更新当前棋盘类型，以便函数返回时外部知道加载了什么类型。
                            // 游戏状态已加载到静态变量 savedBoard, savedX, savedY, savedHistory 中。
                            // 调用此菜单的外部代码需要从这些静态变量中获取加载的游戏数据。
                        }
                        else {
                            _tcscpy_s(loadMessage, _T("存档 1 加载失败！")); // 设置失败消息。
                            messageIsError = true;
                        }
                    }
                    else {
                        _tcscpy_s(loadMessage, _T("存档 1 为空。")); // 存档槽为空。
                        messageIsError = true; // 可以设为 true 或 false，取决于是否视为空错误。
                    }
                }
                // 处理存档槽 2 的点击 (逻辑与槽位1类似)。
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
                // 处理存档槽 3 的点击 (逻辑与槽位1类似)。
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
                // 处理“返回主菜单”按钮的点击。
                else if (clickX >= rBack.left && clickX <= rBack.right && clickY >= rBack.top && clickY <= rBack.bottom) {
                    return currentType; // 直接返回当前的棋盘类型 (如果之前加载成功，currentType 会是加载的类型)。
                }

                // 如果处理了任何点击操作 (actionTaken 为 true)，
                // 消息文本 (loadMessage) 已经被更新，它将在下一次循环迭代的开始处被绘制。
            } // 结束 WM_LBUTTONDOWN 处理。
        } // 结束 peekmessage 处理。
        Sleep(10); // 短暂休眠，以降低 CPU 使用率。
    } // 结束主循环 (while(true))。
}