#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QMessageBox>
#include <QFileDialog>
#include <QTime>
#include <QTimer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    QStringList usableCom;       //可用串口列表
    QSerialPort *myCOM;        //串口标志
    QTimer *myTimer;           //串口读写超时定时标志

    QByteArray COMrecebuf;

    void do_setUi();             //设置界面

    //打开串口并验证
    bool openCom(QSerialPort *myCom,QString &myportName,qint32 baudRate);
    void showMyMessage(QString str);  //显示信息

private slots:
    void myCOM_rece();               //串口数据接收函数
    void TimerUpdate_COM();          //串口读写超时定时处理

    void on_Open_clicked();
    void on_Close_clicked();
    void on_Send_clicked();
    void on_actions_exit_triggered();
};

#endif // MAINWINDOW_H
