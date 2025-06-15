#pragma once // 预处理指令，确保此头文件在一次编译中仅被包含一次，以防止因重复包含导致的编译错误。

#include <vector>     // 引用 C++ 标准库中的 vector 容器，用于存储棋盘的二维数据结构。
#include <graphics.h> // 引用 EasyX 图形库的头文件，用于棋盘的绘制和图形界面相关的操作。

// 枚举类型 BoardType，定义了不同形状的棋盘。
// 每种类型对应一种特定的初始棋子布局和棋盘边界。
enum BoardType {
    CROSS,     // 十字形棋盘
    DIAMOND,   // 菱形棋盘
    TRIANGLE,  // 三角形棋盘
    TRAPEZOID, // 梯形棋盘
    PYRAMID,   // 金字塔形棋盘 (通常指欧式孔明棋的初始布局)
    STAR       // 六角星形棋盘 (大卫之星)
};


// Board 类，封装了孔明棋棋盘的所有数据和操作。
// 它负责管理棋盘的状态、绘制、以及与棋盘交互的基本逻辑。
class Board {
public:
    // 构造函数，根据指定的棋盘类型 (BoardType) 初始化棋盘。
    // @param t: 要创建的棋盘类型。
    Board(BoardType t);

    // 重置棋盘到其初始状态。
    // 会根据当前棋盘的类型 (this->type) 重新设置棋子的布局。
    void reset();

    // 绘制棋盘界面。
    // @param bx: 棋盘绘制区域左上角的 x 坐标，默认为 80。
    // @param by: 棋盘绘制区域左上角的 y 坐标，默认为 80。
    // @param cell: 每个单元格（棋位）的像素大小，默认为 62。
    // @param selectedX: 当前被选中棋子的 x 坐标，默认为 -1 (无选中)。
    // @param selectedY: 当前被选中棋子的 y 坐标，默认为 -1 (无选中)。
    // @param mx: 鼠标当前的 x 坐标，用于高亮显示鼠标悬停的棋位，默认为 -1。
    // @param my: 鼠标当前的 y 坐标，用于高亮显示鼠标悬停的棋位，默认为 -1。
    // @param showHint: 是否显示AI提示或其他提示信息，默认为 true。
    // @param isHighQuality: 是否使用高质量绘制（例如，带抗锯齿的圆形棋子），默认为 true。
    void draw(int bx = 80, int by = 80, int cell = 62, int selectedX = -1, int selectedY = -1,
        int mx = -1, int my = -1, bool showHint = true, bool isHighQuality = true);

    // 检查给定的坐标 (x, y) 是否在棋盘的有效范围内。
    // 'const' 表明此方法不会修改对象的状态。
    // @param x: 要检查的 x 坐标。
    // @param y: 要检查的 y 坐标。
    // @return bool: 如果坐标有效则返回 true，否则返回 false。
    bool isValid(int x, int y) const;  // 添加 const 限定符，表示此函数不修改对象成员

    // 获取指定坐标 (x, y) 处棋位的值。
    // 通常，0 代表空格，1 代表有棋子，-1 或其他值代表无效区域或边界。
    // 'const' 表明此方法不会修改对象的状态。
    // @param x: 要获取值的 x 坐标。
    // @param y: 要获取值的 y 坐标。
    // @return int: 对应棋位的值。
    int get(int x, int y) const;       // 添加 const 限定符

    // 设置指定坐标 (x, y) 处棋位的值。
    // @param x: 要设置值的 x 坐标。
    // @param y: 要设置值的 y 坐标。
    // @param val: 要设置的新值。
    void set(int x, int y, int val);

    // 获取当前棋盘的类型。
    // 'const' 表明此方法不会修改对象的状态。
    // @return BoardType: 当前棋盘的 BoardType 枚举值。
    BoardType getType() const;

    // 获取棋盘在 x 方向（宽度）上的格子数量。
    // 'const' 表明此方法不会修改对象的状态。
    // @return int: 棋盘的宽度。
    int sizeX() const;

    // 获取棋盘在 y 方向（高度）上的格子数量。
    // 'const' 表明此方法不会修改对象的状态。
    // @return int: 棋盘的高度。
    int sizeY() const;

    // 获取对棋盘内部数据的常量引用。
    // 这允许外部代码以只读方式访问棋盘的布局数据，而不能修改它。
    // 'const' 在函数末尾表示此成员函数不会修改类的任何成员变量。
    // 'const' 在返回类型前表示返回的是一个指向常量的引用。
    // @return const std::vector<std::vector<int>>&: 对棋盘数据的常量引用。
    const std::vector<std::vector<int>>& data() const {
        return board;
    }

    // 获取对棋盘内部数据的引用 (非常量版本)。
    // 这允许外部代码修改棋盘的布局数据。需要谨慎使用，以避免破坏棋盘状态的封装性。
    // 通常，如果需要修改棋盘，应通过类提供的公共方法（如 set）进行。
    // @return std::vector<std::vector<int>>&: 对棋盘数据的引用。
    std::vector<std::vector<int>>& data() {
        return board;
    }

private:
    BoardType type; // 存储当前棋盘的类型 (例如 CROSS, DIAMOND 等)。
    std::vector<std::vector<int>> board; // 二维向量，实际存储棋盘的布局。
    // 向量中的每个整数代表对应棋位的状态 (例如，0为空，1为棋子)。
};

// 函数声明：显示棋盘类型选择菜单，并返回用户选择的棋盘类型。
// 此函数通常会创建一个菜单界面，让用户点击选择不同的棋盘样式。
// @return BoardType: 用户选择的棋盘类型。
BoardType boardMenu();