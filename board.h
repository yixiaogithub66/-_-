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
    // ��Board���������޸���Щ����Ϊconst
    bool isValid(int x, int y) const;  // ���const�޶���
    int get(int x, int y) const;       // ���const�޶���
    void set(int x, int y, int val);
    BoardType getType() const;
    int sizeX() const;
    int sizeY() const;

    // ɾ���ظ���������ֻ����ʵ��
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