#pragma once
#include "board.h"
#include "setting.h"
#include <vector>
// 在头文件引用部分添加
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
    // 在Game类的public部分添加：
    void toggleAIHint();  // 开关AI提示功能
    // 新增：获取游戏状态用于存档
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
    // 在 game.h 文件的 Game 类的 private 部分添加：
    DWORD lastAIUpdateTime;  // 跟踪上次更新AI提示的时间
    bool animating;
    int animFromX, animFromY, animToX, animToY, animMidX, animMidY;
    DWORD animStartTime;
    int animDuration;
    double lastFrameTime;  // 跟踪上一帧时间，确保平滑渲染
    // 在Game类的private部分添加：
    AIHelper aiHelper;
    AIMove aiSuggestion;
    bool showAIHint;
};