#pragma once // 预处理指令，确保此头文件在一次编译中仅被包含一次，防止重复定义和编译错误。

#include "board.h"     // 引用棋盘类头文件，Game 类需要使用 Board 对象来管理棋盘状态。
#include "setting.h"   // 引用设置类头文件，Game 类需要使用 Setting 对象来获取游戏配置。
#include <vector>      // 引用 C++ 标准库中的 vector 容器，用于存储历史记录等动态数组。
#include "ai_helper.h" // 引用 AI 助手类头文件。确保 ai_helper.h 也已更新以支持迭代深化搜索算法。

// AI 提示状态的枚举类型
// 用于管理 AI 提示按钮和功能的当前状态。
enum class AIHintState {
    IDLE,         // AI 未激活状态：AI 当前没有在计算，提示按钮显示为常规状态。
    WORKING,      // AI 工作状态：AI 正在计算最佳移动，提示按钮可以显示为例如绿色，表示“计算中”。
    SHOWING_HINT  // AI 显示提示状态：AI 已经计算出提示并且正在显示，提示按钮可以显示为例如红色，表示“提示已给出”，并且提示在棋盘上可见。
};

// Game 类，封装了孔明棋游戏的主要逻辑和流程。
class Game {
public:
    // 构造函数，用于初始化一个新的游戏实例。
    // @param b: 游戏所使用的棋盘对象。
    // @param s: 游戏的设置对象，包含如难度、是否启用提示等配置。
    Game(Board b, const Setting& s);

    // 运行游戏的主循环。
    // 此函数将处理用户输入、游戏逻辑更新、界面绘制等，直到游戏结束或用户退出。
    void run();

    // 切换 AI 提示按钮的动作。
    // 当用户点击 AI 提示按钮时调用此函数。
    // 它会根据当前的 AIHintState 决定是开始计算提示、取消计算还是隐藏提示。
    void toggleAIButtonAction();

    // --- 公共 Getter 方法 ---
    // 这些方法提供了对游戏内部状态的只读访问。

    // 获取对当前游戏棋盘对象的常量引用。
    // @return const Board&: 当前棋盘对象的只读引用。
    const Board& getBoard() const { return board; }

    // 获取当前选中的棋子的 X 坐标。
    // @return int: 选中棋子的 X 坐标；如果没有棋子被选中，通常为 -1。
    int getSelectedX() const { return selectedX; }

    // 获取当前选中的棋子的 Y 坐标。
    // @return int: 选中棋子的 Y 坐标；如果没有棋子被选中，通常为 -1。
    int getSelectedY() const { return selectedY; }

    // 获取棋盘历史记录的常量引用 (用于撤销功能或回放)。
    // boardHistory 存储了每一步移动后的棋盘状态。
    // @return const std::vector<std::vector<std::vector<int>>>&: 棋盘状态历史记录的只读引用。
    const std::vector<std::vector<std::vector<int>>>& getBoardHistory() const { return boardHistory; }

    // 获取选中棋子 X 坐标历史记录的常量引用。
    // @return const std::vector<int>&: 选中棋子 X 坐标历史的只读引用。
    const std::vector<int>& getSelectedXHistory() const { return selectedXHistory; }

    // 获取选中棋子 Y 坐标历史记录的常量引用。
    // @return const std::vector<int>&: 选中棋子 Y 坐标历史的只读引用。
    const std::vector<int>& getSelectedYHistory() const { return selectedYHistory; }

private:
    // --- 私有成员变量 ---
    // 这些变量存储了游戏的核心状态和辅助信息。

    Board board;        // 当前游戏的棋盘对象，管理棋子的布局和有效性。
    Setting setting;    // 当前游戏的设置对象，存储用户配置。
    int selectedX, selectedY; // 当前选中的棋子的坐标。如果为 -1，表示没有棋子被选中。

    // 历史记录，用于实现撤销 (undo) 功能。
    std::vector<std::vector<std::vector<int>>> boardHistory; // 存储每一步棋后的棋盘状态 (棋盘数据的二维向量的集合)。
    std::vector<int> selectedXHistory, selectedYHistory;   // 分别存储每一步棋时 selectedX 和 selectedY 的值。

    std::vector<std::vector<int>> initialBoard; // 存储棋盘的初始状态，用于“重新开始”或“恢复初始棋盘”功能。

    DWORD lastAIUpdateTime; // 上一次 AI 更新或计算的时间戳 (DWORD 是 Windows API 中的数据类型，通常是无符号32位整数)。
    // 可能用于控制 AI 计算的频率或超时。

// 动画相关的成员变量，用于实现棋子移动的平滑动画效果。
    bool animating;        // 标记当前是否正在播放棋子移动动画。
    int animFromX, animFromY; // 动画起始棋位的坐标。
    int animToX, animToY;   // 动画目标棋位的坐标。
    int animMidX, animMidY; // 动画中间点（被跳过的棋子）的坐标。
    DWORD animStartTime;   // 动画开始的时间戳。
    int animDuration;      // 棋子移动动画的总持续时间（毫秒）。

    AIHelper aiHelper;     // AI 助手对象，用于计算最佳移动或提供提示。

    AIHintState aiHintCurrentState; // 当前 AI 提示功能的状态 (IDLE, WORKING, SHOWING_HINT)。
    AIMove currentAIHintMove;       // 存储由 AI 计算出的当前提示移动。

    // --- 私有成员函数 ---
    // 这些函数是 Game 类的内部辅助方法，不直接暴露给外部。

    // 绘制当前游戏界面（不含动画过程中的状态）。
    // 包括棋盘、棋子、得分、按钮等静态元素。
    void drawGame();

    // 绘制带有动画效果的游戏界面。
    // 会在棋子移动过程中动态更新棋子位置，实现平滑过渡。
    void drawGameWithAnim();

    // 播放棋子从 (x1, y1) 移动到 (x2, y2) 的动画。
    // 此函数会设置动画相关的成员变量，并在后续的 drawGameWithAnim 调用中实际绘制动画帧。
    void playMoveAnimation(int x1, int y1, int x2, int y2);

    // 保存当前棋盘状态和选中棋子状态到历史记录中。
    // 通常在每次有效移动后调用。
    void saveHistory();

    // 撤销上一步移动。
    // 从历史记录中恢复上一个棋盘状态和选中状态。
    void undoMove();

    // 将棋盘恢复到初始布局。
    void restoreInitialBoard();

    // 检查从 (x1, y1) 到 (x2, y2) 是否为一次有效的孔明棋移动。
    // @param x1, y1: 起始位置。
    // @param x2, y2: 目标位置。
    // @return bool: 如果移动有效则返回 true，否则返回 false。
    bool isValidMove(int x1, int y1, int x2, int y2);

    // 执行一次概念上的移动 (更新棋盘状态，但不处理动画或历史记录)。
    // 实际的移动操作，包括拿掉中间的棋子，并将起始位置的棋子移动到目标位置。
    // @param x1, y1: 起始位置。
    // @param x2, y2: 目标位置。
    void move(int x1, int y1, int x2, int y2);

    // 检查当前棋盘状态下是否还存在任何有效的移动。
    // @return bool: 如果至少存在一步有效移动则返回 true，否则返回 false (游戏可能结束)。
    bool hasMoves();

    // 计算当前棋盘上剩余棋子的数量。
    // @return int: 棋盘上棋子的总数。
    int countPegs();
};