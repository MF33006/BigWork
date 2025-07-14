#include<graphics.h>
#include<stdio.h>
#include<time.h>
#include<math.h>
//������־
//1.������ʼ����
//2.������������
//3.����͸���ķ���ͼƬ..δʵ��
//4.ʵ�ַ�����ƶ�
#define WIN_WIDTH  485
#define WIN_HEIGHT 917
#define ROWS  8
#define COLS  8
#define OFFSET_X   17
#define OFFSET_Y   274
#define BLOCK_SIZE 52

IMAGE imgBg;//����ͼƬ
IMAGE imgBlocks[7];//ÿ�������ͼƬ

//���巽��Ľṹ������
struct Blocks {
	int type=0;//0��ʾ�հ�
	int x, y;
	int row, col;
};

struct Blocks map[ROWS+2][COLS+2];
int clickCount = 0; //��ʾ����λ�õĵ�����������2�ε����Żύ��
int posX1, posY1; //��¼��1�ε����ķ���λ��
int posX2, posY2; //��¼��2�ε����ķ���λ��

void init()
{
	initgraph(WIN_WIDTH, WIN_HEIGHT); //��������
	loadimage(&imgBg,_T( "res\\bg.png")); //���ر���ͼƬ

	loadimage(&imgBlocks[0], _T("res\\1.png"), BLOCK_SIZE, BLOCK_SIZE, true);
	loadimage(&imgBlocks[1], _T("res\\2.png"), BLOCK_SIZE, BLOCK_SIZE, true);
	loadimage(&imgBlocks[2], _T("res\\3.png"), BLOCK_SIZE, BLOCK_SIZE, true);
	loadimage(&imgBlocks[3], _T("res\\4.png"), BLOCK_SIZE, BLOCK_SIZE, true);
	loadimage(&imgBlocks[4], _T("res\\5.png"), BLOCK_SIZE, BLOCK_SIZE, true);
	loadimage(&imgBlocks[5], _T("res\\6.png"), BLOCK_SIZE, BLOCK_SIZE, true);
	loadimage(&imgBlocks[6], _T("res\\7.png"), BLOCK_SIZE, BLOCK_SIZE, true);

	//�������������
	srand(time(NULL));
	//��ʼ���������飬ע�����ڳ�ʼ����ʱ��ROW+2��COL+2������ʵ�ʵķ��������Ǵ�1��ʼ�ģ�1~8��
	for (int i = 1; i <= ROWS; i++) {
		for (int j = 1; j <= COLS; j++) {

			map[i][j].type = 1 + rand() % 7;//����Ϊ1~7�������
			map[i][j].row = i;
			map[i][j].col = j;
			map[i][j].x = OFFSET_X + (i - 1) * (BLOCK_SIZE + 5);
			map[i][j].y = OFFSET_Y + (j - 1) * (BLOCK_SIZE + 5);

		}
	}
}

void updateWindow()
{
	BeginBatchDraw(); //��ʼ������ͼ(˫���壩
	putimage(0,0, &imgBg); //���Ʊ���ͼƬ
	
	for (int i = 1; i <= ROWS; i++) {
		for (int j = 1; j <= COLS; j++) {
			if (map[i][j].type) {
				IMAGE* img = &imgBlocks[map[i][j].type - 1];
				putimage(map[i][j].x, map[i][j].y, img);//������ͼƬ���Ƶ���Ӧλ��
			}
		}
	}
	EndBatchDraw(); //����������ͼ
}

//���齻������
void exchange(int row1,int col1,int row2,int col2) 
{   
	//�������齻�����ݣ����ͣ�λ�ã��Լ�����ͼƬ���͵�x,y���꣩
	struct Blocks temp= map[row1][col1];
	map[row1][col1] = map[row2][col2];
	map[row2][col2]= temp;

	map[row1][col1].row = row1;
	map[row1][col1].col = col1;
	map[row2][col2].row = row2;
	map[row2][col2].col = col2;
	

	

}

void userClick()
{
	ExMessage msg;
	if (peekmessage(&msg) && msg.message == WM_LBUTTONDOWN) {
		/*
			map[i][j].x = OFFSET_X + (i - 1) * (BLOCK_SIZE + 5);
			map[i][j].y = OFFSET_Y + (j - 1) * (BLOCK_SIZE + 5);
			*/
		if (msg.x < OFFSET_X||msg.y<OFFSET_Y) return;
		int col = (msg.x - OFFSET_X) / (BLOCK_SIZE + 5) + 1;
		int row = (msg.y - OFFSET_Y) / (BLOCK_SIZE + 5) + 1;
		if (col > 8||row>8) return;
		clickCount++;
		if (clickCount == 1) {
			posX1 = col;
			posY1 = row;
		}
		else if (clickCount == 2) {
			posX2= col;
			posY2 = row;
			if (abs(posX2 - posX1) + abs(posY2 - posY1) ==1) {
				exchange(posY1, posX1, posY2, posX2);
				clickCount= 0;
				//������Ч������
			}
			else {
				clickCount = 1;
				posX1 = col;
				posY1 = row;
			}
		}
	}
}

void move() {
	for (int i = 1; i <= ROWS; i++) {
		for (int j = 1; j <= COLS; j++) {
			Blocks* p = &map[i][j];

			p->x;
			p->y;
			// ��ȷ����Ŀ��λ�ã������� i �� y�������� j �� x
			int targetX = OFFSET_X + (j - 1) * (BLOCK_SIZE + 5);
			int targetY = OFFSET_Y + (i - 1) * (BLOCK_SIZE + 5);
			int dx = p->x - targetX;
			int dy = p->y - targetY;

			if (dx) p->x -= dx / abs(dx); //����������ƶ�
			if (dy) p->y -= dy / abs(dy); //���ϻ������ƶ�
		
		}
	}
}


void Huanyuan()
{

}

int main()
{

	init();//��ʼ��

	updateWindow(); //���´���

	while (1) {
		userClick();//�����û��ĵ������������������ݽ�����
	    move(); //��������ƶ�
		//Huanyuan();//��ԭ����
		updateWindow(); //���»���

		Sleep(10);//֡�ȴ����������Ż�.ѭ���в�ͣ�Ļ��ƣ����Ե��»�����˸����Ҫ��֡����
	}
	system("pause"); //��ͣ���ȴ��û�����
	return 0;

}