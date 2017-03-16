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
    connect(socket,SIGNAL(readyRead()),this,SLOT(recvString()));//������������
    connect(socket,SIGNAL(disconnected()),this,SIGNAL(linkRup()));//�ͻ��˶Ͽ�����

    this->totalFlow=0;//������
    this->storageNum=0;//�����
    this->exceptNum=0;//�쳣��
    this->flag=false;
    this->controlPac="000";
    this->nodeID=0;
    this->voltagePre=0.75;
    this->rainTime.setDate(QDate(2013,0,0));
    this->rainTime.setTime(QTime(12,0,0));
    storageTime_v1="000";
    this->emptyTimes=0;//����ʹ�ö��ٴ�������2ʽ
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
    gateway=QString::number(id);//����

}

void MyThread::setMyThreadID(int id)
{
    this->id=id;
}

void MyThread::setHandle(int handle)
{
    this->socket->setSocketDescriptor(handle);//��MyServer��handle���ݸ�socket��handle
    dataBase->connectToMySQL();
    dataBase->selectMySQL_paras(parameters,count);
    dataBase->selectMySQL_saltParas(saltpara,count);

    dataBaseBackup->connectToMySQL();
    dataBaseBackup->selectMySQL_paras(parameters,count);
    dataBaseBackup->selectMySQL_saltParas(saltpara,count);
}


void MyThread::recvString()//��������
{
    int current_nodeid=0;
    this->count_control=0;
    this->count_data=0;
    QString exceptTime;
    QByteArray ba=socket->readAll();
    buffer+=ba.toHex();//ת����ԭʼ����
    this->totalFlow+=(ba.toHex().size()/2);//�ġ�֮ǰ��+=buffer.size()/2
    QByteArray buffer_data;
    int i,j;
    int q;
    char buf[1024],buff[1024];
    int value=0;
    strcpy(buff,ba.toHex().data());//ֻΪ�ж��쳣
    //��������

    while(buffer.size()>52)
    {

        i=buffer.indexOf("7e45");
        j=buffer.indexOf("7e",i+4);
        if(i==-1||j==-1)
            break;

        buffer_data=buffer.mid(i,j-i+2);
        buffer.remove(i,j-i+2);

        strcpy(buf,buffer_data.data());//ֻΪ�ж��쳣
        value=0;

         for(int k=0;k<2;k++)
        {
            value=value*16+switchNum(buf[8+k]);
        }

        if(buffer_data=="0"||buffer_data==""||((buffer_data.size()!=26*2)&&buffer_data.size()!=46*2&&buffer_data.size()!=47*2))//||(value==2&&flag!=true)||(value==1&&flag!=false)||(value!=1&&value!=2))
         {
            this->exceptNum++;//��¼�쳣��
            //�������Ե���־�ļ�
            QFile file("sizeException.txt");
            if(file.open(QIODevice::WriteOnly|QIODevice::WriteOnly|QIODevice::Append))
            {
                QTextStream fileOut(&file);
                QDateTime currenttime=QDateTime::currentDateTime();//��ȡϵͳ���ڵ�ʱ��
                exceptTime=currenttime.toString("yyyy-MM-dd hh:mm:ss"); //������ʾ��ʽ
                fileOut<<exceptTime+"   "+buffer_data.data()+"   "+QString::number(buffer_data.size())<<"\r\n";
                file.close();
            }

        }


        else if(buffer_data.size()==26*2||buffer_data.size()==46*2||buffer_data.size()!=47*2)
        {
            if(value==3)//Э�������͵�ʱ�Ӹ��°�
            {
                if(timeCount==0||timeCount%60==0)//ÿ�ε�һ��������һ��ʱ�� ֮��ÿ8Сʱ����һ��
                {
                    standardtime=QDateTime::currentDateTime();
                    timeCount=0;
                }
                standardtime=standardtime.addSecs(60);
                timeCount++;
                this->storageTime_v1=standardtime.toString("yyyyMMddhhmmsszzz"); //������ʾ��ʽ
                storageTime_v1+="000";
                storageTime_v2=standardtime.toString("yyyy-MM-dd hh:mm:ss"); //������ʾ��shi
//                buffer_data.clear();

            }

            //��ԭʼ���ݰ��������ݿ�
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

                //��ʼ�������ư�  buffer_data.data()�ǽ�QByteArray����ת����char*���ͣ�������˵QByteArray����data()

                 this->controlPac=buffer_data;

                 if (sizeFlag==0)
                 {
                    unlockPocket_hop(controlPac.data(),controlPac.size(),nodeID,hopnum,hop,PacketNum);//�������ư�
                    dataBase->insertMySQL_hop(storageTime_v1,hop,nodeID,PacketNum);//�����ư���Ϣ�������ݿ�

                     dataBaseBackup->insertMySQL_hop(storageTime_v1,hop,nodeID,PacketNum);//�����ư���Ϣ�������ݿ�
                    //�������д��� �����������ݿ�
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
                 this->storageNum+=1;//���������
                 emit dataInfo(id,gateway,totalFlow,storageNum,exceptNum,storageTime_v2,nodeID,arriveTime);//ʵʱ��ʾ
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
                 stateFlag=stateFlag*16+switchNum(buf[48+k]);//��ȡ�ڵ��봫����ͨ��״̬��־
                }
                this->nodetype=0;
               //�������ݰ���Ȼ���������ݰ������ư�ȫ�������ݿ�


               for(int k=0;k<2;k++)
               {
                   nodetype=nodetype*16+switchNum(buf[10+k]);//��ȡ�ڵ�����
               }

                if(this->nodetype==1)//��ʪ�ȹ��մ������ڵ�
                {

                 unlockPocket_TemHumLight(buffer_data.data(),buffer_data.size(),nodeID,fVoltage,fLight,fTemperature,fHumidity);
                 dataBase->insertMySQL_doubledata(storageTime_v1,fVoltage,nodeID,6,timeFinished);//�������ݿ�
                 dataBase->insertMySQL_doubledata(storageTime_v1,fLight,nodeID,4,timeFinished);
                 dataBase->insertMySQL_doubledata(storageTime_v1,fTemperature,nodeID,1,timeFinished);
                 dataBase->insertMySQL_doubledata(storageTime_v1,fHumidity,nodeID,2,timeFinished);

                 dataBaseBackup->insertMySQL_doubledata(storageTime_v1,fVoltage,nodeID,6,timeFinished);//�������ݿ�

                 dataBaseBackup->insertMySQL_doubledata(storageTime_v1,fLight,nodeID,4,timeFinished);

                 dataBaseBackup->insertMySQL_doubledata(storageTime_v1,fTemperature,nodeID,1,timeFinished);

                 dataBaseBackup->insertMySQL_doubledata(storageTime_v1,fHumidity,nodeID,2,timeFinished);


                 this->storageNum+=1;//���������
                 emit dataInfo(id,gateway,totalFlow,storageNum,exceptNum,storageTime_v2,nodeID,arriveTime);//ʵʱ��ʾ
                }
                //
                else// if(stateFlag==0)
                {
                //
                 if(this->nodetype==2)//�������괫�����ڵ�
                 {


                    //��ѯ��һ����������ʱ��
                     dataBase->selectMySQL_rainCtime(rainTime,nodeID);

                     dataBaseBackup->selectMySQL_rainCtime(rainTime,nodeID);

                     int rainnode_addr_ori,tem_outside_int,tem_outside_dec,tem_inside_int,tem_inside_dec,phInt,phDec,rainInt,rainDec,windspeedInt,windspeedDec,winddirectionInt,
                         winddirectionDec,powerInt,powerDec,rainFall_ori, fVoltage_ori;
                     //ԭʼ���ݴ洢

                     unlockPocket_rainTest(buffer_data.data(),buffer_data.size(),nodeID,rainnode_addr_ori,tem_outside_int,tem_outside_dec,
                                           tem_inside_int,tem_inside_dec,phInt,phDec,rainInt,rainDec,windspeedInt,windspeedDec,winddirectionInt,
                                           winddirectionDec,powerInt,powerDec,rainFall_ori, fVoltage_ori);
                     //����
                     unlockPocket_rain(buffer_data.data(),buffer_data.size(),nodeID,rainnode_addr,tem_outside,tem_inside,ph,rainfall,windspeed,winddirection,powervoltage,rain,fVoltage,voltagePre,emptyTimes);
                     //������ⲿ�¶Ȳ���-30.00~80.00���䣬phֵ����3.0~7.0����������0.1~8.0�����ٲ���0.00~30.00��������0~359����ѹ����9.00~12.60���Ƿ��겻��0Ҳ����1������Ϊ�쳣���ݶ���

                     //ÿ����������
                    if(standardtime.time().hour()==12&&!timeFlag)//��12ʱ�����ǵ�һ������
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
                    //�������һ��ʱ����С��30min
                     if(rainTime.secsTo(standardtime)<=1800)
                     {
                         dataBase->insertMYSQL_rainAnalysis(storageTime_v1,rain,rainfall,rainfall/5,nodeID);
                         dataBaseBackup->insertMYSQL_rainAnalysis(storageTime_v1,rain,rainfall,rainfall/5,nodeID);

                     }

                         dataBase->insertMySQL_doubledata(storageTime_v1,(double)rainnode_addr,nodeID,9,timeFinished);//�������ݿ�

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


                         dataBaseBackup->insertMySQL_doubledata(storageTime_v1,(double)rainnode_addr,nodeID,9,timeFinished);//�������ݿ�

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

                         this->storageNum++;//���������
                         emit dataInfo(id,gateway,totalFlow,storageNum,exceptNum,storageTime_v2,nodeID,arriveTime);//ʵʱ��ʾ
                     }
                 else if(this->nodetype==3)//������ʪ�ȴ������ڵ�
                 {
                     unlockPocket_sioltemhum(buffer_data.data(),nodeID,addressCode1,addressCode2,tem_30cm,hum_30cm,tem_15cm,hum_15cm,tem_5cm,hum_5cm,fVoltage,parameters,count);
                     //������Զ��ʦҪ���½�soilTempHumi���ݱ��洢ԭʼ������ʪ������
                     int addrCode1,addrCode2,symbol_30cm,temp1_30cm,temp2_30cm,humi1_30cm,humi2_30cm,symbol_15cm,temp1_15cm,temp2_15cm,humi1_15cm,humi2_15cm,symbol_5cm,temp1_5cm,temp2_5cm,humi1_5cm,humi2_5cm,nodeid,voltagetest;
                     unlockPocket_sioltemphumiTest(buffer_data.data(),buffer_data.size(),addrCode1,addrCode2,symbol_30cm,temp1_30cm,temp2_30cm,humi1_30cm,humi2_30cm,symbol_15cm,temp1_15cm,temp2_15cm,humi1_15cm,humi2_15cm,symbol_5cm,temp1_5cm,temp2_5cm,humi1_5cm,humi2_5cm,voltagetest,nodeid);

                     //������Զ��ʦҪ��
                     dataBase->insertMySQL_doubledata(storageTime_v1,(double)addressCode1,nodeID,18,timeFinished);//�������ݿ�
                     dataBase->insertMySQL_doubledata(storageTime_v1,(double)addressCode2,nodeID,37,timeFinished);//�������ݿ�


                     dataBase->insertMySQL_doubledata(storageTime_v1,tem_30cm,nodeID,19,timeFinished);

                     dataBase->insertMySQL_doubledata(storageTime_v1,hum_30cm,nodeID,20,timeFinished);

                     dataBase->insertMySQL_doubledata(storageTime_v1,tem_15cm,nodeID,21,timeFinished);

                     dataBase->insertMySQL_doubledata(storageTime_v1,hum_15cm,nodeID,22,timeFinished);

                     dataBase->insertMySQL_doubledata(storageTime_v1,tem_5cm,nodeID,23,timeFinished);

                     dataBase->insertMySQL_doubledata(storageTime_v1,hum_5cm,nodeID,24,timeFinished);

                     dataBase->insertMySQL_doubledata(storageTime_v1,fVoltage,nodeID,34,timeFinished);


                     dataBase->inserMySQL_soildata(storageTime_v1,addressCode1,addressCode2,tem_30cm,hum_30cm,tem_15cm,hum_15cm,tem_5cm,hum_5cm,fVoltage,nodeID);//�������
                     //������ʦԭʼʮ�������ݲ������ݿ�
                     dataBase->insertMySQL_sioltemphumiTest(storageTime_v1,addrCode1,addrCode2,symbol_30cm,temp1_30cm,temp2_30cm,humi1_30cm,humi2_30cm,symbol_15cm,temp1_15cm,temp2_15cm,humi1_15cm,humi2_15cm,symbol_5cm,temp1_5cm,temp2_5cm,humi1_5cm,humi2_5cm,voltagetest,nodeid);



                     dataBaseBackup->insertMySQL_doubledata(storageTime_v1,(double)addressCode1,nodeID,18,timeFinished);//�������ݿ�
                     dataBaseBackup->insertMySQL_doubledata(storageTime_v1,(double)addressCode2,nodeID,37,timeFinished);//�������ݿ�


                     dataBaseBackup->insertMySQL_doubledata(storageTime_v1,tem_30cm,nodeID,19,timeFinished);

                     dataBaseBackup->insertMySQL_doubledata(storageTime_v1,hum_30cm,nodeID,20,timeFinished);

                     dataBaseBackup->insertMySQL_doubledata(storageTime_v1,tem_15cm,nodeID,21,timeFinished);

                     dataBaseBackup->insertMySQL_doubledata(storageTime_v1,hum_15cm,nodeID,22,timeFinished);

                     dataBaseBackup->insertMySQL_doubledata(storageTime_v1,tem_5cm,nodeID,23,timeFinished);

                     dataBaseBackup->insertMySQL_doubledata(storageTime_v1,hum_5cm,nodeID,24,timeFinished);

                     dataBaseBackup->insertMySQL_doubledata(storageTime_v1,fVoltage,nodeID,34,timeFinished);


                     dataBaseBackup->inserMySQL_soildata(storageTime_v1,addressCode1,addressCode2,tem_30cm,hum_30cm,tem_15cm,hum_15cm,tem_5cm,hum_5cm,fVoltage,nodeID);//�������
                     //������ʦԭʼʮ�������ݲ������ݿ�
                     dataBase->insertMySQL_sioltemphumiTest(storageTime_v1,addrCode1,addrCode2,symbol_30cm,temp1_30cm,temp2_30cm,humi1_30cm,humi2_30cm,symbol_15cm,temp1_15cm,temp2_15cm,humi1_15cm,humi2_15cm,symbol_5cm,temp1_5cm,temp2_5cm,humi1_5cm,humi2_5cm,voltagetest,nodeid);

                     this->storageNum++;//���������
                     emit dataInfo(id,gateway,totalFlow,storageNum,exceptNum,storageTime_v2,nodeID,arriveTime);//ʵʱ��ʾ
                 }
                 else if(this->nodetype==4)//�ηִ������ڵ�
                 {
                     unlockPocket_salt(buffer_data.data(),buffer_data.size(),nodeID,saltaddrcode,tem,hum,verticalSalt,surfaceSalt,fVoltage,saltpara,count);

                     //�½�salttest���ݱ��洢ԭʼ�η�����
                     int addrCode,symbol_30cm,temp_30cm,humi_30cm,vertSalt,surfSalt,voltagetest,nodeid;
                     unlockPocket_saltTest(buffer_data.data(),buffer_data.size(),addrCode,symbol_30cm,temp_30cm,humi_30cm,vertSalt,surfSalt,voltagetest,nodeid);

                     dataBase->insertMySQL_doubledata(storageTime_v1,(double)saltaddrcode,nodeID,25,timeFinished);//�������ݿ�

                     dataBase->insertMySQL_doubledata(storageTime_v1,tem,nodeID,26,timeFinished);

                     dataBase->insertMySQL_doubledata(storageTime_v1,hum,nodeID,27,timeFinished);

                     dataBase->insertMySQL_doubledata(storageTime_v1,verticalSalt,nodeID,28,timeFinished);

                     dataBase->insertMySQL_doubledata(storageTime_v1,surfaceSalt,nodeID,29,timeFinished);

                     dataBase->insertMySQL_doubledata(storageTime_v1,fVoltage,nodeID,35,timeFinished);

                     //��ԭʼ�η����ݲ������ݿ�
                     dataBase->insertMySQL_saltTest(storageTime_v1,addrCode,symbol_30cm,temp_30cm,humi_30cm,vertSalt,surfSalt,voltagetest,nodeid);


                     dataBaseBackup->insertMySQL_doubledata(storageTime_v1,(double)saltaddrcode,nodeID,25,timeFinished);//�������ݿ�

                     dataBaseBackup->insertMySQL_doubledata(storageTime_v1,tem,nodeID,26,timeFinished);

                     dataBaseBackup->insertMySQL_doubledata(storageTime_v1,hum,nodeID,27,timeFinished);

                     dataBaseBackup->insertMySQL_doubledata(storageTime_v1,verticalSalt,nodeID,28,timeFinished);

                     dataBaseBackup->insertMySQL_doubledata(storageTime_v1,surfaceSalt,nodeID,29,timeFinished);

                     dataBaseBackup->insertMySQL_doubledata(storageTime_v1,fVoltage,nodeID,35,timeFinished);

                     //��ԭʼ�η����ݲ������ݿ�
                     dataBaseBackup->insertMySQL_saltTest(storageTime_v1,addrCode,symbol_30cm,temp_30cm,humi_30cm,vertSalt,surfSalt,voltagetest,nodeid);

                     this->storageNum++;//���������
                     emit dataInfo(id,gateway,totalFlow,storageNum,exceptNum,storageTime_v2,nodeID,arriveTime);//ʵʱ��ʾ
                 }
                 else if(this->nodetype==6)//�𶯴������ڵ�
                 {
                     unlockPocket_shake(buffer_data.data(),buffer_data.size(),nodeID,shake1,shake2,shake3,fVoltage);

                     //����ԭʼ����
                     int shakeOne,shakeTwo,shakeThree,voltagetest,nodeid;
                     unlockPocket_shakeTest(buffer_data.data(),buffer_data.size(),shakeOne,shakeTwo,shakeThree,voltagetest,nodeid);

                     dataBase->insertMySQL_doubledata(storageTime_v1,shake1,nodeID,30,timeFinished);

                     dataBase->insertMySQL_doubledata(storageTime_v1,shake2,nodeID,31,timeFinished);

                     dataBase->insertMySQL_doubledata(storageTime_v1,shake3,nodeID,32,timeFinished);

                     dataBase->insertMySQL_doubledata(storageTime_v1,fVoltage,nodeID,36,timeFinished);

                     //�洢ԭʼ����
                      dataBase->insertMySQL_shakeTest(storageTime_v1,shakeOne,shakeTwo,shakeThree,voltagetest,nodeid);


                      dataBaseBackup->insertMySQL_doubledata(storageTime_v1,shake1,nodeID,30,timeFinished);

                      dataBaseBackup->insertMySQL_doubledata(storageTime_v1,shake2,nodeID,31,timeFinished);

                      dataBaseBackup->insertMySQL_doubledata(storageTime_v1,shake3,nodeID,32,timeFinished);

                      dataBaseBackup->insertMySQL_doubledata(storageTime_v1,fVoltage,nodeID,36,timeFinished);

                      //�洢ԭʼ����
                      dataBaseBackup->insertMySQL_shakeTest(storageTime_v1,shakeOne,shakeTwo,shakeThree,voltagetest,nodeid);

                     this->storageNum++;//���������
                     emit dataInfo(id,gateway,totalFlow,storageNum,exceptNum,storageTime_v2,nodeID,arriveTime);//ʵʱ��ʾ

                 }
                 else if(this->nodetype==7)//������ʪ��
                 {
                     unlockPocket_airHumidity(buffer_data.data(),buffer_data.size(),nodeID,tem,hum);
                     dataBase->insertMySQL_doubledata(storageTime_v1,tem,nodeID,40,timeFinished);
                     dataBase->insertMySQL_doubledata(storageTime_v1,hum,nodeID,41,timeFinished);

                     dataBaseBackup->insertMySQL_doubledata(storageTime_v1,tem,nodeID,40,timeFinished);
                     dataBaseBackup->insertMySQL_doubledata(storageTime_v1,hum,nodeID,41,timeFinished);

                     emit dataInfo(id,gateway,totalFlow,storageNum,exceptNum,storageTime_v2,nodeID,arriveTime);//ʵʱ��ʾ

                 }
                 else if(this->nodetype>7)
                 {
                    this->exceptNum++;
                     //�������Ե���־�ļ�
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
            else if(value==4)//3��ʾack��4��ʾ�д��浽shakestate���ݱ���
            {
                current_nodeid=0;
                for(int k=0;k<4;k++)
                {
                    current_nodeid=current_nodeid*16+switchNum(buf[4+k]);

                }

                dataBase->insertMySQL_shakestate(storageTime_v1,current_nodeid);

                dataBaseBackup->insertMySQL_shakestate(storageTime_v1,current_nodeid);

                this->storageNum++;//���������
            }
            else if (value!=3)//δ֪�Ľڵ����ݰ�
            {

                //flag=false;
                this->exceptNum++;//��¼�쳣��
                //�������Ե���־�ļ�
                 //�������Ե���־�ļ�
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
    QDateTime atime=QDateTime::currentDateTime();//��ȡϵͳ���ڵ�ʱ��
    arriveTime=atime.toString("yyyy-MM-dd hh:mm:ss"); //������ʾ��ʽ
}

void MyThread::setDBSeverBackup(DBServer* dataBase)
{
    this->dataBaseBackup=dataBase;
}

void MyThread::setTimeStamp()
{
    standardtime=QDateTime::currentDateTime();
    arriveTime=standardtime.toString("yyyy-MM-dd hh:mm:ss"); //������ʾ��ʽ
    this->timeStamp=standardtime.toString("yyyyMMddhhmmsszzz");//�ڽ�������ʱ����ʱ���
    this->timeStamp+="000";
    this->timeStamp_v1=arriveTime;
    emit updatetimeStamp(timeStamp_v1,arriveTime);
}
