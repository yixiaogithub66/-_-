#pragma once
#include "board.h"
#include "setting.h"
#include <vector>

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
private:
    Board board;
    Setting setting;
    int selectedX, selectedY;
    std::vector<std::vector<std::vector<int>>> boardHistory;
    std::vector<int> selectedXHistory, selectedYHistory;
    std::vector<std::vector<int>> initialBoard;
    // ∂Øª≠œ‡πÿ
    bool animating;
    int animFromX, animFromY, animToX, animToY, animMidX, animMidY;
    int animFrames, animCurrentFrame;
    float animProgress;
};