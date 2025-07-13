#include<graphics.h>
#include<time.h>
#pragma comment(lib,"winm.lib")


#define WIDTH    485
#define HEIGHT  917
#define ROWS 8
#define COLS 8
#define BLOCK_SIZE 52
#define OFFSET_X 17
#define OFFSET_Y 274


struct Block {
	int type;
	int row, col;
	int x, y;
	bool match;
};

Block map[ROWS + 2][COLS + 2];
IMAGE imgBlocks[7];
IMAGE  imgBg;
IMAGE imgBuffer;
int score = 0;

//��ʼ������Դ����
void initGame()
{
	initgraph(WIDTH, HEIGHT);
	loadimage(&imgBg, _T("res\\bg.png"));
	for (int i = 0; i < 7; i++) {
		loadimage(&imgBlocks[i],_T("res\\block%d.png",i+1),BLOCK_SIZE,BLOCK_SIZE);
	} 
	srand(time(NULL));//���������
}

//�����������
void generateBlocks()
{
	for (int i = 1; i <= ROWS; i++) {
		for (int j = 1; j < +COLS; j++) {
			map[i][j].type = 1 + rand() % 7;
			map[i][j].x = OFFSET_X + (j - 1) * (BLOCK_SIZE + 5);
			map[i][j].y = OFFSET_Y + (i - 1) * (BLOCK_SIZE + 5);
			map[i][j].match = false;
		}
	}
}

//���齻���߼���ʵ��
void swapBlocks(int row1, int col1, int row2, int col2)
{
	Block temp = map[row1][col1];
	map[row1][col1] = map[row2][col2];
	map[row2][col2] = temp;

	map[row1][col1].x = OFFSET_X + (col1 - 1) * (BLOCK_SIZE + 5);
	map[row1][col1].y = OFFSET_Y + (row1 - 1) * (BLOCK_SIZE + 5);
	map[row2][col2].x = OFFSET_X + (col2 - 1) * (BLOCK_SIZE + 5);
	map[row2][col2].y = OFFSET_Y + (row2 - 1) * (BLOCK_SIZE + 5);

}

//��������㷨
bool checkMatch()
{
	bool hasMatch = false;

	//��⣨>=����������
	for (int i = 1; i <= ROWS; i++) {
		for (int j = 1; j <= COLS - 2; j++) {
			if (map[i][j].type == map[i][j + 1].type &&
				map[i][j + 1].type == map[i][j + 2].type) {
				map[i][j].match = true;
				map[i][j+1].match = true;
				map[i][j+2].match = true;
				hasMatch = true;

				//����Ƿ񳬹�������ͬ
				int k = j + 3;
				while (k <= COLS && map[i][j].type == map[i][k].type) {
					map[i][k].match = true;
					k++;
				}
			}
		}
	}
	for (int j = 1; j <= COLS; j++) {
		for (int i = 1; i <= ROWS - 2; i++) {
			if (map[i][j].type == map[i + 1][j].type &&
				map[i + 1][j].type == map[i + 2][j].type) {
				map[i][j].match = true;
				map[i+1][j].match = true;
				map[i+2][j].match = true;
				hasMatch = true;

				//����Ƿ񳬹�������ͬ
				int k = i + 3;
				while (k <= ROWS && map[i][j].type == map[k][j].type) {
					map[k][j].match = true;
					k++;
				}
			}
		}
		return hasMatch;
	}
}

//�������������
void processFalling()
{   
	//���������ķ��鲢���·���
	int eliminate = 0;
	for (int i = 1; i <= ROWS; i++) {
		for (int j = 1; j <= COLS; j++) {
			if (map[i][j].match)
				eliminate++;
		}
	}
	score += eliminate;
    //����������Ч
	if (eliminate > 0) {
		//PlaySound();
	}


	for (int j = 1; j <= COLS; j++) {
		int emptyCount = 0;
		for (int i = ROWS; i >= 1; i--) {
			if (map[i][j].match) {
				emptyCount++;
			}
			else if (emptyCount > 0) {
				swapBlocks(i, j, i + emptyCount, j);
			}
		}
	}
}