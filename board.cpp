#include "board.h"   // 引用自定义的棋盘类头文件，包含了 Board 类的声明和 BoardType 枚举等。
#include <tchar.h>    // 引用 tchar.h 头文件，用于支持通用字符类型（如 TCHAR），使得代码更容易在 ANSI 和 Unicode 环境下编译。
#include <cmath>      // 引用 cmath 数学库，用于数学运算，例如此处的 abs()绝对值函数。

// Board 类的构造函数。
// 当创建一个 Board 对象时，会调用此构造函数。
// @param t: 棋盘的类型 (BoardType 枚举值，例如 CROSS, DIAMOND 等)。
// 它会初始化成员变量 'type' 为传入的类型 't'，然后调用 reset() 方法来根据该类型设置棋盘的初始布局。
Board::Board(BoardType t) : type(t) {
    reset(); // 调用 reset 方法初始化棋盘布局
}

// Board 类的 reset 方法。
// 根据当前棋盘的类型 (this->type) 来重置棋盘的布局到其初始状态。
// board 是一个 std::vector<std::vector<int>> 成员变量，其中：
// -1: 代表棋盘外的无效区域或边界。
//  0: 代表棋盘上的一个空格（可以放棋子的位置）。
//  1: 代表棋盘上有一个棋子。
void Board::reset() {
    if (type == CROSS) { // 如果棋盘类型是十字形 (CROSS)
        // 十字棋盘通常是一个 7x7 的网格，但角落部分是无效的。
        // board.assign(rows, std::vector<int>(cols, initial_value)) 用于将二维向量初始化为指定行数、列数和初始值。
        board.assign(7, std::vector<int>(7, 1)); // 初始化一个 7x7 的棋盘，所有位置先假设都有棋子 (1)。
        // 将四个 2x2 的角落区域标记为无效区域 (-1)。
        for (int i : {0, 1, 5, 6}) // 遍历行索引 0, 1, 5, 6
            for (int j : {0, 1, 5, 6}) // 遍历列索引 0, 1, 5, 6
                board[i][j] = -1;    // 将 board[i][j] 设置为无效
        board[3][3] = 0; // 将中心位置 (3,3) 设置为空格 (0)，这是孔明棋的起始空位。
    }
    else if (type == DIAMOND) { // 如果棋盘类型是菱形 (DIAMOND)
        // 菱形棋盘通常在一个 5x5 的网格内。
        board.assign(5, std::vector<int>(5, 0)); // 初始化一个 5x5 的棋盘，所有位置先设置为空格 (0)。
        // 使用曼哈顿距离来确定菱形区域内的有效棋位。
        // abs(i - centerX) + abs(j - centerY) <= radius 定义了一个菱形。
        for (int i = 0; i < 5; ++i)
            for (int j = 0; j < 5; ++j)
                if (abs(i - 2) + abs(j - 2) <= 2) // 中心点 (2,2)，半径为 2
                    board[i][j] = 1; // 在菱形区域内的位置设置为有棋子 (1)。
        board[2][2] = 0; // 将中心位置 (2,2) 设置为空格 (0)。
    }
    else if (type == TRIANGLE) { // 如果棋盘类型是三角形 (TRIANGLE)
        // 三角形棋盘，通常是逐行增加棋子数量。这里实现的是一个底边为5的等腰直角三角形或类似形状。
        board.clear(); // 清空棋盘数据，准备重新填充。
        for (int i = 0; i < 5; ++i) { // 遍历5行
            std::vector<int> row(5, 0); // 创建一个包含5个空格 (0) 的行。
            for (int j = 0; j <= i; ++j) // 在第 i 行，有 i+1 个棋子。
                row[j] = 1; // 将前 j+1 个位置设置为有棋子 (1)。
            board.push_back(row); // 将构建好的行添加到棋盘中。
        }
        board[0][0] = 0; // 将顶点 (0,0) 设置为空格 (0)。
    }
    else if (type == TRAPEZOID) { // 如果棋盘类型是梯形 (TRAPEZOID)
        // 梯形棋盘 - 定义为一个上宽下窄的形状。
        // 使用一个 5行 x 7列 的网格作为基础，无效区域标记为 -1。
        board.assign(5, std::vector<int>(7, -1)); // 初始化为 5x7，所有位置默认为无效 (-1)。
        // 第一行 (最上面，7个棋子)
        for (int j = 0; j < 7; j++) {
            board[0][j] = 1;
        }
        // 第二行 (6个棋子)
        for (int j = 0; j < 6; j++) {
            board[1][j] = 1;
        }
        // 第三行 (5个棋子)
        for (int j = 0; j < 5; j++) {
            board[2][j] = 1;
        }
        // 第四行 (4个棋子)
        for (int j = 0; j < 4; j++) {
            board[3][j] = 1;
        }
        // 第五行 (最下面，3个棋子)
        for (int j = 0; j < 3; j++) {
            board[4][j] = 1;
        }
        // 设置一个空位，例如在第三行的中间位置。
        board[2][3] = 0;
    }
    else if (type == PYRAMID) { // 如果棋盘类型是金字塔形 (PYRAMID) / 正三角形
        // 金字塔棋盘通常指欧式孔明棋的三角形棋盘。
        // 这里实现的是一个对称的三角形，在一个 5x5 的网格内。
        board.assign(5, std::vector<int>(5, -1)); // 初始化为 5x5，所有位置默认为无效 (-1)。
        for (int i = 0; i < 5; i++) { // 遍历每一行
            // 计算当前行棋子的起始和结束列索引，以形成三角形。
            // 对于一个高度为 N 的三角形，第 i 行 (0-indexed) 的棋子数量是 i+1。
            // 这里是另一种对称金字塔的实现方式。
            int start = 2 - i / 2; // 根据行号计算起始列
            int end = 2 + i / 2;   // 根据行号计算结束列
            if (i % 2 != 0) { // 奇数行，棋子数量比偶数行多一个 (针对某种特定金字塔)
                // 这个金字塔的逻辑似乎是：
                // i=0: start=2, end=2 (1 peg)
                // i=1: start=1, end=2 (2 pegs if start=2-(1/2)=1.5 -> 1, end=2+(1/2)=2.5 -> 2 for typical triangle)
                //      or start=2, end=2 for a centered line of 1
                // 这里的 start/end 逻辑会产生一个更像“钻石尖端”或“正方形金字塔”的布局，而不是标准的等边三角形孔明棋。
                // 假设是标准英式/欧式金字塔 (每行棋子数 1, 2, 3, 4, 5):
                // for (int r = 0; r < 5; ++r) {
                //    int num_pegs_in_row = r + 1;
                //    int start_col = (5 - num_pegs_in_row) / 2; // Centering
                //    for (int c = 0; c < num_pegs_in_row; ++c) {
                //        board[r][start_col + c] = 1;
                //    }
                // }
                // 当前代码的逻辑:
                // i=0: s=2, e=2. pegs at [0][2]
                // i=1: s=1, e=2 (if i/2 integer division). pegs at [1][1], [1][2]
                // i=2: s=1, e=3. pegs at [2][1],[2][2],[2][3]
                // i=3: s=0, e=3. pegs at [3][0],[3][1],[3][2],[3][3]
                // i=4: s=0, e=4. pegs at [4][0],[4][1],[4][2],[4][3],[4][4]
                // 这是一个顶部对齐的金字塔，如果棋盘是正方形的，则它可能不是标准的等边三角形。
                // 如果是标准的等边三角形，可能需要调整start/end的计算或棋盘的表示方式。
                // 对于一个顶点向上的等边三角形，第i行应该有i+1个棋子，并且它们应该居中。
                // 让我们假设它是想要一个类似这样的金字塔：
                //   *
                //  * *
                // * * *
                // ...
                // 对于5行，第 i 行 (0-indexed) 有 i+1 个棋子。
                // 假设棋盘宽度为5，那么第i行的棋子应该从 (4-i)/2 开始放置。
                // 例如：
                // i=0: (4-0)/2 = 2.  board[0][2]=1
                // i=1: (4-1)/2 = 1.  board[1][1]=1, board[1][2]=1
                // i=2: (4-2)/2 = 1.  board[2][1]=1, board[2][2]=1, board[2][3]=1 (不对，应该是board[2][0], board[2][1], board[2][2] for a left-aligned triangle or centered)
                // 当前代码的 start, end 逻辑似乎是为某种特定的金字塔形状设计的，而不是标准的等边三角形孔明棋。
            }
            for (int j = start; j <= end; j++) {
                if (j >= 0 && j < 5) { // 确保列索引在有效范围内
                    board[i][j] = 1;
                }
            }
        }
        // 设置一个空位，例如顶部的棋子。
        board[0][2] = 0; // 假设(0,2)是金字塔的顶点或一个有效的起始空位。
    }
    else if (type == STAR) { // 如果棋盘类型是星形 (STAR) / 大卫之星
        // 星形棋盘通常在一个特定形状的网格上。这里用一个 5x5 的网格来近似。
        board.assign(5, std::vector<int>(5, -1)); // 初始化为 5x5，所有位置默认为无效 (-1)。
        // 中心十字部分
        for (int i = 0; i < 5; i++) {
            board[2][i] = 1;  // 水平中线全部是棋子
            board[i][2] = 1;  // 垂直中线全部是棋子
        }
        // 添加四个角上的棋子，形成星形的突出部分。
        // 这会形成一个更像加号扩展的形状，而不是典型六角星。
        // 真正的六角星孔明棋棋盘结构更复杂。
        // 当前实现:
        // . . * . .
        // . * * * .
        // * * * * *
        // . * * * .
        // . . * . .
        // 这形成了一个钻石形或增强的十字形。
        board[1][1] = 1; // 左上角附近的棋子
        board[1][3] = 1; // 右上角附近的棋子
        board[3][1] = 1; // 左下角附近的棋子
        board[3][3] = 1; // 右下角附近的棋子
        // 设置中心位置为空格。
        board[2][2] = 0;
    }
}

// Board 类的 draw 方法。
// 负责在屏幕上绘制整个棋盘，包括棋子、空格、选中效果、鼠标悬停效果和提示。
// @param bx, by: 棋盘绘制区域左上角的基准坐标 (x, y)。
// @param cell: 每个棋盘格的像素大小。
// @param selX, selY: 当前选中的棋子的逻辑坐标 (行, 列)。如果为 -1，则无选中。
// @param mx, my: 鼠标当前的屏幕坐标 (x, y)，用于悬停效果。如果为 -1，则不处理悬停。
// @param showHint: 是否显示可走位置的提示。
// @param isHighQuality: 是否启用高质量绘制模式 (例如，更平滑的阴影和高光)。
void Board::draw(int bx, int by, int cell, int selX, int selY, int mx, int my, bool showHint, bool isHighQuality) {
    // 根据 isHighQuality 参数调整绘制细节，以提供不同级别的视觉效果。
    int shadowOffset = isHighQuality ? 4 : 2;    // 阴影的偏移量，高质量模式下更大。
    int pieceRadius = isHighQuality ? cell / 2 - 8 : cell / 2 - 10; // 棋子半径，高质量模式下棋子略大或边距不同。
    bool drawHighlight = isHighQuality; // 是否绘制高光，高质量模式下启用。

    // 设置一个柔和的米黄色作为整个绘图窗口的背景。
    setbkcolor(RGB(252, 240, 220));
    cleardevice(); // 清空当前绘图设备，并用新背景色填充。

    // 绘制一个大的圆角矩形作为棋盘区域的背景板，与窗口背景色相同，但可以有边框（如果设置）。
    setfillcolor(RGB(252, 240, 220)); // 填充颜色
    setlinecolor(RGB(252, 240, 220)); // 边框颜色 (与填充色相同，即无明显边框)
    fillroundrect(20, 20, 780, 630, 40, 40); // 绘制圆角矩形，参数为左上角x,y，右下角x,y，圆角椭圆的宽和高。

    // 定义各种元素的颜色。
    // COLORREF 是一个表示颜色的类型 (通常是 DWORD)。RGB(r,g,b)宏用于创建颜色值。
    COLORREF shadowColor = RGB(185, 90, 70);      // 棋子阴影的颜色 (偏暗的橙红色)
    COLORREF pieceColor = RGB(225, 105, 80);     // 棋子的主颜色 (鲜艳的橙红色)
    COLORREF pieceHoverColor = RGB(240, 115, 90); // 鼠标悬停在棋子上时的颜色 (更亮的橙红色)
    COLORREF highlightColor = RGB(255, 220, 200); // 棋子高光的颜色 (非常浅的橙色/米色)
    COLORREF emptyColor = RGB(245, 245, 240);    // 空格的填充颜色 (非常浅的灰色/米色)
    COLORREF emptyBorderColor = RGB(230, 220, 200); // 空格的边框颜色 (比填充色略深的米色)
    COLORREF validMoveColor = RGB(100, 200, 120); // 有效移动位置提示的颜色 (柔和的绿色)
    COLORREF selectedBorderColor = RGB(80, 180, 100); // 选中棋子外圈提示的颜色 (较深的绿色)

    // ... 其余绘制棋盘和棋子的代码 ...

    // 计算当前选中棋子的所有有效可走步数。
    std::vector<std::pair<int, int>> validMoves; // 用于存储可移动到的目标位置 (行, 列)。
    // 只有在显示提示 (showHint)、有棋子被选中 (selX, selY >= 0)、且选中的确实是棋子 (get(selX, selY) == 1) 时才计算。
    if (showHint && selX >= 0 && selY >= 0 && get(selX, selY) == 1) {
        // 标准的上下左右四个方向的跳跃。
        int dx[] = { 2, -2, 0, 0 }; // 行方向的位移 (跳两格)
        int dy[] = { 0, 0, 2, -2 }; // 列方向的位移 (跳两格)
        for (int d = 0; d < 4; ++d) { // 遍历四个方向
            int nx = selX + dx[d];      // 目标位置的行坐标
            int ny = selY + dy[d];      // 目标位置的列坐标
            int mx2 = selX + dx[d] / 2; // 中间被跳过棋子的行坐标
            int my2 = selY + dy[d] / 2; // 中间被跳过棋子的列坐标
            // 检查移动是否有效：目标位置需在棋盘内且为空，中间位置需在棋盘内且有棋子。
            if (isValid(nx, ny) && get(nx, ny) == 0 && isValid(mx2, my2) && get(mx2, my2) == 1)
                validMoves.emplace_back(nx, ny); // 将有效的目标位置添加到列表中。
        }
        // 对于三角形棋盘 (TRIANGLE)，可能存在额外的对角线跳跃规则。
        // 注意：标准的孔明棋规则通常不区分棋盘类型来改变基本跳跃方向，除非是六角棋盘。
        // 这里的对角线规则可能是特定于此项目对 TRIANGLE 棋盘的扩展。
        if (type == TRIANGLE) { // 如果是三角棋盘
            int tdx[] = { 2, -2, 2, -2 }; // 对角线跳跃的行方向位移
            int tdy[] = { 2, -2, -2, 2 }; // 对角线跳跃的列方向位移 (注意这组 dy 与 dx 对应，形成 (2,2), (-2,-2), (2,-2), (-2,2) 四个方向)
            for (int d = 0; d < 4; ++d) { // 遍历四个对角线方向
                int nx = selX + tdx[d];
                int ny = selY + tdy[d];
                int mx2 = selX + tdx[d] / 2;
                int my2 = selY + tdy[d] / 2;
                if (isValid(nx, ny) && get(nx, ny) == 0 && isValid(mx2, my2) && get(mx2, my2) == 1)
                    validMoves.emplace_back(nx, ny);
            }
        }
    }

    // 遍历棋盘上的所有格子并绘制它们。
    // board.size() 是棋盘的行数。
    for (int i = 0; i < board.size(); ++i) {
        // board[i].size() 是第 i 行的列数。
        for (int j = 0; j < board[i].size(); ++j) {
            // 计算当前格子左上角的屏幕坐标。
            int x = bx + j * cell;
            int y = by + i * cell;
            // 如果当前格子是无效区域 (-1)，则不绘制任何东西，直接跳到下一个格子。
            if (board[i][j] == -1) continue;

            bool hovered = false; // 标记当前鼠标是否悬停在此棋子上。
            // 检查鼠标悬停：鼠标坐标有效 (mx,my >=0)，当前格子是棋子 (board[i][j] == 1)。
            if (mx >= 0 && my >= 0 && board[i][j] == 1) {
                int cx = x + cell / 2; // 棋子中心的屏幕 X 坐标
                int cy = y + cell / 2; // 棋子中心的屏幕 Y 坐标
                int r = pieceRadius;   // 棋子的半径 (使用上面根据画质调整后的半径)
                int dist_x = mx - cx;  // 鼠标与棋子中心的 X 方向距离
                int dist_y = my - cy;  // 鼠标与棋子中心的 Y 方向距离
                // 如果鼠标到棋子中心的距离小于等于半径 (勾股定理)，则认为鼠标悬停在棋子上。
                if (dist_x * dist_x + dist_y * dist_y <= r * r) hovered = true;
            }

            // 如果当前格子是棋子 (board[i][j] == 1)
            if (board[i][j] == 1) {
                // 1. 绘制阴影 (为了立体感)
                setfillcolor(shadowColor); // 设置填充色为阴影颜色
                setlinecolor(shadowColor); // 设置边框色为阴影颜色 (实心圆无边框效果)
                // 绘制一个实心圆作为阴影，位置比棋子主体略微右下偏移。
                fillcircle(x + cell / 2 + shadowOffset, y + cell / 2 + shadowOffset, pieceRadius);

                // 2. 绘制棋子主体
                COLORREF c = hovered ? pieceHoverColor : pieceColor; // 根据是否悬停选择棋子颜色。
                setfillcolor(c); // 设置填充色
                setlinecolor(c); // 设置边框色
                fillcircle(x + cell / 2, y + cell / 2, pieceRadius); // 绘制棋子主体圆。

                // 3. 绘制高光效果 (模拟光源照射，增加立体感)
                if (drawHighlight) { // 仅在高质量模式下绘制高光
                    setfillcolor(highlightColor); // 设置填充色为高光颜色
                    setlinecolor(highlightColor); // 设置边框色为高光颜色
                    // 在棋子的左上角绘制一个小一点的亮色圆作为高光。
                    fillcircle(x + cell / 2 - pieceRadius / 2.5, y + cell / 2 - pieceRadius / 2.5, pieceRadius / 3.5);
                }


                // 4. 如果棋子被选中，并且允许显示提示，则绘制选中状态的外圈。
                if (showHint && i == selX && j == selY) {
                    // 绘制一个较粗的白色外圈作为背景。
                    setlinecolor(WHITE);        // 线条颜色为白色
                    setlinestyle(PS_SOLID, 4);  // 实线，粗细为4
                    circle(x + cell / 2, y + cell / 2, pieceRadius + 3); // 半径比棋子稍大

                    // 在白色外圈内绘制一个较细的绿色内圈作为选中标记。
                    setlinecolor(selectedBorderColor); // 线条颜色为选中的边框绿色
                    setlinestyle(PS_SOLID, 2);         // 实线，粗细为2
                    circle(x + cell / 2, y + cell / 2, pieceRadius + 3); // 与白色外圈同半径
                    setlinestyle(PS_SOLID, 1); // 恢复默认线条粗细为1，以防影响后续绘制。
                }
            }
            // 如果当前格子是空格 (board[i][j] == 0)
            else if (board[i][j] == 0) {
                // 1. 绘制空格的底盘样式。
                setfillcolor(emptyColor);       // 设置填充色为空格颜色
                setlinecolor(emptyBorderColor); // 设置边框色为空格边框颜色
                // 绘制一个较大的圆作为底色。
                fillcircle(x + cell / 2, y + cell / 2, pieceRadius + 2); // 半径比棋子略大一点
                // 可以在上面再画一个略小的圆，形成凹陷感或边框效果。
                // setlinestyle(PS_SOLID, 2); // 设置线条粗细
                // circle(x + cell / 2, y + cell / 2, cell / 2 - 10); // 绘制内圈
                // setlinestyle(PS_SOLID, 1); // 恢复默认线条粗细

                // 2. 如果允许显示提示，并且此空格是当前选中棋子的有效可走位置，则高亮显示。
                if (showHint) {
                    for (auto& mv : validMoves) { // 遍历所有有效移动目标位置
                        if (mv.first == i && mv.second == j) { // 如果当前空格是其中之一
                            setfillcolor(validMoveColor); // 设置填充色为有效移动提示颜色
                            setlinecolor(validMoveColor); // 设置边框色
                            // 用提示色填充空格，使其突出显示。
                            fillcircle(x + cell / 2, y + cell / 2, pieceRadius); // 大小与棋子相同或略小
                            break; // 找到后即可跳出循环
                        }
                    }
                }
            }
        }
    }
}

// 检查给定的逻辑坐标 (x, y) 是否在棋盘的有效区域内。
// 'const' 表示此方法不会修改任何 Board 对象的成员变量。
// @param x: 要检查的行坐标。
// @param y: 要检查的列坐标。
// @return bool: 如果坐标有效 (在棋盘边界内且不是标记为 -1 的无效格) 则返回 true，否则返回 false。
bool Board::isValid(int x, int y) const {
    return x >= 0 && y >= 0 &&                 // 坐标必须非负
        x < board.size() &&                 //行坐标必须在棋盘行数范围内
        y < board[x].size() &&              // 列坐标必须在棋盘对应行的列数范围内 (支持非矩形棋盘)
        board[x][y] != -1;                  // 该位置的值不能是 -1 (无效区域)
}

// 获取指定逻辑坐标 (x, y) 处棋位的值 (1:棋子, 0:空格, -1:无效)。
// 'const' 表示此方法不会修改任何 Board 对象的成员变量。
// @param x: 要获取值的行坐标。
// @param y: 要获取值的列坐标。
// @return int: 对应棋位的值。如果坐标无效，行为未定义（应先调用 isValid）。
int Board::get(int x, int y) const { return board[x][y]; }

// 设置指定逻辑坐标 (x, y) 处棋位的值。
// @param x: 要设置值的行坐标。
// @param y: 要设置值的列坐标。
// @param val: 要设置的新值 (1, 0, or -1)。
void Board::set(int x, int y, int val) { board[x][y] = val; }

// 获取当前棋盘的类型。
// 'const' 表示此方法不会修改任何 Board 对象的成员变量。
// @return BoardType: 当前棋盘的 BoardType 枚举值。
BoardType Board::getType() const { return type; }

// 获取棋盘在 x 方向（通常是行数）上的大小。
// 'const' 表示此方法不会修改任何 Board 对象的成员变量。
// @return int: 棋盘的行数。
int Board::sizeX() const { return board.size(); }

// 获取棋盘在 y 方向（通常是第一行的列数，或最大列数）上的大小。
// 注意：对于非矩形棋盘，不同行的列数可能不同。此函数返回第一行的列数。
// 如果棋盘为空，board[0] 会导致错误。应确保 board 不为空。
// 'const' 表示此方法不会修改任何 Board 对象的成员变量。
// @return int: 棋盘第一行的列数。如果棋盘没有行，则行为未定义。
int Board::sizeY() const {
    if (board.empty()) return 0; // 如果棋盘没有行，返回0列
    return board[0].size(); // 返回第一行的列数
}

// 函数：显示棋盘类型选择菜单，并等待用户选择。
// @return BoardType: 用户选择的棋盘类型。
BoardType boardMenu() {
    // 设置菜单背景色为淡蓝色。
    setbkcolor(RGB(220, 240, 255));
    cleardevice(); // 清屏并应用背景色。

    // 设置标题文本样式。
    settextstyle(45, 0, _T("微软雅黑")); // 字体大小45，"微软雅黑"字体。
    settextcolor(RGB(40, 80, 140));   // 文本颜色 (深蓝色)。
    outtextxy(220, 80, _T("选择棋盘样式")); // 在指定位置显示标题。

    // 设置选项按钮的文本样式。
    settextstyle(32, 0, _T("微软雅黑")); // 字体大小32。

    // 定义各个棋盘类型选项按钮的矩形区域 (RECT 结构包含 left, top, right, bottom)。
    // 调整了布局以适应更多选项，按钮更宽。
    RECT r1 = { 170,160,630,200 }; // 十字棋盘 (CROSS)
    RECT r2 = { 170,220,630,260 }; // 菱形棋盘 (DIAMOND)
    RECT r3 = { 170,280,630,320 }; // 三角棋盘 (TRIANGLE)
    RECT r4 = { 170,340,630,380 }; // 梯形棋盘 (TRAPEZOID) (新增)
    RECT r5 = { 170,400,630,440 }; // 金字塔棋盘 (PYRAMID) (新增)
    RECT r6 = { 170,460,630,500 }; // 星形棋盘 (STAR) (新增)

    // 设置按钮的填充颜色 (较浅的蓝灰色)。
    setfillcolor(RGB(200, 220, 240));
    // 绘制所有按钮的圆角矩形背景。
    fillroundrect(r1.left, r1.top, r1.right, r1.bottom, 15, 15); // 圆角半径15x15
    fillroundrect(r2.left, r2.top, r2.right, r2.bottom, 15, 15);
    fillroundrect(r3.left, r3.top, r3.right, r3.bottom, 15, 15);
    fillroundrect(r4.left, r4.top, r4.right, r4.bottom, 15, 15);
    fillroundrect(r5.left, r5.top, r5.right, r5.bottom, 15, 15);
    fillroundrect(r6.left, r6.top, r6.right, r6.bottom, 15, 15);

    // 设置按钮上文字的颜色 (更深的蓝黑色)。
    settextcolor(RGB(20, 40, 80));
    // 在每个按钮矩形内居中绘制对应的棋盘类型名称。
    // DT_CENTER: 水平居中, DT_VCENTER: 垂直居中, DT_SINGLELINE: 单行显示。
    drawtext(_T("十字棋盘"), &r1, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    drawtext(_T("菱形棋盘"), &r2, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    drawtext(_T("三角棋盘"), &r3, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    drawtext(_T("梯形棋盘"), &r4, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    drawtext(_T("金字塔棋盘"), &r5, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    drawtext(_T("星形棋盘"), &r6, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    // 进入无限循环，等待用户通过鼠标点击选择一个棋盘类型。
    while (true) {
        ExMessage m; // 用于存储鼠标消息的结构体。
        // peekmessage 函数检查是否有鼠标消息 (EM_MOUSE)，如果有则将其存入 m 并从消息队列中移除。
        // 使用 while(peekmessage) 是为了处理可能累积的多个鼠标消息，但通常一次点击只会有一个 WM_LBUTTONDOWN。
        while (peekmessage(&m, EM_MOUSE)) {
            if (m.message == WM_LBUTTONDOWN) { // 如果是鼠标左键按下的消息
                int x = m.x; // 获取鼠标点击的屏幕 X 坐标。
                int y = m.y; // 获取鼠标点击的屏幕 Y 坐标。
                // 检查鼠标点击位置是否在某个按钮的矩形区域内。
                if (x >= r1.left && x <= r1.right && y >= r1.top && y <= r1.bottom) return CROSS;
                if (x >= r2.left && x <= r2.right && y >= r2.top && y <= r2.bottom) return DIAMOND;
                if (x >= r3.left && x <= r3.right && y >= r3.top && y <= r3.bottom) return TRIANGLE;
                if (x >= r4.left && x <= r4.right && y >= r4.top && y <= r4.bottom) return TRAPEZOID;
                if (x >= r5.left && x <= r5.right && y >= r5.top && y <= r5.bottom) return PYRAMID;
                if (x >= r6.left && x <= r6.right && y >= r6.top && y <= r6.bottom) return STAR;
            }
        }
        Sleep(10); // 短暂休眠10毫秒，以降低CPU使用率，避免忙等待。
    }
}