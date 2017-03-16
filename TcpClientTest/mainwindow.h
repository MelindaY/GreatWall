#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}
class iothread;
class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    void on_btnConnect_clicked();
    void on_btnSendForever_clicked();
    void on_btnDisConnect_clicked();
    void on_btnExit_clicked();

private:
    Ui::MainWindow *ui;
    iothread* thread;
};

#endif // MAINWINDOW_H
