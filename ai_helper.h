#pragma once // 预处理指令，确保该头文件在一次编译中只被包含一次，防止重复定义

#include "board.h"    // 引用棋盘类头文件，AI需要理解和操作棋盘状态
#include <string>     // 引用字符串库，用于存储移动原因等文本信息
#include <vector>     // 引用向量库，用于存储棋盘数据和可能的移动列表
#include <utility>    // 引用 utility 库，主要为了使用 std::pair (虽然在这个版本中AIMove直接用了成员变量，但保留可能是为了将来的扩展或习惯)
#include <map>        // 引用 map 库，用于实现置换表 (transposition table)，以存储已计算的棋盘状态及其评估结果，避免重复计算

// AI 移动结构体 (保持不变)
// 用于表示 AI 做出的一次移动，包含起点、终点和移动原因
struct AIMove {
    int fromX, fromY;     // 棋子移动的起始位置坐标 (x, y)
    int toX, toY;         // 棋子移动的目标位置坐标 (x, y)
    std::string reason;   // 对此移动的解释或原因，例如 "跳过中间棋子"
};

// 结构体，用于保存 AI 搜索的结果
// 包含评估分数、最佳的第一步移动以及是否存在解
struct AISearchResult {
    int score;            // 当前棋盘状态或搜索路径的评估分数，分数越高通常表示局面越好
    AIMove bestFirstMove; // 在当前搜索分支下，导致最高分数的最佳第一步移动
    bool solutionExists;  // 标记是否找到了一个解（例如，达到最终目标状态）

    // 构造函数，初始化成员变量
    // score 初始化为一个非常小的值，表示初始时未找到有效评估
    // bestFirstMove 初始化为无效移动，reason 为 "未找到移动"
    // solutionExists 初始化为 false
    AISearchResult() : score(-1e9), bestFirstMove({ -1, -1, -1, -1, "No move found" }), solutionExists(false) {}
};

// AI 助手类，封装了孔明棋 AI 的逻辑
class AIHelper {
public:
    // 构造函数，用于 AIHelper 对象的初始化
    AIHelper();
    // 析构函数，用于 AIHelper 对象销毁前的资源清理 (例如，如果动态分配了内存)
    ~AIHelper();

    // 根据当前棋盘状态获取最佳移动 (使用本地 AI 逻辑)
    // @param board: 当前的棋盘对象，AI 将基于此状态进行决策
    // @return AIMove: AI 计算出的最佳移动
    AIMove getBestMove(const Board& board);

    // 函数，用于在棋盘上绘制 AI 建议的移动提示 (可以保持类似实现)
    // @param move: AI 建议的移动
    // @param baseX, baseY: 棋盘绘制的基准坐标 x, y
    // @param cellSize: 棋盘单元格的大小
    void drawHint(const AIMove& move, int baseX = 80, int baseY = 80, int cellSize = 62);

    // 此函数可以移除，或者总是返回 true，因为不再需要 API 密钥
    // 用于检查 AI 是否准备就绪 (例如，在旧版本中可能用于检查网络连接或 API 密钥)
    // @return bool: 如果 AI 准备好则返回 true，否则返回 false
    bool isReady() const;

private:
    // 置换表 (Transposition Table): 将棋盘状态映射到其计算出的搜索结果
    // 键 (key) 将是棋盘的数据 (std::vector<std::vector<int>>)，代表棋盘的唯一布局
    // 值 (value) 是 AISearchResult，包含该布局的评估分数和最佳移动
    // 使用置换表可以避免对相同棋盘状态的重复搜索和评估，提高效率
    std::map<std::vector<std::vector<int>>, AISearchResult> transpositionTable;

    // 递归搜索函数，用于实现 AI 的决策逻辑 (例如 Minimax, Alpha-Beta剪枝等)
    // @param currentBoard: 当前正在评估的棋盘状态
    // @param depth: 当前搜索的深度
    // @param maxDepth: 允许的最大搜索深度，限制搜索的范围
    // @return AISearchResult: 对当前棋盘状态进行搜索后的结果，包含评估分数和最佳移动
    AISearchResult solveRecursive(Board currentBoard, int depth, int maxDepth);

    // 生成给定棋盘状态下所有有效的移动
    // @param board: 当前的棋盘对象
    // @return std::vector<AIMove>: 一个包含所有可能有效移动的列表
    std::vector<AIMove> generatePossibleMoves(const Board& board) const;

    // 评估给定的棋盘状态
    // @param board: 需要评估的棋盘对象
    // @return int: 返回一个评估分数 (分数越高越好，例如，可以设计为 -棋子数量，使得棋子越少分数越高)
    int evaluateBoard(const Board& board) const;

    // 计算棋盘上剩余棋子的数量
    // @param board: 当前的棋盘对象
    // @return int: 棋盘上棋子的总数
    int countPegs(const Board& board) const;

    // 检查当前棋盘状态下是否还有任何可以进行的移动
    // @param board: 当前的棋盘对象
    // @return bool: 如果存在至少一个有效移动则返回 true，否则返回 false
    bool hasMoves(const Board& board) const;

    // 将一个移动应用到棋盘状态上 (返回应用移动后的新棋盘对象)
    // 这个函数很重要，因为它允许 AI 探索不同移动序列的后果，而不会修改原始棋盘状态
    // @param board: 原始棋盘状态
    // @param move: 要应用的移动
    // @return Board: 应用移动后的新棋盘状态
    Board applyMove(const Board& board, const AIMove& move) const;
};