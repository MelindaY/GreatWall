#include "myserver.h"
#include<QFile>
#include<QDateTime>
#include<QDebug>
myServer::myServer(QObject *parent):
    QUdpSocket(parent)
{
    this->udpSocket=new QUdpSocket(this);
    this->config=new Config;
    this->dataBase=new DBServer(config);
    config->loadConfig("config.ini");
    udpSocket->bind((config->getPort()).toUInt());
    if (udpSocket->state()==QUdpSocket::ConnectedState)
    {
        QString inf="Connected";
        emit updateData(inf);
    }
     else
    {
        QString inf="No";
        emit updateData(inf);
    }

    this->buf.clear();
    this->totalFlow=0;//������
    this->storageNum=0;//�����
    this->exceptNum=0;//�쳣��
    this->nodeID=0;
    this->value=0;

    connect(udpSocket, SIGNAL(readyRead()),
            this, SLOT(readPendingDatagrams()));
}



void myServer::readPendingDatagrams()
{

    while (udpSocket->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(datagram.data(), datagram.size());
        buf+=datagram.toHex();

        QFile file(config->getPort()+"Exception.txt");
        if(file.open(QIODevice::WriteOnly|QIODevice::WriteOnly|QIODevice::Append))
        {
            QTextStream fileOut(&file);
            QDateTime currenttime=QDateTime::currentDateTime();//��ȡϵͳ���ڵ�ʱ��
            exceptTime=currenttime.toString("yyyy-MM-dd hh:mm:ss"); //������ʾ��ʽ
            fileOut<<exceptTime+"   "+buf.data()+"   "+QString::number(buf.size())<<"\r\n";
            file.close();
        }
        translateData(buf);
        emit updateData(datagram.data());
    }
}
void myServer::setConfig(Config* config)
{
    this->config=config;
}
void myServer::createDataBase(Config* config)
{
    this->dataBase=new DBServer(config);
    //�������ݿ�
    if(dataBase->connectToMySQL()==false)
    {
        //emit RuningState("error");
    }
    else
    {
//        emit stateInfo("Connect to dataBase Success!");
//        emit stateInfo("Receive port:"+config->getPort());
//        emit stateInfo("IP:"+config->getIP());
//        emit stateInfo("Mysql_user:"+config->getMysql_user());
//        emit stateInfo("getMysql_dbname:"+config->getMysql_user());
//        emit RuningState("right");
    }
    dataBase->createTable();
}
void myServer::translateData(QByteArray data)
{
    char buf[1024];
     strcpy(buf,data.data());
    for(int k=0;k<2;k++)
   {
       value=value*16+switchNum(buf[8+k]);
   }
    if(data=="0"||data==""||data.size()!=26*2)
    {
        this->exceptNum++;//��¼�쳣��
        //�������Ե���־�ļ�
        QFile file(config->getPort()+"Exception.txt");
        if(file.open(QIODevice::WriteOnly|QIODevice::WriteOnly|QIODevice::Append))
        {
            QTextStream fileOut(&file);
            QDateTime currenttime=QDateTime::currentDateTime();//��ȡϵͳ���ڵ�ʱ��
            exceptTime=currenttime.toString("yyyy-MM-dd hh:mm:ss"); //������ʾ��ʽ
            fileOut<<exceptTime+"   "+data.data()+"   "+QString::number(data.size())<<"\r\n";
            file.close();
        }

    }
    else if(data.size()==26*2)
    {
        if(value==1)
        {

            //��ʼ�������ݰ�  data.data()�ǽ�QByteArray����ת����char*���ͣ�������˵QByteArray����data()
             this->controlPac=buf;

             QDateTime atime=QDateTime::currentDateTime();//��ȡϵͳ���ڵ�ʱ��
             storageTime_v1=atime.toString("yyyyMMddhhmmsszzz"); //������ʾ��ʽ
             storageTime_v1+="000";
             storageTime_v2=atime.toString("yyyy-MM-dd hh:mm:ss"); //������ʾ��ʽ

            unlockControlPac(data.data(),data.size(),nodeID,countControl,fVoltage);
            dataBase->insertMySQL_testControl(storageTime_v1,countControl,nodeID,fVoltage);
            this->storageNum+=1;//���������
            //emit dataInfo(id,gateway,totalFlow,storageNum,exceptNum,storageTime_v2,nodeID,arriveTime);//ʵʱ��ʾ
             }
        else if(value==2)//&&flag==true&&current_nodeid==nodeID)
        {
            this->stateFlag=0;

            for(int k=0;k<4;k++)
            {
                this->nodeID=0;
                this->nodeID=this->nodeID*16+switchNum(buf[4+k]);
                QDateTime atime=QDateTime::currentDateTime();//��ȡϵͳ���ڵ�ʱ��
                storageTime_v1=atime.toString("yyyyMMddhhmmsszzz"); //������ʾ��ʽ
                storageTime_v1+="000";
                dataBase->insertMySQL_testData(storageTime_v1,this->nodeID,value);
            }
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
            unlockPocket_TemHumLight(data.data(),data.size(),nodeID,fVoltage,fLight,fTemperature,fHumidity);
            QDateTime atime=QDateTime::currentDateTime();//��ȡϵͳ���ڵ�ʱ��
            storageTime_v2=atime.toString("yyyy-MM-dd hh:mm:ss"); //������ʾ��ʽ
            storageTime_v1=atime.toString("yyyyMMddhhmmsszzz"); //������ʾ��ʽ
            storageTime_v1+="000";
            dataBase->insertMySQL_doubledata(storageTime_v1,fVoltage,nodeID,6);//�������ݿ�
            dataBase->deleteItem_data(nodeID,6);
            dataBase->insertMySQL_datanewest(storageTime_v1,fVoltage,nodeID,6,stateFlag);

            dataBase->insertMySQL_doubledata(storageTime_v1,fLight,nodeID,4);
            dataBase->deleteItem_data(nodeID,4);
            dataBase->insertMySQL_datanewest(storageTime_v1,fLight,nodeID,4,stateFlag);

            dataBase->insertMySQL_doubledata(storageTime_v1,fTemperature,nodeID,1);
            dataBase->deleteItem_data(nodeID,1);
            dataBase->insertMySQL_datanewest(storageTime_v1,fTemperature,nodeID,1,stateFlag);

            dataBase->insertMySQL_doubledata(storageTime_v1,fHumidity,nodeID,2);
            dataBase->deleteItem_data(nodeID,2);
            dataBase->insertMySQL_datanewest(storageTime_v1,fHumidity,nodeID,2,stateFlag);
            //                    dataBase->insertMySQL_hop(storageTime_v1,hop,nodeID,PacketNum);//�����ư���Ϣ�������ݿ�
            this->storageNum+=1;//���������
           // emit dataInfo(id,gateway,totalFlow,storageNum,exceptNum,storageTime_v2,nodeID,arriveTime);//ʵʱ��ʾ
            }
            //
            else// if(stateFlag==0)
            {
            //
             if(this->nodetype==2)//�������괫�����ڵ�
             {
                 unlockPocket_rain(data.data(),data.size(),nodeID,rainnode_addr,tem_outside,tem_inside,ph,rainfall,windspeed,winddirection,powervoltage,rain,fVoltage);
                 //������ⲿ�¶Ȳ���-30.00~80.00���䣬phֵ����3.0~7.0����������0.1~8.0�����ٲ���0.00~30.00��������0~359����ѹ����9.00~12.60���Ƿ��겻��0Ҳ����1������Ϊ�쳣���ݶ���
                 QDateTime atime=QDateTime::currentDateTime();//��ȡϵͳ���ڵ�ʱ��
                 storageTime_v2=atime.toString("yyyy-MM-dd hh:mm:ss"); //������ʾ��ʽ
                 storageTime_v1=atime.toString("yyyyMMddhhmmsszzz"); //������ʾ��ʽ
                 storageTime_v1+="000";
                 //if((tem_outside+30.00)>eps&&(80.00-tem_outside)>eps&&(tem_inside+30.00)>eps&&(80.00-tem_inside)>eps&&(ph-3.0)>eps&&(7.0-ph)>eps&&(rainfall-0.1)>eps&&(8.0-rainfall)>eps&&(windspeed-0.0)>eps&&(30.0-windspeed)>eps&&(winddirection-0)>eps&&(359-winddirection)>eps&&(powervoltage-9.0)>eps&&(12.6-powervoltage)>eps&&(rain==1||rain==0))
                {
                     dataBase->insertMySQL_doubledata(storageTime_v1,(double)rainnode_addr,nodeID,9);//�������ݿ�
                     dataBase->deleteItem_data(nodeID,9);
                     dataBase->insertMySQL_datanewest(storageTime_v1,(double)rainnode_addr,nodeID,9,stateFlag);

                     dataBase->insertMySQL_doubledata(storageTime_v1,tem_outside,nodeID,10);
                     dataBase->deleteItem_data(nodeID,10);
                     dataBase->insertMySQL_datanewest(storageTime_v1,tem_outside,nodeID,10,stateFlag);

                     dataBase->insertMySQL_doubledata(storageTime_v1,tem_inside,nodeID,11);
                     dataBase->deleteItem_data(nodeID,11);
                     dataBase->insertMySQL_datanewest(storageTime_v1,tem_inside,nodeID,11,stateFlag);

                     dataBase->insertMySQL_doubledata(storageTime_v1,ph,nodeID,12);
                     dataBase->deleteItem_data(nodeID,12);
                     dataBase->insertMySQL_datanewest(storageTime_v1,ph,nodeID,12,stateFlag);

                     dataBase->insertMySQL_doubledata(storageTime_v1,rainfall,nodeID,13);
                     dataBase->deleteItem_data(nodeID,13);
                     dataBase->insertMySQL_datanewest(storageTime_v1,rainfall,nodeID,13,stateFlag);

                     dataBase->insertMySQL_doubledata(storageTime_v1,windspeed,nodeID,14);
                     dataBase->deleteItem_data(nodeID,14);
                     dataBase->insertMySQL_datanewest(storageTime_v1,windspeed,nodeID,14,stateFlag);

                     dataBase->insertMySQL_doubledata(storageTime_v1,winddirection,nodeID,15);
                     dataBase->deleteItem_data(nodeID,15);
                     dataBase->insertMySQL_datanewest(storageTime_v1,winddirection,nodeID,15,stateFlag);

                     dataBase->insertMySQL_doubledata(storageTime_v1,powervoltage,nodeID,16);
                     dataBase->deleteItem_data(nodeID,16);
                     dataBase->insertMySQL_datanewest(storageTime_v1,powervoltage,nodeID,16,stateFlag);

                     dataBase->insertMySQL_doubledata(storageTime_v1,(double)rain,nodeID,17);
                     dataBase->deleteItem_data(nodeID,17);
                     dataBase->insertMySQL_datanewest(storageTime_v1,(double)rain,nodeID,17,stateFlag);

                     dataBase->insertMySQL_doubledata(storageTime_v1,fVoltage,nodeID,33);
                     dataBase->deleteItem_data(nodeID,33);
                     dataBase->insertMySQL_datanewest(storageTime_v1,fVoltage,nodeID,33,stateFlag);

                     this->storageNum++;//���������
                  //   emit dataInfo(id,gateway,totalFlow,storageNum,exceptNum,storageTime_v2,nodeID,arriveTime);//ʵʱ��ʾ
                 }
             }
             else if(this->nodetype==3)//������ʪ�ȴ������ڵ�
             {

                 unlockPocket_sioltemhum(data.data(),data.size(),nodeID,addressCode,tem_30cm,hum_30cm,tem_15cm,hum_15cm,tem_5cm,hum_5cm,fVoltage);
                 //������Զ��ʦҪ���½�soilTempHumi���ݱ��洢ԭʼ������ʪ������
                 int addrCode1,addrCode2,symbol_30cm,temp1_30cm,temp2_30cm,humi1_30cm,humi2_30cm,symbol_15cm,temp1_15cm,temp2_15cm,humi1_15cm,humi2_15cm,symbol_5cm,temp1_5cm,temp2_5cm,humi1_5cm,humi2_5cm,nodeid,voltagetest;
                 unlockPocket_sioltemphumiTest(data.data(),data.size(),addrCode1,addrCode2,symbol_30cm,temp1_30cm,temp2_30cm,humi1_30cm,humi2_30cm,symbol_15cm,temp1_15cm,temp2_15cm,humi1_15cm,humi2_15cm,symbol_5cm,temp1_5cm,temp2_5cm,humi1_5cm,humi2_5cm,voltagetest,nodeid);

                 //������Զ��ʦҪ��
                 QDateTime atime=QDateTime::currentDateTime();//��ȡϵͳ���ڵ�ʱ��
                 storageTime_v2=atime.toString("yyyy-MM-dd hh:mm:ss"); //������ʾ��ʽ
                 storageTime_v1=atime.toString("yyyyMMddhhmmsszzz"); //������ʾ��ʽ
                 storageTime_v1+="000";
                 dataBase->insertMySQL_doubledata(storageTime_v1,(double)addressCode,nodeID,18);//�������ݿ�
                 dataBase->deleteItem_data(nodeID,18);
                 dataBase->insertMySQL_datanewest(storageTime_v1,(double)addressCode,nodeID,18,stateFlag);

                 dataBase->insertMySQL_doubledata(storageTime_v1,tem_30cm,nodeID,19);
                 dataBase->deleteItem_data(nodeID,19);
                 dataBase->insertMySQL_datanewest(storageTime_v1,tem_30cm,nodeID,19,stateFlag);

                 dataBase->insertMySQL_doubledata(storageTime_v1,hum_30cm,nodeID,20);
                 dataBase->deleteItem_data(nodeID,20);
                 dataBase->insertMySQL_datanewest(storageTime_v1,hum_30cm,nodeID,20,stateFlag);

                 dataBase->insertMySQL_doubledata(storageTime_v1,tem_15cm,nodeID,21);
                 dataBase->deleteItem_data(nodeID,21);
                 dataBase->insertMySQL_datanewest(storageTime_v1,tem_15cm,nodeID,21,stateFlag);

                 dataBase->insertMySQL_doubledata(storageTime_v1,hum_15cm,nodeID,22);
                 dataBase->deleteItem_data(nodeID,22);
                 dataBase->insertMySQL_datanewest(storageTime_v1,hum_15cm,nodeID,22,stateFlag);

                 dataBase->insertMySQL_doubledata(storageTime_v1,tem_5cm,nodeID,23);
                 dataBase->deleteItem_data(nodeID,23);
                 dataBase->insertMySQL_datanewest(storageTime_v1,tem_5cm,nodeID,23,stateFlag);

                 dataBase->insertMySQL_doubledata(storageTime_v1,hum_5cm,nodeID,24);
                 dataBase->deleteItem_data(nodeID,24);
                 dataBase->insertMySQL_datanewest(storageTime_v1,hum_5cm,nodeID,24,stateFlag);

                 dataBase->insertMySQL_doubledata(storageTime_v1,fVoltage,nodeID,34);
                 dataBase->deleteItem_data(nodeID,34);
                 dataBase->insertMySQL_datanewest(storageTime_v1,fVoltage,nodeID,34,stateFlag);

                 //������ʦԭʼʮ�������ݲ������ݿ�
                 dataBase->insertMySQL_sioltemphumiTest(storageTime_v1,addrCode1,addrCode2,symbol_30cm,temp1_30cm,temp2_30cm,humi1_30cm,humi2_30cm,symbol_15cm,temp1_15cm,temp2_15cm,humi1_15cm,humi2_15cm,symbol_5cm,temp1_5cm,temp2_5cm,humi1_5cm,humi2_5cm,voltagetest,nodeid);
                 //�������
                 this->storageNum++;//���������
               //  emit dataInfo(id,gateway,totalFlow,storageNum,exceptNum,storageTime_v2,nodeID,arriveTime);//ʵʱ��ʾ
             }
             else if(this->nodetype==4)//�ηִ������ڵ�
             {
                 unlockPocket_salt(data.data(),data.size(),nodeID,saltaddrcode,tem,hum,verticalSalt,surfaceSalt,fVoltage);

                 //�½�salttest���ݱ��洢ԭʼ�η�����
                 int addrCode,symbol_30cm,temp_30cm,humi_30cm,vertSalt,surfSalt,voltagetest,nodeid;
                 unlockPocket_saltTest(data.data(),data.size(),addrCode,symbol_30cm,temp_30cm,humi_30cm,vertSalt,surfSalt,voltagetest,nodeid);

                 QDateTime atime=QDateTime::currentDateTime();//��ȡϵͳ���ڵ�ʱ��
                 storageTime_v2=atime.toString("yyyy-MM-dd hh:mm:ss"); //������ʾ��ʽ
                 storageTime_v1=atime.toString("yyyyMMddhhmmsszzz"); //������ʾ��ʽ
                 storageTime_v1+="000";
                 dataBase->insertMySQL_doubledata(storageTime_v1,(double)saltaddrcode,nodeID,25);//�������ݿ�
                 dataBase->deleteItem_data(nodeID,25);
                 dataBase->insertMySQL_datanewest(storageTime_v1,(double)saltaddrcode,nodeID,25,stateFlag);

                 dataBase->insertMySQL_doubledata(storageTime_v1,tem,nodeID,26);
                 dataBase->deleteItem_data(nodeID,26);
                 dataBase->insertMySQL_datanewest(storageTime_v1,tem,nodeID,26,stateFlag);

                 dataBase->insertMySQL_doubledata(storageTime_v1,hum,nodeID,27);
                 dataBase->deleteItem_data(nodeID,27);
                 dataBase->insertMySQL_datanewest(storageTime_v1,hum,nodeID,27,stateFlag);

                 dataBase->insertMySQL_doubledata(storageTime_v1,verticalSalt,nodeID,28);
                 dataBase->deleteItem_data(nodeID,28);
                 dataBase->insertMySQL_datanewest(storageTime_v1,verticalSalt,nodeID,28,stateFlag);

                 dataBase->insertMySQL_doubledata(storageTime_v1,surfaceSalt,nodeID,29);
                 dataBase->deleteItem_data(nodeID,29);
                 dataBase->insertMySQL_datanewest(storageTime_v1,surfaceSalt,nodeID,29,stateFlag);

                 dataBase->insertMySQL_doubledata(storageTime_v1,fVoltage,nodeID,35);
                 dataBase->deleteItem_data(nodeID,35);
                 dataBase->insertMySQL_datanewest(storageTime_v1,fVoltage,nodeID,35,stateFlag);

                 //��ԭʼ�η����ݲ������ݿ�
                 dataBase->insertMySQL_saltTest(storageTime_v1,addrCode,symbol_30cm,temp_30cm,humi_30cm,vertSalt,surfSalt,voltagetest,nodeid);

                 this->storageNum++;//���������
               //  emit dataInfo(id,gateway,totalFlow,storageNum,exceptNum,storageTime_v2,nodeID,arriveTime);//ʵʱ��ʾ
             }
             else if(this->nodetype==6)//�𶯴������ڵ�
             {
                 unlockPocket_shake(data.data(),data.size(),nodeID,shake1,shake2,shake3,fVoltage);

                 //����ԭʼ����
                 int shakeOne,shakeTwo,shakeThree,voltagetest,nodeid;
                 unlockPocket_shakeTest(data.data(),data.size(),shakeOne,shakeTwo,shakeThree,voltagetest,nodeid);

                 QDateTime atime=QDateTime::currentDateTime();//��ȡϵͳ���ڵ�ʱ��
                 storageTime_v2=atime.toString("yyyy-MM-dd hh:mm:ss"); //������ʾ��ʽ
                 storageTime_v1=atime.toString("yyyyMMddhhmmsszzz"); //������ʾ��ʽ
                 storageTime_v1+="000";

                 dataBase->insertMySQL_doubledata(storageTime_v1,shake1,nodeID,30);
                 dataBase->deleteItem_data(nodeID,30);
                 dataBase->insertMySQL_datanewest(storageTime_v1,shake1,nodeID,30,stateFlag);

                 dataBase->insertMySQL_doubledata(storageTime_v1,shake2,nodeID,31);
                 dataBase->deleteItem_data(nodeID,31);
                 dataBase->insertMySQL_datanewest(storageTime_v1,shake2,nodeID,31,stateFlag);

                 dataBase->insertMySQL_doubledata(storageTime_v1,shake3,nodeID,32);
                 dataBase->deleteItem_data(nodeID,32);
                 dataBase->insertMySQL_datanewest(storageTime_v1,shake3,nodeID,32,stateFlag);

                 dataBase->insertMySQL_doubledata(storageTime_v1,fVoltage,nodeID,36);
                 dataBase->deleteItem_data(nodeID,36);
                 dataBase->insertMySQL_datanewest(storageTime_v1,fVoltage,nodeID,36,stateFlag);

                 //�洢ԭʼ����
                  dataBase->insertMySQL_shakeTest(storageTime_v1,shakeOne,shakeTwo,shakeThree,voltagetest,nodeid);

                 this->storageNum++;//���������
             //    emit dataInfo(id,gateway,totalFlow,storageNum,exceptNum,storageTime_v2,nodeID,arriveTime);//ʵʱ��ʾ

             }
             else if(this->nodetype==7)//������ʪ��
             {
                 unlockPocket_airHumidity(data.data(),data.size(),nodeID,tem,hum);
                 dataBase->insertMySQL_doubledata(storageTime_v1,tem,nodeID,40);
                 dataBase->insertMySQL_doubledata(storageTime_v1,hum,nodeID,41);

             }
             else if(this->nodetype>7)
             {
                this->exceptNum++;
                 //�������Ե���־�ļ�
               QFile file(config->getPort()+"Exception.txt");
               if(file.open(QIODevice::WriteOnly|QIODevice::WriteOnly|QIODevice::Append))
               {
                  QTextStream fileOut(&file);
                  QDateTime currenttime=QDateTime::currentDateTime();//��ȡϵͳ���ڵ�ʱ��
                  exceptTime=currenttime.toString("yyyy-MM-dd hh:mm:ss"); //������ʾ��ʽ
                  fileOut<<exceptTime+"   "+data.data()+"   "+QString::number(data.size())<<"\r\n";
                  file.close();
               }
             }
            }
        }
        else if(value==4)//3��ʾack��4��ʾ�д��浽shakestate���ݱ���
        {
            QDateTime atime=QDateTime::currentDateTime();//��ȡϵͳ���ڵ�ʱ��
            storageTime_v1=atime.toString("yyyyMMddhhmmsszzz"); //������ʾ��ʽ
            storageTime_v1+="000";


            dataBase->insertMySQL_shakestate(storageTime_v1,this->nodeID);

            this->storageNum++;//���������
        }
        else//δ֪�Ľڵ����ݰ�
        {

            //flag=false;
            this->exceptNum++;//��¼�쳣��
            //�������Ե���־�ļ�
             //�������Ե���־�ļ�
            QFile file(config->getPort()+"Exception.txt");
            if(file.open(QIODevice::WriteOnly|QIODevice::WriteOnly|QIODevice::Append))
           {
               QTextStream fileOut(&file);
               QDateTime currenttime=QDateTime::currentDateTime();//��ȡϵͳ���ڵ�ʱ��
               exceptTime=currenttime.toString("yyyy-MM-dd hh:mm:ss"); //������ʾ��ʽ
               fileOut<<exceptTime+"   "+data.data()+"   "+QString::number(data.size())<<"\r\n";
               file.close();
           }

        }
    }
}
