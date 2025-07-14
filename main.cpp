#include<graphics.h>
#include<stdio.h>
#include<time.h>
#include<math.h>
#include"tools.h"
//开发日志
//1.构建初始界面
//2.构建方块数组
//3.加载透明的方块图片
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
int clickCount; //表示相邻位置的单击次数，第2次单机才会交换
int posX1, posY1; //记录第1次单击的方块位置
int posX2, posY2; //记录第2次单击的方块位置
bool isMoving;//记录当前是否在移动
bool isSwap;//记录当前是否在交换方块

//定义方块的结构体类型
struct Blocks {
	int type=0;//0表示空白
	int x, y;
	int row, col;
	int match;//匹配次数
};

struct Blocks map[ROWS+2][COLS+2];


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

	clickCount = 0; //初始化点击次数为0
	isMoving = false; //初始化移动标志为false
	isSwap = false; //初始化交换标志为false

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
			map[i][j].match = 0;

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
				putimagePNG(map[i][j].x, map[i][j].y, img);//将方块图片绘制到对应位置
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
				isSwap = true; //设置交换标志
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

void check()
{
	for (int i = 1; i <= ROWS; i++) {
		for (int j = 1; j <= COLS; j++) {
			if (map[i][j].type == map[i + 1][j].type &&
				map[i][j].type == map[i - 1][j].type) {
				map[i][j].match = 1;
				map[i+1][j].match = 1;
				map[i-1][j].match = 1;
			}
			if (map[i][j].type == map[i][j + 1].type &&
				map[i][j].type == map[i][j - 1].type)
			{
				map[i][j].match = 1;
				map[i][j+1].match = 1;
				map[i][j-1].match = 1;
			}
		}
	}
}

void move() {
	for (int i = 1; i <= ROWS; i++) {
		for (int j = 1; j <= COLS; j++) {
			Blocks* p = &map[i][j];
			int dx ;
			int dy ;

			//优化，加快交换速度
			for (int k = 0; k < 4; k++) {
				int targetX = OFFSET_X + (p->col - 1) * (BLOCK_SIZE + 5);
				int targetY = OFFSET_Y + (p->row - 1) * (BLOCK_SIZE + 5);
				dx = p->x - targetX;
				dy = p->y - targetY;

				if (dx) p->x -= dx / abs(dx); //向左或向右移动
				if (dy) p->y -= dy / abs(dy); //向上或向下移动
			}

			if (dx || dy) isMoving = true; //如果有移动，设置移动标志
		}
	}
}


void Huanyuan()
{ 
	//发生移动之后才决定是否还原方块
	int count = 0;
	for (int i = 1; i <= ROWS; i++) {
		for (int j = 1; j <= COLS; j++) {
			count += map[i][j].match;
		}
	}

	if (isSwap &&  !isMoving) {
		//如果没有匹配三个以上的方块，则还原
		if (1) {
			exchange(posY1, posX1, posY2, posX2);
		}
		isSwap = false; //重置交换标志
	}
		
}

int main()
{

	init();//初始化

	updateWindow(); //更新窗口

	while (1) {
		userClick();//处理用户的点击操作，将方块的数据交换了
		check();//检查匹配次数
	    move(); //处理方块的移动
		Huanyuan();//还原方块
		updateWindow(); //更新画面

		Sleep(10);//帧等待，后续可优化.循环中不停的绘制，所以导致画面闪烁，需要加帧缓冲
	}
	system("pause"); //暂停，等待用户输入
	return 0;

}