#include "tool.h"
#include <math.h>

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
void unlockPocket_rain(char buffer[SIZE],int datasize,int &nodeID,int &rainnode_addr,double &tem_outside,double &tem_inside,double &ph,double &rainfall,double &windspeed,double &winddirection,double &powervoltage,int &rain,double &fVoltage)
{
        nodeID=0;
        rainnode_addr=tem_outside=tem_inside=ph=rainfall=windspeed=winddirection=powervoltage=0;
        rain=0;
        int voltage=0;
        int integer;
        int decimals;
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
        /*what if the decimals =0? will it be an error?*/
        tem_outside=(double)integer+(double)decimals/divisor[digits];//整数部分加小数部分才是完整的温度
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
        //解析雨量
        integer=decimals=0;
        for(int i=0;i<2;i++)
        {
            integer=integer*16+switchNum(buffer[54-sizeMinus+i]);
        }
        for(int i=0;i<2;i++)
        {
            decimals=decimals*16+switchNum(buffer[56-sizeMinus+i]);//小数部分最大值为255(<300)
        }
        digits=(decimals>=0)+(decimals>=10)+(decimals>=100);
        rainfall=(double)integer+(double)decimals/divisor[digits];//整数部分加小数部分才是完整的温度
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
}
//解析土壤温湿度传感器
void unlockPocket_sioltemhum(char buffer[SIZE],int datasize,int &nodeID,int &addressCode,double &tem_30cm,double &hum_30cm,double &tem_15cm,double &hum_15cm,double &tem_5cm,double &hum_5cm,double &fVoltage)
{
    int integer;
    int decimals;
    int voltage=0;
    nodeID=addressCode=0;
    int j;
    tem_30cm=hum_30cm=tem_15cm=hum_15cm=tem_5cm=hum_5cm=0;
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
    //解析地址码
    for(int i=0;i<4;i++)
    {
        addressCode=addressCode*10+switchNum(buffer[40-sizeMinus+i]);
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
        integer=integer*10+switchNum(buffer[46-sizeMinus+i]);
    }
    decimals=(double)switchNum(buffer[49-sizeMinus])/10;
    tem_30cm=integer+decimals;
    if(plusORminus==0)//十六进制00表示负温度，0F表示正温度
        tem_30cm=(-1)*tem_30cm;
    //解析30cm深度湿度
    integer=0;
    for(int i=0;i<4;i++)
    {
        integer=integer*16+switchNum(buffer[50-sizeMinus+i]);
    }
    //HADDR K = AADDR K ( XADDR K – ZADDR K )
    hum_30cm=2.7*(integer-0);//以后修改*********************************
    //解析15cm深度温度
    integer=decimals=0;
    plusORminus=0;
    for(int i=0;i<2;i++)
    {
        plusORminus=plusORminus*16+switchNum(buffer[54-sizeMinus+i]);
    }
    for(int i=0;i<3;i++)
    {
        integer=integer*10+switchNum(buffer[56-sizeMinus+i]);
    }
    decimals=(double)switchNum(buffer[59-sizeMinus])/10;
    tem_15cm=integer+decimals;
    if(plusORminus==0)//十六进制00表示负温度，0F表示正温度
        tem_15cm=(-1)*tem_15cm;
    //解析15cm深度湿度
    integer=0;
    for(int i=0;i<4;i++)
    {
        integer=integer*16+switchNum(buffer[60-sizeMinus+i]);
    }
    hum_15cm=2.7*(integer-0);//以后修改*********************************
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
    decimals=(double)switchNum(buffer[69-sizeMinus])/10;
    tem_5cm=integer+decimals;
    if(plusORminus==0)//十六进制00表示负温度，0F表示正温度
        tem_5cm=(-1)*tem_5cm;
    //解析5cm深度湿度
    integer=0;
    for(int i=0;i<4;i++)
    {
        integer=integer*16+switchNum(buffer[70-sizeMinus+i]);
    }
    hum_5cm=2.7*(integer-0);//以后修改*********************************

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
    else if (sizeMinus==28)
    {
        fVoltage=0;
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
        addrCode1=addrCode1*16+switchNum(buffer[40-sizeMinus+i]);
    }
    for(int i=0;i<2;i++)
    {
        addrCode2=addrCode2*16+switchNum(buffer[42-sizeMinus+i]);
    }
    //解析30cm深度温度
    for(int i=0;i<2;i++)
    {
        symbol_30cm=symbol_30cm*16+switchNum(buffer[44-sizeMinus+i]);
    }
    for(int i=0;i<2;i++)
    {
        temp1_30cm=temp1_30cm*16+switchNum(buffer[46-sizeMinus+i]);
    }
    for(int i=0;i<2;i++)
    {
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
        temp1_15cm=temp1_15cm*16+switchNum(buffer[56-sizeMinus+i]);
    }
    for(int i=0;i<2;i++)
    {
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
        temp1_5cm=temp1_5cm*16+switchNum(buffer[66-sizeMinus+i]);
    }
    for(int i=0;i<2;i++)
    {
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
void unlockPocket_salt(char buffer[SIZE],int datasize,int &nodeID,int &saltaddrcode,double &tem,double &hum,double &verticalSalt,double &surfaceSalt,double &fVoltage)
{
    int integer;
    int decimals;//有点问题？
    nodeID=saltaddrcode=0;
    int voltage=0;
    double v1,v2;//垂直电极和表明电极的电压值
    double ECa1,ECa10,ECw10;
    double ECa2,ECa20,ECw20;

    int j;
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
    //解析地址码
    for(int i=0;i<4;i++)
    {
        saltaddrcode=saltaddrcode*10+switchNum(buffer[40-sizeMinus+i]);
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
        integer=integer*10+switchNum(buffer[46-sizeMinus+i]);
    }
    decimals=switchNum(buffer[49-sizeMinus])/10;
    tem=integer+decimals;
    if(plusORminus==0)//十六进制00表示负温度，0F表示正温度
        tem=(-1)*tem;
    //解析通道1深度湿度
    integer=0;
    for(int i=0;i<4;i++)
    {
        integer=integer*16+switchNum(buffer[50-sizeMinus+i]);
    }
    //HADDR K = AADDR K ( XADDR K – ZADDR K )
   // hum=2.7*(integer-0);//以后修改*********************************
    hum=1.9-integer/((pow(2,12)-1)*5.2)*2.25;
    hum=hum/100;
    //解析垂直盐分
    integer=0;
    for(int i=0;i<4;i++)
    {
        integer=integer*16+switchNum(buffer[60-sizeMinus+i]);
    }
    v1=1000*(0.44381-integer/((pow(2,12)-1)*5.2)*2.25);

    ECa1=25000/(2*3.14*6.5*v1);
    ECa10=ECa1*(0.4470+1.4034*exp((-1)*tem/26.815));
    double fun1;
    fun1=(-2.2556)*hum*hum+1.9135*hum+0.0791;
    if(fabs(fun1-0)<=1e-8)
    {
        ECw10=0;
    }
    else
    {
        ECw10=ECa10/fun1;
    }
    verticalSalt=ECw10;

    //解析表明盐分
    integer=0;
    for(int i=0;i<4;i++)
    {
        integer=integer*16+switchNum(buffer[70-sizeMinus+i]);
    }

    v2=1000*(0.44381-integer/((pow(2,12)-1)*5.2)*2.25);
    ECa2=25000/(2*3.14*6.5*v2);//之前这里是v1
    ECa20=ECa2*(0.4470+1.4034*exp((-1)*tem/26.815));

    double fun2;
    fun2=(-1.4731)*hum*hum+1.4124*hum-0.0189;
    if(fabs(fun2-0)<=1e-8)
    {
        ECw20=0;
    }
    else
    {
        ECw20=ECa20/fun2;
    }
    surfaceSalt=ECw20;
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
            fVoltage=exVoltage(voltage);
        }
    }
    else fVoltage=0;
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
        addrCode=addrCode*16+switchNum(buffer[40-sizeMinus+i]);
    }

     //30cm深度温度
     for(int i=0;i<2;i++)
     {
         symbol_30cm=symbol_30cm*16+switchNum(buffer[44-sizeMinus+i]);
     }
     for(int i=0;i<4;i++)
     {
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
            data1=data1*16+switchNum(buffer[44-sizeMinus+j]);
        }
        for(j=0;j<4;j++)
        {
            data2=data2*16+switchNum(buffer[48-sizeMinus+j]);
        }
        for(j=0;j<4;j++)
        {
            data3=data3*16+switchNum(buffer[52-sizeMinus+j]);
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
void unlockControlPac(char buffer[1024],int datasize,int &nodeID,int &countControl,double &fVoltage)
{
    nodeID=countControl=fVoltage=0;
    int sizeFlag=0;//大小标志位 0大数据包 1小数据包
    int voltage=0;

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
            for(int k=0;k<4;k++){
                countControl=countControl*16+switchNum(buffer[10+k]);
            }

        }
        for(int k=0;k<4;k++)
        {
            voltage=voltage*16+switchNum(buffer[46+k]);

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


void unlockPocket_airHumidity(char buffer[SIZE],int datasize,int &nodeID,double &tem,double &hum)//空气温湿度
{
    int j;
    int temFlag=0;                         //温度标记 0表示零上 1表示零下
    tem=hum=0;
    int sizeMinus=0;                     //数据包位置锁定 0表示原数据包 28表示新数据包
    if(datasize==52)
    {
        sizeMinus=28;
        for(j=0;j<4;j++)//节点ID
        {
            nodeID=nodeID*16+switchNum(buffer[4+j]);
        }

    }
    else if(datasize==46*2)
    {
        for(j=0;j<4;j++)//节点ID
        {
            nodeID=nodeID*16+switchNum(buffer[20+j]);
        }
    }

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
























