#include "tool.h"
#include <math.h>
#define pi 3.1415926535898
#define eps 1e-8

int calTempInt(int integer)
{
    int binaryArray[8]={0};
    int contempNum=integer;
    int i=7;
    int sum=0;
    while(contempNum!=0&&i>=0)//将整数转换成二进制
    {
        binaryArray[i]=contempNum%2;
        contempNum=contempNum/2;
        i--;
    }
    //求反码
    for(i=7;i>=1;i--)
    {
        if(binaryArray[i]==0)
        {
            binaryArray[i]=1;
        }
        else
        {
            binaryArray[i]=0;
        }
    }

    //直接进行补码运算
    int flag=1;
    i=7;
    while(flag==1&&i>=0)
    {
        if(binaryArray[i]==0)
        {
            binaryArray[i]=1;
            flag=0;
        }
        else
        {
            binaryArray[i]=0;
            i--;
        }
    }

    //将二进制转换成十进制
    for(i=7;i>=1;i--)
        sum+=binaryArray[i]*((int)pow(2,7-i));

    return sum;
}

/*
        原始数据转换公式
*/
double exCrackdata_v1(double x)//裂缝1
{
    //return(-0.050795*x + 31.6219);
    double y;
    y=4.175*exp(-((x-503.2)/32.53)*((x-503.2)/32.53))+1.879*exp(-((x-567.4)/14.32)*((x-567.4)/14.32))+2.096*exp(-((x-542.7)/19.18)*((x-542.7)/19.18));
    return y;
}

double exCrackdata_v2(double x)//裂缝2
{
    //return(0.060677*x-34.6753);
   double y=0.364*exp(-((x-615.4)/3.212)*((x-615.4)/3.212))+391*exp(-((x-1045)/188.4)*((x-1045)/188.4));
   return y;
}

double exVoltage(int x)//电压转换
{
    return 1252.352/x;
}
double exPower(int x) //新节点电压转换
{
    return x*3.3/pow(2,15);
}

/*double exLight(int xv, int yl)//参数：转换前的电压，光照强度
{
    return(yl * (1252352/xv) / 1023);
}*/

//double exLight(int yl)//参数：转换前的电压，光照强度
//{
//    return(yl *3.7/ 1023);
//}

double exLight(int xv, int yl)//参数：转换前的电压，光照强度
{
    return(yl * (1252352/xv) / 1023);
}

float exTemperature(short int tem)//温度转换
{
    float t_f;
    short int tem1;
        tem1=tem;
        t_f=tem1*0.01-39.66;
    return t_f;
}

float exHumidity(short int hum,float tem)//湿度转换公式
{
    float C1=-4.0,C2=0.0405,C3=-0.0000028,T1=0.01,T2=0.00008;
    short int hum1=hum;
    float rh_lin=C3*hum1*hum1 + C2*hum1 + C1;
    float rh_true=(tem-25)*(T1+T2*hum1)+rh_lin;
    if(rh_true>100)rh_true=100;
    if(rh_true<0.1)rh_true=0.1;
    return rh_true;
}

int  switchNum(char ch)//十六进制转换成十进制
{
    if(ch>='0'&&ch<='9')
        return ch-'0';
    else if(ch=='a'||ch=='A')
        return 10;
    else if(ch=='b'||ch=='B')
        return 11;
    else if(ch=='c'||ch=='C')
        return 12;
    else if(ch=='d'||ch=='D')
        return 13;
    else if(ch=='e'||ch=='E')
        return 14;
    else
        return 15;
}

void unlockPocket_hop(char buffer[SIZE],int datasize,int &nodeID,int &hopNum,int hop[5],int &PacketNum)//解析控制包
{

    hopNum=0;//记录跳步数，最多为5
    int hopValue;//记录每一跳的值
    int j;
    int i;
    for(i=0;i<5;i++)
    {
        hop[i]=65535;
    }
    nodeID=0;
    PacketNum=0;
    for(j=0;j<4;j++)
    {
        nodeID=nodeID*16+switchNum(buffer[20+j]);//
    }
    for(j=0;j<4;j++)
    {
        hopNum=hopNum*16+switchNum(buffer[40+j]);
    }
    if(hopNum>5)//多跳程序崩溃原因
        hopNum=5;
    for(i=0;i<hopNum;i++)
    {
        hopValue=0;
        for(j=0;j<4;j++)
        {
            hopValue=hopValue*16+switchNum(buffer[44+i*4+j]);
        }
        hop[i]=hopValue;
    }

    for(j=0;j<4;j++)
    {
        PacketNum=PacketNum*16+switchNum(buffer[28+j]);
    }
}
//解析温湿度光照数据包
void unlockPocket_TemHumLight(char buffer[SIZE],int datasize,int &nodeID,double &fVoltage,double &fLight,double &fTemperature, double &fHumidity)//解析数据包
{
    unsigned short int	node_id=0;		//节点id
    double  voltage=0;                  //电压原始
    double  light=0;                    //光照原始
    double  temperature=0;              //温度原始
    double  humidity=0;                 //湿度原始
    //double  crackdata=0;
    int j;
    int sizeMinus=0;                     //数据包位置锁定 0表示原数据包 28表示新数据包
    if(datasize==52)
    {
        sizeMinus=28;
        for(j=0;j<4;j++)//节点ID
        {
            node_id=node_id*16+switchNum(buffer[4+j]);
        }
            voltage=0;
    }
    else if(datasize==46*2)
    {
        for(j=0;j<4;j++)//节点ID
        {
            node_id=node_id*16+switchNum(buffer[20+j]);
        }
        for(j=0;j<4;j++)//电压
        {
            voltage=voltage*16+switchNum(buffer[40+j]);
        }
    }
    for(j=0;j<4;j++)//光照
    {
        light=light*16+switchNum(buffer[44-sizeMinus+j]);
    }
    for(j=0;j<4;j++)//
    {
        temperature=temperature*16+switchNum(buffer[48-sizeMinus+j]);
    }
    for(j=0;j<4;j++)//湿度
    {
        humidity=humidity*16+switchNum(buffer[52-sizeMinus+j]);
    }
    nodeID=node_id;
    if(voltage==0)
    {
        fVoltage=0;
        fLight=0;
        fTemperature=0;
        fHumidity=0;
    }
    else
    {
    fVoltage	=exVoltage(voltage);
    fLight		=exLight(voltage,light);
    fTemperature=exTemperature(temperature);
    fHumidity	= exHumidity(humidity,fTemperature);
    }
}
//解析雨量酸雨数据包
void unlockPocket_rain(char buffer[SIZE],int datasize,int &nodeID,int &rainnode_addr,double &tem_outside,double &tem_inside,double &ph,double &rainfall,double &windspeed,double &winddirection,double &powervoltage,int &rain,double &fVoltage,double &voltagePre,int &emptyTimes)
{
        nodeID=0;
        rainnode_addr=tem_outside=tem_inside=ph=rainfall=windspeed=winddirection=powervoltage=0;
        rain=0;
        int voltage=0;
        int integer;
        int decimals;
        int j;
        double rainVoltage;
        int flag;//标志位，为1表示温度是负数，为0表示温度是整数
        int sizeMinus=0;
        //数据包位置锁定 0表示原数据包 28表示新数据包
        rainVoltage=0;
        if(datasize==52)
        {
            sizeMinus=28;
            for(j=0;j<4;j++)//节点ID
            {
                nodeID=nodeID*16+switchNum(buffer[4+j]);
            }
                voltage=0;
        }
        else if(datasize==46*2)
        {
            for(j=0;j<4;j++)//节点ID
            {
                nodeID=nodeID*16+switchNum(buffer[20+j]);
            }
        }
        //解析传感器地址
        for(int i=0;i<2;i++)
        {
            rainnode_addr=rainnode_addr*16+switchNum(buffer[40-sizeMinus+i]);
        }
        //解析外部温度，整数和小数部分
        int digits;
        int divisor[4]={0,10,100,1000};
        integer=decimals=0;
        flag=0;
        for(int i=0;i<2;i++)
        {
            integer=integer*16+switchNum(buffer[42-sizeMinus+i]);//最高位1表示负值
        }
        if(integer>=128)//表示最高位为1
        {
            integer=calTempInt(integer);//
            flag=1;
        }

        for(int i=0;i<2;i++)
        {
            decimals=decimals*16+switchNum(buffer[44-sizeMinus+i]);//小数部分最大值为255(<300)
        }
        digits=(decimals>=0)+(decimals>=10)+(decimals>=100);
        tem_outside=(double)integer+(double)(decimals/divisor[digits]);//整数部分加小数部分才是完整的温度
        if(flag==1)
        {
            tem_outside=(double)(-1)*tem_outside;
        }
        //解析内部温度，整数和小数部分
        integer=decimals=0;
        flag=0;
        for(int i=0;i<2;i++)
        {
            integer=integer*16+switchNum(buffer[46-sizeMinus+i]);//最高位1表示负值
        }

        if(integer>=128)//表示最高位为1
        {
            integer=calTempInt(integer);//进行取补码运算
            flag=1;
        }

        for(int i=0;i<2;i++)
        {
            decimals=decimals*16+switchNum(buffer[48-sizeMinus+i]);//小数部分最大值为255(<300)
        }
        digits=(decimals>=0)+(decimals>=10)+(decimals>=100);
        tem_inside=(double)integer+(double)decimals/divisor[digits];//整数部分加小数部分才是完整的温度
        if(flag==1)
        {
            tem_inside=(double)(-1)*tem_inside;
        }
        //解析ph值
        integer=decimals=0;
        for(int i=0;i<2;i++)
        {
            integer=integer*16+switchNum(buffer[50-sizeMinus+i]);
        }
        for(int i=0;i<2;i++)
        {
            decimals=decimals*16+switchNum(buffer[52-sizeMinus+i]);//小数部分最大值为255(<300)
        }
        digits=(decimals>=0)+(decimals>=10)+(decimals>=100);
        ph=(double)integer+(double)decimals/divisor[digits];//整数部分加小数部分才是完整的温度

        //解析风速
        integer=decimals=0;
        for(int i=0;i<2;i++)
        {
            integer=integer*16+switchNum(buffer[58-sizeMinus+i]);
        }
        for(int i=0;i<2;i++)
        {
            decimals=decimals*16+switchNum(buffer[60-sizeMinus+i]);//小数部分最大值为255(<300)
        }
        digits=(decimals>=0)+(decimals>=10)+(decimals>=100);
        windspeed=(double)integer+(double)decimals/divisor[digits];//整数部分加小数部分才是完整的温度
        //解析风向
        integer=decimals=0;
        for(int i=0;i<2;i++)
        {
            integer=integer*16+switchNum(buffer[62-sizeMinus+i]);
        }
        for(int i=0;i<2;i++)
        {
            decimals=decimals*16+switchNum(buffer[64-sizeMinus+i]);//小数部分最大值为255(<300)
        }
        digits=(decimals>=0)+(decimals>=10)+(decimals>=100);
        winddirection=(double)integer+(double)decimals/divisor[digits];//整数部分加小数部分才是完整的温度
        //解析电压
        integer=decimals=0;
        for(int i=0;i<2;i++)
        {
            integer=integer*16+switchNum(buffer[66-sizeMinus+i]);
        }
        for(int i=0;i<2;i++)
        {
            decimals=decimals*16+switchNum(buffer[68-sizeMinus+i]);//小数部分最大值为255(<300)
        }
        digits=(decimals>=0)+(decimals>=10)+(decimals>=100);
        powervoltage=(double)integer+(double)decimals/divisor[digits];//整数部分加小数部分才是完整的温度
        //解析雨量
        integer=decimals=0;
        for(int i=0;i<2;i++)
        {
            integer=integer*16+switchNum(buffer[54-sizeMinus+i]);
        }
        for(int i=0;i<2;i++)
        {
            decimals=decimals*16+switchNum(buffer[56-sizeMinus+i]);
        }
        digits=(decimals>=0)+(decimals>=10)+(decimals>=100);
        rainVoltage=(double)integer+(double)decimals/divisor[digits];
        if(rainVoltage-voltagePre>=eps)
        {
            rainfall=4.52*(rainVoltage-voltagePre);
        }
        else if(rainVoltage-voltagePre<eps&&(fabs(voltagePre-rainVoltage)>0.1+eps))
            {
                rainfall=(rainVoltage-voltagePre+1.87)*4.52;
                emptyTimes++;
            }
        else
            rainfall=0;

        //解析是否降雨
        for(int i=0;i<2;i++)
        {
            rain=rain*16+switchNum(buffer[70-sizeMinus+i]);
        }
        if(sizeMinus==0)
        {
            for(int j=0;j<4;j++)//电压解析
            {
                voltage=voltage*16+switchNum(buffer[78+j]);
            }
            if(voltage==0)
            {
                fVoltage=0;
        }
        else
        {
            fVoltage	=exVoltage(voltage);
        }
        }
        else fVoltage=0;
        voltagePre=rainVoltage;
}
void unlockPocket_rainTest(char buffer[SIZE],int datasize,int &nodeID,int &rainnode_addr,int &tem_outside_int,int &tem_outside_dec,
                           int &tem_inside_int,int &tem_inside_dec,int &phInt,int &phDec,int &rainInt,int &rainDec,int &windspeedInt,
                           int &windspeedDec,int &winddirectionInt,int &winddirectionDec,int &powerInt,int &powerDec,int &rainFall, int &fVoltage)
{
    nodeID=0;
    rainnode_addr=tem_outside_int=tem_outside_dec=tem_inside_int=tem_inside_dec
            =phInt=phDec=rainInt=rainDec=windspeedInt=windspeedDec=winddirectionInt
            =winddirectionDec=powerInt=powerDec=rainFall=fVoltage=0;
    int voltage=0;
    int j;
    int flag;//标志位，为1表示温度是负数，为0表示温度是整数
    int sizeMinus=0;                     //数据包位置锁定 0表示原数据包 28表示新数据包
    if(datasize==52)
    {
        sizeMinus=28;
        for(j=0;j<4;j++)//节点ID
        {
            nodeID=nodeID*16+switchNum(buffer[4+j]);
        }
            voltage=0;
    }
    else if(datasize==46*2)
    {
        for(j=0;j<4;j++)//节点ID
        {
            nodeID=nodeID*16+switchNum(buffer[20+j]);
        }
    }
    //解析传感器地址
    for(int i=0;i<2;i++)
    {
        rainnode_addr=rainnode_addr*16+switchNum(buffer[40-sizeMinus+i]);
    }
    //外部温度
    for(int i=0;i<2;i++)
    {
        tem_outside_int=tem_outside_int*16+switchNum(buffer[42-sizeMinus+i]);//最高位1表示负值
    }
    for(int i=0;i<2;i++)
    {
        tem_outside_dec=tem_outside_dec*16+switchNum(buffer[44-sizeMinus+i]);//小数部分最大值为255(<300)
    }
    //内部温度
    for(int i=0;i<2;i++)
    {
        tem_inside_int= tem_inside_int*16+switchNum(buffer[46-sizeMinus+i]);//最高位1表示负值
    }
    for(int i=0;i<2;i++)
    {
        tem_inside_dec=tem_inside_dec*16+switchNum(buffer[48-sizeMinus+i]);//小数部分最大值为255(<300)
    }
    //ph
    for(int i=0;i<2;i++)
    {
        phInt=phInt*16+switchNum(buffer[50-sizeMinus+i]);
    }
    for(int i=0;i<2;i++)
    {
        phDec=phDec*16+switchNum(buffer[52-sizeMinus+i]);//小数部分最大值为255(<300)
    }
    //降雨量
    for(int i=0;i<2;i++)
    {
        rainInt=rainInt*16+switchNum(buffer[54-sizeMinus+i]);
    }
    for(int i=0;i<2;i++)
    {
        rainDec=rainDec*16+switchNum(buffer[56-sizeMinus+i]);//小数部分最大值为255(<300)
    }
    //风速
    for(int i=0;i<2;i++)
    {
        windspeedInt=windspeedInt*16+switchNum(buffer[58-sizeMinus+i]);
    }
    for(int i=0;i<2;i++)
    {
        windspeedDec=windspeedDec*16+switchNum(buffer[60-sizeMinus+i]);//小数部分最大值为255(<300)
    }
    //风向
    for(int i=0;i<2;i++)
    {
        winddirectionInt=winddirectionInt*16+switchNum(buffer[62-sizeMinus+i]);
    }
    for(int i=0;i<2;i++)
    {
        winddirectionDec=winddirectionDec*16+switchNum(buffer[64-sizeMinus+i]);//小数部分最大值为255(<300)
    }
    //电压
    for(int i=0;i<2;i++)
    {
        powerInt= powerInt*16+switchNum(buffer[66-sizeMinus+i]);
    }
    for(int i=0;i<2;i++)
    {
        powerDec=powerDec*16+switchNum(buffer[68-sizeMinus+i]);//小数部分最大值为255(<300)
    }
    //是否降雨
    for(int i=0;i<2;i++)
    {
        rainFall=rainFall*16+switchNum(buffer[70-sizeMinus+i]);
    }
}

//解析土壤温湿度传感器
void unlockPocket_sioltemhum(char buffer[SIZE],int &nodeID,int &addrCode1,int &addrCode2,double &tem_30cm,double &hum_30cm,double &tem_15cm,double &hum_15cm,double &tem_5cm,double &hum_5cm,double &fVoltage,double parameters[150][14],int count)
{
    double integer=0;
    double hum1=0;
    double hum2=0;
    double decimals=0;
    int voltage=0;
    nodeID=addrCode1=addrCode2=0;
    double dounum=0;
    tem_30cm=hum_30cm=tem_15cm=hum_15cm=tem_5cm=hum_5cm=0;
    double A30,K30,D30,M30,A15,K15,D15,M15,A5,K5,D5,M5;
    int flag=-1;
    A30=D30=M30=A15=D15=M15=A5=D5=M5=0;
    K30=K15=K5=1;


    int sizeMinus=28;
    //解析节点id
    for(int i=0;i<4;i++)
    {
        nodeID=nodeID*16+switchNum(buffer[4+i]);
    }
    //解析地址码
    for(int i=0;i<2;i++)
    {
        addrCode1=addrCode1*16+switchNum(buffer[40-sizeMinus+i]);
    }
    for(int i=0;i<2;i++)
    {
        addrCode2=addrCode2*16+switchNum(buffer[42-sizeMinus+i]);
    }
    //根据addrCode2从本地表中获取A、K、D、M
    double addr2=(double)addrCode2;
    double addr1=(double)addrCode1;
    for(int i=0;i<count;i++)
    {
        if(fabs(parameters[i][1]-addr2)<=eps&&(fabs(parameters[i][0]-addr1)<=eps))
        {
            flag=i;
            break;
        }
    }
    if(flag!=-1)//没有在本地找到，则需从数据库找参数
    {
        A30=parameters[flag][2];
        K30=parameters[flag][3];
        D30=parameters[flag][4];
        M30=parameters[flag][5];
        A15=parameters[flag][6];
        K15=parameters[flag][7];
        D15=parameters[flag][8];
        M15=parameters[flag][9];
        A5=parameters[flag][10];
        K5=parameters[flag][11];
        D5=parameters[flag][12];
        M5=parameters[flag][13];
        if(fabs(K30)<=eps)
            K30=1;
        if(fabs(K15)<=eps)
            K15=1;
        if(fabs(K5)<=eps)
            K5=1;

    }

    //解析30cm深度温度
    int plusORminus=0;
    integer=decimals=0;
    for(int i=0;i<2;i++)
    {
        plusORminus=plusORminus*16+switchNum(buffer[44-sizeMinus+i]);
    }
    for(int i=0;i<3;i++)
    {
        integer=integer*10+(double)switchNum(buffer[46-sizeMinus+i]);
    }
    dounum=(double)switchNum(buffer[49-sizeMinus]);
    decimals=dounum/10;
    tem_30cm=integer+decimals;
    if(plusORminus==0)//十六进制00表示负温度，0F表示正温度
        tem_30cm=(-1)*tem_30cm;
    //解析30cm深度湿度
    hum1=hum2=0;
    for(int i=0;i<2;i++)
    {
        hum1=hum1*16+(double)switchNum(buffer[50-sizeMinus+i]);
    }
    for(int i=0;i<2;i++)
    {
        hum2=hum2*16+(double)switchNum(buffer[52-sizeMinus+i]);
    }

    hum_30cm=(A30-(hum1*256+hum2)/K30+D30)-tem_30cm*M30;


    //解析15cm深度温度
    integer=decimals=0;
    plusORminus=0;
    for(int i=0;i<2;i++)
    {
        plusORminus=plusORminus*16+switchNum(buffer[54-sizeMinus+i]);
    }
    for(int i=0;i<3;i++)
    {
        integer=integer*10+(double)switchNum(buffer[56-sizeMinus+i]);
    }
    dounum=(double)switchNum(buffer[59-sizeMinus]);
    decimals=dounum/10;
    tem_15cm=integer+decimals;
    if(plusORminus==0)//十六进制00表示负温度，0F表示正温度
        tem_15cm=(-1)*tem_15cm;
    //解析15cm深度湿度
    hum1=hum2=0;
    for(int i=0;i<2;i++)
    {
        hum1=hum1*16+(double)switchNum(buffer[60-sizeMinus+i]);
    }
    for(int i=0;i<2;i++)
    {
        hum2=hum2*16+(double)switchNum(buffer[62-sizeMinus+i]);
    }

    hum_15cm=(A15-(hum1*256+hum2)/K15+D15)-tem_15cm*M15;

    //解析5cm深度温度
    integer=decimals=0;
    plusORminus=0;
    for(int i=0;i<2;i++)
    {
        plusORminus=plusORminus*16+switchNum(buffer[64-sizeMinus+i]);
    }
    for(int i=0;i<3;i++)
    {
        integer=integer*10+switchNum(buffer[66-sizeMinus+i]);
    }
    dounum=(double)switchNum(buffer[69-sizeMinus]);
    decimals=dounum/10;
    tem_5cm=integer+decimals;
    if(plusORminus==0)//十六进制00表示负温度，0F表示正温度
        tem_5cm=(-1)*tem_5cm;
    //解析5cm深度湿度
    hum1=hum2=0;
    for(int i=0;i<2;i++)
    {
        hum1=hum1*16+(double)switchNum(buffer[70-sizeMinus+i]);
    }
    for(int i=0;i<2;i++)
    {
        hum2=hum2*16+(double)switchNum(buffer[72-sizeMinus+i]);
    }
    hum_5cm=(A5-(hum1*256+hum2)/K5+D5)-tem_5cm*M5;


    //电压解析
    for(int j=0;j<4;j++)
    {
        voltage=voltage*16+switchNum(buffer[78+j]);
    }
    if(voltage==0)
    {
        fVoltage=0;
    }
    else
    {
        fVoltage	=exVoltage(voltage);
    }
}

int unlockPocket_sioltemphumiTest(char buffer[SIZE],int datasize,int &addrCode1,int &addrCode2,int &symbol_30cm,int &temp1_30cm,int &temp2_30cm,int &humi1_30cm,int &humi2_30cm,int &symbol_15cm,int &temp1_15cm,int &temp2_15cm,int &humi1_15cm,int &humi2_15cm,int &symbol_5cm,int &temp1_5cm,int &temp2_5cm,int &humi1_5cm,int &humi2_5cm,int &voltagetest,int &nodeID)
{
    int j;
    addrCode1=addrCode2=symbol_30cm=temp1_30cm=temp2_30cm=humi1_30cm=humi2_30cm=symbol_15cm=temp1_15cm=temp2_15cm=humi1_15cm=humi2_15cm=symbol_5cm=temp1_5cm=temp2_5cm=humi1_5cm=humi2_5cm=voltagetest=nodeID=0;
//    tem_30cm=hum_30cm=tem_15cm=hum_15cm=tem_5cm=hum_5cm=0;
    int sizeMinus=0;                     //数据包位置锁定 0表示原数据包 28表示新数据包
    if(datasize==52)
    {
        sizeMinus=28;
        for(j=0;j<4;j++)//节点ID
        {
            nodeID=nodeID*16+switchNum(buffer[4+j]);
        }
           // voltage=0;
    }
    else if(datasize==46*2)
    {
        for(j=0;j<4;j++)//节点ID
        {
            nodeID=nodeID*16+switchNum(buffer[20+j]);
        }
    }
    //解析地址码
    for(int i=0;i<2;i++)
    {
        if(buffer[40-sizeMinus+i]>='A'&&buffer[40-sizeMinus+i]<='F')
        {
            addrCode1=0;
        }
        else
            addrCode1=addrCode1*16+switchNum(buffer[40-sizeMinus+i]);
    }
    for(int i=0;i<2;i++)
    {
        if(buffer[42-sizeMinus+i]>='A'&&buffer[42-sizeMinus+i]<='F')
        {
            addrCode2=0;
        }
        else
        addrCode2=addrCode2*16+switchNum(buffer[42-sizeMinus+i]);
    }
    //解析30cm深度温度
    for(int i=0;i<2;i++)
    {
        symbol_30cm=symbol_30cm*16+switchNum(buffer[44-sizeMinus+i]);
    }
    for(int i=0;i<2;i++)
    {
        if(buffer[46-sizeMinus+i]>='A'&&buffer[46-sizeMinus+i]<='F')
        {
            temp1_30cm=0;
        }
        else
            temp1_30cm=temp1_30cm*16+switchNum(buffer[46-sizeMinus+i]);
    }
    for(int i=0;i<2;i++)
    {
        if(buffer[48-sizeMinus+i]>='A'&&buffer[48-sizeMinus+i]<='F')
        {
            temp2_30cm=0;
        }
        else
          temp2_30cm=temp2_30cm*16+switchNum(buffer[48-sizeMinus+i]);
    }
    //解析30cm深度湿度
    for(int i=0;i<2;i++)
    {
        humi1_30cm=humi1_30cm*16+switchNum(buffer[50-sizeMinus+i]);
    }
    for(int i=0;i<2;i++)
    {
        humi2_30cm=humi2_30cm*16+switchNum(buffer[52-sizeMinus+i]);
    }
    //解析15cm深度温度
    for(int i=0;i<2;i++)
    {
        symbol_15cm=symbol_15cm*16+switchNum(buffer[54-sizeMinus+i]);
    }
    for(int i=0;i<2;i++)
    {
        if(buffer[56-sizeMinus+i]>='A'&&buffer[56-sizeMinus+i]<='F')
        {
            temp1_15cm=0;
        }
        else
           temp1_15cm=temp1_15cm*16+switchNum(buffer[56-sizeMinus+i]);
    }
    for(int i=0;i<2;i++)
    {
        if(buffer[58-sizeMinus+i]>='A'&&buffer[58-sizeMinus+i]<='F')
        {
            temp2_15cm=0;
        }
        else
          temp2_15cm=temp2_15cm*16+switchNum(buffer[58-sizeMinus+i]);
    }
    //解析15cm深度湿度
    for(int i=0;i<2;i++)
    {
        humi1_15cm=humi1_15cm*16+switchNum(buffer[60-sizeMinus+i]);
    }
    for(int i=0;i<2;i++)
    {
        humi2_15cm=humi2_15cm*16+switchNum(buffer[62-sizeMinus+i]);
    }
    //解析5cm深度温度
    for(int i=0;i<2;i++)
    {
        symbol_5cm=symbol_5cm*16+switchNum(buffer[64-sizeMinus+i]);
    }
    for(int i=0;i<2;i++)
    {
        if(buffer[66-sizeMinus+i]>='A'&&buffer[66-sizeMinus+i]<='F')
        {
            temp1_5cm=0;
        }
        else
           temp1_5cm=temp1_5cm*16+switchNum(buffer[66-sizeMinus+i]);
    }
    for(int i=0;i<2;i++)
    {
        if(buffer[68-sizeMinus+i]>='A'&&buffer[68-sizeMinus+i]<='F')
        {
            temp2_5cm=0;
        }
        else
           temp2_5cm=temp2_5cm*16+switchNum(buffer[68-sizeMinus+i]);
    }
    //解析5cm深度湿度
    for(int i=0;i<2;i++)
    {
        humi1_5cm=humi1_5cm*16+switchNum(buffer[70-sizeMinus+i]);
    }
    for(int i=0;i<2;i++)
    {
        humi2_5cm=humi2_5cm*16+switchNum(buffer[72-sizeMinus+i]);
    }
    if(sizeMinus==0)
    {
            for(int j=0;j<4;j++)//电压
            {
                voltagetest=voltagetest*16+switchNum(buffer[78+j]);
            }
    }
    else voltagetest=0;
    return 0;
}

//解析土壤盐分传感器
void unlockPocket_salt(char buffer[SIZE],int datasize,int &nodeID,int &saltaddrcode,double &tem,double &hum,double &verticalSalt,double &surfaceSalt,double &fVoltage,double saltpara[109][2],int count)
{
       double Di;
       int flag=0;
       double integer;
       double decimals;
       nodeID=saltaddrcode=0;
       int voltage=0;
       double v1,v2;//垂直电极和表明电极的电压值
       double ECa1,ECa2,ECa3;
       double ECa21,ECa22,ECa23;
       double dounum;
       int sizeMinus=28;

       //根据addrCode2从本地表中获取A、K、D、M


       //解析节点id
       for(int i=0;i<4;i++)
       {
           nodeID=nodeID*16+switchNum(buffer[4+i]);
       }
       //解析地址码
       for(int i=0;i<4;i++)
       {
           saltaddrcode=saltaddrcode*10+switchNum(buffer[40-sizeMinus+i]);
       }
       double addr2=(double)saltaddrcode;
       for(int i=0;i<count;i++)
       {
           if(fabs(saltpara[i][0]-addr2)<=eps)
           {
               flag=i;
               break;
           }
       }
       if(flag!=-1)//没有在本地找到，则需从数据库找参数
       {
           Di=saltpara[flag][1];


       }


       //解析通道1温度
       int plusORminus=0;
       integer=decimals=0;
       for(int i=0;i<2;i++)
       {
           plusORminus=plusORminus*16+switchNum(buffer[44-sizeMinus+i]);
       }
       for(int i=0;i<3;i++)
       {
           integer=integer*10+(double)switchNum(buffer[46-sizeMinus+i]);
       }
       dounum=(double)switchNum(buffer[49-sizeMinus]);
       decimals=dounum/10;
       tem=integer+decimals;
       if(plusORminus==0)//十六进制00表示负温度，0F表示正温度
           tem=(-1)*tem;
       //解析通道1深度湿度
       integer=0;
       for(int i=0;i<4;i++)
       {
           integer=integer*16+(double)switchNum(buffer[50-sizeMinus+i]);
       }
       hum=integer/3500+0;
       //解析垂直盐分
       integer=0;
       for(int i=0;i<4;i++)
       {
           integer=integer*16+(double)switchNum(buffer[60-sizeMinus+i]);
       }
       v1=integer/8;//电压值
       ECa1=0.4470+1.4034*exp((-1)*tem/26.815);
       ECa2=24.4*pow(10,-7)/(2*pi*6.5*0.001*(0.455-v1*5.5*0.0001/5.2));
       ECa3=2.36*hum*hum-0.36*hum+0.04;
       if(fabs(ECa3)<=eps)
           verticalSalt=0;
       else
       {
           verticalSalt=ECa1*ECa2/ECa3+Di;
       }

       //解析表明盐分
       integer=0;
       for(int i=0;i<4;i++)
       {
           integer=integer*16+(double)switchNum(buffer[70-sizeMinus+i]);
       }
       v2=integer/8;
       ECa21=0.4470+1.4034*exp((-1)*tem/26.815);
       ECa22=24.4*pow(10,-7)/(2*pi*6.5*0.001*(0.455-v2*5.5*0.0001/5.2));
       ECa23=2.36*hum*hum-0.36*hum+0.04;
       if(fabs(ECa23)<=eps)
           surfaceSalt=0;
       else
       {
           surfaceSalt=ECa21*ECa22/ECa23+Di+0.08;
       }

       //电压解析
       for(int j=0;j<4;j++)
       {
           voltage=voltage*16+switchNum(buffer[78+j]);
       }
       if(voltage==0)
       {
           fVoltage=0;
       }
       else
       {
           fVoltage=exVoltage(voltage);
       }


}

void unlockPocket_saltTest(char buffer[1024],int datasize,int &addrCode,int &symbol_30cm,int &temp_30cm,int &humi_30cm,int &vertSalt,int &surfSalt,int &voltagetest,int &nodeid)//盐分原始数据
{
    addrCode=symbol_30cm=temp_30cm=humi_30cm=vertSalt=surfSalt=nodeid=voltagetest=0;
    int j;
    int sizeMinus=0;                     //数据包位置锁定 0表示原数据包 28表示新数据包
    if(datasize==52)
    {
        sizeMinus=28;
        for(j=0;j<4;j++)//节点ID
        {
            nodeid=nodeid*16+switchNum(buffer[4+j]);
        }
            //voltage=0;
    }
    else if(datasize==46*2)
    {
        for(j=0;j<4;j++)//节点ID
        {
            nodeid=nodeid*16+switchNum(buffer[20+j]);
        }
    }
    //地址码
    for(int i=0;i<4;i++)
    {
        if(buffer[40-sizeMinus+i]>='A'&&buffer[40-sizeMinus+i]<='F')
        {
            addrCode=0;
        }
        else
            addrCode=addrCode*10+switchNum(buffer[40-sizeMinus+i]);
    }

     //30cm深度温度
     for(int i=0;i<2;i++)
     {
         symbol_30cm=symbol_30cm*16+switchNum(buffer[44-sizeMinus+i]);
     }
     for(int i=0;i<4;i++)
     {
         if(buffer[46-sizeMinus+i]>='A'&&buffer[46-sizeMinus+i]<='F')
         {
             temp_30cm=0;
         }
         else
            temp_30cm=temp_30cm*16+switchNum(buffer[46-sizeMinus+i]);
     }

     //30cm深度湿度
     for(int i=0;i<4;i++)
     {
         humi_30cm=humi_30cm*16+switchNum(buffer[50-sizeMinus+i]);
     }

     //垂直盐分
     for(int i=0;i<4;i++)
     {
         vertSalt=vertSalt*16+switchNum(buffer[60-sizeMinus+i]);
     }

     //表明盐分
     for(int i=0;i<4;i++)
     {
         surfSalt=surfSalt*16+switchNum(buffer[70-sizeMinus+i]);
     }
     if(sizeMinus==0)
     {
         for(int j=0;j<4;j++)//电压
         {
             voltagetest=voltagetest*16+switchNum(buffer[78+j]);
         }
     }
     else voltagetest=0;
}
double shakeFunction(int data)
{
    double x;
    x=data*3.3/8192;
    return x;
}
void unlockPocket_shake(char buffer[1024],int buffersize,int &nodeID,double &shake1,double &shake2,double &shake3,double &fVoltage)//解析震动
{
        unsigned short int	node_id=0;		//节点id
        int  data1=0;
        int  data2=0;
        int  data3=0;
        int  voltage=0;
        int j;
        int sizeMinus=0;                     //数据包位置锁定 0表示原数据包 28表示新数据包
        if(buffersize==52)
        {
            sizeMinus=28;
            for(j=0;j<4;j++)//节点ID
            {
                node_id=node_id*16+switchNum(buffer[4+j]);
            }
                voltage=0;
        }
        else if(buffersize==46*2)
        {
            for(j=0;j<4;j++)//节点ID
            {
                node_id=node_id*16+switchNum(buffer[20+j]);
            }
        }
        for(j=0;j<4;j++)
        {
            data1=data1*16+switchNum(buffer[42-sizeMinus+j]);
        }
        for(j=0;j<4;j++)
        {
            data2=data2*16+switchNum(buffer[46-sizeMinus+j]);
        }
        for(j=0;j<4;j++)
        {
            data3=data3*16+switchNum(buffer[50-sizeMinus+j]);
        }

        nodeID=node_id;
        shake1 	=shakeFunction(data1);
        shake2 	=shakeFunction(data2);
        shake3 	=shakeFunction(data3);

        if(sizeMinus==0)
        {
            for(j=0;j<4;j++)//电压解析
            {
                voltage=voltage*16+switchNum(buffer[78+j]);
            }
            if(voltage==0)
            {
                fVoltage=0;
            }
            else
            {
                fVoltage	=exVoltage(voltage);
            }
        }
        else fVoltage=0;
}

void unlockPocket_shakeTest(char buffer[1024],int datasize,int &shakeOne,int &shakeTwo,int &shakeThree,int &voltagetest,int &nodeid)
{
    shakeOne=shakeTwo=shakeThree=voltagetest=nodeid=0;
    int j=0;
    int sizeMinus=0;                     //数据包位置锁定 0表示原数据包 28表示新数据包
    if(datasize==52)
    {
        sizeMinus=28;
        for(j=0;j<4;j++)//节点ID
        {
            nodeid=nodeid*16+switchNum(buffer[4+j]);
        }
            //voltage=0;
    }
    else if(datasize==46*2)
    {
        for(j=0;j<4;j++)//节点ID
        {
            nodeid=nodeid*16+switchNum(buffer[20+j]);
        }
    }
    for(j=0;j<4;j++)
    {
        shakeOne=shakeOne*16+switchNum(buffer[44-sizeMinus+j]);
    }
    for(j=0;j<4;j++)
    {
        shakeTwo=shakeTwo*16+switchNum(buffer[48-sizeMinus+j]);
    }
    for(j=0;j<4;j++)
    {
        shakeThree=shakeThree*16+switchNum(buffer[52-sizeMinus+j]);
    }
    if(sizeMinus==0)
    {
        for(j=0;j<4;j++)//电压
        {
            voltagetest=voltagetest*16+switchNum(buffer[78+j]);
        }
    }
    else voltagetest=0;
}
//控制包
void unlockControlPac(char buffer[1024],int datasize,int &nodeID,int &countControl,double &fVoltage,double &RSSI,double &coorVoltage)
{
    nodeID=countControl=fVoltage=RSSI=coorVoltage=0;
    int sizeFlag=0;//大小标志位 0大数据包 1小数据包
    int nodeVoltage=0;
    int coVoltage=0;
    int rssi=0;
        //nodeid
        for(int k=0;k<4;k++)
        {
            nodeID=nodeID*16+switchNum(buffer[4+k]);

        }
        //count值
        for(int k=0;k<4;k++)
        {
            countControl=countControl*16+switchNum(buffer[10+k]);
        }
        //出现溢出重新解析并记录截取内容
        if(countControl>=65535)
        {
            countControl=0;
            int countArr[4];
            for(int k=0;k<4;k++)
            {
                countControl=countControl*16+switchNum(buffer[10+k]);
            }

        }
        for(int k=0;k<4;k++)
        {
            nodeVoltage=nodeVoltage*16+switchNum(buffer[14+k]);

        }
        for(int k=0;k<2;k++)
        {
           rssi=rssi*16+switchNum(buffer[18+k]);
        }
        for(int k=0;k<4;k++)
        {
            coVoltage=coVoltage*16+switchNum(buffer[20+k]);

        }
        RSSI=double((rssi)-256);
        fVoltage=double(exPower(nodeVoltage));
        coorVoltage=double(exPower(coVoltage));

}


void unlockPocket_airHumidity(char buffer[SIZE],int datasize,int &nodeID,double &tem,double &hum)//空气温湿度
{
    int integer;
    int decimals;
    int temFlag=0;                         //温度标记 0表示零上 1表示零下
    tem=hum=0;
    int j,voltage=0;
    int sizeMinus=0;                     //数据包位置锁定 0表示原数据包 28表示新数据包
    nodeID=0;

    QString nodeidError;
    nodeidError.clear();
    for(j=0;j<4;j++)//节点ID
    {
        nodeID=nodeID*16+switchNum(buffer[4+j]);

        nodeidError=QString::number(nodeID);

    }
    voltage=0;

    for(int i=0;i<2;i++)
    {
        temFlag=temFlag*16+switchNum(buffer[12+i]);
    }
    for(int i=0;i<4;i++)
    {
        tem=tem*16+switchNum(buffer[14+i]);
    }
    if(temFlag==0)
    {
        tem=tem*0.1;
    }
    else
    {
        tem=-(tem-32768)*0.1;
    }
    for(int i=0;i<4;i++)
    {
        hum=hum*16+switchNum(buffer[18+i]);
    }
    hum=hum*0.1;
}
























