#include "ai_helper.h"
#include <sstream>
#include <iostream>
#include <graphics.h>
#include <windows.h>
#include <winhttp.h>
#include <C:\Users\yixiao\source\repos\孔明棋\libs\include\nlohmann/json.hpp>

// 添加 WinHTTP 库链接
#pragma comment(lib, "winhttp.lib")

AIHelper::AIHelper() {
    apiEndpoint = "https://api.deepseek.com/v1/chat/completions";
    // curl 初始化已移除
}

AIHelper::~AIHelper() {
    // curl 清理已移除
}

void AIHelper::setApiKey(const std::string& key) {
    this->apiKey = key;
}

bool AIHelper::isReady() const {
    return !apiKey.empty();
}

std::string AIHelper::serializeBoardState(const Board& board) {
    std::stringstream ss;

    // 添加棋盘类型信息
    ss << "棋盘类型: ";
    switch (board.getType()) {
    case CROSS: ss << "十字棋盘\n"; break;
    case DIAMOND: ss << "菱形棋盘\n"; break;
    case TRIANGLE: ss << "三角棋盘\n"; break;
    case TRAPEZOID: ss << "梯形棋盘\n"; break;
    case PYRAMID: ss << "金字塔棋盘\n"; break;
    case STAR: ss << "星形棋盘\n"; break;
    default: ss << "未知类型棋盘\n";
    }

    ss << "当前棋盘状态 (O表示有棋子，.表示空位，空白表示无效位置):\n";
    const auto& data = board.data();
    for (int i = 0; i < board.sizeX(); i++) {
        for (int j = 0; j < board.sizeY(); j++) {
            if (data[i][j] == -1) {
                ss << "  ";  // 无效位置
            }
            else if (data[i][j] == 1) {
                ss << "O ";  // 棋子
            }
            else {
                ss << ". ";  // 空位
            }
        }
        ss << "\n";
    }

    ss << "\n坐标系统: 第一个数字是行(从0开始)，第二个数字是列(从0开始)\n";

    return ss.str();
}

std::string AIHelper::generateRulesDescription() {
    std::stringstream ss;

    ss << "孔明棋规则:\n";
    ss << "1. 棋子只能沿水平或垂直方向跳过相邻的棋子，落在空位上\n";
    ss << "2. 棋子不能沿对角线移动\n";
    ss << "3. 被跳过的棋子会被移除\n";
    ss << "4. 目标是最终只剩一颗棋子\n";
    ss << "5. 只有当两个位置之间有且只有一个棋子时，才能从一个位置跳到另一个位置\n";

    return ss.str();
}

std::string AIHelper::sendRequest(const std::string& prompt) {
    // 先检查API密钥是否为空
    if (apiKey.empty()) {
        return "{\"error\":\"API密钥未设置，请在设置中添加有效的DeepSeek API密钥\"}";
    }

    std::string readBuffer;

    try {
        // 构建JSON请求
        nlohmann::json requestJson = {
            {"model", "deepseek-reasoner"},
            {"messages", {
                {{"role", "system"}, {"content", "你是一个孔明棋专家AI助手，专门分析棋盘状态并提供最佳移动建议。你需要清晰地解释为什么某个移动是最佳选择。"}},
                {{"role", "user"}, {"content", prompt}}
            }},
            {"stream", false}
        };

        std::string requestData = requestJson.dump();

        // 转换为宽字符串
        std::wstring host = L"api.deepseek.com";
        std::wstring path = L"/v1/chat/completions";

        // 初始化 WinHTTP
        HINTERNET hSession = WinHttpOpen(
            L"Peg Solitaire Game/1.0",
            WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
            WINHTTP_NO_PROXY_NAME,
            WINHTTP_NO_PROXY_BYPASS,
            0);

        if (!hSession) {
            return "{\"error\":\"无法初始化WinHTTP会话\"}";
        }

        HINTERNET hConnect = WinHttpConnect(
            hSession,
            host.c_str(),
            INTERNET_DEFAULT_HTTPS_PORT,
            0);

        if (!hConnect) {
            WinHttpCloseHandle(hSession);
            return "{\"error\":\"无法连接到DeepSeek服务器\"}";
        }

        HINTERNET hRequest = WinHttpOpenRequest(
            hConnect,
            L"POST",
            path.c_str(),
            NULL,
            WINHTTP_NO_REFERER,
            WINHTTP_DEFAULT_ACCEPT_TYPES,
            WINHTTP_FLAG_SECURE);

        if (!hRequest) {
            WinHttpCloseHandle(hConnect);
            WinHttpCloseHandle(hSession);
            return "{\"error\":\"无法创建HTTP请求\"}";
        }

        // 设置请求头
        std::string authHeader = "Authorization: Bearer " + apiKey;

        // 安全转换为宽字符串
        std::wstring contentType = L"Content-Type: application/json";
        if (!WinHttpAddRequestHeaders(hRequest, contentType.c_str(), -1, WINHTTP_ADDREQ_FLAG_ADD)) {
            WinHttpCloseHandle(hRequest);
            WinHttpCloseHandle(hConnect);
            WinHttpCloseHandle(hSession);
            return "{\"error\":\"添加Content-Type请求头失败\"}";
        }

        // 尝试安全地转换授权头
        try {
            std::wstring wAuthHeader(authHeader.begin(), authHeader.end());
            if (!WinHttpAddRequestHeaders(hRequest, wAuthHeader.c_str(), -1, WINHTTP_ADDREQ_FLAG_ADD)) {
                WinHttpCloseHandle(hRequest);
                WinHttpCloseHandle(hConnect);
                WinHttpCloseHandle(hSession);
                return "{\"error\":\"添加Authorization请求头失败\"}";
            }
        }
        catch (...) {
            WinHttpCloseHandle(hRequest);
            WinHttpCloseHandle(hConnect);
            WinHttpCloseHandle(hSession);
            return "{\"error\":\"转换授权头时出错\"}";
        }

        // 发送请求
        if (!WinHttpSendRequest(
            hRequest,
            WINHTTP_NO_ADDITIONAL_HEADERS,
            0,
            (LPVOID)requestData.c_str(),
            (DWORD)requestData.size(),
            (DWORD)requestData.size(),
            0)) {

            DWORD error = GetLastError();
            WinHttpCloseHandle(hRequest);
            WinHttpCloseHandle(hConnect);
            WinHttpCloseHandle(hSession);
            return "{\"error\":\"发送请求失败，错误码: " + std::to_string(error) + "\"}";
        }

        // 接收响应
        if (!WinHttpReceiveResponse(hRequest, NULL)) {
            DWORD error = GetLastError();
            WinHttpCloseHandle(hRequest);
            WinHttpCloseHandle(hConnect);
            WinHttpCloseHandle(hSession);
            return "{\"error\":\"接收响应失败，错误码: " + std::to_string(error) + "\"}";
        }

        // 获取HTTP状态码
        DWORD statusCode = 0;
        DWORD statusCodeSize = sizeof(DWORD);
        if (WinHttpQueryHeaders(
            hRequest,
            WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
            WINHTTP_HEADER_NAME_BY_INDEX,
            &statusCode,
            &statusCodeSize,
            WINHTTP_NO_HEADER_INDEX)) {

            if (statusCode != 200) {
                WinHttpCloseHandle(hRequest);
                WinHttpCloseHandle(hConnect);
                WinHttpCloseHandle(hSession);
                return "{\"error\":\"服务器返回错误状态码: " + std::to_string(statusCode) + "\"}";
            }
        }

        // 读取响应数据
        DWORD bytesAvailable = 0;
        DWORD bytesRead = 0;
        char buffer[4096];

        while (WinHttpQueryDataAvailable(hRequest, &bytesAvailable) && bytesAvailable > 0) {
            if (bytesAvailable > sizeof(buffer)) {
                bytesAvailable = sizeof(buffer);
            }

            if (WinHttpReadData(hRequest, buffer, bytesAvailable, &bytesRead)) {
                readBuffer.append(buffer, bytesRead);
            }
            else {
                DWORD error = GetLastError();
                WinHttpCloseHandle(hRequest);
                WinHttpCloseHandle(hConnect);
                WinHttpCloseHandle(hSession);
                return "{\"error\":\"读取响应数据失败，错误码: " + std::to_string(error) + "\"}";
            }
        }

        // 清理资源
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
    }
    catch (const std::exception& e) {
        return "{\"error\":\"发生异常: " + std::string(e.what()) + "\"}";
    }
    catch (...) {
        return "{\"error\":\"发生未知异常\"}";
    }

    if (readBuffer.empty()) {
        return "{\"error\":\"服务器返回了空响应\"}";
    }

    return readBuffer;
}

AIMove AIHelper::parseResponse(const std::string& response, const Board& board) {
    AIMove move = { -1, -1, -1, -1, "无法获取有效建议" };

    try {
        // 检查响应是否为空
        if (response.empty()) {
            move.reason = "服务器返回了空响应";
            return move;
        }

        // 尝试解析JSON
        nlohmann::json jsonResponse = nlohmann::json::parse(response);

        // 检查是否有错误信息
        if (jsonResponse.contains("error")) {
            std::string errorMsg = jsonResponse["error"].is_string() ?
                jsonResponse["error"].get<std::string>() : "未知错误";
            move.reason = "API错误: " + errorMsg;
            return move;
        }

        // 验证响应格式
        if (!jsonResponse.contains("choices") || jsonResponse["choices"].empty() ||
            !jsonResponse["choices"][0].contains("message") ||
            !jsonResponse["choices"][0]["message"].contains("content")) {
            move.reason = "API返回格式不正确";
            return move;
        }

        // 获取响应内容
        std::string content = jsonResponse["choices"][0]["message"]["content"];
        if (content.empty()) {
            move.reason = "API返回的内容为空";
            return move;
        }

        // 寻找坐标格式 (x1,y1)到(x2,y2)
        size_t pos = content.find("(");
        if (pos != std::string::npos) {
            // 提取起始坐标
            size_t endPos = content.find(")", pos);
            if (endPos != std::string::npos) {
                std::string coordStr = content.substr(pos + 1, endPos - pos - 1);
                size_t commaPos = coordStr.find(",");
                if (commaPos != std::string::npos) {
                    try {
                        move.fromX = std::stoi(coordStr.substr(0, commaPos));
                        move.fromY = std::stoi(coordStr.substr(commaPos + 1));
                    }
                    catch (const std::exception& e) {
                        move.reason = "解析起始坐标失败: " + std::string(e.what());
                        return move;
                    }
                }
            }

            // 寻找目标坐标
            pos = content.find("(", endPos);
            if (pos != std::string::npos) {
                endPos = content.find(")", pos);
                if (endPos != std::string::npos) {
                    std::string coordStr = content.substr(pos + 1, endPos - pos - 1);
                    size_t commaPos = coordStr.find(",");
                    if (commaPos != std::string::npos) {
                        try {
                            move.toX = std::stoi(coordStr.substr(0, commaPos));
                            move.toY = std::stoi(coordStr.substr(commaPos + 1));
                        }
                        catch (const std::exception& e) {
                            move.reason = "解析目标坐标失败: " + std::string(e.what());
                            return move;
                        }
                    }
                }
            }
        }

        // 坐标解析失败
        if (move.fromX < 0 || move.toX < 0) {
            move.reason = "无法从AI响应中提取有效的移动坐标";
            return move;
        }

        // 提取解释部分
        size_t reasonPos = content.find("原因:");
        if (reasonPos != std::string::npos) {
            move.reason = content.substr(reasonPos);
        }
        else {
            reasonPos = content.find("理由:");
            if (reasonPos != std::string::npos) {
                move.reason = content.substr(reasonPos);
            }
            else {
                // 如果没有明确的标记，取整个回复作为解释
                move.reason = content;
            }
        }

        // 验证移动是否合法
        if (move.fromX >= 0 && move.toX >= 0) {
            // 检查坐标是否在棋盘范围内
            if (!board.isValid(move.fromX, move.fromY) || !board.isValid(move.toX, move.toY)) {
                move.fromX = move.fromY = move.toX = move.toY = -1;
                move.reason = "AI提供的坐标超出棋盘范围";
                return move;
            }

            // 确保起点有棋子，终点是空的
            if (board.get(move.fromX, move.fromY) != 1 || board.get(move.toX, move.toY) != 0) {
                move.fromX = move.fromY = move.toX = move.toY = -1;
                move.reason = "AI提供的移动不符合基本规则（起点必须有棋子，终点必须为空）";
                return move;
            }

            // 确保是直线移动
            if (move.fromX != move.toX && move.fromY != move.toY) {
                // 检查是否在特殊棋盘上允许对角线移动
                BoardType type = board.getType();
                if (type != TRIANGLE && type != PYRAMID && type != STAR) {
                    // 不允许对角线移动的棋盘
                    move.fromX = move.fromY = move.toX = move.toY = -1;
                    move.reason = "AI提供的移动不符合直线移动规则";
                    return move;
                }

                // 即使在允许对角线移动的棋盘上，也需要确保是2格对角线移动
                int dx = abs(move.toX - move.fromX);
                int dy = abs(move.toY - move.fromY);
                if (dx != 2 || dy != 2) {
                    move.fromX = move.fromY = move.toX = move.toY = -1;
                    move.reason = "AI提供的对角线移动距离不正确";
                    return move;
                }
            }
            else {
                // 确保水平或垂直移动距离为2
                int dx = abs(move.toX - move.fromX);
                int dy = abs(move.toY - move.fromY);
                if (dx + dy != 2) {
                    // 不是有效跳跃距离
                    move.fromX = move.fromY = move.toX = move.toY = -1;
                    move.reason = "AI提供的移动距离不正确";
                    return move;
                }
            }

            // 验证中间格子有棋子
            int midX = (move.fromX + move.toX) / 2;
            int midY = (move.fromY + move.toY) / 2;
            if (!board.isValid(midX, midY)) {
                move.fromX = move.fromY = move.toX = move.toY = -1;
                move.reason = "AI提供的移动中间位置无效";
                return move;
            }

            if (board.get(midX, midY) != 1) {
                // 中间没有棋子可跳
                move.fromX = move.fromY = move.toX = move.toY = -1;
                move.reason = "AI提供的移动缺少中间棋子";
                return move;
            }
        }
    }
    catch (const nlohmann::json::parse_error& e) {
        move.reason = "解析JSON响应失败: " + std::string(e.what());
    }
    catch (const std::exception& e) {
        move.reason = "解析响应时发生异常: " + std::string(e.what());
    }
    catch (...) {
        move.reason = "解析响应时发生未知错误";
    }

    return move;
}

AIMove AIHelper::getBestMove(const Board& board) {
    if (!isReady()) {
        return { -1, -1, -1, -1, "API密钥未设置" };
    }

    std::stringstream prompt;
    prompt << "我正在玩孔明棋，需要你分析当前局面并给出最佳移动建议。\n\n";
    prompt << generateRulesDescription() << "\n";
    prompt << serializeBoardState(board) << "\n";
    prompt << "请分析当前局面，按照孔明棋规则（只能水平或垂直跳跃，不能对角线移动），给出一步最佳移动，包括:\n";
    prompt << "1. 起始位置坐标(行,列)和目标位置坐标(行,列)\n";
    prompt << "2. 为什么这是最佳移动的解释\n";
    prompt << "请使用格式: 从(x1,y1)移动到(x2,y2)。原因:...";

    std::string response = sendRequest(prompt.str());
    return parseResponse(response, board);
}

void AIHelper::drawHint(const AIMove& move, int baseX, int baseY, int cellSize) {
    if (move.fromX < 0 || move.toX < 0) {
        // 无有效移动提示
        settextstyle(22, 0, _T("微软雅黑"));
        settextcolor(RGB(180, 40, 40));
        outtextxy(610, 120, _T("无可用AI提示"));
        return;
    }

    // 绘制起点和终点的高亮
    int x1 = baseX + move.fromY * cellSize + cellSize / 2;
    int y1 = baseY + move.fromX * cellSize + cellSize / 2;
    int x2 = baseX + move.toY * cellSize + cellSize / 2;
    int y2 = baseY + move.toX * cellSize + cellSize / 2;

    // 起点高亮(蓝色)
    setlinecolor(RGB(20, 120, 220));
    setlinestyle(PS_SOLID, 3);
    circle(x1, y1, cellSize / 2 - 5);

    // 终点高亮(绿色)
    setlinecolor(RGB(40, 200, 40));
    circle(x2, y2, cellSize / 2 - 5);

    // 连接线
    setlinecolor(RGB(100, 160, 200));
    setlinestyle(PS_DASH, 2);
    line(x1, y1, x2, y2);

    // 恢复线样式
    setlinestyle(PS_SOLID, 1);

    // 绘制解释文本
    settextstyle(18, 0, _T("微软雅黑"));
    settextcolor(RGB(40, 80, 150));

    // 绘制标题
    settextstyle(22, 0, _T("微软雅黑"));
    settextcolor(RGB(40, 80, 150));
    outtextxy(610, 120, _T("AI提示:"));

    // 绘制移动信息
    TCHAR moveText[50];
    _stprintf_s(moveText, _T("(%d,%d)→(%d,%d)"),
        move.fromX, move.fromY, move.toX, move.toY);
    settextcolor(RGB(220, 80, 20));
    outtextxy(610, 150, moveText);

    // 绘制简短解释
    settextstyle(18, 0, _T("微软雅黑"));
    settextcolor(RGB(60, 60, 60));
    RECT r = { 610, 180, 790, 320 };

    // 将解释转换为宽字符
    wchar_t shortReason[100] = L"此移动可以提高棋盘平衡性，为后续走法创造空间。";
    drawtext(shortReason, &r, DT_WORDBREAK);
}