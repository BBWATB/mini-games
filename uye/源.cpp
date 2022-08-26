#include <iostream>
#include <easyx.h>
#include <time.h>

constexpr auto block_w = 60;               // 砖块宽度
constexpr auto block_h = 30;               // 砖块高度
constexpr auto space = 2;                  // 砖块间距
constexpr auto num_w = 10;                 // 砖块横向数量
constexpr auto num_h = 10;                 // 砖块纵向数量
constexpr auto screen_w = block_w * num_w; // 屏幕宽度
constexpr auto screen_h = block_h * num_h; // 砖块占屏幕高度
constexpr auto board_w = screen_w / 3;     // 球板宽度
constexpr auto board_h = 10;               // 球板高度

int game_start = 0;    // 游戏开始标志
int score = 0;         // 得分
int arr[num_h][num_w]; // 砖块数组

typedef struct Ball
{
    int x = 0;                          /// 小球横坐标
    int y = 0;                          /// 小球纵坐标
    int r = 10;                         /// 小球半径
    int speed_x = 0;                    /// 小球横向速度
    int speed_y = 1;                    /// 小球纵向速度
    COLORREF ball_color = RGB(0, 0, 0); /// 小球颜色
};

void Ball_move(Ball& ball);
void Board(ExMessage* msg, Ball& ball);
void Check_edge(Ball& ball);
void Check_crash(int& x, int& y, Ball& ball);
void Crash(Ball& ball);
void Dreate_blocks();
void Draw_the_blocks();
int Game_state(Ball& ball);
void Dynamic_center(const wchar_t* text, int y, int width);
void Show_score();

/// <summary>
/// 小球移动函数
/// </summary>
/// <param name="ball">目标小球</param>
void Ball_move(Ball& ball)
{
    if (game_start)
    {
        ball.y -= ball.speed_y;
        ball.x += ball.speed_x;
    }
    setfillcolor(ball.ball_color);
    fillcircle(ball.x, ball.y, ball.r);
}

/// <summary>
/// 球板函数
/// </summary>
/// <param name="msg">鼠标状态指针</param>
/// <param name="ball">目标小球</param>
void Board(ExMessage* msg, Ball& ball)
{
    static int site = screen_w + board_w / 2;
    static int last_site = site;
    if (peekmessage(msg, EM_MOUSE))
    {
        // 球板位置
        site = msg->x - board_w / 2;
        if (msg->lbutton)
        {
            game_start = 1;
        }
    }
    setfillcolor(BLUE);
    fillrectangle(site, screen_h + 400, site + board_w, screen_h + 400 + board_h);
    // 若游戏没有开始，则球的位置跟随球板位置
    if (!game_start)
    {
        ball.x = site + board_w / 2;
    }
    else if (
        ball.x <= site + board_w + ball.r &&
        ball.x >= site - ball.r &&
        ball.y <= screen_h + 400 + board_h &&
        ball.y >= screen_h + 400 - ball.r)
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
        ball.y = screen_h + 400 - ball.r - 1; // 防止卡板子
    }
    last_site = site;
}

/// <summary>
/// 窗口边缘碰撞检测
/// </summary>
/// <param name="ball">目标小球</param>
void Check_edge(Ball& ball)
{
    if (ball.x <= ball.r)
    {
        ball.speed_x *= -1;
        ball.x = ball.r + 1;
    }
    else if (ball.x >= screen_w - ball.r)
    {
        ball.speed_x *= -1;
        ball.x = screen_w - ball.r - 1;
    }
    else if (ball.y <= ball.r)
    {
        ball.speed_y *= -1;
        ball.y = ball.r + 1;
    }
}

/// <summary>
/// 检测碰撞位置函数
/// </summary>
/// <param name="x">砖块横坐标</param>
/// <param name="y">砖块纵坐标</param>
/// <param name="ball">目标小球</param>
void Check_crash(int& x, int& y, Ball& ball)
{
    bool is_ball_out_of_left_range = ball.x >= block_w * x - ball.r && ball.x <= block_w * x;
    bool is_ball_out_of_right_range = ball.x <= block_w * (x + 1) - space + ball.r && ball.x >= block_w * (x + 1) - space;
    bool is_ball_out_of_top_range = ball.y >= block_h * y - ball.r && ball.y <= block_h * y;
    bool is_ball_out_of_bottom_range = ball.y <= block_h * (y + 1) - space + ball.r && ball.y >= block_h * (y + 1) - space;

    // 调整小球碰撞后位置
    if (is_ball_out_of_left_range)
    {
        ball.x = block_w * x - ball.r - 1;
    }
    else if (is_ball_out_of_right_range)
    {
        ball.x = block_w * (x + 1) - space + ball.r + 1;
    }
    if (is_ball_out_of_top_range)
    {
        ball.y = block_h * y - ball.r - 1;
    }
    else if (is_ball_out_of_bottom_range)
    {
        ball.y = block_h * (y + 1) - space + ball.r + 1;
    }

    // 碰撞不同位置后的速度变化
    // 左上
    if (is_ball_out_of_left_range && is_ball_out_of_top_range)
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
    // 左下
    else if (is_ball_out_of_left_range && is_ball_out_of_bottom_range)
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
    // 右上
    else if (is_ball_out_of_right_range && is_ball_out_of_top_range)
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
    // 右下
    else if (is_ball_out_of_right_range && is_ball_out_of_bottom_range)
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
    // 左/右
    else if (is_ball_out_of_left_range || is_ball_out_of_right_range)
    {
        ball.speed_x *= -1;
    }
    // 上/下
    else if (is_ball_out_of_top_range || is_ball_out_of_bottom_range)
    {
        ball.speed_y *= -1;
    }

    --arr[y][x];
    ++score;
}

/// <summary>
/// 碰撞函数
/// </summary>
/// <param name="ball">目标小球</param>
void Crash(Ball& ball)
{
    int x, y;
    for (y = 0; y < num_h; y++)
    {
        for (x = 0; x < num_w; x++)
        {
            // 检测砖块碰撞
            if (
                arr[y][x] &&
                ball.x <= block_w * (x + 1) - space + ball.r &&
                ball.x >= block_w * x - ball.r &&
                ball.y <= block_h * (y + 1) - space + ball.r &&
                ball.y >= block_h * y - ball.r
                )
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

/// <summary>
/// 创造砖块
/// </summary>
void Dreate_blocks()
{
    int x = 0, y = 0;
    int i, j;
    int empty = 20;
    int blood_three = 15;
    int blood_two = 30;

    // 全部设为1血
    for (x = 0; x < num_w; x++)
    {
        for (y = 0; y < num_h; y++)
        {
            arr[y][x] = 1;
        }
    }
    // 随机设为2血
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
    // 随机设为3血
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
    // 随机设为空
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

/// <summary>
/// 绘制砖块
/// </summary>
void Draw_the_blocks()
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

/// <summary>
/// 判断游戏状态
/// </summary>
/// <param name="ball">目标小球</param>
/// <returns>游戏状态</returns>
int Game_state(Ball& ball)
{
    int x, y, state = 0;
    if (ball.y >= screen_h + 500)
    {
        Dynamic_center(L"you lost", screen_h + 40, screen_w);
        state = 0;
    }
    else
    {
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
        Dynamic_center(L"you win", screen_h + 40, screen_w);
        state = 2;
    }
    Dynamic_center(L"Press the Right button to restart the game", screen_h + 60, screen_w);
    return state;
}

/// <summary>
/// 水平居中显示
/// </summary>
/// <param name="text">文本</param>
/// <param name="y">纵坐标</param>
/// <param name="width">屏幕宽度</param>
void Dynamic_center(const wchar_t* text, int y, int width)
{
    outtextxy(width / 2 - textwidth(text) / 2, y, text);
}

/// <summary>
/// 显示分数
/// </summary>
void Show_score()
{
    wchar_t score_text[20];
    swprintf_s(score_text, L"Score: %d", score);
    Dynamic_center(score_text, screen_h + 20, screen_w);
}

int main()
{
    int play = 1;
    wchar_t buf[20] = L"score: ";
    ExMessage msg;
    Ball ball;
    srand((unsigned int)time(NULL));

    initgraph(screen_w, screen_h + 500, EW_SHOWCONSOLE);
    setbkcolor(RGB(150, 150, 200));

    while (true)
    {
        if (play == 1)
        {
            score = 0;
            Dreate_blocks();
            ball = { screen_w / 2, screen_h + 350, 10, 0, 1, RGB(rand() % 256, rand() % 256, rand() % 256) };
        }

        while (play == 1)
        {
            BeginBatchDraw();
            cleardevice();

            Draw_the_blocks();
            Board(&msg, ball);
            Crash(ball);

            Show_score();

            Ball_move(ball);
            EndBatchDraw();

            Sleep(1);

            play = Game_state(ball);
        }

        GetAsyncKeyState(VK_RBUTTON); // 清除游戏进行时的右键点击状态
        while (GetAsyncKeyState(VK_RBUTTON))
        {
            play = 1;
            game_start = 0;
            Sleep(10);
        }
    }
    return 0;
}