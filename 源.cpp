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
//��ĸ��ֲ���


struct buttom* create_buttom(int x, int y, int w, int h, COLORREF color, const char* str);//���������ĺ���
void draw_buttom(struct buttom* Bu);//���ư���
void draw_title();//���Ʊ���
int mouse_in_buttom(struct buttom* Bu, MOUSEMSG m);//�ж�����Ƿ���밴���ڣ��԰�ť���ж�̬�仯
int click_buttom(struct buttom* Bu, MOUSEMSG m);//��꽻��
void BeginMenu();//��ʼ�˵�
void InitFish(int type);/*��ʼ��������ĺ���*/
void LoadResourse();/*������Դ�ĺ��������ر��������ͼƬ*/
void DrawFish();/*��ʾ���ͼƬ*/
void ResetFish();/*����δ���Ե���*/
int OnTimer(int duration);/*���ö�ʱ������ ��������ζ��ٶ�*/
void MoveFish();/*ʹ�������Զ��ƶ�*/
void Control();/*�����������Ҳ�����ɫ����ƶ�*/
int EatFish(int type);/*����ĺ���*/
int GameOver();/*��Ϸ����*/
void ReadGrade();/*��߷�*/
int Start_eatfish();/*d�����С����Ϸ����*/
void draw_info_operate();/*���Ʋ�������*/
void draw_info_author();/*����������Ϣ*/
void draw_info_about();/*������Ϸ���*/


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
//�ṹ�嶨����Ĳ���


enum Menu
{
	Start,//��ʼ
	Score,//�÷�
	About,//���
	Operate,//��������
	Author,//����
	Exit,//�˳�
	Home,//��ҳ��
};
enum Menu menuState ;


HWND A = initgraph(WIDTH, HEIGHT, EW_SHOWCONSOLE);//��������
int max_score;
int grade;
IMAGE background;
IMAGE fishIMG[7][4];/*�ڶ�άΪ 0��1Ԫ��������ı��������룻2��3Ԫ��Ϊ�ҳ�����ı��������� ��һάΪ�������*/

//������
int main()
{
	BeginMenu();
	return 0;
}

//�����С����Ϸ����
int Start_eatfish()
{
	srand((unsigned int)time(NULL));
	LoadResourse();//������Դ
	ReadGrade();
	BeginBatchDraw();//˫����
	for (;;)
	{
		putimage(0, 0, &background);
		DrawFish();//��Ļ���
		FlushBatchDraw();//˫����
		Control();//���̽���
		if (OnTimer(5))
			MoveFish();
		ResetFish();//������
		if (GameOver() == 1)
		{
			MessageBox(A, "GameOver", "You Lose", MB_OK);
			break;
		}
	}
	EndBatchDraw();//˫����
	closegraph();
    return 0;
}
	
//��ʼ��������
void InitFish(int type)
{
	if (type == ROLE)
	{
		fish[type].x = WIDTH/2-50;
		fish[type].y = HEIGHT/2-50;//��Ļ�����ĵ�����
		fish[type].dir = ASPECT_RIGHT;//��ʼ������
		fish[type].wide = FISH_MIN_WIDE + 50;
		fish[type].height = FISH_MIN_HEIGHT + 50;
		//��ʼ�Ľ�ɫ��ĳ��� ����С���������30����λ
	}
	else
	{
		fish[type].type = rand() % (FISH_MAX_NUMS - 1) + 1;//����Ӻ���������ѡȡһ��
		int dir = rand() % 10 >= 5 ? ASPECT_LEFT : ASPECT_RIGHT;//�����������2ѡ1
		fish[type].dir = dir;
		fish[type].y = rand() % 90 * 10 + 50;//y��������������
		fish[type].x = dir == ASPECT_LEFT ? rand() % BOARD + WIDTH : -1 * rand() % BOARD;
		//ͨ���жϳ������ȷ��x�������ڣ�������������x��������[-Board,0]����
		//���㳯���������ʼ����x��������[WIDTH,Board+WIDTH]֮��
		fish[type].wide = FISH_MIN_WIDE + 10*type;//ÿ��������㳤����������10
		fish[type].height = FISH_MIN_HEIGHT +10*type ;
	}
}

//������Դ
void LoadResourse()
{
	loadimage(&background, "./Res/blue.jpg", WIDTH, HEIGHT);//���ر���
	char fileName[20] = { "" };
	for (int i= 0; i < FISH_MAX_NUMS; i++)
	{
		InitFish(i);
		for (int j = 0; j < 4; j++)
		{
			switch (j)
			{
			case 0:
				sprintf(fileName, "./Res/%d_left_y.jpg", i); //��������ͼ
				break;
			case 1:
				sprintf(fileName, "./Res/%d_left.jpg", i); //����ԭͼ
				break;
			case 2:
				sprintf(fileName, "./Res/%d_right_y.jpg",i); //�ҳ�������ͼ
				break;
			case 3:
				sprintf(fileName, "./Res/%d_right.jpg", i); //�ҳ���ԭͼ
				break;
			}			
			loadimage(&fishIMG[i][j], fileName, fish[i].wide, fish[i].height);//�������ͼ��
		}
	}
}

//��ʾ��Դ��ͼƬ��
void DrawFish()
{
	for (int i = 0; i < FISH_MAX_NUMS; i++)
	{
		putimage(fish[i].x, fish[i].y, &fishIMG[i][fish[i].dir], SRCAND);
		putimage(fish[i].x, fish[i].y, &fishIMG[i][fish[i].dir + 1], SRCPAINT);
		//����ͼ//��Ԫ��դ������
	}
}

//������
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

//��ʱ��
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

//����Զ��ƶ�
void MoveFish()//����Զ��ƶ�
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

//���̽���
void Control()//���̽���
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
	if (GetAsyncKeyState(65) )//��A������  65��ӦA
	{
		for (int i = 1; i <= 7; i++)
		{
			InitFish(i);
		}
	}
	if (GetAsyncKeyState(83))//��Sʹ��ص�����
	{
		InitFish(0);
	}
}

//����
int EatFish(int type)
{
	int LeftX_max = max(fish[ROLE].x, fish[type].x);//�������������Ͻ�xֵ������Ǹ�ֵ
	int LeftY_max = max(fish[ROLE].y, fish[type].y);//�������������Ͻ�yֵ������Ǹ�ֵ
	int RightX_min = min(fish[ROLE].x + fish[ROLE].wide, fish[type].x + fish[type].wide);
	//��������������½�xֵ������Ǹ�ֵ
	int RightY_min = min(fish[ROLE].y + fish[ROLE].height, fish[type].y + fish[type].height);
	//��������������½�xֵ������Ǹ�ֵ
	if (LeftX_max> RightX_min || LeftY_max > RightY_min)//û�ཻ
	{
		return 0;
	}
	else
	{
		if (fish[ROLE].wide * fish[ROLE].height > fish[type].wide * fish[type].height)
			//��������жϴ�С
		{
			InitFish(type);
			fish[0].height += PerGrow;
			fish[0].wide += PerGrow;//����
			grade+=1;
			return 1;
		}
		else
		{
			return 2;
		}
	}
}

//��Ϸ����
int GameOver()
{
	for (int i = 1; i < FISH_MAX_NUMS; i++)
	{
		if (EatFish(i) == 2 || GetAsyncKeyState(27))
			return 1;
	}
	return 0;
}

//������ť
struct buttom* create_buttom(int x, int y, int w, int h, COLORREF color, const char* str)
{
	struct buttom* Bu = (struct buttom*)malloc(sizeof(struct buttom));
	Bu->x = x;//����x����
	Bu->y = y;//����y����
	Bu->width = w;//�������
	Bu->height = h;//�����߶�
	Bu->color = color;//������ɫ
	Bu->str = (char*)malloc(strlen(str) + 1);
	strcpy(Bu->str, str);
	return Bu;
}

//���ư���
void draw_buttom(struct buttom* Bu)
{
	setfillcolor(Bu->color);//���������ɫ
	settextstyle(35, 0, "����");//���������С����������
	setlinecolor(BLACK);//����������ɫ
	settextcolor(BLACK);//������ɫ
	setbkmode(TRANSPARENT);//����ı���Ϊ͸��
	fillrectangle(Bu->x, Bu->y, Bu->x + Bu->width, Bu->y + Bu->height);
	//�������ε����꼰���Ϳ�
	outtextxy(Bu->x + 20, Bu->y + 10, Bu->str);//���ֲ��þ���
}

//���Ʊ���
void draw_title()
{
	settextstyle(100, 0, "����");//settextstyle(100, 0, "����");���������СΪ100���������0���� ����Ӧ��࣬���塣
	settextcolor(BLUE);//������ɫ
	setbkmode(TRANSPARENT);//���屳��Ϊ͸��
	outtextxy(580, 50, "�����С��");//�����������ݺ�λ�� xֵΪ580 yֵΪ50
}

//���Ʋ������̵Ľ���
void draw_info_operate()
{
	struct buttom* Return = create_buttom(0, 0, 110, 50, YELLOW, "����");//4
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
		settextstyle(100, 0, "����");
		outtextxy(30, 80, "��������");
		settextstyle(40, 0, "����");
		outtextxy(30, 190, "���������ҽ��в�����");
		outtextxy(30, 250, "�� A ��������");
		outtextxy(30, 310, "�� S ��ɫ˲�Ƶ���Ļ����");
		outtextxy(30, 370, "�� D ����");
		outtextxy(30, 430, "�� ESC ��ɱ");
		EndBatchDraw();
		if (click_buttom(Return, m) || GetAsyncKeyState(68))//��D���������������ɷ������˵�
		{
			BeginMenu();
			break;
		}
		EndBatchDraw();
	}
}

//�������ߵ���Ϣ
void draw_info_author()
{
	struct buttom* Return = create_buttom(0, 0, 110, 50, YELLOW, "����");//4
    settextcolor(BLACK);
	setbkmode(TRANSPARENT);
	char fileName[20] = { "" };
	IMAGE Pic_Author;
    MOUSEMSG m = GetMouseMsg();
	sprintf(fileName, "./Res/uestc1.jpg");
	loadimage(&Pic_Author, fileName, WIDTH, HEIGHT);//�������ͼ��

	for (;;)
	{
		BeginBatchDraw();
		putimage(0, 0, &Pic_Author);
		draw_buttom(Return);
		settextstyle(40, 0, "����");
		outtextxy(1000, 20, "����0913�����λ˧��");
		outtextxy(1000, 80, "�鳤�����գ����Ը���ʡ������");
		outtextxy(1000, 140, "��Ա����Ԩ������ɽ��ʡ������");
		outtextxy(1000, 200, "��Ա����ة�������Ĵ�ʡ�ڽ���");
		if (click_buttom(Return, m) || GetAsyncKeyState(68))
		{
			BeginMenu();
			break;

		}
		EndBatchDraw();
	}
}

//������Ϸ
void draw_info_about()
{
	struct buttom* Return = create_buttom(0, 0, 110, 50, YELLOW, "����");//4
	char fileName[20] = { "" };
	IMAGE Pic_about;
    MOUSEMSG m = GetMouseMsg();
	sprintf(fileName, "./Res/about.jpg");
	loadimage(&Pic_about, fileName, WIDTH, HEIGHT);//�������ͼ��	
	settextcolor(BLACK);
	setbkmode(TRANSPARENT);

	for (;;)
	{
	    BeginBatchDraw();
		putimage(0, 0, &Pic_about); 
		draw_buttom(Return);
		settextstyle(80, 0, "����");
		outtextxy(700, 50, "��Ϸ���");
		settextstyle(40, 0, "����");
		outtextxy(30, 260, "�����С��������ѧ��ͯ��ʱ�ڵ�һ�����û��䡣");
		outtextxy(30, 320, "�����С����һ��������Ϸ������Ե�����Щû�о�������Ȥ�������Ϸ����ң�ѧ������");
		outtextxy(30, 380, "��������������˴����С������һ�ּ򵥺������Ϸ��");
		outtextxy(30, 440, "�����ô�ѧ����æµ��������Ѱ��Ƭ�̷��ɣ��ҵ��Լ�ͯ��Ļ��䡣");
		outtextxy(680, 600, "ϣ������ܹ�ϲ����");
		if (click_buttom(Return, m) || GetAsyncKeyState(68))
		{
			BeginMenu();
			break;
		}
		EndBatchDraw();
	}
}

//��꽻��
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

//����
int click_buttom(struct buttom* Bu, MOUSEMSG m)
{
	m = GetMouseMsg();
	if (mouse_in_buttom(Bu, m) && m.uMsg == WM_LBUTTONDOWN)
	{
        return 1;
	}
		
	else return 0;
}

//��ʼ�˵�
void  BeginMenu()
{
	IMAGE bk;
	loadimage(&bk, "./Res/background.jpg", WIDTH, HEIGHT);
	putimage(0, 0, &bk);
	draw_title();
	struct buttom* play = create_buttom(WIDTH / 2 - 100, HEIGHT / 2 - 200, 180, 50, YELLOW, "��ʼ��Ϸ");//2
	struct buttom* score = create_buttom(WIDTH / 2 - 100, HEIGHT / 2 - 100, 180, 50, YELLOW, "��߷���");//2
	struct buttom* operate = create_buttom(WIDTH / 2 - 100, HEIGHT / 2, 180, 50, YELLOW, "��������");//4
	struct buttom* author = create_buttom(WIDTH / 2 - 100, HEIGHT / 2 + 100 , 180, 50, YELLOW, "��������");//5
	struct buttom* about = create_buttom(WIDTH / 2 - 100, HEIGHT / 2 + 200, 180, 50, YELLOW, "��Ϸ���");//6
	struct buttom* exit = create_buttom(WIDTH / 2 - 100, HEIGHT / 2 + 300, 180, 50, YELLOW, "�˳���Ϸ");//7  
	MOUSEMSG m = GetMouseMsg();
	//�������ΰ���
	BeginBatchDraw();
	for(;;)
    {
		draw_buttom(play);//���ư�ť
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
			printf("������߷�Ϊ��%d\n", max_score);
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

//����ɼ�
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

