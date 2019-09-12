#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QDebug"
#define DEBUG 1
/*
 *
 *
 QStringList usableCom;       //可用串口列表
    QSerialPort *myCOM;        //串口标志
    QTimer *myTimer;           //串口读写超时定时标志

    QByteArray COMrecebuf;
 ****************/
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //先自动识别可用串口,并储存
    usableCom.clear();
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        //usableCom<<info.portName();
        if(info.isBusy()==true)
        {
            qDebug()<<"串口忙";
        }
        else
        {
            usableCom<<info.description()+"("+info.portName()+")";
            qDebug()<<"info.description() is "<<info.description();
            qDebug()<<"info.portName() is "<<info.portName();
            qDebug()<<"info.serialNumber() is "<<info.serialNumber();
            qDebug()<<"info.systemLocation() is "<<info.systemLocation();
            qDebug()<<"info.vendorIdentifier() is "<<info.vendorIdentifier();
        }
    }
    do_setUi();

    myCOM = new QSerialPort();//建立串口描述符标志
    myTimer = new QTimer();    //建立串口读写超时定时器
    connect(myCOM,SIGNAL(readyRead()),this,SLOT(myCOM_rece()));
    connect(myTimer,SIGNAL(timeout()),this,SLOT(TimerUpdate_COM()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

//设置界面
void MainWindow::do_setUi()
{
    int i = 0;
    ui->myCOM->clear();
    ui->myCOM->addItem("");
    for(i=0;i<usableCom.size();i++)
    {
        ui->myCOM->addItem(usableCom.at(i));
    }
    ui->Baudrate->setCurrentIndex(6);
    ui->Close->setEnabled(false);
    ui->Open->setEnabled(true);
    ui->Send->setEnabled(false);
}

//打开串口并验证
bool MainWindow::openCom(QSerialPort *myCom, QString &myportName, qint32 baudRate)
{
    //获取串口号,此处根据自己的放肆进行编写
    QString str = "COM"+myportName.section("(COM",1).remove(")");
    qDebug()<<"str=  "<<str;
    myCom->setPortName(str);
    //设置波特率
    qDebug()<<"baudRate=  "<<baudRate;
    if(baudRate == 0)
    {
        QMessageBox::information(this, tr("打开失败"), tr("波特率错误"));
        return false;
    }
    //验证
    if(! myCom->open(QIODevice::ReadWrite))
    {
        myCom->close();
         QMessageBox::information(this, tr("打开失败"), tr("该串口已被占用"));
        return false;
    }
    myCom->setBaudRate(baudRate);
    //设置数据位
    myCom->setDataBits(QSerialPort::Data8);
    //设置校验
    myCom->setParity(QSerialPort::NoParity);
    //设置停止位
    myCom->setStopBits(QSerialPort::OneStop);
    //设置数据流控制
    myCom->setFlowControl(QSerialPort::NoFlowControl);

    return true;
}

//显示信息
void MainWindow::showMyMessage(QString str)
{

    QDateTime time = QDateTime::currentDateTime();//获取系统现在的时间
    QString timeType = time.toString("hh:mm:ss："); //设置显示格式
    //显示接收到的数据时的时间
    ui->textBrowser->setText(ui->textBrowser->document()->toPlainText()+timeType+str+"\n");
    QTextCursor cursor = ui->textBrowser->textCursor();
    cursor.movePosition(QTextCursor::End);
    ui->textBrowser->setTextCursor(cursor);//鼠标光标移动
}

//串口数据接收函数
void MainWindow::myCOM_rece()
{
    myTimer->start(100);            //一有数据过来，立即启动定时器接收，100毫秒接收完毕，可自行设置
    COMrecebuf.append(myCOM->readAll());
}

//串口读写超时定时处理
void MainWindow::TimerUpdate_COM()
{
    myTimer->stop();
    if(COMrecebuf.length()!=0)
    {
        showMyMessage(COMrecebuf);//有数据则显示
    }
    COMrecebuf.clear();

}

void MainWindow::on_Open_clicked()
{

    //先打开串口
    QString portName =ui->myCOM->currentText();
    if(portName == NULL)
    {
        QMessageBox::information(this, tr("打开失败"), tr("该串口设备不存在或被占用"));
        return;
    }
    if(! openCom(myCOM,portName,ui->Baudrate->currentText().toInt()))
    {

        if(myCOM->isOpen())
        {
            myCOM->close();
        }

        return;
    }
    ui->Send->setEnabled(true);
    ui->Open->setEnabled(false);
    ui->Close->setEnabled(true);
}

void MainWindow::on_Close_clicked()
{
    ui->Send->setEnabled(false);
    ui->Open->setEnabled(true);
    ui->Close->setEnabled(false);
    if(myCOM->isOpen())
    {
        myCOM->close();
    }
}

void MainWindow::on_Send_clicked()
{
      myCOM->write(ui->lineEdit->text().toLatin1(),ui->lineEdit->text().length());
}

void MainWindow::on_actions_exit_triggered()
{

}
