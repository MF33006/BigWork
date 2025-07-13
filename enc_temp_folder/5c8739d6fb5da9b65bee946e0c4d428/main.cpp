#include <graphics.h>
#include <conio.h>
#include <time.h>
#include <iostream>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

// ��Ϸ����
#define ROWS 8
#define COLS 8
#define BLOCK_SIZE 52
#define OFFSET_X 17
#define OFFSET_Y 274
#define WIN_WIDTH 485
#define WIN_HEIGHT 917

// ����ṹ��
struct Block {
    int type;    // 1-7����7��ͼ��
    int x, y;    // ��Ļ����
    bool matched; // �Ƿ񱻱������
};

// ȫ�ֱ���
Block map[ROWS + 2][COLS + 2]; // ��������������ڱ߽���
IMAGE imgBlocks[7];            // �洢7�ַ���ͼƬ
IMAGE imgBg;                   // ����ͼƬ
int clickCount = 0;            // ���������
int selectRow1, selectCol1;    // ��һ��ѡ��ķ���
int selectRow2, selectCol2;    // �ڶ���ѡ��ķ���
int score = 0;                 // ��Ϸ����

// ��ʼ����Ϸ
void initGame() {
    initgraph(WIN_WIDTH, WIN_HEIGHT);
    loadimage(&imgBg, L"res/bg.png");

    // ���ط���ͼƬ
    for (int i = 0; i < 7; i++) {
        wchar_t path[30];
        swprintf_s(path, L"res/block%d.png", i + 1);
        loadimage(&imgBlocks[i], path, BLOCK_SIZE, BLOCK_SIZE);
    }

    srand(static_cast<unsigned int>(time(NULL)));
}

// �����������
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

// ������Ϸ����
void drawGame() {
    putimage(0, 0, &imgBg);

    // ��ʾ����
    settextcolor(WHITE);
    settextstyle(36, 0, _T("Arial"));
    wchar_t scoreStr[20];
    swprintf_s(scoreStr, L"����: %d", score);
    outtextxy(200, 50, scoreStr);

    // �������з���
    for (int i = 1; i <= ROWS; i++) {
        for (int j = 1; j <= COLS; j++) {
            if (!map[i][j].matched) {
                putimage(map[i][j].x, map[i][j].y, &imgBlocks[map[i][j].type - 1]);
            }
        }
    }
}

// ��������
void swapBlocks(int row1, int col1, int row2, int col2) {
    Block temp = map[row1][col1];
    map[row1][col1] = map[row2][col2];
    map[row2][col2] = temp;

    // ��������
    map[row1][col1].x = OFFSET_X + (col1 - 1) * (BLOCK_SIZE + 5);
    map[row1][col1].y = OFFSET_Y + (row1 - 1) * (BLOCK_SIZE + 5);
    map[row2][col2].x = OFFSET_X + (col2 - 1) * (BLOCK_SIZE + 5);
    map[row2][col2].y = OFFSET_Y + (row2 - 1) * (BLOCK_SIZE + 5);
}

// ��鲢��ǿ������ķ���
bool checkMatches() {
    bool hasMatches = false;

    // ������
    for (int i = 1; i <= ROWS; i++) {
        for (int j = 1; j <= COLS - 2; j++) {
            if (map[i][j].type == map[i][j + 1].type &&
                map[i][j].type == map[i][j + 2].type &&
                !map[i][j].matched) {
                map[i][j].matched = true;
                map[i][j + 1].matched = true;
                map[i][j + 2].matched = true;
                hasMatches = true;

                // ����Ƿ񳬹�3����ͬ
                int k = j + 3;
                while (k <= COLS && map[i][k].type == map[i][j].type) {
                    map[i][k].matched = true;
                    k++;
                }
            }
        }
    }

    // ������
    for (int j = 1; j <= COLS; j++) {
        for (int i = 1; i <= ROWS - 2; i++) {
            if (map[i][j].type == map[i + 1][j].type &&
                map[i][j].type == map[i + 2][j].type &&
                !map[i][j].matched) {
                map[i][j].matched = true;
                map[i + 1][j].matched = true;
                map[i + 2][j].matched = true;
                hasMatches = true;

                // ����Ƿ񳬹�3����ͬ
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

// ��������������
void processFalling() {
    // ���������ķ��鲢���·���
    int eliminated = 0;
    for (int i = 1; i <= ROWS; i++) {
        for (int j = 1; j <= COLS; j++) {
            if (map[i][j].matched) eliminated++;
        }
    }
    score += eliminated * 10;

    // ����������Ч
   // if (eliminated > 0) {
  //      PlaySound(L"res/eliminate.wav", NULL, SND_ASYNC | SND_FILENAME);
   // }

    // ��������
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

        // ���������·���
        for(int i = emptyPos; i >= 1; i--) {
            map[i][j].type = 1 + rand() % 7;
            map[i][j].matched = false;
            map[i][j].x = OFFSET_X + (j - 1) * (BLOCK_SIZE + 5);
            map[i][j].y = OFFSET_Y + (i - 1) * (BLOCK_SIZE + 5);
        }
    }
}

// ���������
void handleMouse() {
    ExMessage msg;
    if (peekmessage(&msg, EX_MOUSE)) {
        if (msg.message == WM_LBUTTONDOWN) {
            // �������ķ���λ��
            int col = (msg.x - OFFSET_X) / (BLOCK_SIZE + 5) + 1;
            int row = (msg.y - OFFSET_Y) / (BLOCK_SIZE + 5) + 1;

            // �߽���
            if (col < 1 || col > COLS || row < 1 || row > ROWS) return;

            clickCount++;
            if (clickCount == 1) {
                // ��һ�ε��
                selectRow1 = row;
                selectCol1 = col;
            }
            else if (clickCount == 2) {
                // �ڶ��ε��
                selectRow2 = row;
                selectCol2 = col;

                // ����Ƿ�����
                if ((abs(selectRow1 - selectRow2) == 1 && selectCol1 == selectCol2) ||
                    (abs(selectCol1 - selectCol2) == 1 && selectRow1 == selectRow2)) {

                    // ��������
                    swapBlocks(selectRow1, selectCol1, selectRow2, selectCol2);

                    // ����Ƿ���ƥ��
                    if (checkMatches()) {
                        // ��������������
                        processFalling();

                        // �����������
                        while (checkMatches()) {
                            Sleep(300); // �����������
                            processFalling();
                            drawGame();
                            Sleep(100);
                        }
                    }
                    else {
                        // û��ƥ���򽻻�����
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
        Sleep(50); // ����CPUռ��
    }

    closegraph();
    return 0;
}