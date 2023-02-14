# 导读  
完成公交车智能调度服务方案（先来先服务，最短寻找时间优先，顺便服务）的确定并控制公交车的运行。  
程序会先读取配置文件dict.dic来获取各站之间距离，站点个数，调度策略选择，之后再执行程序。  
**分为代码版与动画版两个部分**  
# 代码版    
## 说明  
环形轨道，一辆车，车辆可以双向任意行驶。  
我们规定车辆的原始位置为0（该位置也是车站1的位置），按顺时针方向每个单位位置坐标加1。如果轨道总长为10，则按顺时针方向走，位置9的下一个为位置0。  
车站编号同理，也是按顺时针方向依次递增。  
车速固定，每秒一个单位。停车接人或乘客下车时需要停车一秒钟。  
无论一次停站完成几个服务停留时间统一为1秒钟。各站之间距离相等，车辆经过站点时，根据调度策略，车辆可以停也可以不停。  
其他位置不允许停车。车辆只能在站点停站时才能改变行驶方向。  
## 配置  
各站之间距离可配置，站点个数可配置，调度策略可配置。这三个参数保存在配置文件中，程序要通过读配置文件获取。配置文件的名字为dict.dic。  
配置文件为文本文件，以#号开头的行是注释，井号只可能出现在每一行的开头。  
每行一个参数，格式为：'参数 = 值'的形式。  
每个参数前无空格，参数名、等号、参数值用空格分隔。  
其中参数有三个，即TOTAL_STATION，代表站点总数，为大于1且小于等于20的整数；DISTANCE，代表每站之间的距离，为大于0且小于6的整数；STRATEGY，代表调度策略，只能是FCFS（先来先服务），SSTF（最短寻找时间优先）和 SCAN（顺便服务）之一。  
如果某个参数没有出现在配置文件中，则该参数取缺省值。三个参数的缺省值如下：  
    TOTAL_STATION = 5  
    STRATEGY = FCFS  
    DISTANCE = 2  
## 输入格式  
若干行，每行一个指令。  
指令共5种。分别为end、clock、counterclockwise、clockwise 和target。其中end是程序退出指令。  
clock是时钟指令，每出现一次代表过了一秒钟  
counterclockwise、clockwise、target为请求指令，如果它们出现，同一行内后边一定有一个整数。  
如果是counterclockwise和clockwise，代表站台上的请求，后边的整数代表请求发生的站点号，counterclockwise表示逆时针方向乘车请求，clockwise代表顺时针方向乘车请求。  
如果是target，代表车厢内下车请求，后边的整数代表要去的站点号。  
## 输出格式
程序开始，先输出一次初始状态，然后每个clock输出一次当前状态；程序退出时输出end。格式如下：  
    TIME:秒数
    BUS:
    position:0
    target:0000000000
    STATION:
    clockwise:0000000000
    counterclockwise:0000000000
首先输出当前的时间，即已过的秒数。  
然后三行代表车辆，BUS:固定不变，position:固定不变，后边的数字代表当前车辆位置，target:固定不变，后边一排数字依次代表车内站点请求情况，0表示没有请求，1表示有请求。  
最后三行代表各站点的状态，STATION: 固定不变，clockwise: 固定不变，后边的数字依次代表各站点顺时针方向的请求情况，0表示没有请求，1表示有请求。  
counterclockwise: 固定不变，后边的数字依次代表各站点逆时针方向的请求情况，0表示没有请求，1表示有请求。  
## 代码
见bus_code.c
# 动画版
## 输入区  
T代表target命令，C代表clockwise命令，U代表counterclockwise命令，E代表end命令。  
点击相应按钮会弹出对话框输入站点。  
##  输出区  
输出区用于展示当前系统的状态。每隔一个时间单位(clock)刷新一次，clock可自行设置。  
红点表示站点，蓝点表示各个distance，绿点表示当前公交车所在的位置。  
## 代码  
见bus_graphy.cpp  

