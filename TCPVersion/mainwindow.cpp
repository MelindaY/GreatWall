#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "myserver.h"
#include "config.h"
#include<QTextCodec>   //Qt���ַ�����ͷ�ļ�
#include <Qpalette>
#include <qdatetime.h>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    setWindowFlags(windowFlags() | Qt::WindowMaximizeButtonHint);
    setWindowFlags(windowFlags() | Qt::WindowMinimizeButtonHint);

    ui->setupUi(this);
    //���ñ��
    //��ʼ�����
     ui->tableWidget_userlist->setWindowTitle("user list");  //����
     ui->tableWidget_userlist->setEditTriggers(QAbstractItemView::NoEditTriggers);   //����ֹ�༭
     ui->tableWidget_userlist->setSelectionBehavior(QAbstractItemView::SelectRows);  //����ѡ�еķ�ʽ
    //����������
     ui->tableWidget_userlist->setColumnCount(7);
     ui->tableWidget_userlist->setRowCount(0);
    //����ÿ�п�
     ui->tableWidget_userlist->setColumnWidth(0,80);
     ui->tableWidget_userlist->setColumnWidth(1,80);
     ui->tableWidget_userlist->setColumnWidth(2,80);
     ui->tableWidget_userlist->setColumnWidth(3,100);
     ui->tableWidget_userlist->setColumnWidth(4,100);
     ui->tableWidget_userlist->setColumnWidth(5,80);
     ui->tableWidget_userlist->setColumnWidth(6,100);
     ui->tableWidget_userlist->setColumnWidth(7,80);
     //���ñ�ͷ
     QTextCodec::setCodecForTr(QTextCodec::codecForName("gbk"));//ʹ������ʾ����ʱ����������
     QStringList header;
     header.append(QObject::tr("gateway"));
     //header.append(QObject::tr("������"));
     header.append(QObject::tr("total flow"));
     header.append(QObject::tr("storageNum"));
     header.append(QObject::tr("exceptionNum"));
     header.append(QObject::tr("storageTime"));
     header.append(QObject::tr("nodeID"));
     header.append(QObject::tr("access time"));
    // header.append(QObject::tr("voltage"));
     ui->tableWidget_userlist->setHorizontalHeaderLabels(header);
     //�������ñ��
     //����������ɫ
     //ui->groupBox_4->setStyleShcalTempInteet("color:black");
     ui->groupBox_5->setStyleSheet("color:black");
     ui->groupBox->setStyleSheet("color:black");
     //����������ɫ����
     //��ʼ��һ���̣߳�������źźͲ���������
     this->server=new MyServer;
     server->setConfig(config);
     connect(server,SIGNAL(updateLinkNum(int)),this,SLOT(recvupdateLinkNum(int)));
     connect(server,SIGNAL(server_dataInfo(int,QString,int,int,int,QString,int,QString)),this,SLOT(updateDataInfo(int,QString,int,int,int,QString,int,QString)));
     connect(server,SIGNAL(newClientComing()),this,SLOT(recvNewClientComing()));
     connect(server,SIGNAL(RuningState(QString)),this,SLOT(recvRuningState(QString)));
     connect(server,SIGNAL(stateInfo(QString)),this,SLOT(recvStateInfo(QString)));
     connect(server,SIGNAL(updateTimeStamp(QString,QString)),this,SLOT(recvTimeStamp(QString,QString)));

     //��ʼ�������ļ�����
     this->config=new Config;
     //config->loadConfig("D:\\Qt\\qtcreator-2.4.1\\MyServer\\config.ini");
     config->loadConfig("config.ini");
     ui->serverState_2->addItem("Reading configuration file success !");
     //ui->ledPort->setText(config->getPort());
     QDateTime currenttime=QDateTime::currentDateTime();//��ȡϵͳ���ڵ�ʱ��
     QString startingTime=currenttime.toString("yyyy-MM-dd hh:mm:ss"); //������ʾ��ʽ
     ui->ledStartTime->setText(startingTime);
     server->createDataBase(config);
     server->listen(QHostAddress::Any,(config->getPort()).toUInt());

     //��ʱ��ÿ���Ӹ���ʱ��
     timer = new QTimer();
     connect(timer, SIGNAL(timeout()), this, SLOT(qtimeSlot()));
     timer->start(1000);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::recvRuningState(QString info)
{
    ui->ledRunState->setText(info);
}

void MainWindow::recvStateInfo(QString state)
{
    ui->serverState_2->addItem(state);
}

void MainWindow::updateDataInfo(int id,QString gateway,int totalFlow,int storageNum,int exceptNum,QString storageTime,int nodeID,QString arriveTime)
{
    ui->tableWidget_userlist->setItem(id,0,new QTableWidgetItem(gateway));
    ui->tableWidget_userlist->setItem(id,1,new QTableWidgetItem(QString::number(totalFlow)));
    ui->tableWidget_userlist->setItem(id,2,new QTableWidgetItem(QString::number(storageNum)));
    ui->tableWidget_userlist->setItem(id,3,new QTableWidgetItem(QString::number(exceptNum)));
    ui->tableWidget_userlist->setItem(id,4,new QTableWidgetItem(storageTime));
    ui->tableWidget_userlist->setItem(id,5,new QTableWidgetItem(QString::number(nodeID)));
    ui->tableWidget_userlist->setItem(id,6,new QTableWidgetItem(arriveTime));
    //ui->tableWidget_userlist->setItem(id,7,new QTableWidgetItem(QString::number(fVoltage)));
}

void MainWindow::recvNewClientComing()
{
    int row = ui->tableWidget_userlist->rowCount(); //��ȡ�������
    ui->tableWidget_userlist->setRowCount(row+1);  //����һ��
}

void MainWindow::on_ExitBotton_2_clicked()
{
    exit(0);
}

void MainWindow::qtimeSlot()
{
    QTime qtimeObj = QTime::currentTime();
    QString strTime = qtimeObj.toString("h:m ap");
    QDate qdateObj = QDate::currentDate();
    QString strDate = qdateObj.toString("yyyy-MM-dd"); //���ڡ��·ݡ��졢��
    strDate.append(" ");

    strDate.append(strTime);
    ui->label_currentTime->setText (strDate);
}
void MainWindow::recvTimeStamp(QString stampTime,QString currentTime)
{
    ui->label_beat->setText(stampTime);
    ui->label_nextArrival->setText(currentTime);
}
void MainWindow::recvupdateLinkNum(int linkNum)
{
    ui->label_linkNum->setText(QString::number(linkNum));
}
