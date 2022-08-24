#include <iostream>
#include <tuple>
#include <easyx.h>
#include <time.h>

constexpr auto block_w = 60; // 砖块宽度
constexpr auto block_h = 20; // 高度
constexpr auto space = 2;    // 间距
constexpr auto num_w = 10;   // 横向数量;
constexpr auto num_h = 10;   // 纵向数量;

int flag_1 = 0; // 发射标志
int flag_2 = 0; // 第一次弹射标志
float decide = 4; // 碰撞范围追加
int arr[num_h][num_w];


typedef struct Ball //定义小球参数 坐标 半径 速度
{
	int x;
	int y;
	int r;
	int speed_x;
	int speed_y;
	COLORREF ball_color;
} Ball;

void Ball_move(Ball& ball);
void Board(ExMessage* msg, Ball& ball);
void Check_board(int* site, Ball* ball);
void Check_edge(Ball* ball);

void Ball_move(Ball& ball) //小球移动函数
{
	if (flag_1)
	{
		ball.y -= ball.speed_y;
		ball.x += ball.speed_x;
	}
	setfillcolor(ball.ball_color);
	fillcircle(ball.x, ball.y - ball.r, ball.r);
	Sleep(1);
}

void Board(ExMessage* msg, Ball& ball) //球板函数
{
	static int site = block_w * 5 + 10;
	if (peekmessage(msg, EM_MOUSE))
	{
		site = msg->x; //球板位置
		if (msg->lbutton)
		{
			flag_1 = 1;
		}
	}
	if (!flag_1) //若没有按下左键，则球的位置跟随球板位置
	{
		ball.x = site + 50;
		ball.y = block_h * num_h + 400;
	}
	setfillcolor(BLUE);
	fillrectangle(site, block_h * num_h + 400, site + 100, block_h * num_h + 410);
	if (ball.x <= site + 100 + ball.r && ball.x >= site - ball.r &&
		ball.y <= block_h * num_h + 410 &&
		ball.y >= block_h * num_h + 400 - ball.r && flag_2)
	{
		Check_board(&site, &ball);
	}
}

void Check_board(int* site, Ball* ball) //检测板子碰撞函数
{
	if (ball->x <= *site + 100 + ball->r && ball->x >= *site + 90 && ball->y >= block_h * num_h + 400 - ball->r) //碰到板子右角
	{
		ball->speed_x *= -1;
		ball->speed_y *= -1;
	}
	else if (ball->x >= *site - ball->r && ball->x <= *site + 10 && ball->y >= block_h * num_h + 400 - ball->r) //碰到板子左角
	{
		ball->speed_x *= -1;
		ball->speed_y *= -1;
	}
	else
	{
		if (ball->y >= block_h * num_h + 400) //碰到板子顶边
		{
			ball->speed_y *= -1;
			ball->speed_x += -1 + rand() % 2;
		}
	}
}

void Check_edge(Ball* ball) //窗口边缘碰撞检测
{
	if (ball->x <= ball->r)
	{
		ball->speed_x *= -1;
		ball->x = ball->r + 1;
	}
	else if (ball->x >= block_w * num_w - ball->r)
	{
		ball->speed_x *= -1;
		ball->x = block_w * num_w - ball->r - 1;
	}
	else if (ball->y <= ball->r)
	{
		ball->speed_y *= -1;
		ball->y = ball->r + 1;
	}
}

int main()
{
	srand((unsigned int)time(NULL));
	int play = 1;
	ExMessage msg;
	COLORREF ball_color = RGB(rand() % 255, rand() % 255, rand() % 255); // 不能和背景颜色一样
	Ball ball = { 300, 450, 10, 1, 1 ,ball_color }; // 初始化小球 坐标 半径 速度 颜色
	initgraph(block_w * num_w, block_h * num_h + 500, EW_SHOWCONSOLE); // 初始化窗口
	while (play)
	{
		BeginBatchDraw();

		setbkcolor(RGB(255, 255, 255)); // 背景
		cleardevice();

		Board(&msg, ball);
		Ball_move(ball);

		Check_edge(&ball);

		EndBatchDraw();
	}
	std::ignore = getchar();
	closegraph();
	return 0;
}