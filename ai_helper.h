#pragma once
#include "board.h"
#include <string>
#include <vector>
#include <utility> // For std::pair
#include <map>   // For transposition table

// AI Move Structure (remains the same)
struct AIMove {
    int fromX, fromY;     // Starting position
    int toX, toY;         // Target position
    std::string reason;   // Explanation for the move
};

// Structure to hold search results, including the score and the first move of a sequence
struct AISearchResult {
    int score;
    AIMove bestFirstMove;
    bool solutionExists;

    AISearchResult() : score(-1e9), bestFirstMove({ -1, -1, -1, -1, "No move found" }), solutionExists(false) {}
};

class AIHelper {
public:
    AIHelper();
    ~AIHelper();

    // Get the best move based on the current board state using local AI
    AIMove getBestMove(const Board& board);

    // Function to draw the AI's suggested move (can remain similar)
    void drawHint(const AIMove& move, int baseX = 80, int baseY = 80, int cellSize = 62);

    // This can be removed or always return true as API key is no longer needed
    bool isReady() const;

private:
    // Transposition table: maps a board state to its computed search result
    // The key will be the board's data (std::vector<std::vector<int>>)
    std::map<std::vector<std::vector<int>>, AISearchResult> transpositionTable;

    // Recursive search function
    AISearchResult solveRecursive(Board currentBoard, int depth, int maxDepth);

    // Generates all valid moves from the given board state
    std::vector<AIMove> generatePossibleMoves(const Board& board) const;

    // Evaluates the given board state
    // Returns a score (higher is better, e.g., -number_of_pegs)
    int evaluateBoard(const Board& board) const;

    // Counts pegs on the board
    int countPegs(const Board& board) const;

    // Checks if there are any moves left
    bool hasMoves(const Board& board) const;

    // Applies a move to a board state (returns a new board)
    Board applyMove(const Board& board, const AIMove& move) const;
};