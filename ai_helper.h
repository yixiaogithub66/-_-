#pragma once // Ԥ����ָ�ȷ����ͷ�ļ���һ�α�����ֻ������һ�Σ���ֹ�ظ�����

#include "board.h"    // ����������ͷ�ļ���AI��Ҫ���Ͳ�������״̬
#include <string>     // �����ַ����⣬���ڴ洢�ƶ�ԭ����ı���Ϣ
#include <vector>     // ���������⣬���ڴ洢�������ݺͿ��ܵ��ƶ��б�
#include <utility>    // ���� utility �⣬��ҪΪ��ʹ�� std::pair (��Ȼ������汾��AIMoveֱ�����˳�Ա������������������Ϊ�˽�������չ��ϰ��)
#include <map>        // ���� map �⣬����ʵ���û��� (transposition table)���Դ洢�Ѽ��������״̬������������������ظ�����

// AI �ƶ��ṹ�� (���ֲ���)
// ���ڱ�ʾ AI ������һ���ƶ���������㡢�յ���ƶ�ԭ��
struct AIMove {
    int fromX, fromY;     // �����ƶ�����ʼλ������ (x, y)
    int toX, toY;         // �����ƶ���Ŀ��λ������ (x, y)
    std::string reason;   // �Դ��ƶ��Ľ��ͻ�ԭ������ "�����м�����"
};

// �ṹ�壬���ڱ��� AI �����Ľ��
// ����������������ѵĵ�һ���ƶ��Լ��Ƿ���ڽ�
struct AISearchResult {
    int score;            // ��ǰ����״̬������·������������������Խ��ͨ����ʾ����Խ��
    AIMove bestFirstMove; // �ڵ�ǰ������֧�£�������߷�������ѵ�һ���ƶ�
    bool solutionExists;  // ����Ƿ��ҵ���һ���⣨���磬�ﵽ����Ŀ��״̬��

    // ���캯������ʼ����Ա����
    // score ��ʼ��Ϊһ���ǳ�С��ֵ����ʾ��ʼʱδ�ҵ���Ч����
    // bestFirstMove ��ʼ��Ϊ��Ч�ƶ���reason Ϊ "δ�ҵ��ƶ�"
    // solutionExists ��ʼ��Ϊ false
    AISearchResult() : score(-1e9), bestFirstMove({ -1, -1, -1, -1, "No move found" }), solutionExists(false) {}
};

// AI �����࣬��װ�˿����� AI ���߼�
class AIHelper {
public:
    // ���캯�������� AIHelper ����ĳ�ʼ��
    AIHelper();
    // �������������� AIHelper ��������ǰ����Դ���� (���磬�����̬�������ڴ�)
    ~AIHelper();

    // ���ݵ�ǰ����״̬��ȡ����ƶ� (ʹ�ñ��� AI �߼�)
    // @param board: ��ǰ�����̶���AI �����ڴ�״̬���о���
    // @return AIMove: AI �����������ƶ�
    AIMove getBestMove(const Board& board);

    // �����������������ϻ��� AI ������ƶ���ʾ (���Ա�������ʵ��)
    // @param move: AI ������ƶ�
    // @param baseX, baseY: ���̻��ƵĻ�׼���� x, y
    // @param cellSize: ���̵�Ԫ��Ĵ�С
    void drawHint(const AIMove& move, int baseX = 80, int baseY = 80, int cellSize = 62);

    // �˺��������Ƴ����������Ƿ��� true����Ϊ������Ҫ API ��Կ
    // ���ڼ�� AI �Ƿ�׼������ (���磬�ھɰ汾�п������ڼ���������ӻ� API ��Կ)
    // @return bool: ��� AI ׼�����򷵻� true�����򷵻� false
    bool isReady() const;

private:
    // �û��� (Transposition Table): ������״̬ӳ�䵽���������������
    // �� (key) �������̵����� (std::vector<std::vector<int>>)���������̵�Ψһ����
    // ֵ (value) �� AISearchResult�������ò��ֵ���������������ƶ�
    // ʹ���û�����Ա������ͬ����״̬���ظ����������������Ч��
    std::map<std::vector<std::vector<int>>, AISearchResult> transpositionTable;

    // �ݹ���������������ʵ�� AI �ľ����߼� (���� Minimax, Alpha-Beta��֦��)
    // @param currentBoard: ��ǰ��������������״̬
    // @param depth: ��ǰ���������
    // @param maxDepth: ��������������ȣ����������ķ�Χ
    // @return AISearchResult: �Ե�ǰ����״̬����������Ľ����������������������ƶ�
    AISearchResult solveRecursive(Board currentBoard, int depth, int maxDepth);

    // ���ɸ�������״̬��������Ч���ƶ�
    // @param board: ��ǰ�����̶���
    // @return std::vector<AIMove>: һ���������п�����Ч�ƶ����б�
    std::vector<AIMove> generatePossibleMoves(const Board& board) const;

    // ��������������״̬
    // @param board: ��Ҫ���������̶���
    // @return int: ����һ���������� (����Խ��Խ�ã����磬�������Ϊ -����������ʹ������Խ�ٷ���Խ��)
    int evaluateBoard(const Board& board) const;

    // ����������ʣ�����ӵ�����
    // @param board: ��ǰ�����̶���
    // @return int: ���������ӵ�����
    int countPegs(const Board& board) const;

    // ��鵱ǰ����״̬���Ƿ����κο��Խ��е��ƶ�
    // @param board: ��ǰ�����̶���
    // @return bool: �����������һ����Ч�ƶ��򷵻� true�����򷵻� false
    bool hasMoves(const Board& board) const;

    // ��һ���ƶ�Ӧ�õ�����״̬�� (����Ӧ���ƶ���������̶���)
    // �����������Ҫ����Ϊ������ AI ̽����ͬ�ƶ����еĺ�����������޸�ԭʼ����״̬
    // @param board: ԭʼ����״̬
    // @param move: ҪӦ�õ��ƶ�
    // @return Board: Ӧ���ƶ����������״̬
    Board applyMove(const Board& board, const AIMove& move) const;
};