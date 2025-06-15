#include "ai_helper.h" // ���� AI �������ͷ�ļ��������� AIHelper ��������Լ���صĽṹ�壨�� AIMove, AISearchResult����
#include <iostream>    // ���ñ�׼����������⣬��Ҫ���ڵ���Ŀ�ģ����磬�����Ϣ������̨����
#include <graphics.h>  // ���� EasyX ͼ�ο⣬��������Ϸ�����ϻ��� AI ��ʾ��ͼ��Ԫ�ء�
#include <algorithm>   // ���ñ�׼�㷨�⣬�����������򡢲��ҵȲ������ڴ˴�����δ����ֱ��ʹ�ã���������Ϊ��ͨ���Ի�δ����չ����
#include <vector>      // ���ñ�׼�����⣬AI �߼��й㷺ʹ�� vector ���洢����״̬�����ܵ��ƶ��б�ȡ�
#include <string>      // ���ñ�׼�ַ����⣬���ڴ����ַ����������� AIMove �д洢�ƶ�ԭ����ʽ�������

// Ϊ�������������һ����ʼ������Ⱥ�һ���ܵ����������ơ�
// MAX_OVERALL_AI_SEARCH_DEPTH �ǵ�������������ղ������ơ�AI �����������������ȵľ��档
const int MAX_OVERALL_AI_SEARCH_DEPTH = 8;

// AIHelper ��Ĺ��캯����
// �ڴ��� AIHelper ����ʱ���á�Ŀǰ���캯����Ϊ�գ�û�н�������ĳ�ʼ��������
AIHelper::AIHelper() {
    // ���캯����
}

// AIHelper �������������
// ������ AIHelper ����ʱ���á�Ŀǰ����������Ϊ�գ�û����Ҫ�����������Դ��
AIHelper::~AIHelper() {
    // ����������
}

// ��� AI �Ƿ�׼��������
// ������汾�У����� AI �߼��Ǳ��صģ����������ⲿ API ��Կ�����
// ���Դ˺����򵥵ط��� true����ʾ AI ���ǿ��õġ�
// 'const' ��ʾ�˺��������޸��κ� AIHelper ����ĳ�Ա������
bool AIHelper::isReady() const {
    return true; // ���� AI ����׼������
}

// ���㲢����������ʣ�����ӵ�������
// @param board: Ҫ�������̶���
// @return int: ������ֵΪ 1 (��������) �ĵ�Ԫ��������
// 'const' ��ʾ�˺��������޸��κ� AIHelper ����ĳ�Ա������Ҳ�����޸Ĵ���� board ����
int AIHelper::countPegs(const Board& board) const {
    int pegCount = 0; // ��ʼ�����Ӽ�����
    const auto& boardData = board.data(); // ��ȡ�������ݵĳ������ã����⸴��
    // �������̵�ÿһ��
    for (const auto& row : boardData) {
        // ������ǰ���е�ÿһ����Ԫ��
        for (int cellState : row) {
            if (cellState == 1) { // �����Ԫ��״̬Ϊ 1����ʾ������
                pegCount++; // ���Ӽ�������һ
            }
        }
    }
    return pegCount; // ������������
}

// ��鵱ǰ����״̬���Ƿ����κο��Խ��е���Ч�ƶ���
// @param board: Ҫ�������̶���
// @return bool: �����������һ����Ч�ƶ��򷵻� true�����򷵻� false��
// 'const' ��ʾ�˺��������޸��κ� AIHelper ����ĳ�Ա������Ҳ�����޸Ĵ���� board ����
bool AIHelper::hasMoves(const Board& board) const {
    // ͨ������ generatePossibleMoves �������п��ܵ��ƶ���
    // ������ɵ��ƶ��б�Ϊ�գ���˵��������Ч�ƶ���
    return !generatePossibleMoves(board).empty();
}

// ��һ��ָ�����ƶ�Ӧ�õ�һ������״̬�ϣ�������Ӧ�ø��ƶ���������̶���
// ������������޸�ԭʼ�� board ���󣬶��Ƿ���һ���µ� Board ʵ����
// @param board: ԭʼ����״̬��
// @param move: ҪӦ�õ� AI �ƶ���
// @return Board: Ӧ���ƶ����������״̬��
// 'const' ��ʾ�˺��������޸��κ� AIHelper ����ĳ�Ա������Ҳ�����޸Ĵ���� board ���󣨾��� move ���� const����
Board AIHelper::applyMove(const Board& board, const AIMove& move) const {
    Board newBoard = board; // ����ԭʼ���̵�һ������
    // ���㱻�������м����ӵ�����
    int midX = (move.fromX + move.toX) / 2;
    int midY = (move.fromY + move.toY) / 2;

    // ���������̵�״̬��
    newBoard.set(move.fromX, move.fromY, 0); // ����ʼλ������Ϊ�� (0)
    newBoard.set(midX, midY, 0);             // �����������м�λ������Ϊ�� (0)
    newBoard.set(move.toX, move.toY, 1);     // ��Ŀ��λ������Ϊ������ (1)
    return newBoard; // �����޸ĺ��������
}

// ���ɲ����ظ�������״̬�����п��ܵ���Ч�ƶ���
// @param board: ��ǰ�����̶���
// @return std::vector<AIMove>: һ������������Ч�ƶ���������
// 'const' ��ʾ�˺��������޸��κ� AIHelper ����ĳ�Ա������Ҳ�����޸Ĵ���� board ����
std::vector<AIMove> AIHelper::generatePossibleMoves(const Board& board) const {
    std::vector<AIMove> moves; // ���ڴ洢�ҵ�����Ч�ƶ�
    const int rows = board.sizeX(); // ��ȡ���̵�����������Ҫά�ȴ�С��
    // const int cols = board.sizeY(); // ���� board.sizeY() ����һ�µ����������ֵ�����б�ע�͵��ˣ�
    BoardType type = board.getType(); // ��ȡ���̵����ͣ���ͬ���͵����̿����в�ͬ���ƶ�����

    if (rows == 0) return moves; // �����������Ϊ0��ֱ�ӷ��ؿ��ƶ��б�

    // �����׼��Ծ���������仯 (�ϡ��¡����Ҹ�������)
    int standard_dx[] = { 0, 0, 2, -2 }; // �з���ı仯
    int standard_dy[] = { 2, -2, 0, 0 }; // �з���ı仯
    int num_standard_directions = 4;     // ��׼���������

    // ����Խ�����Ծ���������仯 (�ĸ��Խ��߷����������)
    int diagonal_dx[] = { 2, 2, -2, -2 }; // �з���ı仯
    int diagonal_dy[] = { 2, -2, 2, -2 }; // �з���ı仯
    int num_diagonal_directions = 4;      // �Խ��߷��������

    // �������̵�ÿһ����Ԫ��
    for (int r = 0; r < rows; ++r) {
        // ע�⣺����ʹ�� board.sizeY() ��ȡ��ǰ��������ʵ�ʵ�������ڶ�ά�ȵĴ�С
        for (int c = 0; c < board.sizeY(); ++c) {
            // �����ǰ��Ԫ����Ч�����ߵ�ǰ��Ԫ��û������ (ֵ��Ϊ1)��������
            if (!board.isValid(r, c) || board.get(r, c) != 1) continue;

            // ���Ա�׼��Ծ (�ϡ��¡�����)
            for (int i = 0; i < num_standard_directions; ++i) {
                int toR = r + standard_dx[i];     // ����Ŀ��������
                int toC = c + standard_dy[i];     // ����Ŀ��������
                int midR = r + standard_dx[i] / 2; // �����м������� (������������)
                int midC = c + standard_dy[i] / 2; // �����м������� (������������)

                // ����ƶ��Ƿ���Ч��
                // 1. Ŀ��λ�� (toR, toC) �Ƿ�����������Ϊ�� (ֵΪ0)
                // 2. �м�λ�� (midR, midC) �Ƿ����������������� (ֵΪ1)
                if (board.isValid(toR, toC) && board.get(toR, toC) == 0 &&
                    board.isValid(midR, midC) && board.get(midR, midC) == 1) {
                    // �����Ч���򽫴��ƶ���ӵ��б���
                    moves.push_back({ r, c, toR, toC, "Standard Jump" }); // �ƶ�ԭ��Ϊ "��׼��Ծ"
                }
            }

            // ������������� TRIANGLE (������), PYRAMID (��������), �� STAR (����)������Ⳣ�ԶԽ�����Ծ
            // ��Щ��������ͨ������Խ����ƶ�
            if (type == TRIANGLE || type == PYRAMID || type == STAR) {
                for (int i = 0; i < num_diagonal_directions; ++i) {
                    int toR = r + diagonal_dx[i];     // ����Խ���Ŀ��������
                    int toC = c + diagonal_dy[i];     // ����Խ���Ŀ��������
                    int midR = r + diagonal_dx[i] / 2; // ����Խ����м�������
                    int midC = c + diagonal_dy[i] / 2; // ����Խ����м�������

                    // ͬ�����Խ����ƶ�����Ч��
                    if (board.isValid(toR, toC) && board.get(toR, toC) == 0 &&
                        board.isValid(midR, midC) && board.get(midR, midC) == 1) {
                        // �����Ч���򽫴��ƶ���ӵ��б���
                        moves.push_back({ r, c, toR, toC, "Diagonal Jump" }); // �ƶ�ԭ��Ϊ "�Խ�����Ծ"
                    }
                }
            }
        }
    }
    return moves; // ���������ҵ�����Ч�ƶ�
}

// ������������״̬�ķ�����
// ����Խ�ߣ��������Խ�á�
// @param board: Ҫ���������̶���
// @return int: ����������
// 'const' ��ʾ�˺��������޸��κ� AIHelper ����ĳ�Ա������Ҳ�����޸Ĵ���� board ����
int AIHelper::evaluateBoard(const Board& board) const {
    int pegs = countPegs(board); // ��ȡ��ǰ�����ϵ�������
    if (pegs == 1) { // ���ֻʣһ�����ӣ������ʤ
        return 10000; // ����һ���ǳ��ߵķ�����ʾʤ��
    }
    if (!hasMoves(board)) { // ���û�п��е��ƶ��� (����������Ϊ1)
        // ����һ����ľ��棬������һ�����ֵ�δӮ��
        // ����һ���ǳ��͵ķ�������������Խ�࣬����Խ�͡�
        return -20000 - pegs;
    }
    // һ������£����������������ɷ��� (����Խ��Խ��)������һ��Ȩ�ء�
    // ����Ĳ����ǹ�����������������
    return -pegs * 10;
}

// �ݹ���������ʹ�������������������û������ҵ�����ƶ����С�
// @param currentBoard: ��ǰ��������������״̬��
// @param depth: ��ǰ��������ȡ�
// @param maxDepth: ���εݹ������������������ȡ�
// @return AISearchResult: ������� currentBoard ��������������͵��¸÷������ײ��ƶ���
AISearchResult AIHelper::solveRecursive(Board currentBoard, int depth, int maxDepth) {
    // ����ǰ����״̬ת��Ϊ�������û�����ĸ�ʽ (���磬���ڲ��Ķ�ά��������)��
    std::vector<std::vector<int>> boardKey = currentBoard.data();
    // ����û������Ƿ��Ѵ��ڵ�ǰ����״̬�ļ�������
    if (transpositionTable.count(boardKey)) {
        // ������ڣ�ֱ�ӷ��ش洢�Ľ���������ظ����㡣
        return transpositionTable[boardKey];
    }

    // ������ǰ����״̬�ļ�ʱ������
    int currentScore = evaluateBoard(currentBoard);

    // �ݹ����ֹ������
    // 1. �ﵽ���������� (depth == maxDepth)��
    // 2. ��ǰ������ʤ��״̬ (currentScore == 10000)��
    // 3. ��ǰ����û�п��е��ƶ� ( !hasMoves(currentBoard) )��
    if (depth == maxDepth || currentScore == 10000 || !hasMoves(currentBoard)) {
        AISearchResult res;
        // ���շ������ȥ��ǰ��ȣ��Թ���AIѡ��·�����̵Ľ⡣
        res.score = currentScore - depth;
        res.solutionExists = true; // ����ҵ���һ��ȷ����״̬���������վ֣�Ҳ�����������߽磩��
        res.bestFirstMove = { -1, -1, -1, -1, "Terminal state" }; // ���Ϊ��ֹ״̬��û�к����ƶ���
        transpositionTable[boardKey] = res; // ����������û���
        return res;
    }

    // ���ɵ�ǰ����״̬�����п��ܵ��ƶ���
    std::vector<AIMove> possibleMoves = generatePossibleMoves(currentBoard);

    AISearchResult bestResultForThisNode; // ���ڴ洢�Ӵ˽ڵ�����ܴﵽ����ѽ����
    bestResultForThisNode.solutionExists = false; // ��ʼ����δ�ҵ��⡣

    // �������п��ܵ��ƶ���
    for (const auto& move : possibleMoves) {
        Board nextBoard = applyMove(currentBoard, move); // Ӧ�õ�ǰ�ƶ����õ���һ������״̬��
        // �ݹ���� solveRecursive �������ƶ���ľ��档
        AISearchResult resultFromChild = solveRecursive(nextBoard, depth + 1, maxDepth);

        // ����ӽڵ������������һ����Ч�Ľ⡣
        if (resultFromChild.solutionExists) {
            // ������ǵ�ǰ�ڵ��ҵ��ĵ�һ���⣬�����ӽڵ�Ľ�ȵ�ǰ��֪����ѽ���á�
            if (!bestResultForThisNode.solutionExists || resultFromChild.score > bestResultForThisNode.score) {
                bestResultForThisNode.score = resultFromChild.score; // ������ѷ�����
                bestResultForThisNode.bestFirstMove = move;          // ���µ�����ѷ������ײ��ƶ���
                bestResultForThisNode.solutionExists = true;         // ������ҵ��⡣
            }
        }
    }

    // ��������������ӽڵ����Ȼû���ҵ�һ����Ч�� solutionExists Ϊ true ��·��
    // (���������������������������У�����ӽڵ㶼���� solutionExists=true���򲻻�������
    //  ����������·���������������Ϊ�˴���ĳ������������߼�����)��
    //  �����ܵ�����ǣ���ʹ�����ӽڵ㶼������ solutionExists=true�������ǵķ��������ܺܵ͡�
    //  �˴����߼�ȷ����ʹ���к���·���������룬��ǰ�ڵ㱾��Ҳ����һ������ֵ��
    if (!bestResultForThisNode.solutionExists) {
        bestResultForThisNode.score = currentScore - depth; // ʹ�õ�ǰ�ڵ�ļ�ʱ������������ȥ��ȣ���
        bestResultForThisNode.bestFirstMove = { -1,-1,-1,-1, "No improving moves found at this depth" }; // ���û���ҵ��Ľ����ƶ���
        bestResultForThisNode.solutionExists = true; // ��ʹû�С����á����ƶ���Ҳ��Ϊ����һ����������״̬��
    }

    transpositionTable[boardKey] = bestResultForThisNode; // ���˽ڵ�ļ����������û���
    return bestResultForThisNode;
}

// ��ȡ����ƶ��ĺ�����ʹ�õ�������� (Iterative Deepening Search)��
// @param board: ��ǰ����Ϸ����״̬��
// @return AIMove: AI �����������ƶ���
AIMove AIHelper::getBestMove(const Board& board) {
    AIMove finalBestMove = { -1, -1, -1, -1, "���� AI: δ�ҵ����������" }; // ��ʼ����������ƶ�Ϊ��Ч״̬��
    int bestOverallScore = -1000000; // ��ʼ��ȫ����ѷ���Ϊһ���ǳ��͵�ֵ��

    bool winFound = false; // ����Ƿ��Ѿ��ҵ���һ��ʤ����·����

    // �����ѭ���������1��ʼ�����������������ȣ�ֱ���ﵽ MAX_OVERALL_AI_SEARCH_DEPTH��
    for (int currentMaxIterativeDepth = 1; currentMaxIterativeDepth <= MAX_OVERALL_AI_SEARCH_DEPTH; ++currentMaxIterativeDepth) {
        transpositionTable.clear(); // ��ÿ���µĵ�����������ȣ���ʼǰ������û���
        // ������Ϊ֮ǰ��ȼ���ķ������ܲ������ڸ����������

// ���õݹ���⺯����ʹ�õ�ǰ�ĵ��������Ϊ���������ơ�
        AISearchResult iterationResult = solveRecursive(board, 0, currentMaxIterativeDepth);

        // ������ε����ҵ���һ����Ч�Ľ⡣
        if (iterationResult.solutionExists) {
            // ����Ƿ���ʤ������ (������9000��10000֮�䣬10000��ֱ��ʤ������ȥ��Ⱥ�����Ե�)��
            if (iterationResult.score > 9000 && iterationResult.score <= 10000) { // ����Ƿ���ʤ������
                // ����ǵ�һ���ҵ�ʤ�����棬���ߵ�ǰʤ������ķ������ߣ�·�����ţ���
                if (!winFound || iterationResult.score > bestOverallScore) {
                    finalBestMove = iterationResult.bestFirstMove; // ������������ƶ���
                    bestOverallScore = iterationResult.score;      // ����ȫ����ѷ�����
                    winFound = true;                              // ������ҵ�ʤ��·����
                    // �����ƶ�ԭ����Ҫ���ڵ��ԣ�����ֱ�ӻ��ƣ���
                    finalBestMove.reason = "���� AI: �ҵ�ʤ��! ��� " + std::to_string(currentMaxIterativeDepth) +
                        ", ·������: " + std::to_string(iterationResult.score);
                }
            }

            // �����ǰ��û���ҵ�ʤ�����棬���ұ��ε����Ľ������֪��ȫ����ѷ���Ҫ�á�
            if (!winFound && iterationResult.score > bestOverallScore) {
                finalBestMove = iterationResult.bestFirstMove; // ������������ƶ���
                bestOverallScore = iterationResult.score;      // ����ȫ����ѷ�����
                // �����ƶ�ԭ��
                finalBestMove.reason = "���� AI: ��� " + std::to_string(currentMaxIterativeDepth) +
                    " ʱ���, ����: " + std::to_string(iterationResult.score);
            }
        }
        // ����Ѿ��ҵ���ʤ��·�����������ǰ��ֹ���������Ϊ����ͨ����Ҫ����ʤ����
        if (winFound) break;
    }

    // �������������� finalBestMove ��Ȼ�ǳ�ʼ����Ч״̬����û���ҵ��κν⣩��
    if (finalBestMove.fromX == -1) {
        // ��Ϊ���÷��������Ի�ȡ��ǰ���п��ܵ��ƶ���
        std::vector<AIMove> currentPossibleMoves = generatePossibleMoves(board);
        if (!currentPossibleMoves.empty()) {
            // ������ڿ��е��ƶ�����ѡ���һ�����е��ƶ���Ϊ���ս����
            finalBestMove = currentPossibleMoves[0];
            finalBestMove.reason = "���� AI: ���÷�����ѡ���һ�������ƶ���";
        }
        else {
            // �����һ�����е��ƶ���û�У�����Ϊ��·���ߡ�
            finalBestMove.reason = "���� AI: ��ǰ״̬����·���ߡ�";
        }
    }
    return finalBestMove; // �������ռ����������ƶ���
}

// ����Ϸ�����ϻ��� AI ��ʾ���Ӿ�Ԫ�أ����磬������ʼ���ӡ�Ŀ��λ�ú��ƶ�·������
// �˺�������ֻ�����Ӿ���ʾ��ԲȦ�����������������ı�ԭ��
// @param move: AI ������ƶ���
// @param baseX, baseY: ���̻����������ϽǵĻ�׼���ꡣ
// @param cellSize: ������ÿ����Ԫ������ش�С��
void AIHelper::drawHint(const AIMove& move, int baseX, int baseY, int cellSize) {
    // ��� move ��Ч (���� fromX < 0)���򲻻����κ���ʾ��
    if (move.fromX < 0 || move.toX < 0) {
        // ��ѡ�������Ҫ�������ڴ˴����� "�޿�����ʾ" ֮����ı���
        // ����:
        // settextstyle(22, 0, _T("΢���ź�"));
        // settextcolor(RGB(180, 40, 40));
        // outtextxy(610, 120, _T("�޿���AI��ʾ")); // ������Ҫ��������
        return;
    }

    // �������ڻ���ԲȦ�����������ĵ����ꡣ
    int x1_center = baseX + move.fromY * cellSize + cellSize / 2; // ��ʼ�������ĵ���Ļ X ����
    int y1_center = baseY + move.fromX * cellSize + cellSize / 2; // ��ʼ�������ĵ���Ļ Y ����
    int x2_center = baseX + move.toY * cellSize + cellSize / 2;   // Ŀ��λ�����ĵ���Ļ X ����
    int y2_center = baseY + move.toX * cellSize + cellSize / 2;   // Ŀ��λ�����ĵ���Ļ Y ����

    // ������ʼ���� (���磬����ɫԲȦ)��
    setlinecolor(RGB(20, 120, 220)); // ����������ɫΪ��ɫ
    setlinestyle(PS_SOLID, 3);     // ����������ʽΪʵ�ߣ���ϸΪ3
    circle(x1_center, y1_center, cellSize / 2 - 5); // ����ԲȦ���뾶�ȵ�Ԫ����С

    // ����Ŀ���λ (���磬����ɫԲȦ)��
    setlinecolor(RGB(40, 200, 40));  // ����������ɫΪ��ɫ
    // setlinestyle(PS_SOLID, 3); // ���Ա�����ͬ��ʽ�������Ҫ����
    circle(x2_center, y2_center, cellSize / 2 - 5); // ����ԲȦ

    // �����ƶ�·�������ߡ�
    setlinecolor(RGB(100, 160, 200)); // ����������ɫΪ����ɫ
    setlinestyle(PS_DASH, 2);        // ����������ʽΪ���ߣ���ϸΪ2
    line(x1_center, y1_center, x2_center, y2_center); // ����ʼ�㵽Ŀ������ֱ��

    // ��������ʽ����ΪĬ��ֵ���Թ�������ͼ����ʹ�á�
    setlinestyle(PS_SOLID, 1); // �ָ�Ϊʵ�ߣ���ϸΪ1
    setlinecolor(BLACK);       // �ָ�Ϊ��ɫ����������Ŀ��ʹ�õ�Ĭ��������ɫ

    // ���»��� "reason" �ı��Ĵ������ѱ��Ƴ���ע�͵�����Ϊ drawHint ����ֻ�����Ӿ���ʾ��
    // settextstyle(22, 0, _T("΢���ź�"));
    // settextcolor(RGB(40, 80, 150));
    // outtextxy(610, 120, _T("AI ��ʾ:")); // ��������˱��⣬������Ҫ����λ��

    // TCHAR moveText[100];
    // _stprintf_s(moveText, _T("�� (%d,%d) �� (%d,%d)"), move.fromX, move.fromY, move.toX, move.toY);
    // settextcolor(RGB(220, 80, 20));
    // outtextxy(610, 150, moveText); // ������Ҫ����λ��

    // RECT r = { 610, 180, 790, 320 }; // ������ʾԭ����ı�������
    // #ifdef UNICODE
    //     wchar_t wReason[256];
    //     MultiByteToWideChar(CP_ACP, 0, move.reason.c_str(), -1, wReason, sizeof(wReason)/sizeof(wchar_t));
    //     drawtext(wReason, &r, DT_WORDBREAK | DT_NOCLIP); // ���ƿ��ַ�ԭ���ı�
    // #else
    //     char szReason[256];
    //     strncpy_s(szReason, move.reason.c_str(), sizeof(szReason) -1);
    //     szReason[sizeof(szReason)-1] = '\0'; //ȷ���Կ��ַ���β
    //     drawtext(szReason, &r, DT_WORDBREAK | DT_NOCLIP); // ���ƶ��ֽ��ַ�ԭ���ı�
    // #endif
}