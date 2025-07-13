#include <graphics.h>
#include <conio.h>
#include <time.h>
#include <iostream>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

// 游戏常量
#define ROWS 8
#define COLS 8
#define BLOCK_SIZE 52
#define OFFSET_X 17
#define OFFSET_Y 274
#define WIN_WIDTH 485
#define WIN_HEIGHT 917

// 方块结构体
struct Block {
    int type;    // 1-7代表7种图案
    int x, y;    // 屏幕坐标
    bool matched; // 是否被标记消除
};

// 全局变量
Block map[ROWS + 2][COLS + 2]; // 多出两行两列用于边界检查
IMAGE imgBlocks[7];            // 存储7种方块图片
IMAGE imgBg;                   // 背景图片
int clickCount = 0;            // 点击计数器
int selectRow1, selectCol1;    // 第一次选择的方块
int selectRow2, selectCol2;    // 第二次选择的方块
int score = 0;                 // 游戏分数

// 初始化游戏
void initGame() {
    initgraph(WIN_WIDTH, WIN_HEIGHT);
    loadimage(&imgBg, L"res/bg.png");

    // 加载方块图片
    for (int i = 0; i < 7; i++) {
        wchar_t path[30];
        swprintf_s(path, L"res/block%d.png", i + 1);
        loadimage(&imgBlocks[i], path, BLOCK_SIZE, BLOCK_SIZE);
    }

    srand(static_cast<unsigned int>(time(NULL)));
}

// 生成随机方块
void generateBlocks() {
    for (int i = 1; i <= ROWS; i++) {
        for (int j = 1; j <= COLS; j++) {
            map[i][j].type = 1 + rand() % 7;
            map[i][j].x = OFFSET_X + (j - 1) * (BLOCK_SIZE + 5);
            map[i][j].y = OFFSET_Y + (i - 1) * (BLOCK_SIZE + 5);
            map[i][j].matched = false;
        }
    }
}

// 绘制游戏界面
void drawGame() {
    putimage(0, 0, &imgBg);

    // 显示分数
    settextcolor(WHITE);
    settextstyle(36, 0, _T("Arial"));
    wchar_t scoreStr[20];
    swprintf_s(scoreStr, L"分数: %d", score);
    outtextxy(200, 50, scoreStr);

    // 绘制所有方块
    for (int i = 1; i <= ROWS; i++) {
        for (int j = 1; j <= COLS; j++) {
            if (!map[i][j].matched) {
                putimage(map[i][j].x, map[i][j].y, &imgBlocks[map[i][j].type - 1]);
            }
        }
    }
}

// 交换方块
void swapBlocks(int row1, int col1, int row2, int col2) {
    Block temp = map[row1][col1];
    map[row1][col1] = map[row2][col2];
    map[row2][col2] = temp;

    // 更新坐标
    map[row1][col1].x = OFFSET_X + (col1 - 1) * (BLOCK_SIZE + 5);
    map[row1][col1].y = OFFSET_Y + (row1 - 1) * (BLOCK_SIZE + 5);
    map[row2][col2].x = OFFSET_X + (col2 - 1) * (BLOCK_SIZE + 5);
    map[row2][col2].y = OFFSET_Y + (row2 - 1) * (BLOCK_SIZE + 5);
}

// 检查并标记可消除的方块
bool checkMatches() {
    bool hasMatches = false;

    // 横向检查
    for (int i = 1; i <= ROWS; i++) {
        for (int j = 1; j <= COLS - 2; j++) {
            if (map[i][j].type == map[i][j + 1].type &&
                map[i][j].type == map[i][j + 2].type &&
                !map[i][j].matched) {
                map[i][j].matched = true;
                map[i][j + 1].matched = true;
                map[i][j + 2].matched = true;
                hasMatches = true;

                // 检查是否超过3个相同
                int k = j + 3;
                while (k <= COLS && map[i][k].type == map[i][j].type) {
                    map[i][k].matched = true;
                    k++;
                }
            }
        }
    }

    // 纵向检查
    for (int j = 1; j <= COLS; j++) {
        for (int i = 1; i <= ROWS - 2; i++) {
            if (map[i][j].type == map[i + 1][j].type &&
                map[i][j].type == map[i + 2][j].type &&
                !map[i][j].matched) {
                map[i][j].matched = true;
                map[i + 1][j].matched = true;
                map[i + 2][j].matched = true;
                hasMatches = true;

                // 检查是否超过3个相同
                int k = i + 3;
                while (k <= ROWS && map[k][j].type == map[i][j].type) {
                    map[k][j].matched = true;
                    k++;
                }
            }
        }
    }

    return hasMatches;
}

// 处理方块下落和填充
void processFalling() {
    // 计算消除的方块并更新分数
    int eliminated = 0;
    for (int i = 1; i <= ROWS; i++) {
        for (int j = 1; j <= COLS; j++) {
            if (map[i][j].matched) eliminated++;
        }
    }
    score += eliminated * 10;

    // 播放消除音效
   // if (eliminated > 0) {
  //      PlaySound(L"res/eliminate.wav", NULL, SND_ASYNC | SND_FILENAME);
   // }

    // 方块下落
    for (int j = 1; j <= COLS; j++) {
        int emptyPos = ROWS;
        for (int i = ROWS; i >= 1; i--) {
            if (!map[i][j].matched) {
                if (emptyPos != i) {
                    swapBlocks(i, j, emptyPos, j);
                }
                emptyPos--;
            }
        }

        // 顶部生成新方块
        for(int i = emptyPos; i >= 1; i--) {
            map[i][j].type = 1 + rand() % 7;
            map[i][j].matched = false;
            map[i][j].x = OFFSET_X + (j - 1) * (BLOCK_SIZE + 5);
            map[i][j].y = OFFSET_Y + (i - 1) * (BLOCK_SIZE + 5);
        }
    }
}

// 处理鼠标点击
void handleMouse() {
    ExMessage msg;
    if (peekmessage(&msg, EX_MOUSE)) {
        if (msg.message == WM_LBUTTONDOWN) {
            // 计算点击的方块位置
            int col = (msg.x - OFFSET_X) / (BLOCK_SIZE + 5) + 1;
            int row = (msg.y - OFFSET_Y) / (BLOCK_SIZE + 5) + 1;

            // 边界检查
            if (col < 1 || col > COLS || row < 1 || row > ROWS) return;

            clickCount++;
            if (clickCount == 1) {
                // 第一次点击
                selectRow1 = row;
                selectCol1 = col;
            }
            else if (clickCount == 2) {
                // 第二次点击
                selectRow2 = row;
                selectCol2 = col;

                // 检查是否相邻
                if ((abs(selectRow1 - selectRow2) == 1 && selectCol1 == selectCol2) ||
                    (abs(selectCol1 - selectCol2) == 1 && selectRow1 == selectRow2)) {

                    // 交换方块
                    swapBlocks(selectRow1, selectCol1, selectRow2, selectCol2);

                    // 检查是否有匹配
                    if (checkMatches()) {
                        // 处理消除和下落
                        processFalling();

                        // 连续消除检查
                        while (checkMatches()) {
                            Sleep(300); // 消除动画间隔
                            processFalling();
                            drawGame();
                            Sleep(100);
                        }
                    }
                    else {
                        // 没有匹配则交换回来
                        swapBlocks(selectRow1, selectCol1, selectRow2, selectCol2);
                    }
                }

                clickCount = 0;
            }
        }
    }
}

int main() {
    initGame();
    generateBlocks();

    while (true) {
        handleMouse();
        drawGame();
        Sleep(50); // 减少CPU占用
    }

    closegraph();
    return 0;
}