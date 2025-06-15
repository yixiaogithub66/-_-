#pragma once
#include "board.h"
#include <string>
#include <vector>
#include <utility>

// 表示AI建议的移动
struct AIMove {
    int fromX, fromY;     // 起始位置
    int toX, toY;         // 目标位置
    std::string reason;   // 解释为什么这是好的移动
};

class AIHelper {
public:
    AIHelper();
    ~AIHelper();

    // 设置API密钥
    void setApiKey(const std::string& key);

    // 根据当前棋盘状态获取AI建议的移动
    AIMove getBestMove(const Board& board);

    // 检查是否设置了API密钥
    bool isReady() const;

    // 在界面上绘制AI建议
    void drawHint(const AIMove& move, int baseX = 80, int baseY = 80, int cellSize = 62);

private:
    std::string apiKey;
    std::string apiEndpoint;

    // 将棋盘状态转换为文本描述以发送给API
    std::string serializeBoardState(const Board& board);

    // 发送HTTP请求到DeepSeek API
    std::string sendRequest(const std::string& prompt);

    // 解析API响应获取移动建议
    AIMove parseResponse(const std::string& response, const Board& board);

    // 生成游戏规则描述
    std::string generateRulesDescription();
};
