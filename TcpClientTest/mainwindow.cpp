#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "iothread.h"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    thread=new iothread;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnConnect_clicked()
{
    ui->btnConnect->setEnabled(false);
    ui->ledState->setText("connection");
    thread->connectHost();
}

void MainWindow::on_btnSendForever_clicked()
{
    thread->start();
}

void MainWindow::on_btnDisConnect_clicked()
{
    ui->ledState->setText("ready");
    ui->btnConnect->setEnabled("true");
    thread->disconnectHost();
}

void MainWindow::on_btnExit_clicked()
{
    exit(0);
}
