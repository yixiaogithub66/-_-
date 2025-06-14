#pragma once
#include "board.h"
#include <string>
#include <vector>
#include <utility>

// ��ʾAI������ƶ�
struct AIMove {
    int fromX, fromY;     // ��ʼλ��
    int toX, toY;         // Ŀ��λ��
    std::string reason;   // ����Ϊʲô���Ǻõ��ƶ�
};

class AIHelper {
public:
    AIHelper();
    ~AIHelper();

    // ����API��Կ
    void setApiKey(const std::string& key);

    // ���ݵ�ǰ����״̬��ȡAI������ƶ�
    AIMove getBestMove(const Board& board);

    // ����Ƿ�������API��Կ
    bool isReady() const;

    // �ڽ����ϻ���AI����
    void drawHint(const AIMove& move, int baseX = 80, int baseY = 80, int cellSize = 62);

private:
    std::string apiKey;
    std::string apiEndpoint;

    // ������״̬ת��Ϊ�ı������Է��͸�API
    std::string serializeBoardState(const Board& board);

    // ����HTTP����DeepSeek API
    std::string sendRequest(const std::string& prompt);

    // ����API��Ӧ��ȡ�ƶ�����
    AIMove parseResponse(const std::string& response, const Board& board);

    // ������Ϸ��������
    std::string generateRulesDescription();
};
