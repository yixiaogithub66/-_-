#pragma once // Ԥ����ָ�ȷ����ͷ�ļ���һ�α����н�������һ�Σ��Է�ֹ���ظ��������µı������

#include "board.h" // ����������ͷ�ļ�����Ϊ�浵�Ͷ���������Ҫ�����������ݡ�
#include <string>  // ���� C++ ��׼���е� string �࣬���ڴ����ļ�����·����
#include <vector>  // ���� C++ ��׼���е� vector ���������ڴ洢��Ϸ��ʷ��¼�ȶ�̬���顣

// SaveManager �࣬��������Ϸ�ı���ͼ����߼���
// ����װ�˽���Ϸ״̬д���ļ��Լ����ļ���ȡ��Ϸ״̬�Ĺ��ܡ�
class SaveManager {
public:
    // ���캯�������ڳ�ʼ�� SaveManager ����
    // ���ܻ�����������Ĭ�ϵĴ浵Ŀ¼��
    SaveManager();

    // ���浱ǰ����Ϸ״̬��ָ���ļ���
    // @param board: ��ǰ����Ϸ���̶�����״̬�������档
    // @param selectedX: ��ǰѡ�е����ӵ� X ���ꡣ
    // @param selectedY: ��ǰѡ�е����ӵ� Y ���ꡣ
    // @param history: ����������״̬��ʷ��¼ (ÿһ��������̲���)��
    // @param selXHistory: ѡ������ X �������ʷ��¼��
    // @param selYHistory: ѡ������ Y �������ʷ��¼��
    // @param filename: �浵�ļ������ƣ�Ĭ��Ϊ "save.dat"��
    // @return bool: �������ɹ��򷵻� true�����򷵻� false��
    bool saveGame(const Board& board,
        int selectedX, int selectedY,
        const std::vector<std::vector<std::vector<int>>>& history,
        const std::vector<int>& selXHistory,
        const std::vector<int>& selYHistory,
        const std::string& filename = "save.dat");

    // ��ָ���ļ�������Ϸ״̬��
    // @param board: ���ڽ��ռ��ص�����״̬�����̶��� (�ᱻ�޸�)��
    // @param selectedX: ���ڽ��ռ��ص�ѡ������ X ����ı��� (�ᱻ�޸�)��
    // @param selectedY: ���ڽ��ռ��ص�ѡ������ Y ����ı��� (�ᱻ�޸�)��
    // @param history: ���ڽ��ռ��ص�����״̬��ʷ��¼�ı��� (�ᱻ�޸�)��
    // @param selXHistory: ���ڽ��ռ��ص�ѡ������ X ������ʷ��¼�ı��� (�ᱻ�޸�)��
    // @param selYHistory: ���ڽ��ռ��ص�ѡ������ Y ������ʷ��¼�ı��� (�ᱻ�޸�)��
    // @param boardType: ���ڽ��ռ��ص��������͵ı��� (�ᱻ�޸�)��ȷ�����غ�����ȷ�ؽ����̡�
    // @param filename: �浵�ļ������ƣ�Ĭ��Ϊ "save.dat"��
    // @return bool: ������سɹ��򷵻� true�����򷵻� false��
    bool loadGame(Board& board,
        int& selectedX, int& selectedY,
        std::vector<std::vector<std::vector<int>>>& history,
        std::vector<int>& selXHistory,
        std::vector<int>& selYHistory,
        BoardType& boardType, // �������������ڴ��ݼ��ص���������
        const std::string& filename = "save.dat");

    // ���ָ���Ĵ浵�ļ��Ƿ���ڡ�
    // 'const' �����˷��������޸Ķ����״̬��
    // @param filename: Ҫ���Ĵ浵�ļ������ơ�
    // @return bool: ����ļ������򷵻� true�����򷵻� false��
    bool saveExists(const std::string& filename = "save.dat") const; // ����ָ���ļ�������Ĭ��Ϊ "save.dat"


private:
    std::string saveDir; // �洢�浵�ļ���Ŀ¼·����
    // ���磬������ "./saves/" ���û��ĵ��µ��ض��ļ��С�
};

// ������������ʾ�浵/�����˵����棬��������Ӧ�Ĳ�����
// �˺������ܻ����û�ѡ���Ǳ��浱ǰ��Ϸ������֮ǰ����Ϸ�����Ƿ������˵���
// @param currentType: ��ǰ��Ϸ���������ͣ���������˲�ͬ���͵Ĵ浵�������Ϳ��ܻᱻ�ı䡣
// @return BoardType: ������ɺ��ر��Ǽ�����Ϸ�󣩵��������͡����δ���ػ����ʧ�ܣ����ܷ��ش���� currentType��
BoardType saveLoadMenu(BoardType currentType);