#pragma once
#include <vector>
#include <graphics.h>

enum BoardType { CROSS, DIAMOND, TRIANGLE };

class Board {
public:
    Board(BoardType t);
    void reset();
    void draw(int bx = 80, int by = 80, int cell = 62, int selectedX = -1, int selectedY = -1, int mx = -1, int my = -1, bool showHint = true);
    bool isValid(int x, int y);
    int get(int x, int y);
    void set(int x, int y, int val);
    BoardType getType() const;
    int sizeX() const;
    int sizeY() const;
    std::vector<std::vector<int>>& data();
private:
    BoardType type;
    std::vector<std::vector<int>> board;
};

BoardType boardMenu();