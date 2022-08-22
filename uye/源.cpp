#include <stdio.h>
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
} Ball;

int main()
{
	srand((unsigned int)time(NULL));
	int play = 1;
	COLORREF ball_color = RGB(rand() % 255, rand() % 255, rand() % 255); // 不能和背景颜色一样
	Ball ball[] = { 0, 450, 10, -1 + rand() % 2, 2 }; // 初始化小球 坐标 半径 速度
	initgraph(block_w * num_w, block_h * num_h + 500, EW_SHOWCONSOLE); // 初始化窗口
	while (play)
	{
		BeginBatchDraw();

		setbkcolor(RGB(255, 255, 255)); // 背景
		cleardevice();

		setfillcolor(ball_color);
		fillcircle(ball[0].x, ball[0].y - ball[0].r, ball[0].r);

		EndBatchDraw();
	}
	getchar();
	closegraph();
	return 0;
}