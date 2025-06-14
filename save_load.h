#pragma once
#include "board.h"
#include <string>
#include <vector>

class SaveManager {
public:
    SaveManager();

    // ������Ϸ״̬
    bool saveGame(const Board& board,
        int selectedX, int selectedY,
        const std::vector<std::vector<std::vector<int>>>& history,
        const std::vector<int>& selXHistory,
        const std::vector<int>& selYHistory,
        const std::string& filename = "save.dat");

    // ������Ϸ״̬
    bool loadGame(Board& board,
        int& selectedX, int& selectedY,
        std::vector<std::vector<std::vector<int>>>& history,
        std::vector<int>& selXHistory,
        std::vector<int>& selYHistory,
        BoardType& boardType,
        const std::string& filename = "save.dat");

    // ���浵�Ƿ����
    bool saveExists(const std::string& filename) const;

private:
    std::string saveDir; // �浵Ŀ¼
};

// �浵�˵�����
BoardType saveLoadMenu(BoardType currentType);