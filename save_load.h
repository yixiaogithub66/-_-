#pragma once // 预处理指令，确保此头文件在一次编译中仅被包含一次，以防止因重复包含导致的编译错误。

#include "board.h" // 引用棋盘类头文件，因为存档和读档操作需要处理棋盘数据。
#include <string>  // 引用 C++ 标准库中的 string 类，用于处理文件名和路径。
#include <vector>  // 引用 C++ 标准库中的 vector 容器，用于存储游戏历史记录等动态数组。

// SaveManager 类，负责处理游戏的保存和加载逻辑。
// 它封装了将游戏状态写入文件以及从文件读取游戏状态的功能。
class SaveManager {
public:
    // 构造函数，用于初始化 SaveManager 对象。
    // 可能会在这里设置默认的存档目录。
    SaveManager();

    // 保存当前的游戏状态到指定文件。
    // @param board: 当前的游戏棋盘对象，其状态将被保存。
    // @param selectedX: 当前选中的棋子的 X 坐标。
    // @param selectedY: 当前选中的棋子的 Y 坐标。
    // @param history: 完整的棋盘状态历史记录 (每一步后的棋盘布局)。
    // @param selXHistory: 选中棋子 X 坐标的历史记录。
    // @param selYHistory: 选中棋子 Y 坐标的历史记录。
    // @param filename: 存档文件的名称，默认为 "save.dat"。
    // @return bool: 如果保存成功则返回 true，否则返回 false。
    bool saveGame(const Board& board,
        int selectedX, int selectedY,
        const std::vector<std::vector<std::vector<int>>>& history,
        const std::vector<int>& selXHistory,
        const std::vector<int>& selYHistory,
        const std::string& filename = "save.dat");

    // 从指定文件加载游戏状态。
    // @param board: 用于接收加载的棋盘状态的棋盘对象 (会被修改)。
    // @param selectedX: 用于接收加载的选中棋子 X 坐标的变量 (会被修改)。
    // @param selectedY: 用于接收加载的选中棋子 Y 坐标的变量 (会被修改)。
    // @param history: 用于接收加载的棋盘状态历史记录的变量 (会被修改)。
    // @param selXHistory: 用于接收加载的选中棋子 X 坐标历史记录的变量 (会被修改)。
    // @param selYHistory: 用于接收加载的选中棋子 Y 坐标历史记录的变量 (会被修改)。
    // @param boardType: 用于接收加载的棋盘类型的变量 (会被修改)，确保加载后能正确重建棋盘。
    // @param filename: 存档文件的名称，默认为 "save.dat"。
    // @return bool: 如果加载成功则返回 true，否则返回 false。
    bool loadGame(Board& board,
        int& selectedX, int& selectedY,
        std::vector<std::vector<std::vector<int>>>& history,
        std::vector<int>& selXHistory,
        std::vector<int>& selYHistory,
        BoardType& boardType, // 新增参数，用于传递加载的棋盘类型
        const std::string& filename = "save.dat");

    // 检查指定的存档文件是否存在。
    // 'const' 表明此方法不会修改对象的状态。
    // @param filename: 要检查的存档文件的名称。
    // @return bool: 如果文件存在则返回 true，否则返回 false。
    bool saveExists(const std::string& filename = "save.dat") const; // 允许指定文件名，并默认为 "save.dat"


private:
    std::string saveDir; // 存储存档文件的目录路径。
    // 例如，可以是 "./saves/" 或用户文档下的特定文件夹。
};

// 函数声明：显示存档/读档菜单界面，并处理相应的操作。
// 此函数可能会让用户选择是保存当前游戏、加载之前的游戏，还是返回主菜单。
// @param currentType: 当前游戏的棋盘类型，如果加载了不同类型的存档，此类型可能会被改变。
// @return BoardType: 操作完成后（特别是加载游戏后）的棋盘类型。如果未加载或加载失败，可能返回传入的 currentType。
BoardType saveLoadMenu(BoardType currentType);