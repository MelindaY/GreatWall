#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "myserver.h"
#include "config.h"
#include<QTextCodec>   //Qt的字符编码头文件
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
    //设置表格
    //初始化表格
     ui->tableWidget_userlist->setWindowTitle("user list");  //表名
     ui->tableWidget_userlist->setEditTriggers(QAbstractItemView::NoEditTriggers);   //表格禁止编辑
     ui->tableWidget_userlist->setSelectionBehavior(QAbstractItemView::SelectRows);  //整行选中的方式
    //设置行列数
     ui->tableWidget_userlist->setColumnCount(7);
     ui->tableWidget_userlist->setRowCount(0);
    //设置每列宽
     ui->tableWidget_userlist->setColumnWidth(0,80);
     ui->tableWidget_userlist->setColumnWidth(1,80);
     ui->tableWidget_userlist->setColumnWidth(2,80);
     ui->tableWidget_userlist->setColumnWidth(3,100);
     ui->tableWidget_userlist->setColumnWidth(4,100);
     ui->tableWidget_userlist->setColumnWidth(5,80);
     ui->tableWidget_userlist->setColumnWidth(6,100);
     ui->tableWidget_userlist->setColumnWidth(7,80);
     //设置表头
     QTextCodec::setCodecForTr(QTextCodec::codecForName("gbk"));//使界面显示中文时不出现乱码
     QStringList header;
     header.append(QObject::tr("gateway"));
     //header.append(QObject::tr("连接数"));
     header.append(QObject::tr("total flow"));
     header.append(QObject::tr("storageNum"));
     header.append(QObject::tr("exceptionNum"));
     header.append(QObject::tr("storageTime"));
     header.append(QObject::tr("nodeID"));
     header.append(QObject::tr("access time"));
    // header.append(QObject::tr("voltage"));
     ui->tableWidget_userlist->setHorizontalHeaderLabels(header);
     //结束设置表格
     //设置字体颜色
     //ui->groupBox_4->setStyleShcalTempInteet("color:black");
     ui->groupBox_5->setStyleSheet("color:black");
     ui->groupBox->setStyleSheet("color:black");
     //设置字体颜色结束
     //初始化一级线程，将相关信号和槽连接起来
     this->server=new MyServer;
     server->setConfig(config);
     connect(server,SIGNAL(updateLinkNum(int)),this,SLOT(recvupdateLinkNum(int)));
     connect(server,SIGNAL(server_dataInfo(int,QString,int,int,int,QString,int,QString)),this,SLOT(updateDataInfo(int,QString,int,int,int,QString,int,QString)));
     connect(server,SIGNAL(newClientComing()),this,SLOT(recvNewClientComing()));
     connect(server,SIGNAL(RuningState(QString)),this,SLOT(recvRuningState(QString)));
     connect(server,SIGNAL(stateInfo(QString)),this,SLOT(recvStateInfo(QString)));
     connect(server,SIGNAL(updateTimeStamp(QString,QString)),this,SLOT(recvTimeStamp(QString,QString)));

     //初始化配置文件对象，
     this->config=new Config;
     //config->loadConfig("D:\\Qt\\qtcreator-2.4.1\\MyServer\\config.ini");
     config->loadConfig("config.ini");
     ui->serverState_2->addItem("Reading configuration file success !");
     //ui->ledPort->setText(config->getPort());
     QDateTime currenttime=QDateTime::currentDateTime();//获取系统现在的时间
     QString startingTime=currenttime.toString("yyyy-MM-dd hh:mm:ss"); //设置显示格式
     ui->ledStartTime->setText(startingTime);
     server->createDataBase(config);
     server->listen(QHostAddress::Any,(config->getPort()).toUInt());

     //计时器每分钟更新时钟
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
    int row = ui->tableWidget_userlist->rowCount(); //获取表格行数
    ui->tableWidget_userlist->setRowCount(row+1);  //表格加一行
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
    QString strDate = qdateObj.toString("yyyy-MM-dd"); //星期、月份、天、年
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
