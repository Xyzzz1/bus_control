#include<stdio.h>
#include<math.h>
#define MAX_TOTAL_STATION 20  //最多站数
#define STRING_MAXLEN 20//有用的行最大字符数 
typedef struct state
{
   int time;
   int position;
   int target[MAX_TOTAL_STATION];
   int clockwise[MAX_TOTAL_STATION];
   int counterclockwise[MAX_TOTAL_STATION];
}state;  //公交车与请求的状态
state state1;
int total_station;  //总站数
int strategy1; //策略
int distance; //站间距
int request2[1000];
int bus_direction; //对应公交车行驶方向（0顺时针，1逆时针）
void control(int strategy1);
int input(void);
int judge(char *ptr);
void output(void);
int get_total_station(FILE*);//读取站点个数
int get_strategy(FILE*);//策略
int get_distance(FILE*);//两站之间距离 
void manner1(int,int);
void run2(int,int);
int direction(int x,int total_station,int distance);
int confirm(int);
void manner2(int,int); 
int manner2_confirm(int (*ptr)[total_station]);                 //确定调度策略，返回公交车状态
int manner2_pause(int (*ptr)[total_station], int *bus_state);   //对完成请求停靠或顺便服务停靠时下一个状态的确定，返回公交车状态
int manner2_running(int (*ptr)[total_station]);                 //对公交车行驶过程中下一个状态的确定，返回公交车状态
void manner2_update(int (*ptr)[total_station], int *bus_state); //更新请求数组和state1的值（只更新bus_state=1或0，bus_state=3时的state1在manner2_pause中更新）
void manner3(int,int); 
void run(int get,int*request);
int if_request(int*request);
int for_distance(int n);
int judge_direction(int*request);
int find_target(int*request);
int dispatch(int*request,int get,int*ptrstop,int targer_station,int original_direction);
void SCAN(void);
int main(int argc, char *argv[]){
    FILE*fp=fopen("dict.dic","r");
    total_station=get_total_station(fp);
    distance=get_distance(fp);
    strategy1=get_strategy(fp);
    control(strategy1);
    fclose(fp);
    return 0;
} 
void control(int strategy1){
    if(strategy1==1)
    manner1(total_station,distance);
    else if(strategy1==2)
    manner2(total_station,distance); 
    else if(strategy1==3)
    manner3(total_station,distance); 
} 
int input(void)
{
    char string[40];
    char *ptr = string;
    fgets(string, 40, stdin);
    if (string[0] == 'e') //读到的是end
        return 0;
    else if (string[4] == 'k' && string[5] != 'w') //读到clock
        return 1;
    else if (string[0] == 't') //读到target
        return judge(ptr) + 100;
    else if (string[5] == 'w') //读到clockwise
        return judge(ptr) + 1000;
    else //读到counterclockwise
        return judge(ptr) + 10000;
}
int judge(char *ptr) //判断并返回target或clockwise或counterclockwise后的数字
{
    int i = 0;
    while (*(ptr + i) != '\n')
        i++;
    i = i - 2;
    if (*(ptr + i) == ' ')
        return *(ptr + i + 1) - 48;
    else
        return (*(ptr + i) - 48) * 10 + *(ptr + i + 1) - 48;
}
void output(void){
    int i,j,k;
    printf("TIME:%d\n",state1.time);
    printf("BUS:\n");
    printf("position:%d\n",state1.position);
    printf("target:");
    for(i=0;i<total_station-1;i++){
        printf("%d",state1.target[i]);
    }
    printf("%d\n",state1.target[total_station-1]);
    printf("STATION:\n");
    printf("clockwise:");
    for(j=0;j<total_station-1;j++){
        printf("%d",state1.clockwise[j]);
    }
    printf("%d\n",state1.clockwise[total_station-1]);
    printf("counterclockwise:");
    for(k=0;k<total_station-1;k++){
        printf("%d",state1.counterclockwise[k]);
    }
    printf("%d\n",state1.counterclockwise[total_station-1]);
} 
int get_total_station(FILE *fp)
{
	FILE *ptr = fp;
	char c;
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
				fscanf(ptr, "%d", &station);
				break;
			}
		}
	}
	if (feof(ptr))
		station = 5;
	rewind(fp);
	return station;
}
int get_distance(FILE*fp)
{
    FILE*ptr=fp;
    char c=fgetc(ptr);
    char s[STRING_MAXLEN];
    int distance;
    for(;feof(ptr)==0;)
    {
        if(c=='#')//
        {
            for(;c!='\n';)
            {
                c=fgetc(ptr);
            }
        }
        else
        {
            fgets(s,9,ptr);
            if(strcmp(s,"DISTANCE")==0)
            {
                for(int i=1;i<=3;i++)
                {
                    c=fgetc(ptr);
                }
                fscanf(ptr,"%d",&distance);
                break;
            }
            else
            {
                for(;c!='\n';)
                {
                    c=fgetc(ptr);
                }
            }
        }
    }
    c=fgetc(ptr);
    if(feof(ptr))
    distance=2;
    rewind(fp);
    return distance;
}
int get_strategy(FILE*fp)
{
    FILE*ptr=fp;
    char c=fgetc(ptr);
    char s[STRING_MAXLEN],name[5]="XXXX";//name:绛栫暐缂╁啓
    int strategy;
    for(;feof(ptr)==0;)
    {
        if(c=='#')
        {
            for(;c!='\n';)
            {
                c=fgetc(ptr);
            }
        }
        else
        {
            fgets(s,9,ptr);
            if(strcmp(s,"STRATEGY")==0)
            {
                for(int i=1;i<=3;i++)
                {
                    c=fgetc(ptr);
                }
                fscanf(ptr,"%s",name);
                break;
            }
            else
            {
                for(;c!='\n';)
                {
                    c=fgetc(ptr);
                }
            }
        }    
    }
    rewind(fp);
    if(strcmp(name,"FCFS")==0)
    return 1;
    else if(strcmp(name,"SSTF")==0)
    return 2;
    else if(strcmp(name,"SCAN")==0)
    return 3;
    else
    return 1;
}
void manner1(int total_station,int distance){
    state1.time=0;
    state1.position=0;
    int i;
    for(i=0;i<total_station;i++){
        state1.target[i]=0;
    }
    for(i=0;i<total_station;i++){
        state1.clockwise[i]=0;
    }
    for(i=0;i<total_station;i++){
        state1.counterclockwise[i]=0;
    }
    output();
    for(i=0;i<1000;i++){
        request2[i]=total_station+1;
    }
    int a=input();
    int x=0;
    while(a!=0){
            while(a>100){
                if(confirm(a)==1){
                request2[x]=a;
                x++;
                if(a>100&&a<1000)
                state1.target[a-101]=1;
                else if(a>1000&&a<10000)
                state1.clockwise[a-1001]=1;
                else if(a>10000)
                state1.counterclockwise[a-10001]=1;
                a=input();
            }
            else if(confirm(a)!=1){
                a=input();
            }
            }   
            while(a==1){
                state1.time++;
                run2(total_station,distance);
                output();
                a=input();
            }       
        }
        printf("end\n");
}
void run2(int total_station,int distance){
    int i;
    int x;
    for(i=0;i<1000;i++){
        if(request2[i]!=0&&request2[i]!=total_station+1){
            if(request2[i]>100&&request2[i]<1000){//是target
                x=(request2[i]-101)*distance;
                int dic1=direction(x,total_station,distance);
                if(dic1==0){
                    int y=(request2[i]%100)-(request2[i+1]%100);
                    if(y!=0){
                    state1.target[request2[i]-101]=0;
                    request2[i]=0;
                    break;
                    }
                    else if(y==0){
                        while(((request2[i]%100)-(request2[i+1]%100))==0){
                            if(request2[i]>1000&&request2[i]<10000){
                                state1.clockwise[request2[i]-1001]=0;
                            }
                            else if(request2[i]>10000){
                                state1.counterclockwise[request2[i]-10001]=0;
                            }
                            else if(request2[i]>100&&request2[i]<1000){
                                state1.target[request2[i]-101]=0;
                            }
                            if(request2[i+1]>1000&&request2[i+1]<10000){
                                state1.clockwise[request2[i+1]-1001]=0;
                            }
                            else if(request2[i+1]>10000){
                                state1.counterclockwise[request2[i+1]-10001]=0;
                            }
                            else if(request2[i+1]>100&&request2[i+1]<1000){
                                state1.target[request2[i+1]-101]=0;
                        }
                        i++;
                    }
                    int t=i;
                    int z;
                    for(z=0;z<=t;z++){
                        request2[z]=0;
                    }
                }
            }
            else if(dic1==1){
                if(state1.position!=total_station*distance-1){
                    state1.position++;
                }
                else
                state1.position=0;
            }
            else if(dic1==-1){
                if(state1.position!=0){
                    state1.position--;
                }
                else
                state1.position=total_station*distance-1;
            }
            break;
        }
        if(request2[i]>1000&&request2[i]<10000){//是clockwise
                x=(request2[i]-1001)*distance;
                int dic2=direction(x,total_station,distance);
                if(dic2==0){
                    int y=(request2[i]%100)-(request2[i+1]%100);
                    if(y!=0){
                    state1.clockwise[request2[i]-1001]=0;
                    request2[i]=0;
                    break;
                    }
                    else if(y==0){
                        while(((request2[i]%100)-(request2[i+1]%100))==0){
                            if(request2[i]>1000&&request2[i]<10000){
                                state1.clockwise[request2[i]-1001]=0;
                            }
                            else if(request2[i]>10000){
                                state1.counterclockwise[request2[i]-10001]=0;
                            }
                            else if(request2[i]>100&&request2[i]<1000){
                                state1.target[request2[i]-101]=0;
                            }
                            if(request2[i+1]>1000&&request2[i+1]<10000){
                                state1.clockwise[request2[i+1]-1001]=0;
                            }
                            else if(request2[i+1]>10000){
                                state1.counterclockwise[request2[i+1]-10001]=0;
                            }
                            else if(request2[i+1]>100&&request2[i+1]<1000){
                                state1.target[request2[i+1]-101]=0;
                        }
                        i++;
                    }
                    int t=i;
                    int z;
                    for(z=0;z<=t;z++){
                        request2[z]=0;
                    }
                }
            }
            else if(dic2==1){
                if(state1.position!=total_station*distance-1){
                    state1.position++;
                }
                else
                state1.position=0;
            }
            else if(dic2==-1){
                if(state1.position!=0){
                    state1.position--;
                }
                else
                state1.position=total_station*distance-1;
            }
            break;
        }
        if(request2[i]>10000){//是counterclockwise
                x=(request2[i]-10001)*distance;
                int dic3=direction(x,total_station,distance);
                if(dic3==0){
                    int y=(request2[i]%100)-(request2[i+1]%100);
                    if(y!=0){
                    state1.counterclockwise[request2[i]-10001]=0;
                    request2[i]=0;
                    break;
                    }
                    else if(y==0){
                        while(((request2[i]%100)-(request2[i+1]%100))==0){
                            if(request2[i]>1000&&request2[i]<10000){
                                state1.clockwise[request2[i]-1001]=0;
                            }
                            else if(request2[i]>10000){
                                state1.counterclockwise[request2[i]-10001]=0;
                            }
                            else if(request2[i]>100&&request2[i]<1000){
                                state1.target[request2[i]-101]=0;
                            }
                            if(request2[i+1]>1000&&request2[i+1]<10000){
                                state1.clockwise[request2[i+1]-1001]=0;
                            }
                            else if(request2[i+1]>10000){
                                state1.counterclockwise[request2[i+1]-10001]=0;
                            }
                            else if(request2[i+1]>100&&request2[i+1]<1000){
                                state1.target[request2[i+1]-101]=0;
                        }
                        i++;
                    }
                    int t=i;
                    int z;
                    for(z=0;z<=t;z++){
                        request2[z]=0;
                    }
                }
            }
            else if(dic3==1){
                if(state1.position!=total_station*distance-1){
                    state1.position++;
                }
                else
                state1.position=0;
            }
            else if(dic3==-1){
                if(state1.position!=0){
                    state1.position--;
                }
                else
                state1.position=total_station*distance-1;
            }
            break;
        }
    }
    else if(request2[i]==total_station+1){
        break;
    }
}
}
int direction(int x,int total_station,int distance){
    int o;
    if(((total_station*distance)%2)==0){
        int num=state1.position;//bus所在位置 
        int num1=num-x;
        if(num1>0){
            if(num1<(total_station*distance/2)){
                o=-1; 
            }
            else
            o=1;
        }
        else if(num1==0)
        o=0;
        else if(num1<0){
             int num2=-num1;
             if(num2<=(total_station*distance/2)){
                o=1;
             }
             else
             o=-1;
            
        }
    }
    else if(((total_station*distance)%2)!=0){
        int num=state1.position;//bus所在位置 
        int num1=num-x;
        if(num1>0){
            if(num1<=(((total_station*distance)-1)/2)){
                o=-1; 
            }
            else
            o=1;
        }
        else if(num1==0)
        o=0;
        else if(num1<0){
             int num2=-num1;
             if(num2<=(((total_station*distance)-1)/2)){
                o=1;
             }
             else
             o=-1;
            
    }
}
    return o;
}
int confirm(int i){
    int x=1;
    int a;
    for(a=0;a<1000;a++){
    
        if(request2[a]==total_station+1){
            x=1;
            break;}
        else if(i==request2[a]){
        
        x=0;
        break;}
        
    }

    return x;
    }
void manner2(int total_station,int distance)
{
    int i, j, order, total_position = total_station * distance;
    int bus_state = 0;             //0表示空闲或服务完上个请求，1表示调度策略确定后开始行驶或正在行驶，3表示顺便停靠,10是一个特殊的返回值，避免顺便停靠的结果被manner2_update覆盖掉
    int command[3][total_station]; //请求数组，待满足的请求，0没有或已满足，1候选，2正在满足,行标：0 target，1 clockwise，2 counterclockwise
    int(*ptr)[total_station], *ptr_state;
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
    output();
    order = input();
    while (order != 0)
    {
        while (order > 100)
        {
            if (order > 100 && order <= 1000) //将请求存到请求数组中并更新state1的值
            {
                order = order % 100 - 1;
                if (command[0][order] == 0)
                    command[0][order] = 1;
            }
            else if (order > 1000 & order <= 10000)
            {
                order = order % 1000 - 1;
                if (command[1][order] == 0)
                    command[1][order] = 1;
            }
            else if (order > 10000)
            {
                order = order % 10000 - 1;
                if (command[2][order] == 0)
                    command[2][order] = 1;
            }
            for (i = 0; i < 3; i++)
                for (j = 0; j < total_station; j++)
                {
                    if (i == 0)
                        if (command[i][j] != 0)
                            state1.target[j] = 1;
                    if (i == 1)
                        if (command[i][j] != 0)
                            state1.clockwise[j] = 1;
                    if (i == 2)
                        if (command[i][j] != 0)
                            state1.counterclockwise[j] = 1;
                }
            order = input();
        }
        while (order == 1)
        {
            if (bus_state == 10 || bus_state == 11)
                bus_state = 1;
            if (bus_state == 13)
                bus_state = 3;
            if (bus_state == 0 || bus_state == 3)
                bus_state = manner2_pause(ptr, &bus_state);
            if (bus_state == 1)
                bus_state = manner2_running(ptr);
            if (bus_state == 0 || bus_state == 1 || bus_state == 3)
                manner2_update(ptr, &bus_state);
            state1.time++;
            output();
            order = input();
        }
    }
    printf("end\n");
}
void manner2_update(int (*ptr)[total_station], int *bus_state)
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
int manner2_running(int (*ptr)[total_station])
{
    int i, j;
    int needed_position; //需要到达的position
    int next_station;    //下一个站点
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
int manner2_pause(int (*ptr)[total_station], int *bus_state)
{
    int i, j, now_position, next_station;
    now_position = state1.position / distance;
    if (*bus_state == 0)
    {
        for (i = 0; i < 3; i++) //避免compare.distance=0的情况
            if (ptr[i][now_position] != 0)
            {
                ptr[i][now_position] = 0;
                if(i==0)
                state1.target[now_position]=0;
                else if(i==1)
                state1.clockwise[now_position]=0;
                else
                state1.counterclockwise[now_position]=0;
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
int manner2_confirm(int (*ptr)[total_station])
{
    typedef struct confirm //行标：0 target，1 clockwise，2 counterclockwise
    {
        int distance[3];  //对应路程
        int station[3];   //对应站台
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
void run(int get,int*request)//根据读入数据改变状态
{
    if (get == 1)
    {
        state1.time++;
        if (bus_direction == 1)
        {
            if(state1.position == total_station * distance - 1)
                state1.position = 0;
            else
                state1.position++; 
        }
        else if (bus_direction == -1)
        {
            if(state1.position == 0)
                state1.position += total_station * distance - 1;
            else
                state1.position--;
        }
    }
    else if (get > 100 && get < 1000)
    {
        //if(!((get%100-1)*distance==state1.position&&bus_direction==0))
            state1.target[get % 100 - 1] = 1;
        request[get % 100 - 1]=1;
    }
    else if (get > 1000 && get < 10000)
    {
        //if(!((get%100-1)*distance==state1.position&&bus_direction==0))
            state1.clockwise[get % 1000 - 1] = 1;
        request[get % 1000 - 1]=1;
    }
    else if (get > 10000)
    {
        //if(!((get%100-1)*distance==state1.position&&bus_direction==0))
            state1.counterclockwise[get % 10000 - 1] = 1;
        request[get % 10000 - 1]=1;
    }
}
int if_request(int*request)//判断是否有请求 是1 否0
{
    int*p=request;
    for(int i=1;i<=total_station;p++,i++)
    {
        if(*p==1)
        return 1;
    }
    return 0;
}
int for_distance(int n)//算距离（绝对值）
{
    if(n>0)
    return n;
    else
    return -n;
}
int judge_direction(int*request)//根据请求所在站点判断接下来运行方向
{
    int*p=request;
    int sum=0;//总的有请求的站点数
    int nearest;//最近的有请求的站点
    int least_distance=100;//最近站点的距离
    for(int i=1;i<=total_station;p++,i++)
    {
        if(*p==1)//若站点有请求
        {
            sum++;
            if(for_distance(state1.position-(i-1)*distance)<least_distance)//若该站点是最近的请求站点
            {
                least_distance=for_distance(state1.position-(i-1)*distance);
                nearest=i;
            }
        }
    }
    if(sum==0)
    return 0;
    else if((nearest-1)*distance-state1.position==0)
        return 0;
    else if((nearest-1)*distance-state1.position>0)
    {
        if((nearest-1)*distance-state1.position<=total_station*distance/2)
            return 1;
        else if((nearest-1)*distance-state1.position>total_station*distance/2)
            return -1;
    }
    else if((nearest-1)*distance-state1.position<0)
    {
        if((nearest-1)*distance-state1.position<=-total_station*distance/2)
            return 1;
        else if((nearest-1)*distance-state1.position>-total_station*distance/2)
            return -1;
    }
    else 
        return 0;
}
int find_target(int*request)//找目标站点
{
    int*p=request;
    int sum=0;//总的有请求的站点数
    int nearest;//最近的有请求的站点
    int least_distance=100;//最近站点的距离
    int n=0;//检测有无目标站点
    for(int i=1;i<=total_station;p++,i++)
    {
        if(*p==1)//若站点有请求
        {
            sum++;
            if(for_distance(state1.position-(i-1)*distance)<least_distance)//若该站点是最近的请求站点
            {
                least_distance=for_distance(state1.position-(i-1)*distance);
                nearest=i;
                n++;
            }
        }
    }
    if(n!=0)
        return nearest;
    else
        return state1.position/distance+1;
}
int dispatch(int*request,int get,int*ptrstop,int targer_station,int original_direction)//调度 请求数组 指令类型 停站相关 目标站点
{//返回当前方向
    if(bus_direction==0&&*ptrstop==0)//若不动且没有在站点停过
    {
        if(if_request(request)==1)//若有请求
        {
            bus_direction=judge_direction(request);
        }
    }
    else//在行驶
    {
        if(state1.position%distance==0)//到站点
        {
            if(request[state1.position/distance]==1)//在该站点有请求
            {
                if(*ptrstop==1)//上一秒在站点停了
                {               
                    if(state1.position/distance+1!=get%100||get==1)  
                    {                             
                        state1.clockwise[state1.position/distance]=0;
                        state1.counterclockwise[state1.position/distance]=0;
                        state1.target[state1.position/distance]=0;
                        request[state1.position/distance]=0;
                    }
                    if(/*if_request(request)==1&&get==1&&*/state1.position/distance+1==targer_station)
                        bus_direction=judge_direction(request);
                    else
                        bus_direction=original_direction;
                    *ptrstop=0;
                }
                else
                {
                    *ptrstop=1;
                    bus_direction=0;
                }
            }
            else if(if_request(request)==1)//在该站点没有请求但是在其他站点有请求
            {
                bus_direction=judge_direction(request);
            }
        }
    }
    return bus_direction;
}
void SCAN(void)
{
    state1.time = 0;
    state1.position = 0;
    int request[10]={0};
    int get = 0;
    int stop=0;//是否已经在站点停过 是1 否0
    int*ptrstop=&stop;
    int original_direction=0;//原本的方向
    int target_station=1;//目标站点
    bus_direction=0;
    for (int i = 1; i <= 10; i++)
    {
        state1.target[i - 1] = 0;
        state1.clockwise[i - 1] = 0;
        state1.counterclockwise[i - 1] = 0;
    }
    output();
    for (;;)
    {
        get = input();
        if(get!=0)
            run(get,request);
        else
        {
            printf("end\n");
            break;
        }
        if(bus_direction==0)
            original_direction=dispatch(request,get,ptrstop,target_station,original_direction);
        else
            dispatch(request,get,ptrstop,target_station,original_direction);
        if(state1.position/distance+1==target_station)
            target_station=find_target(request);
        if(get==1)
        output();
    }
}


