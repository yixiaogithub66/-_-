#pragma once
#include "board.h"
#include "setting.h"
#include <vector>
#include "ai_helper.h" // Make sure ai_helper.h is also up-to-date with iterative deepening

// Enum for AI Hint status
enum class AIHintState {
    IDLE,        // AI is not active, button is normal
    WORKING,     // AI is calculating, button is green
    SHOWING_HINT // AI has provided a hint, button is red, hint is visible
};

class Game {
public:
    Game(Board b, const Setting& s);
    void run();
    // void drawGame(); // Made private as it's called by run()
    // void drawGameWithAnim(); // Made private
    // void playMoveAnimation(int x1, int y1, int x2, int y2); // Made private
    // void saveHistory(); // Made private
    // void undoMove(); // Made private
    // void restoreInitialBoard(); // Made private
    // bool isValidMove(int x1, int y1, int x2, int y2); // Made private
    // void move(int x1, int y1, int x2, int y2); // Made private
    // bool hasMoves(); // Made private
    // int countPegs(); // Made private
    void toggleAIButtonAction();

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

    DWORD lastAIUpdateTime;
    bool animating;
    int animFromX, animFromY, animToX, animToY, animMidX, animMidY;
    DWORD animStartTime;
    int animDuration; // General animation duration for piece movement

    AIHelper aiHelper;

    AIHintState aiHintCurrentState;
    AIMove currentAIHintMove; // Stores the hint calculated by AI

    void drawGame();
    void drawGameWithAnim();
    void playMoveAnimation(int x1, int y1, int x2, int y2);
    void saveHistory();
    void undoMove();
    void restoreInitialBoard();
    bool isValidMove(int x1, int y1, int x2, int y2);
    void move(int x1, int y1, int x2, int y2); // Conceptual move
    bool hasMoves();
    int countPegs();
};