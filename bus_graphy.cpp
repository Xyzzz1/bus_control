#include<graphics.h>
#include<conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<math.h>
#include<time.h>
#define PI 3.1415926535
#define MAX_TOTAL_STATION 10 //最多站数
#define STRING_MAXLEN 20	 //有用的行的最大字符数
#define total_station 10
int letter[4][4] = { {570,410,630,460},{570,470,630,520},{570,530,630,580},{570,590,630,640} };
int number[3][4] = { {650, 410, 710, 460},{650,470,710,520},{650,530,710,580} };
typedef struct state
{
	int time;
	int position;
	int target[MAX_TOTAL_STATION];
	int clockwise[MAX_TOTAL_STATION];
	int counterclockwise[MAX_TOTAL_STATION];
} state; //公交车与请求的状态
state state1;
//int total_station; //总站数
int strategy1;	   //策略
int distance;	   //站间距
int bus_direction; //对应公交车行驶方向（0顺时针，1逆时针）
int get_distance(FILE* fp);
int get_total_station(FILE* fp);
int manner2_confirm(int(*ptr)[total_station]);					//确定调度策略，返回公交车状态
int manner2_pause(int(*ptr)[total_station], int* bus_state);	//对完成请求停靠或顺便服务停靠时下一个状态的确定，返回公交车状态
int manner2_running(int(*ptr)[total_station]);					//对公交车行驶过程中下一个状态的确定，返回公交车状态
void manner2_update(int(*ptr)[total_station], int* bus_state); //更新请求数组和state1的值（只更新bus_state=1或0，bus_state=3时的state1在manner2_pause中更新）
void init_graph(void);
int main()
{
	FILE* fp;
	fopen_s(&fp, "dict.dic", "r");
	//total_station = get_total_station(fp);
	distance = get_distance(fp);
	int i, j, total_position = total_station * distance, time_count = 1, flag = 0;
	int bus_state = 0;			   //0表示空闲或服务完上个请求，1表示调度策略确定后开始行驶或正在行驶，3表示顺便停靠,10是一个特殊的返回值，避免顺便停靠的结果被manner2_update覆盖掉
	int command[3][total_station]; //请求数组，待满足的请求，0没有或已满足，1候选，2正在满足,行标：0 target，1 clockwise，2 counterclockwise 
	int(*ptr)[total_station], * ptr_state;
	double duration, x, y, rad, new_rad = 0, copyrad, copynew_rad = 0;
	wchar_t time[3], T[total_station + 1], C[total_station + 1], U[total_station + 1], position[3];
	rad = 360 * PI / 180 / total_position;
	T[total_station] = C[total_station] = U[total_station] = '\0';
	position[2] = '\0';
	for (i = 0; i < total_station; i++)
		T[i] = C[i] = U[i] = '0';
	clock_t start, finish;
	ptr = command;
	ptr_state = &bus_state;
	for (j = 0; j < 3; j++) //初始化请求数组
		for (i = 0; i < total_station; i++)
			command[j][i] = 0;
	state1.time = 0;
	state1.position = 0;
	for (i = 0; i < total_station; i++) //初始化state1
	{
		state1.target[i] = 0;
		state1.counterclockwise[i] = 0;
		state1.clockwise[i] = 0;
	}
	init_graph();//画图
	settextstyle(27, 0, _T("微软雅黑"));
	ExMessage m;
	while (1)
	{
		start = clock();
		duration = 0.0;
		while (duration < 2.0)//设置的时间单位
		{
			m = getmessage(EM_MOUSE);
			if (m.message == WM_LBUTTONDOWN)
			{
				if (m.x >= 650 && m.x <= 710 && m.y >= 410 && m.y <= 460)
				{
					wchar_t s[10];
					InputBox(s, 10, L"请输入一个数字(target)");
					int r = _wtoi(s);
					command[0][r - 1] = 1;
					state1.target[r - 1] = 1;
					T[r - 1] = '1';
					outtextxy(635, 160, T);
				}
				else if (m.x >= 650 && m.x <= 710 && m.y >= 470 && m.y <= 520)
				{
					wchar_t s[10];
					InputBox(s, 10, L"请输入一个数字(clockwise)");
					int r = _wtoi(s);
					command[1][r - 1] = 1;
					state1.clockwise[r - 1] = 1;
					C[r - 1] = '1';
					outtextxy(670, 190, C);
				}
				else if (m.x >= 650 && m.x <= 710 && m.y >= 530 && m.y <= 580)
				{
					wchar_t s[10];
					InputBox(s, 10, L"请输入一个数字(counterclockwise)");
					int r = _wtoi(s);
					command[2][r - 1] = 1;
					state1.counterclockwise[r - 1] = 1;
					U[r - 1] = '1';
					outtextxy(740, 220, U);
				}
				else if (m.x >= 570 && m.x <= 630 && m.y >= 590 && m.y <= 640)//如果点击end按钮
				{
					flag = 1;
					break;
				}
			}
			flushmessage();
			finish = clock();
			duration = (double)(finish - start) / CLOCKS_PER_SEC;
		}
		if (flag == 1)
			break;

		//更新输出区的position
		if (state1.position <= 9)
		{
			position[0] = state1.position + 48;
			position[1] = '\0';
			setfillcolor(WHITE);
			solidrectangle(655, 130, 685, 160);
		}
		else
		{
			position[0] = state1.position / 10 + 48;
			position[1] = state1.position % 10 + 48;
		}
		outtextxy(655, 130, position);

		if (bus_state == 10 || bus_state == 11)
			bus_state = 1;
		if (bus_state == 13)
			bus_state = 3;
		if (bus_state == 0 || bus_state == 3)
			bus_state = manner2_pause(ptr, &bus_state);
		if (bus_state == 1)//更新公交车的绿点
		{
			copynew_rad = 0;
			for (i = 0; i < total_position; i++)
			{
				copyrad = 360 * PI / 180 / total_position;
				x = 200 * sin(copynew_rad);
				y = 200 * cos(copynew_rad);
				setfillcolor(BLUE);
				solidcircle(250 + x, 300 - y, 6);
				copynew_rad = copynew_rad + copyrad;
			}
			for (i = 0; i < total_station; i++)
			{
				copyrad = 360 * PI / 180 / total_station;
				x = 200 * sin(copynew_rad);
				y = 200 * cos(copynew_rad);
				setfillcolor(RED);
				solidcircle(250 + x, 300 - y, 6);
				copynew_rad = copynew_rad + copyrad;
			}//擦去之前的绿点
			if (bus_direction == 0)
			{
				new_rad = new_rad + rad;
				x = 200 * sin(new_rad);
				y = 200 * cos(new_rad);
				setfillcolor(GREEN);
				solidcircle(250 + x, 300 - y, 6);
			}
			else
			{
				new_rad = new_rad - rad;
				x = 200 * sin(new_rad);
				y = 200 * cos(new_rad);
				setfillcolor(GREEN);
				solidcircle(250 + x, 300 - y, 6);
			}
			bus_state = manner2_running(ptr);
		}
		if (bus_state == 0 || bus_state == 1 || bus_state == 3)
			manner2_update(ptr, &bus_state);
		//更新输出区的target，clockwise，counterclockwise
		for (i = 0; i < total_station; i++)
		{
			if (state1.target[i] == 0)
				T[i] = '0';
			if (state1.target[i] == 1)
				T[i] = '1';
			if (state1.clockwise[i] == 0)
				C[i] = '0';
			if (state1.clockwise[i] == 1)
				C[i] = '1';
			if (state1.counterclockwise[i] == 0)
				U[i] = '0';
			if (state1.counterclockwise[i] == 1)
				U[i] = '1';
		}
		outtextxy(635, 160, T);
		outtextxy(670, 190, C);
		outtextxy(740, 220, U);

		//更新输出区时间
		time[2] = '\0';
		if (time_count <= 9)
		{
			time[0] = time_count + 48;
			time[1] = '\0';
		}
		else
		{
			time[0] = time_count / 10 + 48;
			time[1] = time_count % 10 + 48;
		}
		outtextxy(630, 100, time);
		time_count++;
	}
	closegraph();
	return 0;
}
int get_total_station(FILE* fp)
{
	FILE* ptr = fp;
	char c = '0';
	char s[STRING_MAXLEN];
	int station;
	for (; !feof(ptr);)
	{
		if (c == '#')
		{
			for (; c != '\n';)
			{
				c = fgetc(ptr);
			}
		}
		else
		{
			fgets(s, 14, ptr);
			if (strcmp(s, "TOTAL_STATION") == 0)
			{
				for (int i = 1; i <= 3; i++)
				{
					c = fgetc(ptr);
				}
				fscanf_s(ptr, "%d", &station);
				break;
			}
		}
	}
	if (feof(ptr))
		station = 5;
	rewind(fp);
	return station;
}
int get_distance(FILE* fp)
{
	FILE* ptr = fp;
	char c = '0';
	char s[STRING_MAXLEN];
	int distance;
	for (; feof(ptr) == 0;)
	{
		if (c == '#') //
		{
			for (; c != '\n';)
			{
				c = fgetc(ptr);
			}
		}
		else
		{
			fgets(s, 9, ptr);
			if (strcmp(s, "DISTANCE") == 0)
			{
				for (int i = 1; i <= 3; i++)
				{
					c = fgetc(ptr);
				}
				fscanf_s(ptr, "%d", &distance);
				break;
			}
			else
			{
				for (; c != '\n';)
				{
					c = fgetc(ptr);
				}
			}
		}
	}
	c = fgetc(ptr);
	if (feof(ptr))
		distance = 2;
	rewind(fp);
	return distance;
}
void init_graph()
{
	//画轨道
	int i, total_position;
	double x, y, rad, new_rad = 0;
	wchar_t string[3];
	string[2] = '\0';
	total_position = total_station * distance;
	initgraph(1000, 700);
	setbkcolor(WHITE);
	cleardevice();
	setlinecolor(BLACK);
	circle(250, 300, 200);
	for (i = 0; i < total_position; i++)
	{
		rad = 360 * PI / 180 / total_position;
		x = 200 * sin(new_rad);
		y = 200 * cos(new_rad);
		setfillcolor(BLUE);
		solidcircle(250 + x, 300 - y, 6);
		new_rad = new_rad + rad;
	}
	for (i = 0; i < total_station; i++)
	{
		rad = 360 * PI / 180 / total_station;
		x = 200 * sin(new_rad);
		y = 200 * cos(new_rad);
		setfillcolor(RED);
		solidcircle(250 + x, 300 - y, 6);
		x = 220 * sin(new_rad);
		y = 220 * cos(new_rad);
		settextcolor(BLACK);
		if (i < 9)
		{
			string[0] = i + 49;
			string[1] = '\0';
		}
		else if (i < 19)
		{
			string[0] = '1';
			string[1] = i - 9 + 48;
		}
		else
		{
			string[0] = '2';
			string[1] = '0';
		}

		outtextxy(250 + x - 3, 300 - y - 6, string);
		new_rad = new_rad + rad;
	}
	new_rad = 0;
	rad = 360 * PI / 180 / total_position;
	x = 200 * sin(new_rad);
	y = 200 * cos(new_rad);
	y = 200 * cos(new_rad);
	setfillcolor(GREEN);
	solidcircle(250 + x, 300 - y, 6);
	solidcircle(200, 580, 6);
	RECT R = { 215,570,270,590 };
	drawtext(_T("公交车"), &R, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	setfillcolor(RED);
	solidcircle(200, 600, 6);
	RECT R2 = { 215,590,270,610 };
	drawtext(_T("站点"), &R2, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	setfillcolor(BLUE);
	solidcircle(200, 620, 6);
	RECT R3 = { 215,610,270,630 };
	drawtext(_T("位置"), &R3, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	//画输入区
	setlinestyle(PS_SOLID, 2);
	rectangle(letter[0][0], letter[0][1], letter[0][2], letter[0][3]);
	rectangle(letter[1][0], letter[1][1], letter[1][2], letter[1][3]);
	rectangle(letter[2][0], letter[2][1], letter[2][2], letter[2][3]);
	rectangle(letter[3][0], letter[3][1], letter[3][2], letter[3][3]);
	rectangle(number[0][0], number[0][1], number[0][2], number[0][3]);
	rectangle(number[1][0], number[1][1], number[1][2], number[1][3]);
	rectangle(number[2][0], number[2][1], number[2][2], number[2][3]);
	settextstyle(38, 0, _T("微软雅黑"));
	RECT r1 = { letter[0][0], letter[0][1], letter[0][2], letter[0][3] };
	RECT r2 = { letter[1][0], letter[1][1], letter[1][2], letter[1][3] };
	RECT r3 = { letter[2][0], letter[2][1], letter[2][2], letter[2][3] };
	RECT r4 = { letter[3][0], letter[3][1], letter[3][2], letter[3][3] };
	RECT r5 = { number[0][0], number[0][1], number[0][2], number[0][3] };
	RECT r6 = { number[1][0], number[1][1], number[1][2], number[1][3] };
	RECT r7 = { number[2][0], number[2][1], number[2][2], number[2][3] };
	drawtext(_T("T"), &r1, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	drawtext(_T("C"), &r2, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	drawtext(_T("U"), &r3, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	settextstyle(25, 0, _T("微软雅黑"));
	drawtext(_T("END"), &r4, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	settextstyle(38, 0, _T("微软雅黑"));
	drawtext(_T("0"), &r5, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	drawtext(_T("0"), &r6, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	drawtext(_T("0"), &r7, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	//画输出区
	settextstyle(27, 0, _T("微软雅黑"));
	wchar_t a[] = L"TIME:", b[] = L"position:", c[] = L"target:", d[] = L"clockwise:", e[] = L"counterclockwise:", f[] = L"0", g[] = L"0000000000";
	outtextxy(570, 100, a);
	outtextxy(570, 130, b);
	outtextxy(570, 160, c);
	outtextxy(570, 190, d);
	outtextxy(570, 220, e);
	outtextxy(630, 100, f);
	outtextxy(655, 130, f);
	outtextxy(635, 160, g);
	outtextxy(670, 190, g);
	outtextxy(740, 220, g);
}
void manner2_update(int(*ptr)[total_station], int* bus_state)
{
	int i, j, now_position;
	if (state1.position % distance == 0)
		now_position = state1.position / distance;
	if (*bus_state == 0)
	{
		for (i = 0; i < 3; i++)
			for (j = 0; j < total_station; j++)
				if (ptr[i][j] == 2)
				{
					ptr[0][j] = 0;
					ptr[1][j] = 0;
					ptr[2][j] = 0;
					state1.target[j] = 0;
					state1.clockwise[j] = 0;
					state1.counterclockwise[j] = 0;
				}
	}
	else
	{
		if (bus_direction == 0)
		{
			if (state1.position < (total_station * distance - 1))
				state1.position++;
			else
				state1.position = 0;
		}
		else
		{
			if (state1.position > 0)
				state1.position--;
			else
				state1.position = total_station * distance - 1;
		}
	}
}
int manner2_running(int(*ptr)[total_station])
{
	int i, j;
	int needed_position = 0; //需要到达的position
	int next_station;	 //下一个站点
	for (i = 0; i < 3; i++)
		for (j = 0; j < total_station; j++)
			if (ptr[i][j] == 2)
			{
				needed_position = j * distance;
				break;
			}
	if (state1.position == needed_position)
		return 0;
	else
	{
		if (bus_direction == 0) //判断能否顺便服务
		{
			if (state1.position < (total_station * distance - 1))
			{
				if ((state1.position + 1) % distance == 0)
				{
					next_station = (state1.position + 1) / distance;
					if (ptr[0][next_station] == 1 || ptr[1][next_station] == 1)
					{
						if (ptr[0][next_station] == 2 || ptr[1][next_station] == 2 || ptr[2][next_station] == 2)
							return 1;
						else
							return 3;
					}
					else
						return 1;
				}
				else
					return 1;
			}
			else
			{
				next_station = 0;
				if (ptr[0][0] == 1 || ptr[1][0] == 1)
				{
					if (ptr[0][0] == 2 || ptr[1][0] == 2 || ptr[2][0] == 2)
						return 1;
					else
						return 3;
				}
				else
					return 1;
			}
		}
		else
		{
			if (state1.position > 0)
			{
				if ((state1.position - 1) % distance == 0)
				{
					next_station = (state1.position - 1) / distance;
					if (ptr[0][next_station] == 1 || ptr[2][next_station] == 1)
					{
						if (ptr[0][next_station] == 2 || ptr[1][next_station] == 2 || ptr[2][next_station] == 2)
							return 1;
						else
							return 3;
					}
					else
						return 1;
				}
				else
					return 1;
			}
			else
			{
				next_station = total_station * distance - 1;
				if (ptr[0][next_station] == 1 || ptr[2][next_station] == 1)
				{
					if (ptr[0][next_station] == 2 || ptr[1][next_station] == 2 || ptr[2][next_station] == 2)
						return 1;
					else
						return 3;
				}
				else
					return 1;
			}
		}
	}
}
int manner2_pause(int(*ptr)[total_station], int* bus_state)
{
	int i, now_position, next_station;
	now_position = state1.position / distance;
	if (*bus_state == 0)
	{
		for (i = 0; i < 3; i++) //避免compare.distance=0的情况
			if (ptr[i][now_position] != 0)
			{
				ptr[i][now_position] = 0;
				if (i == 0)
					state1.target[now_position] = 0;
				else if (i == 1)
					state1.clockwise[now_position] = 0;
				else
					state1.counterclockwise[now_position] = 0;
			}
		*bus_state = manner2_confirm(ptr);
		if (*bus_state == 1)
		{
			if (state1.target[now_position] == 1 || state1.clockwise[now_position] == 1 || state1.counterclockwise[now_position] == 1) //更新如果本站有请求且根据规则可以为该请求服务，则该请求立即完成，不再停1秒钟。
			{
				state1.target[now_position] = 0;
				state1.clockwise[now_position] = 0;
				state1.counterclockwise[now_position] = 0;
				ptr[0][now_position] = ptr[1][now_position] = ptr[2][now_position] = 0;
			}
			if (bus_direction == 0)
				state1.position++;
			else
			{
				if (now_position == 0)
					state1.position = total_station * distance - 1;
				else
					state1.position--;
			}
			if (distance == 1) //如果distance=1需要判断能否顺便服务
			{
				next_station = now_position;
				if (bus_direction == 0)
				{
					if (ptr[0][next_station] == 1 || ptr[1][next_station] == 1)
					{
						if (ptr[0][next_station] == 2 || ptr[1][next_station] == 2 || ptr[2][next_station] == 2)
							*bus_state = 11; //11和13是两个特殊的返回值，避免结果被update覆盖掉
						else
							*bus_state = 13;
					}
					else
						*bus_state = 11;
				}
				else
				{
					if (ptr[0][next_station] == 1 || ptr[2][next_station] == 1)
					{
						if (ptr[0][next_station] == 2 || ptr[1][next_station] == 2 || ptr[2][next_station] == 2)
							*bus_state = 11;
						else
							*bus_state = 13;
					}
					else
						*bus_state = 11;
				}
			}
			else
				*bus_state = 11;
		}
	}
	else if (*bus_state == 3) //更新请求数组和state1的值
	{
		ptr[0][now_position] = 0;
		state1.target[now_position] = 0;
		if (bus_direction == 0)
		{
			ptr[1][now_position] = 0;
			state1.clockwise[now_position] = 0;
		}
		else
		{
			ptr[2][now_position] = 0;
			state1.counterclockwise[now_position] = 0;
		}
		*bus_state = 10; //一个特殊的返回值，避免顺便停靠的结果被manner2_update覆盖掉
	}
	return *bus_state;
}
int manner2_confirm(int(*ptr)[total_station])
{
	typedef struct confirm //行标：0 target，1 clockwise，2 counterclockwise
	{
		int distance[3];  //对应路程
		int station[3];	  //对应站台
		int direction[3]; //所需要公交车的方向（0顺时针，1逆时针）
	} confirm;
	confirm compare;
	int i, j, copy1, copy2, now_position;
	for (i = 0; i < 3; i++) //最初赋值
	{
		compare.distance[i] = 100;
		compare.station[i] = 100;
		compare.direction[i] = 100;
	}
	now_position = state1.position / distance;
	for (j = 0; j < 3; j++) //确定compare初始值
	{
		for (i = 0; i < total_station; i++)
		{
			if (ptr[j][i] == 0)
				continue;
			else if (ptr[j][i] == 1)
			{
				compare.station[j] = i;
				if (i > now_position)
				{
					copy1 = i - now_position;
					compare.distance[j] = total_station - (i - now_position);
					compare.station[j] = i;
					if (copy1 < compare.distance[j])
					{
						compare.distance[j] = copy1;
						compare.direction[j] = 0;
					}
					else if (copy1 > compare.distance[j])
						compare.direction[j] = 1;
					else
						compare.direction[j] = 0;
				}
				else
				{
					copy1 = now_position - i;
					compare.distance[j] = total_station - (now_position - i);
					compare.station[j] = i;
					if (copy1 < compare.distance[j])
					{
						compare.distance[j] = copy1;
						compare.direction[j] = 1;
					}
					else if ((copy1 > compare.distance[j]))
						compare.direction[j] = 0;
					else
						compare.direction[j] = 0;
				}
				break;
			}
		}
	}
	if (compare.distance[0] == 100 && compare.distance[1] == 100 && compare.distance[2] == 100)
		return 0;
	else
	{
		for (j = 0; j < 3; j++) //确定compare中各数组最终值
		{
			if (compare.distance[j] == 100)
				continue;
			else
			{
				i = compare.station[j] + 1;
				for (i = i; i < total_station; i++)
				{
					if (ptr[j][i] == 0)
						continue;
					else if (ptr[j][i] == 1)
					{
						if (i < now_position)
						{
							copy1 = now_position - i;
							copy2 = total_station - copy1;
							if (copy1 < copy2)
							{
								if (copy1 < compare.distance[j])
								{
									compare.distance[j] = copy1;
									compare.station[j] = i;
									compare.direction[j] = 1;
								}
								else if (compare.distance[j] < copy1)
									continue;
								else
									continue;
							}
							else if (copy2 < copy1)
							{
								if (copy2 < compare.distance[j])
								{
									compare.distance[j] = copy2;
									compare.station[j] = i;
									compare.direction[j] = 0;
								}
								else if (compare.distance[j] < copy2)
									continue;
								else
								{
									compare.station[j] = i;
									compare.direction[j] = 0;
								}
							}
							else
								continue;
						}
						else if (i > now_position)
						{
							copy1 = i - now_position;
							copy2 = total_station - copy1;
							if (copy1 < copy2)
							{
								if (copy1 < compare.distance[j])
								{
									compare.distance[j] = copy1;
									compare.station[j] = i;
									compare.direction[j] = 0;
								}
								else if (compare.distance[j] < copy1)
									continue;
								else
								{
									compare.station[j] = i;
									compare.direction[j] = 0;
								}
							}
							else if (copy2 < copy1)
							{
								if (copy2 < compare.distance[j])
								{
									compare.distance[j] = copy2;
									compare.station[j] = i;
									compare.direction[j] = 1;
								}
								else if (compare.distance[j] < copy2)
									continue;
								else
									continue;
							}
							else
								continue;
						}
					}
				}
			}
		}
		if (compare.distance[0] < compare.distance[1]) //调度策略确定
		{
			if (compare.distance[0] < compare.distance[2])
			{
				ptr[0][compare.station[0]] = 2;
				bus_direction = compare.direction[0];
			}
			else if (compare.distance[0] == compare.distance[2])
			{
				if (compare.direction[0] == 0 && compare.direction[2] == 0)
				{
					ptr[0][compare.station[0]] = 2;
					ptr[2][compare.station[2]] = 2;
					bus_direction = 0;
				}
				else if (compare.direction[0] == 0 && compare.direction[2] == 1)
				{
					ptr[0][compare.station[0]] = 2;
					bus_direction = 0;
				}
				else if (compare.direction[0] == 1 && compare.direction[2] == 0)
				{
					ptr[2][compare.station[2]] = 2;
					bus_direction = 0;
				}
				else if (compare.direction[0] == 1 && compare.direction[2] == 1)
				{
					ptr[0][compare.station[0]] = 2;
					ptr[2][compare.station[2]] = 2;
					bus_direction = 1;
				}
			}
			else
			{
				ptr[2][compare.station[2]] = 2;
				bus_direction = compare.direction[2];
			}
		}
		else if (compare.distance[1] < compare.distance[0])
		{
			if (compare.distance[1] < compare.distance[2])
			{
				ptr[1][compare.station[1]] = 2;
				bus_direction = compare.direction[1];
			}
			else if (compare.distance[2] < compare.distance[1])
			{
				ptr[2][compare.station[2]] = 2;
				bus_direction = compare.direction[2];
			}
			else
			{
				if (compare.direction[1] == 0 && compare.direction[2] == 0)
				{
					ptr[1][compare.station[1]] = 2;
					ptr[2][compare.station[2]] = 2;
					bus_direction = 0;
				}
				else if (compare.direction[1] == 1 && compare.direction[2] == 0)
				{
					ptr[2][compare.station[2]] = 2;
					bus_direction = 0;
				}
				else if (compare.direction[1] == 0 && compare.direction[2] == 1)
				{
					ptr[1][compare.station[1]] = 2;
					bus_direction = 0;
				}
				else
				{
					ptr[1][compare.station[1]] = 2;
					ptr[2][compare.station[2]] = 2;
					bus_direction = 1;
				}
			}
		}
		else
		{
			if (compare.distance[2] < compare.distance[1])
			{
				ptr[2][compare.station[2]] = 2;
				bus_direction = compare.direction[2];
			}
			else if (compare.distance[1] < compare.distance[2])
			{
				if (compare.direction[0] == 0 && compare.direction[1] == 0)
				{
					ptr[0][compare.station[0]] = 2;
					ptr[1][compare.station[1]] = 2;
					bus_direction = 0;
				}
				else if (compare.direction[0] == 0 && compare.direction[1] == 1)
				{
					ptr[0][compare.station[0]] = 2;
					bus_direction = 0;
				}
				else if (compare.direction[0] == 1 && compare.direction[1] == 0)
				{
					ptr[1][compare.station[1]] = 2;
					bus_direction = 0;
				}
				else
				{
					ptr[0][compare.station[0]] = 2;
					ptr[1][compare.station[1]] = 2;
					bus_direction = 1;
				}
			}
			else
			{
				if (compare.direction[0] == 0 && compare.direction[1] == 0 && compare.direction[2] == 0)
				{
					ptr[0][compare.station[0]] = 2;
					ptr[1][compare.station[1]] = 2;
					ptr[2][compare.station[2]] = 2;
					bus_direction = 0;
				}
				else if (compare.direction[0] == 0 && compare.direction[1] == 0 && compare.direction[2] == 1)
				{
					ptr[0][compare.station[0]] = 2;
					ptr[1][compare.station[1]] = 2;
					bus_direction = 0;
				}
				else if (compare.direction[0] == 0 && compare.direction[1] == 1 && compare.direction[2] == 0)
				{
					ptr[0][compare.station[0]] = 2;
					ptr[2][compare.station[2]] = 2;
					bus_direction = 0;
				}
				else if (compare.direction[0] == 1 && compare.direction[1] == 0 && compare.direction[2] == 0)
				{
					ptr[1][compare.station[1]] = 2;
					ptr[2][compare.station[2]] = 2;
					bus_direction = 0;
				}
				else if (compare.direction[0] == 1 && compare.direction[1] == 1 && compare.direction[2] == 0)
				{
					ptr[2][compare.station[2]] = 2;
					bus_direction = 0;
				}
				else if (compare.direction[0] == 0 && compare.direction[1] == 1 && compare.direction[2] == 1)
				{
					ptr[0][compare.station[0]] = 2;
					bus_direction = 0;
				}
				else if (compare.direction[0] == 1 && compare.direction[1] == 0 && compare.direction[2] == 1)
				{
					ptr[1][compare.station[1]] = 2;
					bus_direction = 0;
				}
				else if (compare.direction[0] == 1 && compare.direction[1] == 1 && compare.direction[2] == 1)
				{
					ptr[0][compare.station[0]] = 2;
					ptr[1][compare.station[1]] = 2;
					ptr[2][compare.station[2]] = 2;
					bus_direction = 1;
				}
			}
		}
		return 1;
	}
}