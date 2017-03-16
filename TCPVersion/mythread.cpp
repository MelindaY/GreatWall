#include "mythread.h"
#include "tool.h"
#include "dbServer.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QApplication>
#define eps 1e-8
MyThread::MyThread(QObject *parent):
    QTcpSocket(parent)
{
    this->socket=new QTcpSocket;
    this->buffer.clear();
    connect(socket,SIGNAL(readyRead()),this,SLOT(recvString()));//接收数据连接
    connect(socket,SIGNAL(disconnected()),this,SIGNAL(linkRup()));//客户端断开连接

    this->totalFlow=0;//总流量
    this->storageNum=0;//入库数
    this->exceptNum=0;//异常数
    this->flag=false;
    this->controlPac="000";
    this->nodeID=0;
    this->voltagePre=0.75;
    this->rainTime.setDate(QDate(2013,0,0));
    this->rainTime.setTime(QTime(12,0,0));
    storageTime_v1="000";
    this->emptyTimes=0;//计算使用多少次雨量的2式
    this->timeFinished.clear();
    this->timeFlag=false;
    for(int i=0;i<150;i++)
        {
            for(int j=0;j<14;j++)
                parameters[i][j]=0;
        }
    count=0;
    for(int i=0;i<109;i++)
    {
        for(int j=0;j<2;j++)
            saltpara[i][j]=0;
    }
    this->timeCount=0;
}

MyThread::~MyThread()
{
}

void MyThread::setgateway()
{
    gateway=QString::number(id);//网关

}

void MyThread::setMyThreadID(int id)
{
    this->id=id;
}

void MyThread::setHandle(int handle)
{
    this->socket->setSocketDescriptor(handle);//将MyServer的handle传递给socket的handle
    dataBase->connectToMySQL();
    dataBase->selectMySQL_paras(parameters,count);
    dataBase->selectMySQL_saltParas(saltpara,count);

    dataBaseBackup->connectToMySQL();
    dataBaseBackup->selectMySQL_paras(parameters,count);
    dataBaseBackup->selectMySQL_saltParas(saltpara,count);
}


void MyThread::recvString()//接收数据
{
    int current_nodeid=0;
    this->count_control=0;
    this->count_data=0;
    QString exceptTime;
    QByteArray ba=socket->readAll();
    buffer+=ba.toHex();//转换成原始数据
    this->totalFlow+=(ba.toHex().size()/2);//改。之前是+=buffer.size()/2
    QByteArray buffer_data;
    int i,j;
    int q;
    char buf[1024],buff[1024];
    int value=0;
    strcpy(buff,ba.toHex().data());//只为判断异常
    //处理心跳

    while(buffer.size()>52)
    {

        i=buffer.indexOf("7e45");
        j=buffer.indexOf("7e",i+4);
        if(i==-1||j==-1)
            break;

        buffer_data=buffer.mid(i,j-i+2);
        buffer.remove(i,j-i+2);

        strcpy(buf,buffer_data.data());//只为判断异常
        value=0;

         for(int k=0;k<2;k++)
        {
            value=value*16+switchNum(buf[8+k]);
        }

        if(buffer_data=="0"||buffer_data==""||((buffer_data.size()!=26*2)&&buffer_data.size()!=46*2&&buffer_data.size()!=47*2))//||(value==2&&flag!=true)||(value==1&&flag!=false)||(value!=1&&value!=2))
         {
            this->exceptNum++;//记录异常数
            //创建独自的日志文件
            QFile file("sizeException.txt");
            if(file.open(QIODevice::WriteOnly|QIODevice::WriteOnly|QIODevice::Append))
            {
                QTextStream fileOut(&file);
                QDateTime currenttime=QDateTime::currentDateTime();//获取系统现在的时间
                exceptTime=currenttime.toString("yyyy-MM-dd hh:mm:ss"); //设置显示格式
                fileOut<<exceptTime+"   "+buffer_data.data()+"   "+QString::number(buffer_data.size())<<"\r\n";
                file.close();
            }

        }


        else if(buffer_data.size()==26*2||buffer_data.size()==46*2||buffer_data.size()!=47*2)
        {
            if(value==3)//协调器发送的时钟更新包
            {
                if(timeCount==0||timeCount%60==0)//每次第一条包更新一次时间 之后每8小时更新一次
                {
                    standardtime=QDateTime::currentDateTime();
                    timeCount=0;
                }
                standardtime=standardtime.addSecs(60);
                timeCount++;
                this->storageTime_v1=standardtime.toString("yyyyMMddhhmmsszzz"); //设置显示格式
                storageTime_v1+="000";
                storageTime_v2=standardtime.toString("yyyy-MM-dd hh:mm:ss"); //设置显示格shi
//                buffer_data.clear();

            }

            //将原始数据包插入数据库
            dataBase->insertMySQL_originPac(storageTime_v1,buffer_data.data(),0,timeFinished);

            dataBaseBackup->insertMySQL_originPac(storageTime_v1,buffer_data.data(),0,timeFinished);
            QFile file("Data.txt");
            if(file.open(QIODevice::WriteOnly|QIODevice::WriteOnly|QIODevice::Append))
            {
                QTextStream fileOut(&file);
                fileOut<<storageTime_v1+"   "+buffer_data.data()+"   "+QString::number(buffer_data.size())<<"\r\n";
                file.close();
            }
            int sizeFlag=0;
            if(buffer_data.size()==26*2)
            {
               sizeFlag=1;
            }
            else
                sizeFlag=0;
            if(value==1)
            {

                //开始解析控制包  buffer_data.data()是将QByteArray类型转换成char*类型，并不是说QByteArray包含data()

                 this->controlPac=buffer_data;

                 if (sizeFlag==0)
                 {
                    unlockPocket_hop(controlPac.data(),controlPac.size(),nodeID,hopnum,hop,PacketNum);//解析控制包
                    dataBase->insertMySQL_hop(storageTime_v1,hop,nodeID,PacketNum);//将控制包信息插入数据库

                     dataBaseBackup->insertMySQL_hop(storageTime_v1,hop,nodeID,PacketNum);//将控制包信息插入数据库
                    //下面两行代码 更新最新数据库
                 }

                 else
                 {
                    unlockControlPac(buffer_data.data(),buffer_data.size(),nodeID,countControl,fVoltage,RSSI,coorVoltage);
                    dataBase->insertMySQL_testControl(storageTime_v1,countControl,nodeID,1,timeFinished);
                    dataBase->insertMySQL_testControl(storageTime_v1,fVoltage,nodeID,2,timeFinished);
                    dataBase->insertMySQL_testControl(storageTime_v1,RSSI,nodeID,3,timeFinished);
                    dataBase->insertMySQL_testControl(storageTime_v1,coorVoltage,nodeID,4,timeFinished);

                    dataBaseBackup->insertMySQL_testControl(storageTime_v1,countControl,nodeID,1,timeFinished);
                    dataBaseBackup->insertMySQL_testControl(storageTime_v1,fVoltage,nodeID,2,timeFinished);
                    dataBaseBackup->insertMySQL_testControl(storageTime_v1,RSSI,nodeID,3,timeFinished);
                    dataBaseBackup->insertMySQL_testControl(storageTime_v1,coorVoltage,nodeID,4,timeFinished);
                 }
                 this->storageNum+=1;//更新入库数
                 emit dataInfo(id,gateway,totalFlow,storageNum,exceptNum,storageTime_v2,nodeID,arriveTime);//实时显示
            }
            else if(value==2)//&&flag==true&&current_nodeid==nodeID)
            {

                current_nodeid=0;
                for(int k=0;k<4;k++)
                {
                    current_nodeid=current_nodeid*16+switchNum(buf[4+k]);

                }
                this->nodeID=current_nodeid;

                for(int k=0;k<2;k++)
                {
                 stateFlag=stateFlag*16+switchNum(buf[48+k]);//获取节点与传感器通信状态标志
                }
                this->nodetype=0;
               //解析数据包，然后将所有数据包括控制包全插入数据库


               for(int k=0;k<2;k++)
               {
                   nodetype=nodetype*16+switchNum(buf[10+k]);//获取节点类型
               }

                if(this->nodetype==1)//温湿度光照传感器节点
                {

                 unlockPocket_TemHumLight(buffer_data.data(),buffer_data.size(),nodeID,fVoltage,fLight,fTemperature,fHumidity);
                 dataBase->insertMySQL_doubledata(storageTime_v1,fVoltage,nodeID,6,timeFinished);//插入数据库
                 dataBase->insertMySQL_doubledata(storageTime_v1,fLight,nodeID,4,timeFinished);
                 dataBase->insertMySQL_doubledata(storageTime_v1,fTemperature,nodeID,1,timeFinished);
                 dataBase->insertMySQL_doubledata(storageTime_v1,fHumidity,nodeID,2,timeFinished);

                 dataBaseBackup->insertMySQL_doubledata(storageTime_v1,fVoltage,nodeID,6,timeFinished);//插入数据库

                 dataBaseBackup->insertMySQL_doubledata(storageTime_v1,fLight,nodeID,4,timeFinished);

                 dataBaseBackup->insertMySQL_doubledata(storageTime_v1,fTemperature,nodeID,1,timeFinished);

                 dataBaseBackup->insertMySQL_doubledata(storageTime_v1,fHumidity,nodeID,2,timeFinished);


                 this->storageNum+=1;//更新入库数
                 emit dataInfo(id,gateway,totalFlow,storageNum,exceptNum,storageTime_v2,nodeID,arriveTime);//实时显示
                }
                //
                else// if(stateFlag==0)
                {
                //
                 if(this->nodetype==2)//雨量酸雨传感器节点
                 {


                    //查询上一条降雨量的时间
                     dataBase->selectMySQL_rainCtime(rainTime,nodeID);

                     dataBaseBackup->selectMySQL_rainCtime(rainTime,nodeID);

                     int rainnode_addr_ori,tem_outside_int,tem_outside_dec,tem_inside_int,tem_inside_dec,phInt,phDec,rainInt,rainDec,windspeedInt,windspeedDec,winddirectionInt,
                         winddirectionDec,powerInt,powerDec,rainFall_ori, fVoltage_ori;
                     //原始数据存储

                     unlockPocket_rainTest(buffer_data.data(),buffer_data.size(),nodeID,rainnode_addr_ori,tem_outside_int,tem_outside_dec,
                                           tem_inside_int,tem_inside_dec,phInt,phDec,rainInt,rainDec,windspeedInt,windspeedDec,winddirectionInt,
                                           winddirectionDec,powerInt,powerDec,rainFall_ori, fVoltage_ori);
                     //解析
                     unlockPocket_rain(buffer_data.data(),buffer_data.size(),nodeID,rainnode_addr,tem_outside,tem_inside,ph,rainfall,windspeed,winddirection,powervoltage,rain,fVoltage,voltagePre,emptyTimes);
                     //如果内外部温度不在-30.00~80.00区间，ph值不在3.0~7.0，雨量不在0.1~8.0，风速不在0.00~30.00，风向不在0~359，电压不在9.00~12.60，是否降雨不是0也不是1，则认为异常数据丢弃

                     //每天雨量计算
                    if(standardtime.time().hour()==12&&!timeFlag)//是12时并且是第一条数据
                    {
                        timeFlag=true;
                        QDateTime currenTime=QDateTime::currentDateTime();
                        QString time=currenTime.toString("yyyyMMddhhmmsszzz");
                        time+='000';

                        //get the date of previous day
                        DayPrevious=QString::number(standardtime.addDays(-1).date().month());
                        DayPrevious+=QString::number(standardtime.addDays(-1).date().day());
                        DayPrevious+=QString::number(12);
                        dataBase->selectMySQL_voltagePrevious(DayPrevious,PreviousVoltage,nodeID);
                        dataBaseBackup->selectMySQL_voltagePrevious(DayPrevious,PreviousVoltage,nodeID);
                        QFile file("voltage.txt");
                        if(file.open(QIODevice::WriteOnly|QIODevice::WriteOnly|QIODevice::Append))
                        {
                           QTextStream fileOut(&file);
                           fileOut<<QString::number(PreviousVoltage)<<"\r\n";
                           file.close();
                        }
                        dailyRain=(powervoltage-PreviousVoltage)*4.52+emptyTimes*8.4524;
                        dataBase->insertMySQL_doubledata(storageTime_v1,dailyRain,nodeID,0,time);
                        dataBaseBackup->insertMySQL_doubledata(storageTime_v1,dailyRain,nodeID,0,time);
                        dailyRain=0;
                        emptyTimes=0;
                    }
                    if(standardtime.time().hour()!=12)
                    {
                        timeFlag=false;
                    }
                    //如果与上一条时间间隔小于30min
                     if(rainTime.secsTo(standardtime)<=1800)
                     {
                         dataBase->insertMYSQL_rainAnalysis(storageTime_v1,rain,rainfall,rainfall/5,nodeID);
                         dataBaseBackup->insertMYSQL_rainAnalysis(storageTime_v1,rain,rainfall,rainfall/5,nodeID);

                     }

                         dataBase->insertMySQL_doubledata(storageTime_v1,(double)rainnode_addr,nodeID,9,timeFinished);//插入数据库

                         dataBase->insertMySQL_doubledata(storageTime_v1,tem_outside,nodeID,10,timeFinished);

                         dataBase->insertMySQL_doubledata(storageTime_v1,tem_inside,nodeID,11,timeFinished);

                         dataBase->insertMySQL_doubledata(storageTime_v1,ph,nodeID,12,timeFinished);

                         dataBase->insertMySQL_doubledata(storageTime_v1,rainfall,nodeID,13,timeFinished);

                         dataBase->insertMySQL_doubledata(storageTime_v1,windspeed,nodeID,14,timeFinished);

                         dataBase->insertMySQL_doubledata(storageTime_v1,winddirection,nodeID,15,timeFinished);

                         dataBase->insertMySQL_doubledata(storageTime_v1,powervoltage,nodeID,16,timeFinished);

                         dataBase->insertMySQL_doubledata(storageTime_v1,(double)rain,nodeID,17,timeFinished);

                         dataBase->insertMySQL_doubledata(storageTime_v1,fVoltage,nodeID,33,timeFinished);

                         dataBase->insertMySQL_rainTest(storageTime_v1,rainnode_addr_ori,tem_outside_int,tem_outside_dec,
                                                        tem_inside_int,tem_inside_dec,phInt,phDec,rainInt,rainDec,
                                                        windspeedInt,windspeedDec,winddirectionInt,winddirectionDec,powerInt,powerDec,rainFall_ori, fVoltage_ori,nodeID);


                         dataBaseBackup->insertMySQL_doubledata(storageTime_v1,(double)rainnode_addr,nodeID,9,timeFinished);//插入数据库

                         dataBaseBackup->insertMySQL_doubledata(storageTime_v1,tem_outside,nodeID,10,timeFinished);

                         dataBaseBackup->insertMySQL_doubledata(storageTime_v1,tem_inside,nodeID,11,timeFinished);

                         dataBaseBackup->insertMySQL_doubledata(storageTime_v1,ph,nodeID,12,timeFinished);

                         dataBaseBackup->insertMySQL_doubledata(storageTime_v1,rainfall,nodeID,13,timeFinished);

                         dataBaseBackup->insertMySQL_doubledata(storageTime_v1,windspeed,nodeID,14,timeFinished);

                         dataBaseBackup->insertMySQL_doubledata(storageTime_v1,winddirection,nodeID,15,timeFinished);

                         dataBaseBackup->insertMySQL_doubledata(storageTime_v1,powervoltage,nodeID,16,timeFinished);

                         dataBaseBackup->insertMySQL_doubledata(storageTime_v1,(double)rain,nodeID,17,timeFinished);

                         dataBaseBackup->insertMySQL_doubledata(storageTime_v1,fVoltage,nodeID,33,timeFinished);

                         dataBaseBackup->insertMySQL_rainTest(storageTime_v1,rainnode_addr_ori,tem_outside_int,tem_outside_dec,
                                                        tem_inside_int,tem_inside_dec,phInt,phDec,rainInt,rainDec,
                                                        windspeedInt,windspeedDec,winddirectionInt,winddirectionDec,powerInt,powerDec,rainFall_ori, fVoltage_ori,nodeID);

                         this->storageNum++;//更新入库数
                         emit dataInfo(id,gateway,totalFlow,storageNum,exceptNum,storageTime_v2,nodeID,arriveTime);//实时显示
                     }
                 else if(this->nodetype==3)//土壤温湿度传感器节点
                 {
                     unlockPocket_sioltemhum(buffer_data.data(),nodeID,addressCode1,addressCode2,tem_30cm,hum_30cm,tem_15cm,hum_15cm,tem_5cm,hum_5cm,fVoltage,parameters,count);
                     //按照张远老师要求，新建soilTempHumi数据表，存储原始土壤温湿度数据
                     int addrCode1,addrCode2,symbol_30cm,temp1_30cm,temp2_30cm,humi1_30cm,humi2_30cm,symbol_15cm,temp1_15cm,temp2_15cm,humi1_15cm,humi2_15cm,symbol_5cm,temp1_5cm,temp2_5cm,humi1_5cm,humi2_5cm,nodeid,voltagetest;
                     unlockPocket_sioltemphumiTest(buffer_data.data(),buffer_data.size(),addrCode1,addrCode2,symbol_30cm,temp1_30cm,temp2_30cm,humi1_30cm,humi2_30cm,symbol_15cm,temp1_15cm,temp2_15cm,humi1_15cm,humi2_15cm,symbol_5cm,temp1_5cm,temp2_5cm,humi1_5cm,humi2_5cm,voltagetest,nodeid);

                     //结束张远老师要求
                     dataBase->insertMySQL_doubledata(storageTime_v1,(double)addressCode1,nodeID,18,timeFinished);//插入数据库
                     dataBase->insertMySQL_doubledata(storageTime_v1,(double)addressCode2,nodeID,37,timeFinished);//插入数据库


                     dataBase->insertMySQL_doubledata(storageTime_v1,tem_30cm,nodeID,19,timeFinished);

                     dataBase->insertMySQL_doubledata(storageTime_v1,hum_30cm,nodeID,20,timeFinished);

                     dataBase->insertMySQL_doubledata(storageTime_v1,tem_15cm,nodeID,21,timeFinished);

                     dataBase->insertMySQL_doubledata(storageTime_v1,hum_15cm,nodeID,22,timeFinished);

                     dataBase->insertMySQL_doubledata(storageTime_v1,tem_5cm,nodeID,23,timeFinished);

                     dataBase->insertMySQL_doubledata(storageTime_v1,hum_5cm,nodeID,24,timeFinished);

                     dataBase->insertMySQL_doubledata(storageTime_v1,fVoltage,nodeID,34,timeFinished);


                     dataBase->inserMySQL_soildata(storageTime_v1,addressCode1,addressCode2,tem_30cm,hum_30cm,tem_15cm,hum_15cm,tem_5cm,hum_5cm,fVoltage,nodeID);//插入结束
                     //将张老师原始十进制数据插入数据库
                     dataBase->insertMySQL_sioltemphumiTest(storageTime_v1,addrCode1,addrCode2,symbol_30cm,temp1_30cm,temp2_30cm,humi1_30cm,humi2_30cm,symbol_15cm,temp1_15cm,temp2_15cm,humi1_15cm,humi2_15cm,symbol_5cm,temp1_5cm,temp2_5cm,humi1_5cm,humi2_5cm,voltagetest,nodeid);



                     dataBaseBackup->insertMySQL_doubledata(storageTime_v1,(double)addressCode1,nodeID,18,timeFinished);//插入数据库
                     dataBaseBackup->insertMySQL_doubledata(storageTime_v1,(double)addressCode2,nodeID,37,timeFinished);//插入数据库


                     dataBaseBackup->insertMySQL_doubledata(storageTime_v1,tem_30cm,nodeID,19,timeFinished);

                     dataBaseBackup->insertMySQL_doubledata(storageTime_v1,hum_30cm,nodeID,20,timeFinished);

                     dataBaseBackup->insertMySQL_doubledata(storageTime_v1,tem_15cm,nodeID,21,timeFinished);

                     dataBaseBackup->insertMySQL_doubledata(storageTime_v1,hum_15cm,nodeID,22,timeFinished);

                     dataBaseBackup->insertMySQL_doubledata(storageTime_v1,tem_5cm,nodeID,23,timeFinished);

                     dataBaseBackup->insertMySQL_doubledata(storageTime_v1,hum_5cm,nodeID,24,timeFinished);

                     dataBaseBackup->insertMySQL_doubledata(storageTime_v1,fVoltage,nodeID,34,timeFinished);


                     dataBaseBackup->inserMySQL_soildata(storageTime_v1,addressCode1,addressCode2,tem_30cm,hum_30cm,tem_15cm,hum_15cm,tem_5cm,hum_5cm,fVoltage,nodeID);//插入结束
                     //将张老师原始十进制数据插入数据库
                     dataBase->insertMySQL_sioltemphumiTest(storageTime_v1,addrCode1,addrCode2,symbol_30cm,temp1_30cm,temp2_30cm,humi1_30cm,humi2_30cm,symbol_15cm,temp1_15cm,temp2_15cm,humi1_15cm,humi2_15cm,symbol_5cm,temp1_5cm,temp2_5cm,humi1_5cm,humi2_5cm,voltagetest,nodeid);

                     this->storageNum++;//更新入库数
                     emit dataInfo(id,gateway,totalFlow,storageNum,exceptNum,storageTime_v2,nodeID,arriveTime);//实时显示
                 }
                 else if(this->nodetype==4)//盐分传感器节点
                 {
                     unlockPocket_salt(buffer_data.data(),buffer_data.size(),nodeID,saltaddrcode,tem,hum,verticalSalt,surfaceSalt,fVoltage,saltpara,count);

                     //新建salttest数据表，存储原始盐分数据
                     int addrCode,symbol_30cm,temp_30cm,humi_30cm,vertSalt,surfSalt,voltagetest,nodeid;
                     unlockPocket_saltTest(buffer_data.data(),buffer_data.size(),addrCode,symbol_30cm,temp_30cm,humi_30cm,vertSalt,surfSalt,voltagetest,nodeid);

                     dataBase->insertMySQL_doubledata(storageTime_v1,(double)saltaddrcode,nodeID,25,timeFinished);//插入数据库

                     dataBase->insertMySQL_doubledata(storageTime_v1,tem,nodeID,26,timeFinished);

                     dataBase->insertMySQL_doubledata(storageTime_v1,hum,nodeID,27,timeFinished);

                     dataBase->insertMySQL_doubledata(storageTime_v1,verticalSalt,nodeID,28,timeFinished);

                     dataBase->insertMySQL_doubledata(storageTime_v1,surfaceSalt,nodeID,29,timeFinished);

                     dataBase->insertMySQL_doubledata(storageTime_v1,fVoltage,nodeID,35,timeFinished);

                     //将原始盐分数据插入数据库
                     dataBase->insertMySQL_saltTest(storageTime_v1,addrCode,symbol_30cm,temp_30cm,humi_30cm,vertSalt,surfSalt,voltagetest,nodeid);


                     dataBaseBackup->insertMySQL_doubledata(storageTime_v1,(double)saltaddrcode,nodeID,25,timeFinished);//插入数据库

                     dataBaseBackup->insertMySQL_doubledata(storageTime_v1,tem,nodeID,26,timeFinished);

                     dataBaseBackup->insertMySQL_doubledata(storageTime_v1,hum,nodeID,27,timeFinished);

                     dataBaseBackup->insertMySQL_doubledata(storageTime_v1,verticalSalt,nodeID,28,timeFinished);

                     dataBaseBackup->insertMySQL_doubledata(storageTime_v1,surfaceSalt,nodeID,29,timeFinished);

                     dataBaseBackup->insertMySQL_doubledata(storageTime_v1,fVoltage,nodeID,35,timeFinished);

                     //将原始盐分数据插入数据库
                     dataBaseBackup->insertMySQL_saltTest(storageTime_v1,addrCode,symbol_30cm,temp_30cm,humi_30cm,vertSalt,surfSalt,voltagetest,nodeid);

                     this->storageNum++;//更新入库数
                     emit dataInfo(id,gateway,totalFlow,storageNum,exceptNum,storageTime_v2,nodeID,arriveTime);//实时显示
                 }
                 else if(this->nodetype==6)//震动传感器节点
                 {
                     unlockPocket_shake(buffer_data.data(),buffer_data.size(),nodeID,shake1,shake2,shake3,fVoltage);

                     //解析原始数据
                     int shakeOne,shakeTwo,shakeThree,voltagetest,nodeid;
                     unlockPocket_shakeTest(buffer_data.data(),buffer_data.size(),shakeOne,shakeTwo,shakeThree,voltagetest,nodeid);

                     dataBase->insertMySQL_doubledata(storageTime_v1,shake1,nodeID,30,timeFinished);

                     dataBase->insertMySQL_doubledata(storageTime_v1,shake2,nodeID,31,timeFinished);

                     dataBase->insertMySQL_doubledata(storageTime_v1,shake3,nodeID,32,timeFinished);

                     dataBase->insertMySQL_doubledata(storageTime_v1,fVoltage,nodeID,36,timeFinished);

                     //存储原始数据
                      dataBase->insertMySQL_shakeTest(storageTime_v1,shakeOne,shakeTwo,shakeThree,voltagetest,nodeid);


                      dataBaseBackup->insertMySQL_doubledata(storageTime_v1,shake1,nodeID,30,timeFinished);

                      dataBaseBackup->insertMySQL_doubledata(storageTime_v1,shake2,nodeID,31,timeFinished);

                      dataBaseBackup->insertMySQL_doubledata(storageTime_v1,shake3,nodeID,32,timeFinished);

                      dataBaseBackup->insertMySQL_doubledata(storageTime_v1,fVoltage,nodeID,36,timeFinished);

                      //存储原始数据
                      dataBaseBackup->insertMySQL_shakeTest(storageTime_v1,shakeOne,shakeTwo,shakeThree,voltagetest,nodeid);

                     this->storageNum++;//更新入库数
                     emit dataInfo(id,gateway,totalFlow,storageNum,exceptNum,storageTime_v2,nodeID,arriveTime);//实时显示

                 }
                 else if(this->nodetype==7)//空气温湿度
                 {
                     unlockPocket_airHumidity(buffer_data.data(),buffer_data.size(),nodeID,tem,hum);
                     dataBase->insertMySQL_doubledata(storageTime_v1,tem,nodeID,40,timeFinished);
                     dataBase->insertMySQL_doubledata(storageTime_v1,hum,nodeID,41,timeFinished);

                     dataBaseBackup->insertMySQL_doubledata(storageTime_v1,tem,nodeID,40,timeFinished);
                     dataBaseBackup->insertMySQL_doubledata(storageTime_v1,hum,nodeID,41,timeFinished);

                     emit dataInfo(id,gateway,totalFlow,storageNum,exceptNum,storageTime_v2,nodeID,arriveTime);//实时显示

                 }
                 else if(this->nodetype>7)
                 {
                    this->exceptNum++;
                     //创建独自的日志文件
                   QFile file("nodetypeException.txt");
                   if(file.open(QIODevice::WriteOnly|QIODevice::WriteOnly|QIODevice::Append))
                   {
                      QTextStream fileOut(&file);
                      fileOut<<storageTime_v2+"   "+buffer_data.data()+"   "+QString::number(buffer_data.size())<<"\r\n";
                      file.close();
                   }
                 }
                }
            }
            else if(value==4)//3表示ack，4表示有存活，存到shakestate数据表中
            {
                current_nodeid=0;
                for(int k=0;k<4;k++)
                {
                    current_nodeid=current_nodeid*16+switchNum(buf[4+k]);

                }

                dataBase->insertMySQL_shakestate(storageTime_v1,current_nodeid);

                dataBaseBackup->insertMySQL_shakestate(storageTime_v1,current_nodeid);

                this->storageNum++;//更新入库数
            }
            else if (value!=3)//未知的节点数据包
            {

                //flag=false;
                this->exceptNum++;//记录异常数
                //创建独自的日志文件
                 //创建独自的日志文件
                QFile file("unknownnodeException.txt");
                if(file.open(QIODevice::WriteOnly|QIODevice::WriteOnly|QIODevice::Append))
               {
                   QTextStream fileOut(&file);
                   fileOut<<storageTime_v2+"   "+buffer_data.data()+"   "+QString::number(buffer_data.size())<<"\r\n";
                   file.close();
               }

            }
     }
   }
}

void MyThread::setDBServer(DBServer* dataBase)
{
    this->dataBase=dataBase;
    QDateTime atime=QDateTime::currentDateTime();//获取系统现在的时间
    arriveTime=atime.toString("yyyy-MM-dd hh:mm:ss"); //设置显示格式
}

void MyThread::setDBSeverBackup(DBServer* dataBase)
{
    this->dataBaseBackup=dataBase;
}

void MyThread::setTimeStamp()
{
    standardtime=QDateTime::currentDateTime();
    arriveTime=standardtime.toString("yyyy-MM-dd hh:mm:ss"); //设置显示格式
    this->timeStamp=standardtime.toString("yyyyMMddhhmmsszzz");//在建立连接时更新时间戳
    this->timeStamp+="000";
    this->timeStamp_v1=arriveTime;
    emit updatetimeStamp(timeStamp_v1,arriveTime);
}
