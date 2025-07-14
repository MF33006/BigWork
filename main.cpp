#include<graphics.h>
#include<stdio.h>
#include<time.h>
#include<math.h>
//开发日志
//1.构建初始界面
//2.构建方块数组
//3.加载透明的方块图片..未实现
//4.实现方块的移动
#define WIN_WIDTH  485
#define WIN_HEIGHT 917
#define ROWS  8
#define COLS  8
#define OFFSET_X   17
#define OFFSET_Y   274
#define BLOCK_SIZE 52

IMAGE imgBg;//背景图片
IMAGE imgBlocks[7];//每个方块的图片

//定义方块的结构体类型
struct Blocks {
	int type=0;//0表示空白
	int x, y;
	int row, col;
};

struct Blocks map[ROWS+2][COLS+2];
int clickCount = 0; //表示相邻位置的单击次数，第2次单机才会交换
int posX1, posY1; //记录第1次单击的方块位置
int posX2, posY2; //记录第2次单击的方块位置

void init()
{
	initgraph(WIN_WIDTH, WIN_HEIGHT); //创建窗口
	loadimage(&imgBg,_T( "res\\bg.png")); //加载背景图片

	loadimage(&imgBlocks[0], _T("res\\1.png"), BLOCK_SIZE, BLOCK_SIZE, true);
	loadimage(&imgBlocks[1], _T("res\\2.png"), BLOCK_SIZE, BLOCK_SIZE, true);
	loadimage(&imgBlocks[2], _T("res\\3.png"), BLOCK_SIZE, BLOCK_SIZE, true);
	loadimage(&imgBlocks[3], _T("res\\4.png"), BLOCK_SIZE, BLOCK_SIZE, true);
	loadimage(&imgBlocks[4], _T("res\\5.png"), BLOCK_SIZE, BLOCK_SIZE, true);
	loadimage(&imgBlocks[5], _T("res\\6.png"), BLOCK_SIZE, BLOCK_SIZE, true);
	loadimage(&imgBlocks[6], _T("res\\7.png"), BLOCK_SIZE, BLOCK_SIZE, true);

	//配置随机数种子
	srand(time(NULL));
	//初始化方块数组，注意由于初始化的时候ROW+2，COL+2，所以实际的方块数组是从1开始的（1~8）
	for (int i = 1; i <= ROWS; i++) {
		for (int j = 1; j <= COLS; j++) {

			map[i][j].type = 1 + rand() % 7;//类型为1~7的随机数
			map[i][j].row = i;
			map[i][j].col = j;
			map[i][j].x = OFFSET_X + (i - 1) * (BLOCK_SIZE + 5);
			map[i][j].y = OFFSET_Y + (j - 1) * (BLOCK_SIZE + 5);

		}
	}
}

void updateWindow()
{
	BeginBatchDraw(); //开始批量绘图(双缓冲）
	putimage(0,0, &imgBg); //绘制背景图片
	
	for (int i = 1; i <= ROWS; i++) {
		for (int j = 1; j <= COLS; j++) {
			if (map[i][j].type) {
				IMAGE* img = &imgBlocks[map[i][j].type - 1];
				putimage(map[i][j].x, map[i][j].y, img);//将方块图片绘制到对应位置
			}
		}
	}
	EndBatchDraw(); //结束批量绘图
}

//方块交换函数
void exchange(int row1,int col1,int row2,int col2) 
{   
	//两个方块交换数据（类型，位置，以及决定图片类型的x,y坐标）
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
				//播放音效。。。
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
			// 正确计算目标位置：行索引 i → y，列索引 j → x
			int targetX = OFFSET_X + (j - 1) * (BLOCK_SIZE + 5);
			int targetY = OFFSET_Y + (i - 1) * (BLOCK_SIZE + 5);
			int dx = p->x - targetX;
			int dy = p->y - targetY;

			if (dx) p->x -= dx / abs(dx); //向左或向右移动
			if (dy) p->y -= dy / abs(dy); //向上或向下移动
		
		}
	}
}


void Huanyuan()
{

}

int main()
{

	init();//初始化

	updateWindow(); //更新窗口

	while (1) {
		userClick();//处理用户的点击操作，将方块的数据交换了
	    move(); //处理方块的移动
		//Huanyuan();//还原方块
		updateWindow(); //更新画面

		Sleep(10);//帧等待，后续可优化.循环中不停的绘制，所以导致画面闪烁，需要加帧缓冲
	}
	system("pause"); //暂停，等待用户输入
	return 0;

}