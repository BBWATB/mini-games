#include <iostream>
#include <easyx.h>
#include <time.h>

constexpr auto block_w = 60;  // 砖块宽度
constexpr auto block_h = 30;  // 砖块高度
constexpr auto space = 2;     // 砖块间距
constexpr auto num_w = 10;    // 砖块横向数量
constexpr auto num_h = 10;    // 砖块纵向数量
constexpr auto board_w = 100; // 球板宽度
constexpr auto board_h = 10;  // 球板高度

int game_start = 0; // 游戏开始标志
int score = 0;
int arr[num_h][num_w];

typedef struct Ball // 定义小球参数 坐标 半径 速度 颜色
{
    int x = 0;
    int y = 0;
    int r = 10;
    int speed_x = 0;
    int speed_y = 1;
    COLORREF ball_color = RGB(0, 0, 0);
} Ball;

void Ball_move(Ball& ball);
void Board(ExMessage* msg, Ball& ball);
void Check_edge(Ball& ball);
void Check_crash(int& x, int& y, Ball& ball);
void Crash(Ball& ball);
void Dreate_blocks();
void Draw_the_blocks();
int Game_state(Ball& ball);
void Dynamic_center(const wchar_t* text, int y, int width);

void Ball_move(Ball& ball) // 小球移动函数
{
    if (game_start)
    {
        ball.y -= ball.speed_y;
        ball.x += ball.speed_x;
    }
    setfillcolor(ball.ball_color);
    fillcircle(ball.x, ball.y, ball.r);
}

void Board(ExMessage* msg, Ball& ball) // 球板函数
{
    static int site = block_w * num_w + board_w / 2;
    static int last_site = site;
    if (peekmessage(msg, EM_MOUSE))
    {
        site = msg->x - board_w / 2; // 球板位置
        if (msg->lbutton)
        {
            game_start = 1;
        }
    }
    setfillcolor(BLUE);
    fillrectangle(site, block_h * num_h + 400, site + board_w, block_h * num_h + 400 + board_h); // 绘制球板
    if (!game_start)                                                                             // 若游戏没有开始，则球的位置跟随球板位置
    {
        ball.x = site + board_w / 2;
    }
    else if (
        ball.x <= site + board_w + ball.r &&
        ball.x >= site - ball.r &&
        ball.y <= block_h * num_h + 400 + board_h &&
        ball.y >= block_h * num_h + 400 - ball.r)
    {
        ball.speed_y *= -1;
        score += abs(ball.speed_x);
        if (rand() % 3 < 2)
        {
            if (last_site - site < 0 && ball.speed_x < 4)
            {
                ++ball.speed_x;
            }
            else if (last_site - site > 0 && ball.speed_x > -4)
            {
                --ball.speed_x;
            }
        }
        ball.y = block_h * num_h + 400 - ball.r - 1; // 防止卡板子
    }
    last_site = site;
}

void Check_edge(Ball& ball) // 窗口边缘碰撞检测
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

void Check_crash(int& x, int& y, Ball& ball) // 检测碰撞位置函数
{
    bool is_ball_out_of_left_range = ball.x >= block_w * x - ball.r && ball.x <= block_w * x;
    bool is_ball_out_of_right_range = ball.x <= block_w * (x + 1) - space + ball.r && ball.x >= block_w * (x + 1) - space;
    bool is_ball_out_of_top_range = ball.y >= block_h * y - ball.r && ball.y <= block_h * y;
    bool is_ball_out_of_bottom_range = ball.y <= block_h * (y + 1) - space + ball.r && ball.y >= block_h * (y + 1) - space;

    if (is_ball_out_of_left_range) // 左
    {
        ball.x = block_w * x - ball.r - 1;
    }
    else if (is_ball_out_of_right_range) // 右
    {
        ball.x = block_w * (x + 1) - space + ball.r + 1;
    }

    if (is_ball_out_of_top_range) // 上
    {
        ball.y = block_h * y - ball.r - 1;
    }
    else if (is_ball_out_of_bottom_range) // 下
    {
        ball.y = block_h * (y + 1) - space + ball.r + 1;
    }

    if (is_ball_out_of_left_range && is_ball_out_of_top_range) // 左上
    {
        if (ball.speed_x >= 0 && ball.speed_y < 0)
        {
            ball.speed_x = -ball.speed_x - (abs(ball.speed_x) > 3 ? 0 : 1);
            ball.speed_y *= -1;
        }
        else if (ball.speed_x < 0)
        {
            ball.speed_y *= -1;
        }
        else
        {
            ball.speed_x *= -1;
        }
    }
    else if (is_ball_out_of_left_range && is_ball_out_of_bottom_range) // 左下
    {
        if (ball.speed_x >= 0 && ball.speed_y > 0)
        {
            ball.speed_x = -ball.speed_x - (abs(ball.speed_x) > 3 ? 0 : 1);
            ball.speed_y *= -1;
        }
        else if (ball.speed_x < 0)
        {
            ball.speed_y *= -1;
        }
        else
        {
            ball.speed_x *= -1;
        }
    }
    else if (is_ball_out_of_right_range && is_ball_out_of_top_range) // 右上
    {
        if (ball.speed_x <= 0 && ball.speed_y < 0)
        {
            ball.speed_x = -ball.speed_x + (abs(ball.speed_x) > 3 ? 0 : 1);
            ball.speed_y *= -1;
        }
        else if (ball.speed_x > 0)
        {
            ball.speed_y *= -1;
        }
        else
        {
            ball.speed_x *= -1;
        }
    }
    else if (is_ball_out_of_right_range && is_ball_out_of_bottom_range) // 右下
    {
        if (ball.speed_x <= 0 && ball.speed_y > 0)
        {
            ball.speed_x = -ball.speed_x + (abs(ball.speed_x) > 3 ? 0 : 1);
            ball.speed_y *= -1;
        }
        else if (ball.speed_x > 0)
        {
            ball.speed_y *= -1;
        }
        else
        {
            ball.speed_x *= -1;
        }
    }
    else if (is_ball_out_of_left_range || is_ball_out_of_right_range) // 左右
    {
        ball.speed_x *= -1;
    }
    else if (is_ball_out_of_top_range || is_ball_out_of_bottom_range) // 上下
    {
        ball.speed_y *= -1;
    }

    --arr[y][x];
    ++score;
}

void Crash(Ball& ball) // 碰撞函数
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

void Dreate_blocks() // 创造砖块
{
    int x = 0, y = 0;
    int i, j;
    int empty = 20;
    int blood_three = 15;
    int blood_two = 30;
    for (x = 0; x < num_w; x++)
    {
        for (y = 0; y < num_h; y++)
        {
            arr[y][x] = 1;
        }
    }

    while (blood_two)
    {
        i = rand() % num_w;
        j = rand() % num_h;
        if (arr[j][i] == 1)
        {
            arr[j][i] = 2;
            --blood_two;
        }
    }
    while (blood_three)
    {
        i = rand() % num_w;
        j = rand() % num_h;
        if (arr[j][i] == 1)
        {
            arr[j][i] = 3;
            --blood_three;
        }
    }
    while (empty)
    {
        i = rand() % num_w;
        j = rand() % num_h;
        if (arr[j][i] == 1)
        {
            arr[j][i] = 0;
            --empty;
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
                solidrectangle(block_w * x, block_h * y, block_w * (x + 1) - space, block_h * (y + 1) - space);
            }
        }
    }
}

int Game_state(Ball& ball) // 判断小球是否掉落
{
    int x, y;
    if (ball.y >= block_h * num_h + 500)
    {
        Dynamic_center(L"you lost", num_h * (block_h + 4), num_w * block_w);
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
    Dynamic_center(L"you win", num_h * (block_h + 4), num_w * block_w);
    return 2;
}

void Dynamic_center(const wchar_t* text, int y, int width)
{
    outtextxy(width / 2 - textwidth(text) / 2, y, text);
}

int main()
{
    int play = 1;
    wchar_t buf[20] = L"score: ";
    ExMessage msg;
    Ball ball;
    srand((unsigned int)time(NULL));
    initgraph(block_w * num_w, block_h * num_h + 500, EW_SHOWCONSOLE); // 初始化窗口
    while (true)
    {
        if (play == 1)
        {
            score = 0;
            Dreate_blocks();
            ball = { 0, block_h * num_h + 350, 10, 0, 1, RGB(rand() % 256, rand() % 256, rand() % 256) };
            setbkcolor(RGB(150, 150, 200)); // 背景
        }

        while (play == 1)
        {
            BeginBatchDraw();
            cleardevice();

            Draw_the_blocks();
            Board(&msg, ball); // 绘制板子
            Crash(ball);       // 碰撞检测
            _itow_s(score, buf + 7, 12, 10);
            Dynamic_center(buf, num_h * (block_h + 2), num_w * block_w);

            Ball_move(ball); // 移动小球
            EndBatchDraw();

            Sleep(1);

            play = Game_state(ball);
        }

        Dynamic_center(L"Press the Right button to restart the game", num_h * (block_h + 6), num_w * block_w);

        GetAsyncKeyState(VK_RBUTTON); // 忽略游戏时的点击
        while (GetAsyncKeyState(VK_RBUTTON))
        {
            play = 1;
            game_start = 0;
            Sleep(10);
        }
    }
    return 0;
}