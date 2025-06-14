#pragma once
#include <vector>
#include <graphics.h>

enum BoardType { CROSS, DIAMOND, TRIANGLE, TRAPEZOID, PYRAMID, STAR };


class Board {
public:
    Board(BoardType t);
    void reset();
    void draw(int bx = 80, int by = 80, int cell = 62, int selectedX = -1, int selectedY = -1,
        int mx = -1, int my = -1, bool showHint = true, bool isHighQuality = true);
    // 在Board类声明中修改这些方法为const
    bool isValid(int x, int y) const;  // 添加const限定符
    int get(int x, int y) const;       // 添加const限定符
    void set(int x, int y, int val);
    BoardType getType() const;
    int sizeX() const;
    int sizeY() const;

    // 删除重复的声明，只保留实现
    const std::vector<std::vector<int>>& data() const {
        return board;
    }

    std::vector<std::vector<int>>& data() {
        return board;
    }
private:
    BoardType type;
    std::vector<std::vector<int>> board;
};

BoardType boardMenu();