#include "ai_helper.h"
#include <sstream>
#include <iostream>
#include <graphics.h>
#include <windows.h>
#include <winhttp.h>
#include <C:\Users\yixiao\source\repos\������\libs\include\nlohmann/json.hpp>

// ��� WinHTTP ������
#pragma comment(lib, "winhttp.lib")

AIHelper::AIHelper() {
    apiEndpoint = "https://api.deepseek.com/v1/chat/completions";
    // curl ��ʼ�����Ƴ�
}

AIHelper::~AIHelper() {
    // curl �������Ƴ�
}

void AIHelper::setApiKey(const std::string& key) {
    this->apiKey = key;
}

bool AIHelper::isReady() const {
    return !apiKey.empty();
}

std::string AIHelper::serializeBoardState(const Board& board) {
    std::stringstream ss;

    // �������������Ϣ
    ss << "��������: ";
    switch (board.getType()) {
    case CROSS: ss << "ʮ������\n"; break;
    case DIAMOND: ss << "��������\n"; break;
    case TRIANGLE: ss << "��������\n"; break;
    case TRAPEZOID: ss << "��������\n"; break;
    case PYRAMID: ss << "����������\n"; break;
    case STAR: ss << "��������\n"; break;
    default: ss << "δ֪��������\n";
    }

    ss << "��ǰ����״̬ (O��ʾ�����ӣ�.��ʾ��λ���հױ�ʾ��Чλ��):\n";
    const auto& data = board.data();
    for (int i = 0; i < board.sizeX(); i++) {
        for (int j = 0; j < board.sizeY(); j++) {
            if (data[i][j] == -1) {
                ss << "  ";  // ��Чλ��
            }
            else if (data[i][j] == 1) {
                ss << "O ";  // ����
            }
            else {
                ss << ". ";  // ��λ
            }
        }
        ss << "\n";
    }

    ss << "\n����ϵͳ: ��һ����������(��0��ʼ)���ڶ�����������(��0��ʼ)\n";

    return ss.str();
}

std::string AIHelper::generateRulesDescription() {
    std::stringstream ss;

    ss << "���������:\n";
    ss << "1. ����ֻ����ˮƽ��ֱ�����������ڵ����ӣ����ڿ�λ��\n";
    ss << "2. ���Ӳ����ضԽ����ƶ�\n";
    ss << "3. �����������ӻᱻ�Ƴ�\n";
    ss << "4. Ŀ��������ֻʣһ������\n";
    ss << "5. ֻ�е�����λ��֮������ֻ��һ������ʱ�����ܴ�һ��λ��������һ��λ��\n";

    return ss.str();
}

std::string AIHelper::sendRequest(const std::string& prompt) {
    // �ȼ��API��Կ�Ƿ�Ϊ��
    if (apiKey.empty()) {
        return "{\"error\":\"API��Կδ���ã����������������Ч��DeepSeek API��Կ\"}";
    }

    std::string readBuffer;

    try {
        // ����JSON����
        nlohmann::json requestJson = {
            {"model", "deepseek-reasoner"},
            {"messages", {
                {{"role", "system"}, {"content", "����һ��������ר��AI���֣�ר�ŷ�������״̬���ṩ����ƶ����顣����Ҫ�����ؽ���Ϊʲôĳ���ƶ������ѡ��"}},
                {{"role", "user"}, {"content", prompt}}
            }},
            {"stream", false}
        };

        std::string requestData = requestJson.dump();

        // ת��Ϊ���ַ���
        std::wstring host = L"api.deepseek.com";
        std::wstring path = L"/v1/chat/completions";

        // ��ʼ�� WinHTTP
        HINTERNET hSession = WinHttpOpen(
            L"Peg Solitaire Game/1.0",
            WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
            WINHTTP_NO_PROXY_NAME,
            WINHTTP_NO_PROXY_BYPASS,
            0);

        if (!hSession) {
            return "{\"error\":\"�޷���ʼ��WinHTTP�Ự\"}";
        }

        HINTERNET hConnect = WinHttpConnect(
            hSession,
            host.c_str(),
            INTERNET_DEFAULT_HTTPS_PORT,
            0);

        if (!hConnect) {
            WinHttpCloseHandle(hSession);
            return "{\"error\":\"�޷����ӵ�DeepSeek������\"}";
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
            return "{\"error\":\"�޷�����HTTP����\"}";
        }

        // ��������ͷ
        std::string authHeader = "Authorization: Bearer " + apiKey;

        // ��ȫת��Ϊ���ַ���
        std::wstring contentType = L"Content-Type: application/json";
        if (!WinHttpAddRequestHeaders(hRequest, contentType.c_str(), -1, WINHTTP_ADDREQ_FLAG_ADD)) {
            WinHttpCloseHandle(hRequest);
            WinHttpCloseHandle(hConnect);
            WinHttpCloseHandle(hSession);
            return "{\"error\":\"���Content-Type����ͷʧ��\"}";
        }

        // ���԰�ȫ��ת����Ȩͷ
        try {
            std::wstring wAuthHeader(authHeader.begin(), authHeader.end());
            if (!WinHttpAddRequestHeaders(hRequest, wAuthHeader.c_str(), -1, WINHTTP_ADDREQ_FLAG_ADD)) {
                WinHttpCloseHandle(hRequest);
                WinHttpCloseHandle(hConnect);
                WinHttpCloseHandle(hSession);
                return "{\"error\":\"���Authorization����ͷʧ��\"}";
            }
        }
        catch (...) {
            WinHttpCloseHandle(hRequest);
            WinHttpCloseHandle(hConnect);
            WinHttpCloseHandle(hSession);
            return "{\"error\":\"ת����Ȩͷʱ����\"}";
        }

        // ��������
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
            return "{\"error\":\"��������ʧ�ܣ�������: " + std::to_string(error) + "\"}";
        }

        // ������Ӧ
        if (!WinHttpReceiveResponse(hRequest, NULL)) {
            DWORD error = GetLastError();
            WinHttpCloseHandle(hRequest);
            WinHttpCloseHandle(hConnect);
            WinHttpCloseHandle(hSession);
            return "{\"error\":\"������Ӧʧ�ܣ�������: " + std::to_string(error) + "\"}";
        }

        // ��ȡHTTP״̬��
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
                return "{\"error\":\"���������ش���״̬��: " + std::to_string(statusCode) + "\"}";
            }
        }

        // ��ȡ��Ӧ����
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
                return "{\"error\":\"��ȡ��Ӧ����ʧ�ܣ�������: " + std::to_string(error) + "\"}";
            }
        }

        // ������Դ
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
    }
    catch (const std::exception& e) {
        return "{\"error\":\"�����쳣: " + std::string(e.what()) + "\"}";
    }
    catch (...) {
        return "{\"error\":\"����δ֪�쳣\"}";
    }

    if (readBuffer.empty()) {
        return "{\"error\":\"�����������˿���Ӧ\"}";
    }

    return readBuffer;
}

AIMove AIHelper::parseResponse(const std::string& response, const Board& board) {
    AIMove move = { -1, -1, -1, -1, "�޷���ȡ��Ч����" };

    try {
        // �����Ӧ�Ƿ�Ϊ��
        if (response.empty()) {
            move.reason = "�����������˿���Ӧ";
            return move;
        }

        // ���Խ���JSON
        nlohmann::json jsonResponse = nlohmann::json::parse(response);

        // ����Ƿ��д�����Ϣ
        if (jsonResponse.contains("error")) {
            std::string errorMsg = jsonResponse["error"].is_string() ?
                jsonResponse["error"].get<std::string>() : "δ֪����";
            move.reason = "API����: " + errorMsg;
            return move;
        }

        // ��֤��Ӧ��ʽ
        if (!jsonResponse.contains("choices") || jsonResponse["choices"].empty() ||
            !jsonResponse["choices"][0].contains("message") ||
            !jsonResponse["choices"][0]["message"].contains("content")) {
            move.reason = "API���ظ�ʽ����ȷ";
            return move;
        }

        // ��ȡ��Ӧ����
        std::string content = jsonResponse["choices"][0]["message"]["content"];
        if (content.empty()) {
            move.reason = "API���ص�����Ϊ��";
            return move;
        }

        // Ѱ�������ʽ (x1,y1)��(x2,y2)
        size_t pos = content.find("(");
        if (pos != std::string::npos) {
            // ��ȡ��ʼ����
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
                        move.reason = "������ʼ����ʧ��: " + std::string(e.what());
                        return move;
                    }
                }
            }

            // Ѱ��Ŀ������
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
                            move.reason = "����Ŀ������ʧ��: " + std::string(e.what());
                            return move;
                        }
                    }
                }
            }
        }

        // �������ʧ��
        if (move.fromX < 0 || move.toX < 0) {
            move.reason = "�޷���AI��Ӧ����ȡ��Ч���ƶ�����";
            return move;
        }

        // ��ȡ���Ͳ���
        size_t reasonPos = content.find("ԭ��:");
        if (reasonPos != std::string::npos) {
            move.reason = content.substr(reasonPos);
        }
        else {
            reasonPos = content.find("����:");
            if (reasonPos != std::string::npos) {
                move.reason = content.substr(reasonPos);
            }
            else {
                // ���û����ȷ�ı�ǣ�ȡ�����ظ���Ϊ����
                move.reason = content;
            }
        }

        // ��֤�ƶ��Ƿ�Ϸ�
        if (move.fromX >= 0 && move.toX >= 0) {
            // ��������Ƿ������̷�Χ��
            if (!board.isValid(move.fromX, move.fromY) || !board.isValid(move.toX, move.toY)) {
                move.fromX = move.fromY = move.toX = move.toY = -1;
                move.reason = "AI�ṩ�����곬�����̷�Χ";
                return move;
            }

            // ȷ����������ӣ��յ��ǿյ�
            if (board.get(move.fromX, move.fromY) != 1 || board.get(move.toX, move.toY) != 0) {
                move.fromX = move.fromY = move.toX = move.toY = -1;
                move.reason = "AI�ṩ���ƶ������ϻ������������������ӣ��յ����Ϊ�գ�";
                return move;
            }

            // ȷ����ֱ���ƶ�
            if (move.fromX != move.toX && move.fromY != move.toY) {
                // ����Ƿ�����������������Խ����ƶ�
                BoardType type = board.getType();
                if (type != TRIANGLE && type != PYRAMID && type != STAR) {
                    // ������Խ����ƶ�������
                    move.fromX = move.fromY = move.toX = move.toY = -1;
                    move.reason = "AI�ṩ���ƶ�������ֱ���ƶ�����";
                    return move;
                }

                // ��ʹ������Խ����ƶ��������ϣ�Ҳ��Ҫȷ����2��Խ����ƶ�
                int dx = abs(move.toX - move.fromX);
                int dy = abs(move.toY - move.fromY);
                if (dx != 2 || dy != 2) {
                    move.fromX = move.fromY = move.toX = move.toY = -1;
                    move.reason = "AI�ṩ�ĶԽ����ƶ����벻��ȷ";
                    return move;
                }
            }
            else {
                // ȷ��ˮƽ��ֱ�ƶ�����Ϊ2
                int dx = abs(move.toX - move.fromX);
                int dy = abs(move.toY - move.fromY);
                if (dx + dy != 2) {
                    // ������Ч��Ծ����
                    move.fromX = move.fromY = move.toX = move.toY = -1;
                    move.reason = "AI�ṩ���ƶ����벻��ȷ";
                    return move;
                }
            }

            // ��֤�м����������
            int midX = (move.fromX + move.toX) / 2;
            int midY = (move.fromY + move.toY) / 2;
            if (!board.isValid(midX, midY)) {
                move.fromX = move.fromY = move.toX = move.toY = -1;
                move.reason = "AI�ṩ���ƶ��м�λ����Ч";
                return move;
            }

            if (board.get(midX, midY) != 1) {
                // �м�û�����ӿ���
                move.fromX = move.fromY = move.toX = move.toY = -1;
                move.reason = "AI�ṩ���ƶ�ȱ���м�����";
                return move;
            }
        }
    }
    catch (const nlohmann::json::parse_error& e) {
        move.reason = "����JSON��Ӧʧ��: " + std::string(e.what());
    }
    catch (const std::exception& e) {
        move.reason = "������Ӧʱ�����쳣: " + std::string(e.what());
    }
    catch (...) {
        move.reason = "������Ӧʱ����δ֪����";
    }

    return move;
}

AIMove AIHelper::getBestMove(const Board& board) {
    if (!isReady()) {
        return { -1, -1, -1, -1, "API��Կδ����" };
    }

    std::stringstream prompt;
    prompt << "������������壬��Ҫ�������ǰ���沢��������ƶ����顣\n\n";
    prompt << generateRulesDescription() << "\n";
    prompt << serializeBoardState(board) << "\n";
    prompt << "�������ǰ���棬���տ��������ֻ��ˮƽ��ֱ��Ծ�����ܶԽ����ƶ���������һ������ƶ�������:\n";
    prompt << "1. ��ʼλ������(��,��)��Ŀ��λ������(��,��)\n";
    prompt << "2. Ϊʲô��������ƶ��Ľ���\n";
    prompt << "��ʹ�ø�ʽ: ��(x1,y1)�ƶ���(x2,y2)��ԭ��:...";

    std::string response = sendRequest(prompt.str());
    return parseResponse(response, board);
}

void AIHelper::drawHint(const AIMove& move, int baseX, int baseY, int cellSize) {
    if (move.fromX < 0 || move.toX < 0) {
        // ����Ч�ƶ���ʾ
        settextstyle(22, 0, _T("΢���ź�"));
        settextcolor(RGB(180, 40, 40));
        outtextxy(610, 120, _T("�޿���AI��ʾ"));
        return;
    }

    // ���������յ�ĸ���
    int x1 = baseX + move.fromY * cellSize + cellSize / 2;
    int y1 = baseY + move.fromX * cellSize + cellSize / 2;
    int x2 = baseX + move.toY * cellSize + cellSize / 2;
    int y2 = baseY + move.toX * cellSize + cellSize / 2;

    // ������(��ɫ)
    setlinecolor(RGB(20, 120, 220));
    setlinestyle(PS_SOLID, 3);
    circle(x1, y1, cellSize / 2 - 5);

    // �յ����(��ɫ)
    setlinecolor(RGB(40, 200, 40));
    circle(x2, y2, cellSize / 2 - 5);

    // ������
    setlinecolor(RGB(100, 160, 200));
    setlinestyle(PS_DASH, 2);
    line(x1, y1, x2, y2);

    // �ָ�����ʽ
    setlinestyle(PS_SOLID, 1);

    // ���ƽ����ı�
    settextstyle(18, 0, _T("΢���ź�"));
    settextcolor(RGB(40, 80, 150));

    // ���Ʊ���
    settextstyle(22, 0, _T("΢���ź�"));
    settextcolor(RGB(40, 80, 150));
    outtextxy(610, 120, _T("AI��ʾ:"));

    // �����ƶ���Ϣ
    TCHAR moveText[50];
    _stprintf_s(moveText, _T("(%d,%d)��(%d,%d)"),
        move.fromX, move.fromY, move.toX, move.toY);
    settextcolor(RGB(220, 80, 20));
    outtextxy(610, 150, moveText);

    // ���Ƽ�̽���
    settextstyle(18, 0, _T("΢���ź�"));
    settextcolor(RGB(60, 60, 60));
    RECT r = { 610, 180, 790, 320 };

    // ������ת��Ϊ���ַ�
    wchar_t shortReason[100] = L"���ƶ������������ƽ���ԣ�Ϊ�����߷�����ռ䡣";
    drawtext(shortReason, &r, DT_WORDBREAK);
}