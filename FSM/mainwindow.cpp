#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->serverTest=new server();
    serverTest->connectHost();
}

MainWindow::~MainWindow()
{
    delete ui;
}


