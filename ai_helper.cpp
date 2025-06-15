#include "ai_helper.h"
#include <iostream> 
#include <graphics.h> 
#include <algorithm> 
#include <vector>
#include <string> // Required for std::to_string

// Define an initial search depth and an overall maximum for iterative deepening
const int MAX_OVERALL_AI_SEARCH_DEPTH = 8; // Ultimate limit for iterative deepening. 

AIHelper::AIHelper() {
    // Constructor
}

AIHelper::~AIHelper() {
    // Destructor
}

bool AIHelper::isReady() const {
    return true;
}

int AIHelper::countPegs(const Board& board) const {
    int pegCount = 0;
    const auto& boardData = board.data();
    for (const auto& row : boardData) {
        for (int cellState : row) {
            if (cellState == 1) {
                pegCount++;
            }
        }
    }
    return pegCount;
}

bool AIHelper::hasMoves(const Board& board) const {
    return !generatePossibleMoves(board).empty();
}


Board AIHelper::applyMove(const Board& board, const AIMove& move) const {
    Board newBoard = board;
    int midX = (move.fromX + move.toX) / 2;
    int midY = (move.fromY + move.toY) / 2;

    newBoard.set(move.fromX, move.fromY, 0);
    newBoard.set(midX, midY, 0);
    newBoard.set(move.toX, move.toY, 1);
    return newBoard;
}


std::vector<AIMove> AIHelper::generatePossibleMoves(const Board& board) const {
    std::vector<AIMove> moves;
    const int rows = board.sizeX();
    // const int cols = board.sizeY(); // Assuming board.sizeY() gives a consistent column count or max
    BoardType type = board.getType();

    if (rows == 0) return moves;

    int standard_dx[] = { 0, 0, 2, -2 };
    int standard_dy[] = { 2, -2, 0, 0 };
    int num_standard_directions = 4;

    int diagonal_dx[] = { 2, 2, -2, -2 };
    int diagonal_dy[] = { 2, -2, 2, -2 };
    int num_diagonal_directions = 4;

    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < board.sizeY(); ++c) { // Use board.sizeY() for column iteration
            if (!board.isValid(r, c) || board.get(r, c) != 1) continue;

            // Try standard jumps
            for (int i = 0; i < num_standard_directions; ++i) {
                int toR = r + standard_dx[i];
                int toC = c + standard_dy[i];
                int midR = r + standard_dx[i] / 2;
                int midC = c + standard_dy[i] / 2;

                if (board.isValid(toR, toC) && board.get(toR, toC) == 0 &&
                    board.isValid(midR, midC) && board.get(midR, midC) == 1) {
                    moves.push_back({ r, c, toR, toC, "Standard Jump" });
                }
            }

            if (type == TRIANGLE || type == PYRAMID || type == STAR) {
                for (int i = 0; i < num_diagonal_directions; ++i) {
                    int toR = r + diagonal_dx[i];
                    int toC = c + diagonal_dy[i];
                    int midR = r + diagonal_dx[i] / 2;
                    int midC = c + diagonal_dy[i] / 2;

                    if (board.isValid(toR, toC) && board.get(toR, toC) == 0 &&
                        board.isValid(midR, midC) && board.get(midR, midC) == 1) {
                        moves.push_back({ r, c, toR, toC, "Diagonal Jump" });
                    }
                }
            }
        }
    }
    return moves;
}

int AIHelper::evaluateBoard(const Board& board) const {
    int pegs = countPegs(board);
    if (pegs == 1) {
        return 10000;
    }
    if (!hasMoves(board)) {
        return -20000 - pegs;
    }
    return -pegs * 10;
}

AISearchResult AIHelper::solveRecursive(Board currentBoard, int depth, int maxDepth) {
    std::vector<std::vector<int>> boardKey = currentBoard.data();
    if (transpositionTable.count(boardKey)) {
        return transpositionTable[boardKey];
    }

    int currentScore = evaluateBoard(currentBoard);

    if (depth == maxDepth || currentScore == 10000 || !hasMoves(currentBoard)) {
        AISearchResult res;
        res.score = currentScore - depth;
        res.solutionExists = true;
        res.bestFirstMove = { -1, -1, -1, -1, "Terminal state" };
        transpositionTable[boardKey] = res;
        return res;
    }

    std::vector<AIMove> possibleMoves = generatePossibleMoves(currentBoard);

    AISearchResult bestResultForThisNode;
    bestResultForThisNode.solutionExists = false;

    for (const auto& move : possibleMoves) {
        Board nextBoard = applyMove(currentBoard, move);
        AISearchResult resultFromChild = solveRecursive(nextBoard, depth + 1, maxDepth);

        if (resultFromChild.solutionExists) {
            if (!bestResultForThisNode.solutionExists || resultFromChild.score > bestResultForThisNode.score) {
                bestResultForThisNode.score = resultFromChild.score;
                bestResultForThisNode.bestFirstMove = move;
                bestResultForThisNode.solutionExists = true;
            }
        }
    }

    if (!bestResultForThisNode.solutionExists) {
        bestResultForThisNode.score = currentScore - depth;
        bestResultForThisNode.bestFirstMove = { -1,-1,-1,-1, "No improving moves found at this depth" };
        bestResultForThisNode.solutionExists = true;
    }

    transpositionTable[boardKey] = bestResultForThisNode;
    return bestResultForThisNode;
}

AIMove AIHelper::getBestMove(const Board& board) {
    AIMove finalBestMove = { -1, -1, -1, -1, "Local AI: No solution found." };
    int bestOverallScore = -1000000; // Initialize with a very low score (using 1e9 can cause issues with int)

    bool winFound = false;

    for (int currentMaxIterativeDepth = 1; currentMaxIterativeDepth <= MAX_OVERALL_AI_SEARCH_DEPTH; ++currentMaxIterativeDepth) {
        transpositionTable.clear();

        AISearchResult iterationResult = solveRecursive(board, 0, currentMaxIterativeDepth);

        if (iterationResult.solutionExists) {
            if (iterationResult.score > 9000 && iterationResult.score <= 10000) {
                if (!winFound || iterationResult.score > bestOverallScore) {
                    finalBestMove = iterationResult.bestFirstMove;
                    bestOverallScore = iterationResult.score;
                    winFound = true;
                    // Keep reason for internal logic, but it won't be drawn
                    finalBestMove.reason = "Local AI: Win found! Depth " + std::to_string(currentMaxIterativeDepth) +
                        ", Path Score: " + std::to_string(iterationResult.score);
                }
            }

            if (!winFound && iterationResult.score > bestOverallScore) {
                finalBestMove = iterationResult.bestFirstMove;
                bestOverallScore = iterationResult.score;
                // Keep reason for internal logic, but it won't be drawn
                finalBestMove.reason = "Local AI: Best at depth " + std::to_string(currentMaxIterativeDepth) +
                    ", Score: " + std::to_string(iterationResult.score);
            }
        }
        if (winFound) break;
    }

    if (finalBestMove.fromX == -1) {
        std::vector<AIMove> currentPossibleMoves = generatePossibleMoves(board);
        if (!currentPossibleMoves.empty()) {
            finalBestMove = currentPossibleMoves[0];
            finalBestMove.reason = "Local AI: Fallback, first available move.";
        }
        else {
            finalBestMove.reason = "Local AI: No possible moves from current state.";
        }
    }
    return finalBestMove;
}

void AIHelper::drawHint(const AIMove& move, int baseX, int baseY, int cellSize) {
    // This function now only draws the visual hint (circles and line), not the text reason.

    if (move.fromX < 0 || move.toX < 0) {
        // Optionally, you could draw "No hint available" text here if desired,
        // but for now, it will just draw nothing if the move is invalid.
        // Example:
        // settextstyle(22, 0, _T("微软雅黑"));
        // settextcolor(RGB(180, 40, 40));
        // outtextxy(610, 120, _T("无可用AI提示")); // Adjust coordinates as needed
        return;
    }

    // Coordinates for drawing circles and lines
    int x1_center = baseX + move.fromY * cellSize + cellSize / 2;
    int y1_center = baseY + move.fromX * cellSize + cellSize / 2;
    int x2_center = baseX + move.toY * cellSize + cellSize / 2;
    int y2_center = baseY + move.toX * cellSize + cellSize / 2;

    // Highlight starting peg (e.g., blue circle)
    setlinecolor(RGB(20, 120, 220)); // Blue color for starting peg
    setlinestyle(PS_SOLID, 3);     // Thicker line
    circle(x1_center, y1_center, cellSize / 2 - 5); // Slightly smaller than cell

    // Highlight target hole (e.g., green circle)
    setlinecolor(RGB(40, 200, 40));  // Green color for target hole
    // setlinestyle(PS_SOLID, 3); // Keep same style or change if desired
    circle(x2_center, y2_center, cellSize / 2 - 5);

    // Draw dashed line for movement path
    setlinecolor(RGB(100, 160, 200)); // Light blue for path line
    setlinestyle(PS_DASH, 2);        // Dashed line, medium thickness
    line(x1_center, y1_center, x2_center, y2_center);

    // Reset line style to default for other drawing operations
    setlinestyle(PS_SOLID, 1);
    setlinecolor(BLACK); // Or your default line color

    // The following lines that drew the "reason" text are now removed/commented out:
    // settextstyle(22, 0, _T("微软雅黑"));
    // settextcolor(RGB(40, 80, 150));
    // outtextxy(610, 120, _T("AI 提示:")); // Or adjust position if you keep this title

    // TCHAR moveText[100];
    // _stprintf_s(moveText, _T("从 (%d,%d) 到 (%d,%d)"), move.fromX, move.fromY, move.toX, move.toY);
    // settextcolor(RGB(220, 80, 20));
    // outtextxy(610, 150, moveText); // Or adjust position

    // RECT r = { 610, 180, 790, 320 }; // Text box for reason
    // #ifdef UNICODE
    //     wchar_t wReason[256];
    //     MultiByteToWideChar(CP_ACP, 0, move.reason.c_str(), -1, wReason, sizeof(wReason)/sizeof(wchar_t));
    //     drawtext(wReason, &r, DT_WORDBREAK | DT_NOCLIP);
    // #else
    //     char szReason[256]; 
    //     strncpy_s(szReason, move.reason.c_str(), sizeof(szReason) -1);
    //     szReason[sizeof(szReason)-1] = '\0'; 
    //     drawtext(szReason, &r, DT_WORDBREAK | DT_NOCLIP);
    // #endif
}