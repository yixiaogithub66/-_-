#include "ai_helper.h" // 引用 AI 助手类的头文件，包含了 AIHelper 类的声明以及相关的结构体（如 AIMove, AISearchResult）。
#include <iostream>    // 引用标准输入输出流库，主要用于调试目的（例如，输出信息到控制台）。
#include <graphics.h>  // 引用 EasyX 图形库，用于在游戏界面上绘制 AI 提示等图形元素。
#include <algorithm>   // 引用标准算法库，可能用于排序、查找等操作（在此代码中未明显直接使用，但可能是为了通用性或未来扩展）。
#include <vector>      // 引用标准向量库，AI 逻辑中广泛使用 vector 来存储棋盘状态、可能的移动列表等。
#include <string>      // 引用标准字符串库，用于处理字符串，例如在 AIMove 中存储移动原因或格式化输出。

// 为迭代深化搜索定义一个初始搜索深度和一个总的最大深度限制。
// MAX_OVERALL_AI_SEARCH_DEPTH 是迭代深化搜索的最终层数限制。AI 不会搜索超过这个深度的局面。
const int MAX_OVERALL_AI_SEARCH_DEPTH = 8;

// AIHelper 类的构造函数。
// 在创建 AIHelper 对象时调用。目前构造函数体为空，没有进行特殊的初始化操作。
AIHelper::AIHelper() {
    // 构造函数体
}

// AIHelper 类的析构函数。
// 在销毁 AIHelper 对象时调用。目前析构函数体为空，没有需要特殊清理的资源。
AIHelper::~AIHelper() {
    // 析构函数体
}

// 检查 AI 是否准备就绪。
// 在这个版本中，由于 AI 逻辑是本地的，不再依赖外部 API 密钥或服务，
// 所以此函数简单地返回 true，表示 AI 总是可用的。
// 'const' 表示此函数不会修改任何 AIHelper 对象的成员变量。
bool AIHelper::isReady() const {
    return true; // 本地 AI 总是准备就绪
}

// 计算并返回棋盘上剩余棋子的数量。
// @param board: 要检查的棋盘对象。
// @return int: 棋盘上值为 1 (代表棋子) 的单元格数量。
// 'const' 表示此函数不会修改任何 AIHelper 对象的成员变量，也不会修改传入的 board 对象。
int AIHelper::countPegs(const Board& board) const {
    int pegCount = 0; // 初始化棋子计数器
    const auto& boardData = board.data(); // 获取棋盘数据的常量引用，避免复制
    // 遍历棋盘的每一行
    for (const auto& row : boardData) {
        // 遍历当前行中的每一个单元格
        for (int cellState : row) {
            if (cellState == 1) { // 如果单元格状态为 1，表示有棋子
                pegCount++; // 棋子计数器加一
            }
        }
    }
    return pegCount; // 返回棋子总数
}

// 检查当前棋盘状态下是否还有任何可以进行的有效移动。
// @param board: 要检查的棋盘对象。
// @return bool: 如果存在至少一个有效移动则返回 true，否则返回 false。
// 'const' 表示此函数不会修改任何 AIHelper 对象的成员变量，也不会修改传入的 board 对象。
bool AIHelper::hasMoves(const Board& board) const {
    // 通过调用 generatePossibleMoves 生成所有可能的移动，
    // 如果生成的移动列表不为空，则说明存在有效移动。
    return !generatePossibleMoves(board).empty();
}

// 将一个指定的移动应用到一个棋盘状态上，并返回应用该移动后的新棋盘对象。
// 这个函数不会修改原始的 board 对象，而是返回一个新的 Board 实例。
// @param board: 原始棋盘状态。
// @param move: 要应用的 AI 移动。
// @return Board: 应用移动后的新棋盘状态。
// 'const' 表示此函数不会修改任何 AIHelper 对象的成员变量，也不会修改传入的 board 对象（尽管 move 不是 const）。
Board AIHelper::applyMove(const Board& board, const AIMove& move) const {
    Board newBoard = board; // 创建原始棋盘的一个副本
    // 计算被跳过的中间棋子的坐标
    int midX = (move.fromX + move.toX) / 2;
    int midY = (move.fromY + move.toY) / 2;

    // 更新新棋盘的状态：
    newBoard.set(move.fromX, move.fromY, 0); // 将起始位置设置为空 (0)
    newBoard.set(midX, midY, 0);             // 将被跳过的中间位置设置为空 (0)
    newBoard.set(move.toX, move.toY, 1);     // 将目标位置设置为有棋子 (1)
    return newBoard; // 返回修改后的新棋盘
}

// 生成并返回给定棋盘状态下所有可能的有效移动。
// @param board: 当前的棋盘对象。
// @return std::vector<AIMove>: 一个包含所有有效移动的向量。
// 'const' 表示此函数不会修改任何 AIHelper 对象的成员变量，也不会修改传入的 board 对象。
std::vector<AIMove> AIHelper::generatePossibleMoves(const Board& board) const {
    std::vector<AIMove> moves; // 用于存储找到的有效移动
    const int rows = board.sizeX(); // 获取棋盘的行数（或主要维度大小）
    // const int cols = board.sizeY(); // 假设 board.sizeY() 给出一致的列数或最大值（此行被注释掉了）
    BoardType type = board.getType(); // 获取棋盘的类型，不同类型的棋盘可能有不同的移动规则

    if (rows == 0) return moves; // 如果棋盘行数为0，直接返回空移动列表

    // 定义标准跳跃的相对坐标变化 (上、下、左、右各跳两格)
    int standard_dx[] = { 0, 0, 2, -2 }; // 行方向的变化
    int standard_dy[] = { 2, -2, 0, 0 }; // 列方向的变化
    int num_standard_directions = 4;     // 标准方向的数量

    // 定义对角线跳跃的相对坐标变化 (四个对角线方向各跳两格)
    int diagonal_dx[] = { 2, 2, -2, -2 }; // 行方向的变化
    int diagonal_dy[] = { 2, -2, 2, -2 }; // 列方向的变化
    int num_diagonal_directions = 4;      // 对角线方向的数量

    // 遍历棋盘的每一个单元格
    for (int r = 0; r < rows; ++r) {
        // 注意：这里使用 board.sizeY() 获取当前棋盘类型实际的列数或第二维度的大小
        for (int c = 0; c < board.sizeY(); ++c) {
            // 如果当前单元格无效，或者当前单元格没有棋子 (值不为1)，则跳过
            if (!board.isValid(r, c) || board.get(r, c) != 1) continue;

            // 尝试标准跳跃 (上、下、左、右)
            for (int i = 0; i < num_standard_directions; ++i) {
                int toR = r + standard_dx[i];     // 计算目标行坐标
                int toC = c + standard_dy[i];     // 计算目标列坐标
                int midR = r + standard_dx[i] / 2; // 计算中间行坐标 (被跳过的棋子)
                int midC = c + standard_dy[i] / 2; // 计算中间列坐标 (被跳过的棋子)

                // 检查移动是否有效：
                // 1. 目标位置 (toR, toC) 是否在棋盘内且为空 (值为0)
                // 2. 中间位置 (midR, midC) 是否在棋盘内且有棋子 (值为1)
                if (board.isValid(toR, toC) && board.get(toR, toC) == 0 &&
                    board.isValid(midR, midC) && board.get(midR, midC) == 1) {
                    // 如果有效，则将此移动添加到列表中
                    moves.push_back({ r, c, toR, toC, "Standard Jump" }); // 移动原因为 "标准跳跃"
                }
            }

            // 如果棋盘类型是 TRIANGLE (三角形), PYRAMID (金字塔形), 或 STAR (星形)，则额外尝试对角线跳跃
            // 这些棋盘类型通常允许对角线移动
            if (type == TRIANGLE || type == PYRAMID || type == STAR) {
                for (int i = 0; i < num_diagonal_directions; ++i) {
                    int toR = r + diagonal_dx[i];     // 计算对角线目标行坐标
                    int toC = c + diagonal_dy[i];     // 计算对角线目标列坐标
                    int midR = r + diagonal_dx[i] / 2; // 计算对角线中间行坐标
                    int midC = c + diagonal_dy[i] / 2; // 计算对角线中间列坐标

                    // 同样检查对角线移动的有效性
                    if (board.isValid(toR, toC) && board.get(toR, toC) == 0 &&
                        board.isValid(midR, midC) && board.get(midR, midC) == 1) {
                        // 如果有效，则将此移动添加到列表中
                        moves.push_back({ r, c, toR, toC, "Diagonal Jump" }); // 移动原因为 "对角线跳跃"
                    }
                }
            }
        }
    }
    return moves; // 返回所有找到的有效移动
}

// 评估给定棋盘状态的分数。
// 分数越高，代表局面越好。
// @param board: 要评估的棋盘对象。
// @return int: 评估分数。
// 'const' 表示此函数不会修改任何 AIHelper 对象的成员变量，也不会修改传入的 board 对象。
int AIHelper::evaluateBoard(const Board& board) const {
    int pegs = countPegs(board); // 获取当前棋盘上的棋子数
    if (pegs == 1) { // 如果只剩一个棋子，代表获胜
        return 10000; // 返回一个非常高的分数表示胜利
    }
    if (!hasMoves(board)) { // 如果没有可行的移动了 (且棋子数不为1)
        // 这是一个输的局面，或者是一个僵局但未赢。
        // 返回一个非常低的分数，并且棋子越多，分数越低。
        return -20000 - pegs;
    }
    // 一般情况下，分数与棋子数量成反比 (棋子越少越好)，乘以一个权重。
    // 这里的策略是鼓励减少棋子数量。
    return -pegs * 10;
}

// 递归解决函数，使用深度优先搜索（结合置换表）来找到最佳移动序列。
// @param currentBoard: 当前正在评估的棋盘状态。
// @param depth: 当前搜索的深度。
// @param maxDepth: 本次递归调用允许的最大搜索深度。
// @return AISearchResult: 包含针对 currentBoard 的最佳评估分数和导致该分数的首步移动。
AISearchResult AIHelper::solveRecursive(Board currentBoard, int depth, int maxDepth) {
    // 将当前棋盘状态转换为可用作置换表键的格式 (例如，其内部的二维向量数据)。
    std::vector<std::vector<int>> boardKey = currentBoard.data();
    // 检查置换表中是否已存在当前棋盘状态的计算结果。
    if (transpositionTable.count(boardKey)) {
        // 如果存在，直接返回存储的结果，避免重复计算。
        return transpositionTable[boardKey];
    }

    // 评估当前棋盘状态的即时分数。
    int currentScore = evaluateBoard(currentBoard);

    // 递归的终止条件：
    // 1. 达到最大搜索深度 (depth == maxDepth)。
    // 2. 当前局面是胜利状态 (currentScore == 10000)。
    // 3. 当前局面没有可行的移动 ( !hasMoves(currentBoard) )。
    if (depth == maxDepth || currentScore == 10000 || !hasMoves(currentBoard)) {
        AISearchResult res;
        // 最终分数会减去当前深度，以鼓励AI选择路径更短的解。
        res.score = currentScore - depth;
        res.solutionExists = true; // 标记找到了一个确定的状态（可能是终局，也可能是搜索边界）。
        res.bestFirstMove = { -1, -1, -1, -1, "Terminal state" }; // 标记为终止状态，没有后续移动。
        transpositionTable[boardKey] = res; // 将结果存入置换表。
        return res;
    }

    // 生成当前棋盘状态下所有可能的移动。
    std::vector<AIMove> possibleMoves = generatePossibleMoves(currentBoard);

    AISearchResult bestResultForThisNode; // 用于存储从此节点出发能达到的最佳结果。
    bestResultForThisNode.solutionExists = false; // 初始假设未找到解。

    // 遍历所有可能的移动。
    for (const auto& move : possibleMoves) {
        Board nextBoard = applyMove(currentBoard, move); // 应用当前移动，得到下一个棋盘状态。
        // 递归调用 solveRecursive 来评估移动后的局面。
        AISearchResult resultFromChild = solveRecursive(nextBoard, depth + 1, maxDepth);

        // 如果子节点的搜索返回了一个有效的解。
        if (resultFromChild.solutionExists) {
            // 如果这是当前节点找到的第一个解，或者子节点的解比当前已知的最佳解更好。
            if (!bestResultForThisNode.solutionExists || resultFromChild.score > bestResultForThisNode.score) {
                bestResultForThisNode.score = resultFromChild.score; // 更新最佳分数。
                bestResultForThisNode.bestFirstMove = move;          // 更新导致最佳分数的首步移动。
                bestResultForThisNode.solutionExists = true;         // 标记已找到解。
            }
        }
    }

    // 如果遍历完所有子节点后，仍然没有找到一个有效的 solutionExists 为 true 的路径
    // (这种情况理论上在深度有限搜索中，如果子节点都返回 solutionExists=true，则不会进入这里，
    //  除非所有子路径都极差，或者这是为了处理某种特殊情况或逻辑错误)。
    //  更可能的情况是，即使所有子节点都返回了 solutionExists=true，但它们的分数都可能很低。
    //  此处的逻辑确保即使所有后续路径都不理想，当前节点本身也会有一个评估值。
    if (!bestResultForThisNode.solutionExists) {
        bestResultForThisNode.score = currentScore - depth; // 使用当前节点的即时评估分数（减去深度）。
        bestResultForThisNode.bestFirstMove = { -1,-1,-1,-1, "No improving moves found at this depth" }; // 标记没有找到改进的移动。
        bestResultForThisNode.solutionExists = true; // 即使没有“更好”的移动，也认为这是一个已评估的状态。
    }

    transpositionTable[boardKey] = bestResultForThisNode; // 将此节点的计算结果存入置换表。
    return bestResultForThisNode;
}

// 获取最佳移动的函数，使用迭代深化搜索 (Iterative Deepening Search)。
// @param board: 当前的游戏棋盘状态。
// @return AIMove: AI 计算出的最佳移动。
AIMove AIHelper::getBestMove(const Board& board) {
    AIMove finalBestMove = { -1, -1, -1, -1, "本地 AI: 未找到解决方案。" }; // 初始化最终最佳移动为无效状态。
    int bestOverallScore = -1000000; // 初始化全局最佳分数为一个非常低的值。

    bool winFound = false; // 标记是否已经找到了一个胜利的路径。

    // 迭代深化循环：从深度1开始，逐渐增加最大搜索深度，直到达到 MAX_OVERALL_AI_SEARCH_DEPTH。
    for (int currentMaxIterativeDepth = 1; currentMaxIterativeDepth <= MAX_OVERALL_AI_SEARCH_DEPTH; ++currentMaxIterativeDepth) {
        transpositionTable.clear(); // 在每次新的迭代（增加深度）开始前，清空置换表。
        // 这是因为之前深度计算的分数可能不适用于更深的搜索。

// 调用递归求解函数，使用当前的迭代深度作为最大深度限制。
        AISearchResult iterationResult = solveRecursive(board, 0, currentMaxIterativeDepth);

        // 如果本次迭代找到了一个有效的解。
        if (iterationResult.solutionExists) {
            // 检查是否是胜利局面 (分数在9000到10000之间，10000是直接胜利，减去深度后可能略低)。
            if (iterationResult.score > 9000 && iterationResult.score <= 10000) { // 检查是否是胜利局面
                // 如果是第一次找到胜利局面，或者当前胜利局面的分数更高（路径更优）。
                if (!winFound || iterationResult.score > bestOverallScore) {
                    finalBestMove = iterationResult.bestFirstMove; // 更新最终最佳移动。
                    bestOverallScore = iterationResult.score;      // 更新全局最佳分数。
                    winFound = true;                              // 标记已找到胜利路径。
                    // 更新移动原因（主要用于调试，不会直接绘制）。
                    finalBestMove.reason = "本地 AI: 找到胜利! 深度 " + std::to_string(currentMaxIterativeDepth) +
                        ", 路径分数: " + std::to_string(iterationResult.score);
                }
            }

            // 如果当前还没有找到胜利局面，并且本次迭代的结果比已知的全局最佳分数要好。
            if (!winFound && iterationResult.score > bestOverallScore) {
                finalBestMove = iterationResult.bestFirstMove; // 更新最终最佳移动。
                bestOverallScore = iterationResult.score;      // 更新全局最佳分数。
                // 更新移动原因。
                finalBestMove.reason = "本地 AI: 深度 " + std::to_string(currentMaxIterativeDepth) +
                    " 时最佳, 分数: " + std::to_string(iterationResult.score);
            }
        }
        // 如果已经找到了胜利路径，则可以提前终止迭代深化，因为我们通常想要最快的胜利。
        if (winFound) break;
    }

    // 迭代深化结束后，如果 finalBestMove 仍然是初始的无效状态（即没有找到任何解）。
    if (finalBestMove.fromX == -1) {
        // 作为备用方案，尝试获取当前所有可能的移动。
        std::vector<AIMove> currentPossibleMoves = generatePossibleMoves(board);
        if (!currentPossibleMoves.empty()) {
            // 如果存在可行的移动，则选择第一个可行的移动作为最终结果。
            finalBestMove = currentPossibleMoves[0];
            finalBestMove.reason = "本地 AI: 备用方案，选择第一个可用移动。";
        }
        else {
            // 如果连一个可行的移动都没有，则标记为无路可走。
            finalBestMove.reason = "本地 AI: 当前状态下无路可走。";
        }
    }
    return finalBestMove; // 返回最终计算出的最佳移动。
}

// 在游戏界面上绘制 AI 提示的视觉元素（例如，高亮起始棋子、目标位置和移动路径）。
// 此函数现在只绘制视觉提示（圆圈和线条），不绘制文本原因。
// @param move: AI 建议的移动。
// @param baseX, baseY: 棋盘绘制区域左上角的基准坐标。
// @param cellSize: 棋盘上每个单元格的像素大小。
void AIHelper::drawHint(const AIMove& move, int baseX, int baseY, int cellSize) {
    // 如果 move 无效 (例如 fromX < 0)，则不绘制任何提示。
    if (move.fromX < 0 || move.toX < 0) {
        // 可选：如果需要，可以在此处绘制 "无可用提示" 之类的文本。
        // 例如:
        // settextstyle(22, 0, _T("微软雅黑"));
        // settextcolor(RGB(180, 40, 40));
        // outtextxy(610, 120, _T("无可用AI提示")); // 根据需要调整坐标
        return;
    }

    // 计算用于绘制圆圈和线条的中心点坐标。
    int x1_center = baseX + move.fromY * cellSize + cellSize / 2; // 起始棋子中心的屏幕 X 坐标
    int y1_center = baseY + move.fromX * cellSize + cellSize / 2; // 起始棋子中心的屏幕 Y 坐标
    int x2_center = baseX + move.toY * cellSize + cellSize / 2;   // 目标位置中心的屏幕 X 坐标
    int y2_center = baseY + move.toX * cellSize + cellSize / 2;   // 目标位置中心的屏幕 Y 坐标

    // 高亮起始棋子 (例如，用蓝色圆圈)。
    setlinecolor(RGB(20, 120, 220)); // 设置线条颜色为蓝色
    setlinestyle(PS_SOLID, 3);     // 设置线条样式为实线，粗细为3
    circle(x1_center, y1_center, cellSize / 2 - 5); // 绘制圆圈，半径比单元格略小

    // 高亮目标空位 (例如，用绿色圆圈)。
    setlinecolor(RGB(40, 200, 40));  // 设置线条颜色为绿色
    // setlinestyle(PS_SOLID, 3); // 可以保持相同样式或根据需要更改
    circle(x2_center, y2_center, cellSize / 2 - 5); // 绘制圆圈

    // 绘制移动路径的虚线。
    setlinecolor(RGB(100, 160, 200)); // 设置线条颜色为淡蓝色
    setlinestyle(PS_DASH, 2);        // 设置线条样式为虚线，粗细为2
    line(x1_center, y1_center, x2_center, y2_center); // 从起始点到目标点绘制直线

    // 将线条样式重置为默认值，以供其他绘图操作使用。
    setlinestyle(PS_SOLID, 1); // 恢复为实线，粗细为1
    setlinecolor(BLACK);       // 恢复为黑色，或者你项目中使用的默认线条颜色

    // 以下绘制 "reason" 文本的代码行已被移除或注释掉，因为 drawHint 现在只负责视觉提示。
    // settextstyle(22, 0, _T("微软雅黑"));
    // settextcolor(RGB(40, 80, 150));
    // outtextxy(610, 120, _T("AI 提示:")); // 如果保留此标题，可能需要调整位置

    // TCHAR moveText[100];
    // _stprintf_s(moveText, _T("从 (%d,%d) 到 (%d,%d)"), move.fromX, move.fromY, move.toX, move.toY);
    // settextcolor(RGB(220, 80, 20));
    // outtextxy(610, 150, moveText); // 可能需要调整位置

    // RECT r = { 610, 180, 790, 320 }; // 用于显示原因的文本框区域
    // #ifdef UNICODE
    //     wchar_t wReason[256];
    //     MultiByteToWideChar(CP_ACP, 0, move.reason.c_str(), -1, wReason, sizeof(wReason)/sizeof(wchar_t));
    //     drawtext(wReason, &r, DT_WORDBREAK | DT_NOCLIP); // 绘制宽字符原因文本
    // #else
    //     char szReason[256];
    //     strncpy_s(szReason, move.reason.c_str(), sizeof(szReason) -1);
    //     szReason[sizeof(szReason)-1] = '\0'; //确保以空字符结尾
    //     drawtext(szReason, &r, DT_WORDBREAK | DT_NOCLIP); // 绘制多字节字符原因文本
    // #endif
}