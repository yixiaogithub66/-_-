#pragma once
#include "board.h"
#include "setting.h"
#include <vector>
// ��ͷ�ļ����ò������
#include "ai_helper.h"
class Game {
public:
    Game(Board b, const Setting& s);
    void run();
    void drawGame();
    void drawGameWithAnim();
    void playMoveAnimation(int x1, int y1, int x2, int y2);
    void saveHistory();
    void undoMove();
    void restoreInitialBoard();
    void showEnd(bool win);
    bool isValidMove(int x1, int y1, int x2, int y2);
    void move(int x1, int y1, int x2, int y2);
    bool hasMoves();
    int countPegs();
    // ��Game���public������ӣ�
    void toggleAIHint();  // ����AI��ʾ����
    // ��������ȡ��Ϸ״̬���ڴ浵
    const Board& getBoard() const { return board; }
    int getSelectedX() const { return selectedX; }
    int getSelectedY() const { return selectedY; }
    const std::vector<std::vector<std::vector<int>>>& getBoardHistory() const { return boardHistory; }
    const std::vector<int>& getSelectedXHistory() const { return selectedXHistory; }
    const std::vector<int>& getSelectedYHistory() const { return selectedYHistory; }

private:
    Board board;
    Setting setting;
    int selectedX, selectedY;
    std::vector<std::vector<std::vector<int>>> boardHistory;
    std::vector<int> selectedXHistory, selectedYHistory;
    std::vector<std::vector<int>> initialBoard;
    // �� game.h �ļ��� Game ��� private ������ӣ�
    DWORD lastAIUpdateTime;  // �����ϴθ���AI��ʾ��ʱ��
    bool animating;
    int animFromX, animFromY, animToX, animToY, animMidX, animMidY;
    DWORD animStartTime;
    int animDuration;
    double lastFrameTime;  // ������һ֡ʱ�䣬ȷ��ƽ����Ⱦ
    // ��Game���private������ӣ�
    AIHelper aiHelper;
    AIMove aiSuggestion;
    bool showAIHint;
};