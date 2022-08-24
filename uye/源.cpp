﻿#include <iostream>
#include <easyx.h>
#include <time.h>
#include <string>

constexpr auto block_w = 60; // 砖块宽度
constexpr auto block_h = 30; // 高度
constexpr auto space = 2;    // 间距
constexpr auto num_w = 10;   // 横向数量;
constexpr auto num_h = 10;   // 纵向数量;

int flag_1 = 0; // 发射标志
int decide = 4; // 碰撞范围
int score = 0;
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
void Check_edge(Ball& ball);
void Check_crash(int& x, int& y, Ball& ball);
void Crash(Ball& ball);
void Dreate_blocks();
void Draw_the_blocks();
void Dynamic_center(const wchar_t* text, int y, int width);

void Ball_move(Ball& ball) //小球移动函数
{
    if (flag_1)
    {
        ball.y -= ball.speed_y;
        ball.x += ball.speed_x;
    }
    setfillcolor(ball.ball_color);
    fillcircle(ball.x, ball.y, ball.r);
}

void Board(ExMessage* msg, Ball& ball) //球板函数
{
    static int site = block_w * num_w + 50;
    if (peekmessage(msg, EM_MOUSE))
    {
        site = msg->x - 50; //球板位置
        if (msg->lbutton)
        {
            flag_1 = 1;
        }
    }
    if (!flag_1) //若没有按下左键，则球的位置跟随球板位置
    {
        ball.x = site + 50;
        ball.y = block_h * num_h + 380;
    }
    setfillcolor(BLUE);
    fillrectangle(site, block_h * num_h + 400, site + 100, block_h * num_h + 410);
    if (
        ball.x <= site + 100 + ball.r &&
        ball.x >= site - ball.r &&
        ball.y <= block_h * num_h + 410 &&
        ball.y >= block_h * num_h + 400 - ball.r && flag_1
        )
    {
        ball.speed_y *= -1;
        if (rand() % 2)
        {
            if (abs(ball.speed_x) < 2)
            {
                ball.speed_x += -1 + rand() % 3;
            }
            else if (ball.speed_x >= 2)
            {
                ball.speed_x -= rand() % 2;
            }
            else if (ball.speed_x <= -2)
            {
                ball.speed_x += rand() % 2;
            }
        }
        ball.x += -5 + rand() % 10;
        ball.y -= ball.r;
    }
}

void Check_edge(Ball& ball) //窗口边缘碰撞检测
{
    if (ball.x <= ball.r)
    {
        ball.speed_x *= -1;
        ball.x = ball.r + 1;
    }
    else if (ball.x >= block_w * num_w - ball.r)
    {
        ball.speed_x *= -1;
        ball.x = block_w * num_w - ball.r - 1;
    }
    else if (ball.y <= ball.r)
    {
        ball.speed_y *= -1;
        ball.y = ball.r + 1;
    }
}

void Check_crash(int& x, int& y, Ball& ball) //检测碰撞位置函数
{
    int str = 0;
    if (
        ball.x <= block_w * (x + 1) - space + ball.r &&
        ball.x >= block_w * (x + 1) - space + ball.r - decide &&
        ball.y <= block_h * (y + 1) - space &&
        ball.y >= block_h * y
        ) // 右侧
    {
        str = 7;
    }
    else if (
        ball.x >= block_w * x - ball.r &&
        ball.x <= block_w * x - ball.r + decide &&
        ball.y <= block_h * (y + 1) - space &&
        ball.y >= block_h * y
        ) // 左侧
    {
        str = 2;
    }
    else if (
        ball.y <= block_h * (y + 1) - space + ball.r &&
        ball.y >= block_h * (y + 1) - space + ball.r - decide &&
        ball.x <= block_w * (x + 1) - space &&
        ball.x >= block_w * x
        ) // 下侧
    {
        str = 5;
    }
    else if (
        ball.y >= block_h * y - ball.r &&
        ball.y <= block_h * y - ball.r + decide &&
        ball.x <= block_w * (x + 1) - space &&
        ball.x >= block_w * x
        ) // 上侧
    {
        str = 4;
    }
    else if (
        ball.x >= block_w * x - ball.r &&
        ball.x <= block_w * x &&
        ball.y >= block_h * y - ball.r &&
        ball.y <= block_h * y
        ) //左上
    {
        if (arr[y][x - 1])
            str = 4;
        else
            str = 1;
    }
    else if (
        ball.x >= block_w * x - ball.r &&
        ball.x <= block_w * x &&
        ball.y <= block_h * (y + 1) - space + ball.r &&
        ball.y >= block_h * (y + 1) - space
        ) //左下
    {
        if (arr[y][x - 1])
            str = 5;
        else
            str = 3;
    }
    else if (
        ball.x <= block_w * (x + 1) - space + ball.r &&
        ball.x >= block_w * (x + 1) - space &&
        ball.y >= block_h * y - ball.r &&
        ball.y <= block_h * y
        ) //右上
    {
        if (arr[y][x + 1])
            str = 4;
        else
            str = 6;
    }
    else if (
        ball.x <= block_w * (x + 1) - space + ball.r &&
        ball.x >= block_w * (x + 1) - space &&
        ball.y <= block_h * (y + 1) - space + ball.r &&
        ball.y >= block_h * (y + 1) - space
        ) //右下
    {
        if (arr[y][x + 1])
            str = 5;
        else
            str = 8;
    }

    switch (str) {
    case 1: // 左上
        if (ball.speed_x >= 0 && ball.speed_y < 0)
        {
            ball.speed_x *= -1;
            ball.speed_y *= -1;
        }
        else if (ball.speed_x >= 0 && ball.speed_y > 0)
        {
            ball.speed_x *= -1;
        }
        else if (ball.speed_x <= 0 && ball.speed_y < 0)
        {
            ball.speed_y *= -1;
        }
        break;
    case 2: // 左
        ball.speed_x *= -1;
        break;
    case 3: // 左下
        if (ball.speed_x >= 0 && ball.speed_y > 0)
        {
            ball.speed_x *= -1;
            ball.speed_y *= -1;
        }
        else if (ball.speed_x >= 0 && ball.speed_y < 0)
        {
            ball.speed_x *= -1;
        }
        else if (ball.speed_x <= 0 && ball.speed_y > 0)
        {
            ball.speed_y *= -1;
        }
        break;
    case 4: // 上
        ball.speed_y *= -1;
        break;
    case 5: // 下
        ball.speed_y *= -1;
        break;
    case 6: // 右上
        if (ball.speed_x <= 0 && ball.speed_y < 0)
        {
            ball.speed_x *= -1;
            ball.speed_y *= -1;
        }
        else if (ball.speed_x <= 0 && ball.speed_y > 0)
        {
            ball.speed_x *= -1;
        }
        else if (ball.speed_x >= 0 && ball.speed_y < 0)
        {
            ball.speed_y *= -1;
        }
        break;
    case 7: // 右
        ball.speed_x *= -1;
        break;
    case 8: // 右下
        if (ball.speed_x <= 0 && ball.speed_y > 0)
        {
            ball.speed_x *= -1;
            ball.speed_y *= -1;
        }
        else if (ball.speed_x <= 0 && ball.speed_y < 0)
        {
            ball.speed_x *= -1;
        }
        else if (ball.speed_x >= 0 && ball.speed_y > 0)
        {
            ball.speed_y *= -1;
        }
        break;
    }
    --arr[y][x];
    ++score;
}

void Crash(Ball& ball) //碰撞函数
{
    int x = 0, y = 0, i = 0;
    for (y = 0; y < num_h; y++)
    {
        for (x = 0; x < num_w; x++)
        {
            // 检测砖块碰撞
            if (arr[y][x] &&
                ball.x <= block_w * (x + 1) - space + ball.r &&
                ball.x >= block_w * x - ball.r &&
                ball.y <= block_h * (y + 1) - space + ball.r &&
                ball.y >= block_h * y - ball.r)
            {
                Check_crash(x, y, ball);
                return;
            }
            // 检测窗口碰撞
            else
            {
                Check_edge(ball);
            }

        }
    }
}

void Dreate_blocks()
{
    int x = 0, y = 0;
    for (x = 0; x < num_w; x++)
    {
        for (y = 0; y < num_h; y++)
        {
            arr[y][x] = 1;
        }
    }
    int empty = 20;
    int blood_three = 15;
    int blood_two = 30;
    while (blood_two)
    {
        int i = rand() % num_w;
        int j = rand() % num_h;
        if (arr[j][i] == 1)
        {
            arr[j][i] = 2;
            blood_two--;
        }
    }
    while (blood_three)
    {
        int i = rand() % num_w;
        int j = rand() % num_h;
        if (arr[j][i] == 1)
        {
            arr[j][i] = 3;
            blood_three--;
        }
    }
    while (empty)
    {
        int i = rand() % num_w;
        int j = rand() % num_h;
        if (arr[j][i] == 1)
        {
            arr[j][i] = 0;
            empty--;
        }
    }
}

void Draw_the_blocks() // 绘制砖块
{
    int x = 0, y = 0;
    for (y = 0; y < num_h; y++)
    {
        for (x = 0; x < num_w; x++)
        {
            if (arr[y][x])
            {
                if (arr[y][x] == 1)
                {
                    setfillcolor(RGB(200, 200, 200));
                }
                else if (arr[y][x] == 2)
                {
                    setfillcolor(RGB(100, 100, 100));
                }
                else if (arr[y][x] == 3)
                {
                    setfillcolor(RGB(50, 50, 50));
                }
                else
                {
                    setfillcolor(RGB(255, 0, 0));
                }
                solidrectangle(block_w * x, block_h * y, block_w * (x + 1) - space, block_h * (y + 1) - space);
            }
        }
    }
}

int Game_state(Ball& ball) //判断小球是否掉落
{
    int x = 0, y = 0;
    if (ball.y >= block_h * num_h + 500)
    {
        return 0;
    }
    for (x = 0; x < num_h; x++)
    {
        for (y = 0; y < num_w; y++)
        {
            if (arr[x][y])
            {
                return 1;
            }
        }
    }
    return 2;
}

void Dynamic_center(const wchar_t* text, int y, int width)
{
    outtextxy(width / 2 - textwidth(text) / 2, y, text);
}

int main()
{
    srand((unsigned int)time(NULL));
    int play = 1;
    ExMessage msg;
    Ball ball = { 300, 450, 10, -1 + rand() % 3, 1 ,RGB(0, 0, 0) }; // 初始化小球 坐标 半径 速度 颜色
    initgraph(block_w * num_w, block_h * num_h + 500, EW_SHOWCONSOLE); // 初始化窗口
    while (true)
    {
        if (play == 1)
        {
            score = 0;
            Dreate_blocks();
            COLORREF ball_color = RGB(rand() % 256, rand() % 256, rand() % 256);
            ball.ball_color = ball_color;
        }
        while (play == 1)
        {
            play = Game_state(ball);

            BeginBatchDraw();
            setbkcolor(RGB(150, 150, 200)); // 背景
            cleardevice();

            Draw_the_blocks();
            Board(&msg, ball); // 绘制板子
            Crash(ball);       // 碰撞检测

            wchar_t buf[11] = L"score: ";
            _itow_s(score, buf + 7, 10, 10);
            outtextxy(num_w * block_w / 2 - 35, 300, buf);

            Ball_move(ball);   // 移动小球
            EndBatchDraw();

            Sleep(1);
        }
        if (play == 2) {
            Dynamic_center(L"you win", 350, num_w * block_w);
        }
        else if (play == 0)
        {
            Dynamic_center(L"you lost", 350, num_w * block_w);
        }
        Dynamic_center(L"Press the Right button to restart the game", 400, num_w * block_w);
        if (GetAsyncKeyState(VK_RBUTTON))
        {
            play = 1;
            flag_1 = 0;
        }
    }
    return 0;
}