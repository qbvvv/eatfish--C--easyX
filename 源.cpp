#include<stdio.h>
#include<graphics.h>
#include<time.h>
#include<stdlib.h>
#include<conio.h>

#define WIDTH 1650
#define HEIGHT 1000  
#define FISH_MIN_WIDE 50
#define FISH_MIN_HEIGHT 20 
#define FISH_MAX_NUMS 7 
#define TIMER_MAX 10
#define BOARD 100 
#define ASPECT_LEFT 0
#define ASPECT_RIGHT 2
#define ROLE 0
#define PerGrow 0
//鱼的各种参数


struct buttom* create_buttom(int x, int y, int w, int h, COLORREF color, const char* str);//创建按键的函数
void draw_buttom(struct buttom* Bu);//绘制按键
void draw_title();//绘制标题
int mouse_in_buttom(struct buttom* Bu, MOUSEMSG m);//判断鼠标是否进入按键内，对按钮进行动态变化
int click_buttom(struct buttom* Bu, MOUSEMSG m);//鼠标交互
void BeginMenu();//开始菜单
void InitFish(int type);/*初始化生成鱼的函数*/
void LoadResourse();/*加载资源的函数：加载背景和鱼的图片*/
void DrawFish();/*显示鱼的图片*/
void ResetFish();/*重置未被吃的鱼*/
int OnTimer(int duration);/*利用定时器减速 种类鱼的游动速度*/
void MoveFish();/*使种类鱼自动移动*/
void Control();/*键盘上下左右操作角色鱼的移动*/
int EatFish(int type);/*吃鱼的函数*/
int GameOver();/*游戏结束*/
void ReadGrade();/*最高分*/
int Start_eatfish();/*d大鱼吃小鱼游戏内容*/
void draw_info_operate();/*绘制操作介绍*/
void draw_info_author();/*绘制作者信息*/
void draw_info_about();/*绘制游戏相关*/


struct buttom {
	int x;
	int y;
	int width;
	int height;
	COLORREF color;
	char* str;
};


struct Fish
{
	int x;
	int y;
	int dir;
	int type;
	int wide;
	int height;
};
struct Fish fish[FISH_MAX_NUMS];
//结构体定义鱼的参数


enum Menu
{
	Start,//开始
	Score,//得分
	About,//简介
	Operate,//操作描述
	Author,//作者
	Exit,//退出
	Home,//主页面
};
enum Menu menuState ;


HWND A = initgraph(WIDTH, HEIGHT, EW_SHOWCONSOLE);//创建窗口
int max_score;
int grade;
IMAGE background;
IMAGE fishIMG[7][4];/*第二维为 0，1元素左朝向鱼的背景和掩码；2，3元素为右朝向鱼的背景和掩码 第一维为鱼的种类*/

//主函数
int main()
{
	BeginMenu();
	return 0;
}

//大鱼吃小鱼游戏内容
int Start_eatfish()
{
	srand((unsigned int)time(NULL));
	LoadResourse();//加载资源
	ReadGrade();
	BeginBatchDraw();//双缓存
	for (;;)
	{
		putimage(0, 0, &background);
		DrawFish();//鱼的绘制
		FlushBatchDraw();//双缓冲
		Control();//键盘交互
		if (OnTimer(5))
			MoveFish();
		ResetFish();//重置鱼
		if (GameOver() == 1)
		{
			MessageBox(A, "GameOver", "You Lose", MB_OK);
			break;
		}
	}
	EndBatchDraw();//双缓冲
	closegraph();
    return 0;
}
	
//初始化生成鱼
void InitFish(int type)
{
	if (type == ROLE)
	{
		fish[type].x = WIDTH/2-50;
		fish[type].y = HEIGHT/2-50;//屏幕正中心的坐标
		fish[type].dir = ASPECT_RIGHT;//初始方向朝右
		fish[type].wide = FISH_MIN_WIDE + 50;
		fish[type].height = FISH_MIN_HEIGHT + 50;
		//初始的角色鱼的长宽 比最小的种类鱼大30个单位
	}
	else
	{
		fish[type].type = rand() % (FISH_MAX_NUMS - 1) + 1;//随机从后六种鱼中选取一种
		int dir = rand() % 10 >= 5 ? ASPECT_LEFT : ASPECT_RIGHT;//方向的左与右2选1
		fish[type].dir = dir;
		fish[type].y = rand() % 90 * 10 + 50;//y轴坐标的随机生成
		fish[type].x = dir == ASPECT_LEFT ? rand() % BOARD + WIDTH : -1 * rand() % BOARD;
		//通过判断朝向，随机确定x轴坐标在，若朝向向右则x轴坐标在[-Board,0]区间
		//若鱼朝向向左，则初始化鱼x轴坐标在[WIDTH,Board+WIDTH]之间
		fish[type].wide = FISH_MIN_WIDE + 10*type;//每条种类的鱼长宽依此增加10
		fish[type].height = FISH_MIN_HEIGHT +10*type ;
	}
}

//加载资源
void LoadResourse()
{
	loadimage(&background, "./Res/blue.jpg", WIDTH, HEIGHT);//加载背景
	char fileName[20] = { "" };
	for (int i= 0; i < FISH_MAX_NUMS; i++)
	{
		InitFish(i);
		for (int j = 0; j < 4; j++)
		{
			switch (j)
			{
			case 0:
				sprintf(fileName, "./Res/%d_left_y.jpg", i); //左朝向掩码图
				break;
			case 1:
				sprintf(fileName, "./Res/%d_left.jpg", i); //左朝向原图
				break;
			case 2:
				sprintf(fileName, "./Res/%d_right_y.jpg",i); //右朝向掩码图
				break;
			case 3:
				sprintf(fileName, "./Res/%d_right.jpg", i); //右朝向原图
				break;
			}			
			loadimage(&fishIMG[i][j], fileName, fish[i].wide, fish[i].height);//加载鱼的图像
		}
	}
}

//显示资源（图片）
void DrawFish()
{
	for (int i = 0; i < FISH_MAX_NUMS; i++)
	{
		putimage(fish[i].x, fish[i].y, &fishIMG[i][fish[i].dir], SRCAND);
		putimage(fish[i].x, fish[i].y, &fishIMG[i][fish[i].dir + 1], SRCPAINT);
		//掩码图//三元光栅操作码
	}
}

//重置鱼
void ResetFish()
{
	for (int i = 1; i < FISH_MAX_NUMS; i++)
	{
		switch (fish[i].dir)
		{
		case ASPECT_LEFT:
			if (fish[i].x < -BOARD)
			{
				InitFish(i);
			}
			break;
		case ASPECT_RIGHT:
			if (fish[i].x > WIDTH)
			{
				InitFish(i);
			}
			break;
		}
	}
}

//定时器
int OnTimer(int duration)
{
	static int startTime;
	int endTime = clock();
	if (endTime - startTime >= duration)
	{
		startTime = endTime;
		return 1;
	}
	return 0;
}

//鱼的自动移动
void MoveFish()//鱼的自动移动
{
	for (int i = 1; i < FISH_MAX_NUMS; i++)
	{
		switch (fish[i].dir)
		{
		case ASPECT_LEFT:
			fish[i].x--;
			break;
		case ASPECT_RIGHT:
			fish[i].x++;
			break;
		}
	}
}

//键盘交互
void Control()//键盘交互
{
	if (GetAsyncKeyState(VK_LEFT) && fish[ROLE].x > 0)
	{
		fish[ROLE].x--;
		fish[ROLE].dir = ASPECT_LEFT;
	}
	if (GetAsyncKeyState(VK_RIGHT) && fish[ROLE].x < WIDTH - fish[ROLE].wide)
	{
		fish[ROLE].x++;
		fish[ROLE].dir = ASPECT_RIGHT;
	}
	if (GetAsyncKeyState(VK_UP) && fish[ROLE].y > 0)
	{
		fish[ROLE].y--;
	}
	if (GetAsyncKeyState(VK_DOWN) && fish[ROLE].y < HEIGHT - fish[ROLE].height)
	{
		fish[ROLE].y++;
	}
	if (GetAsyncKeyState(65) )//按A键清屏  65对应A
	{
		for (int i = 1; i <= 7; i++)
		{
			InitFish(i);
		}
	}
	if (GetAsyncKeyState(83))//按S使鱼回到中心
	{
		InitFish(0);
	}
}

//吃鱼
int EatFish(int type)
{
	int LeftX_max = max(fish[ROLE].x, fish[type].x);//代表两条鱼左上角x值更大的那个值
	int LeftY_max = max(fish[ROLE].y, fish[type].y);//代表两条鱼左上角y值更大的那个值
	int RightX_min = min(fish[ROLE].x + fish[ROLE].wide, fish[type].x + fish[type].wide);
	//代表两条鱼的右下角x值更大的那个值
	int RightY_min = min(fish[ROLE].y + fish[ROLE].height, fish[type].y + fish[type].height);
	//代表两条鱼的右下角x值更大的那个值
	if (LeftX_max> RightX_min || LeftY_max > RightY_min)//没相交
	{
		return 0;
	}
	else
	{
		if (fish[ROLE].wide * fish[ROLE].height > fish[type].wide * fish[type].height)
			//用面积来判断大小
		{
			InitFish(type);
			fish[0].height += PerGrow;
			fish[0].wide += PerGrow;//长大
			grade+=1;
			return 1;
		}
		else
		{
			return 2;
		}
	}
}

//游戏结束
int GameOver()
{
	for (int i = 1; i < FISH_MAX_NUMS; i++)
	{
		if (EatFish(i) == 2 || GetAsyncKeyState(27))
			return 1;
	}
	return 0;
}

//创建按钮
struct buttom* create_buttom(int x, int y, int w, int h, COLORREF color, const char* str)
{
	struct buttom* Bu = (struct buttom*)malloc(sizeof(struct buttom));
	Bu->x = x;//按键x坐标
	Bu->y = y;//按键y坐标
	Bu->width = w;//按键宽度
	Bu->height = h;//按键高度
	Bu->color = color;//按键颜色
	Bu->str = (char*)malloc(strlen(str) + 1);
	strcpy(Bu->str, str);
	return Bu;
}

//绘制按键
void draw_buttom(struct buttom* Bu)
{
	setfillcolor(Bu->color);//按键填充颜色
	settextstyle(35, 0, "楷体");//按键字体大小及字体类型
	setlinecolor(BLACK);//按键边线颜色
	settextcolor(BLACK);//字体颜色
	setbkmode(TRANSPARENT);//字体的背景为透明
	fillrectangle(Bu->x, Bu->y, Bu->x + Bu->width, Bu->y + Bu->height);
	//按键矩形的坐标及长和宽
	outtextxy(Bu->x + 20, Bu->y + 10, Bu->str);//文字布置距离
}

//绘制标题
void draw_title()
{
	settextstyle(100, 0, "楷体");//settextstyle(100, 0, "楷体");代表字体大小为100，后面这个0代表 自适应间距，楷体。
	settextcolor(BLUE);//字体颜色
	setbkmode(TRANSPARENT);//字体背景为透明
	outtextxy(580, 50, "大鱼吃小鱼");//输出字体的内容和位置 x值为580 y值为50
}

//绘制操作过程的界面
void draw_info_operate()
{
	struct buttom* Return = create_buttom(0, 0, 110, 50, YELLOW, "返回");//4
    MOUSEMSG m = GetMouseMsg();
	settextcolor(BLACK);
	setbkmode(TRANSPARENT);
	char fileName[20] = { "" };
	IMAGE Pic_Operate;
	sprintf(fileName, "./Res/operate.jpg");
	loadimage(&Pic_Operate, fileName, WIDTH, HEIGHT);

	for (;;)
	{		
		BeginBatchDraw();
		putimage(0, 0, &Pic_Operate);
		draw_buttom(Return);
		settextstyle(100, 0, "黑体");
		outtextxy(30, 80, "操作介绍");
		settextstyle(40, 0, "楷体");
		outtextxy(30, 190, "按上下左右进行操作鱼");
		outtextxy(30, 250, "按 A 进行清屏");
		outtextxy(30, 310, "按 S 角色瞬移到屏幕中心");
		outtextxy(30, 370, "按 D 返回");
		outtextxy(30, 430, "按 ESC 自杀");
		EndBatchDraw();
		if (click_buttom(Return, m) || GetAsyncKeyState(68))//按D键或者摁按键都可返回主菜单
		{
			BeginMenu();
			break;
		}
		EndBatchDraw();
	}
}

//关于作者的信息
void draw_info_author()
{
	struct buttom* Return = create_buttom(0, 0, 110, 50, YELLOW, "返回");//4
    settextcolor(BLACK);
	setbkmode(TRANSPARENT);
	char fileName[20] = { "" };
	IMAGE Pic_Author;
    MOUSEMSG m = GetMouseMsg();
	sprintf(fileName, "./Res/uestc1.jpg");
	loadimage(&Pic_Author, fileName, WIDTH, HEIGHT);//加载鱼的图像

	for (;;)
	{
		BeginBatchDraw();
		putimage(0, 0, &Pic_Author);
		draw_buttom(Return);
		settextstyle(40, 0, "楷体");
		outtextxy(1000, 20, "来自0913班的三位帅哥");
		outtextxy(1000, 80, "组长金宇陶：来自福建省福州市");
		outtextxy(1000, 140, "组员王凯渊：来自山西省忻州市");
		outtextxy(1000, 200, "组员王彬丞：来自四川省内江市");
		if (click_buttom(Return, m) || GetAsyncKeyState(68))
		{
			BeginMenu();
			break;

		}
		EndBatchDraw();
	}
}

//关于游戏
void draw_info_about()
{
	struct buttom* Return = create_buttom(0, 0, 110, 50, YELLOW, "返回");//4
	char fileName[20] = { "" };
	IMAGE Pic_about;
    MOUSEMSG m = GetMouseMsg();
	sprintf(fileName, "./Res/about.jpg");
	loadimage(&Pic_about, fileName, WIDTH, HEIGHT);//加载鱼的图像	
	settextcolor(BLACK);
	setbkmode(TRANSPARENT);

	for (;;)
	{
	    BeginBatchDraw();
		putimage(0, 0, &Pic_about); 
		draw_buttom(Return);
		settextstyle(80, 0, "黑体");
		outtextxy(700, 50, "游戏简介");
		settextstyle(40, 0, "楷体");
		outtextxy(30, 260, "大鱼吃小鱼是如今大学生童年时期的一份美好回忆。");
		outtextxy(30, 320, "大鱼吃小鱼是一个休闲游戏，它面对的是那些没有精力或兴趣玩大型游戏的玩家（学生）。");
		outtextxy(30, 380, "现如今我们制作了大鱼吃小鱼这样一种简单好玩的游戏。");
		outtextxy(30, 440, "可以让大学生在忙碌的生活中寻求片刻放松，找到自己童年的回忆。");
		outtextxy(680, 600, "希望大家能够喜欢。");
		if (click_buttom(Return, m) || GetAsyncKeyState(68))
		{
			BeginMenu();
			break;
		}
		EndBatchDraw();
	}
}

//鼠标交互
int mouse_in_buttom(struct buttom* Bu, MOUSEMSG m) 
{
	if (Bu->x <= m.x && m.x <= Bu->x + Bu->width && Bu->y <= m.y && m.y <= Bu->y + Bu->height)
	{
		Bu->color = RED;
		return 1;
	}
	Bu->color = YELLOW;
	return 0;
}

//按键
int click_buttom(struct buttom* Bu, MOUSEMSG m)
{
	m = GetMouseMsg();
	if (mouse_in_buttom(Bu, m) && m.uMsg == WM_LBUTTONDOWN)
	{
        return 1;
	}
		
	else return 0;
}

//开始菜单
void  BeginMenu()
{
	IMAGE bk;
	loadimage(&bk, "./Res/background.jpg", WIDTH, HEIGHT);
	putimage(0, 0, &bk);
	draw_title();
	struct buttom* play = create_buttom(WIDTH / 2 - 100, HEIGHT / 2 - 200, 180, 50, YELLOW, "开始游戏");//2
	struct buttom* score = create_buttom(WIDTH / 2 - 100, HEIGHT / 2 - 100, 180, 50, YELLOW, "最高分数");//2
	struct buttom* operate = create_buttom(WIDTH / 2 - 100, HEIGHT / 2, 180, 50, YELLOW, "操作介绍");//4
	struct buttom* author = create_buttom(WIDTH / 2 - 100, HEIGHT / 2 + 100 , 180, 50, YELLOW, "关于作者");//5
	struct buttom* about = create_buttom(WIDTH / 2 - 100, HEIGHT / 2 + 200, 180, 50, YELLOW, "游戏简介");//6
	struct buttom* exit = create_buttom(WIDTH / 2 - 100, HEIGHT / 2 + 300, 180, 50, YELLOW, "退出游戏");//7  
	MOUSEMSG m = GetMouseMsg();
	//创建矩形按键
	BeginBatchDraw();
	for(;;)
    {
		draw_buttom(play);//绘制按钮
		draw_buttom(score);
        draw_buttom(operate);
        draw_buttom(author);
        draw_buttom(about);
        draw_buttom(exit);
		
		if (click_buttom(play, m))
		{
			menuState == Start;
			Start_eatfish();
		}
		else if (click_buttom(score, m))
		{
			menuState == Score;
			printf("您的最高分为：%d\n", max_score);
		}
		else if (click_buttom(operate, m))
		{
			menuState == Operate;

			draw_info_operate();
		}
		else if (click_buttom(author, m))
		{
			menuState == Author;

			draw_info_author();
		}
		else if (click_buttom(about, m))
		{
			menuState == About;
			draw_info_about();
		}
		else if (click_buttom(exit, m))
		{
			menuState == Exit;
			closegraph();
		}
		EndBatchDraw();
	}
}

//读入成绩
void ReadGrade()
{
	char* a;
	FILE* fp;
	fp = fopen("maxscore.txt", "r+");
	a = (char*)malloc(sizeof(char) + 1);
	if (fp == NULL)
	{
		fp = fopen("maxscore.txt", "w+");
		sprintf(a, "%d", grade);
		fwrite(a, sizeof(int), 1, fp);
		return;
    }
	fp = fopen("maxscore.txt", "w");
	fseek(fp, 0, 0);
	fread(&max_score, sizeof(int), 1, fp);
	if (grade > max_score)
		sprintf(a, "%d", grade);
	fwrite(a, sizeof(int), 1, fp);
	fclose(fp);
}

