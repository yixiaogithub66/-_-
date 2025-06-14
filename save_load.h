#pragma once
#include "board.h"
#include <string>
#include <vector>

class SaveManager {
public:
    SaveManager();

    // ±£´æÓÎÏ·×´Ì¬
    bool saveGame(const Board& board,
        int selectedX, int selectedY,
        const std::vector<std::vector<std::vector<int>>>& history,
        const std::vector<int>& selXHistory,
        const std::vector<int>& selYHistory,
        const std::string& filename = "save.dat");

    // ¼ÓÔØÓÎÏ·×´Ì¬
    bool loadGame(Board& board,
        int& selectedX, int& selectedY,
        std::vector<std::vector<std::vector<int>>>& history,
        std::vector<int>& selXHistory,
        std::vector<int>& selYHistory,
        BoardType& boardType,
        const std::string& filename = "save.dat");

    // ¼ì²é´æµµÊÇ·ñ´æÔÚ
    bool saveExists(const std::string& filename) const;

private:
    std::string saveDir; // ´æµµÄ¿Â¼
};

// ´æµµ²Ëµ¥º¯Êı
BoardType saveLoadMenu(BoardType currentType);