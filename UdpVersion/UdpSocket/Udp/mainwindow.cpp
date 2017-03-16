#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
//    connect(server,SIGNAL(updateData(QString)),this,SLOT(recvData(QString)));
    ui->setupUi(this);
    this->server=new myServer;
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::recvData(QString data)
{
    ui->textEdit->setText(data);
}
