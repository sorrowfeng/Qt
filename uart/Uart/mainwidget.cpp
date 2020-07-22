#include "mainwidget.h"
#include "ui_mainwidget.h"
#include <QSerialPort>
#include <QFileDialog>
#include <QSerialPortInfo>
#include <QMessageBox>
#include <QString>
#include <QDebug>
#define cout qDebug() << "[" << __FILE__ << ": " << __LINE__ << "]"

MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget)
{
    ui->setupUi(this);
    file = new QFile(this);

    // 获取可用的端口号，并传入QStringList
    m_serialPortName  = getPortNameList();
    cout << m_serialPortName;
//    加入所有的端口到下拉框
    ui->comboBoxCom->addItems(m_serialPortName);
//    默认设置为第一个
    ui->comboBoxCom->setCurrentIndex(0);


    m_serialPort = new QSerialPort();//实例化串口类一个对象

    if(m_serialPort->isOpen())//如果串口已经打开了 先给他关闭了
    {
        m_serialPort->clear();
        m_serialPort->close();
    }


    //连接信号槽 当下位机发送数据
    //QSerialPort 会发送个 readyRead 信号
    //定义槽函数void receiveInfo()解析数据
    connect(m_serialPort,&QSerialPort::readyRead,this,&MainWidget::receiveInfo);
}

MainWidget::~MainWidget()
{
    if (m_serialPort->isOpen())
    {
        m_serialPort->clear();
        m_serialPort->close();
    }
    delete m_serialPort;
    delete ui;
}
// 获取所有可用的端口号
QStringList MainWidget::getPortNameList()
{
    QStringList m_serialPortName;
    foreach(const QSerialPortInfo &info,QSerialPortInfo::availablePorts())
    {
        m_serialPortName << info.portName();
        cout << "serialPortName:" << info.portName();
    }
    return m_serialPortName;
}

// 处理接受到的数据
void MainWidget::receiveInfo()
{
    QByteArray info = m_serialPort->readAll();
    QByteArray hexData = info.toHex();
    //这里面的协议 你们自己定义就行  单片机发什么 代表什么 我们这里简单模拟一下
    if(hexData == "0x10000")
    {
        //do something
    }
    else if(hexData  == "0x100001")
    {
        //do something
    }
}

//写两个函数 向单片机发送数据
void MainWidget::sendInfo(char* info,int len)
{

    for(int i=0; i<len; ++i)
    {
        printf("0x%x\n", info[i]);
    }
    m_serialPort->write(info,len);//这句是真正的给单片机发数据 用到的是QIODevice::write 具体可以看文档
}

// 发送信息
void MainWidget::sendInfo(const QString &info)
{
    QString temp;
    temp = info;

    QByteArray sendBuf;
    if (temp.contains(" "))
    {
        temp.replace(QString(" "),QString(""));//把空格去掉
    }
    qDebug()<<"Write to serial: "<<temp;
    convertStringToHex(temp, sendBuf); //把QString 转换 为 hex

    m_serialPort->write(sendBuf);//这句是真正的给单片机发数据 用到的是QIODevice::write 具体可以看文档
}


//基本和单片机交互 数据 都是16进制的 这里是 Qstring 转为 16进制的函数
void MainWidget::convertStringToHex(const QString &str, QByteArray &byteData)
{
    int hexdata,lowhexdata;
    int hexdatalen = 0;
    int len = str.length();
    byteData.resize(len/2);
    char lstr,hstr;
    for(int i=0; i<len; )
    {
        //char lstr,
        hstr=str[i].toLatin1();
        if(hstr == ' ')
        {
            i++;
            continue;
        }
        i++;
        if(i >= len)
            break;
        lstr = str[i].toLatin1();
        hexdata = convertCharToHex(hstr);
        lowhexdata = convertCharToHex(lstr);
        if((hexdata == 16) || (lowhexdata == 16))
            break;
        else
            hexdata = hexdata*16+lowhexdata;
        i++;
        byteData[hexdatalen] = (char)hexdata;
        hexdatalen++;
    }

    byteData.resize(hexdatalen);
}

//另一个 函数 char 转为 16进制
char MainWidget::convertCharToHex(char ch)
{
    /*
    0x30等于十进制的48，48也是0的ASCII值，，
    1-9的ASCII值是49-57，，所以某一个值－0x30，，
    就是将字符0-9转换为0-9

    */
    if((ch >= '0') && (ch <= '9'))
         return ch-0x30;
     else if((ch >= 'A') && (ch <= 'F'))
         return ch-'A'+10;
     else if((ch >= 'a') && (ch <= 'f'))
         return ch-'a'+10;
     else return (-1);
}

void MainWidget::on_comboBoxCom_activated(int index)
{
    this->com_index = index;
    qDebug() << com_index;
}

void MainWidget::on_pushButtonLink_clicked()
{
    //设置串口名字
    m_serialPort->setPortName(m_serialPortName[com_index]);
    cout << m_serialPortName[com_index];
    if(!m_serialPort->open(QIODevice::ReadWrite))//用ReadWrite 的模式尝试打开串口
    {
        QMessageBox::warning(this, "警告", "打开失败");
        return;
    }
    //打开成功
    QMessageBox::information(this, "打开", "打开成功");
    ui->labelComText->setText(m_serialPortName[com_index] + "已成功连接");
    m_serialPort->setBaudRate(115200,QSerialPort::AllDirections);//设置波特率和读写方向
    m_serialPort->setDataBits(QSerialPort::Data8);		//数据位为8位
    m_serialPort->setFlowControl(QSerialPort::NoFlowControl);//无流控制
    m_serialPort->setParity(QSerialPort::NoParity);	//无校验位
    m_serialPort->setStopBits(QSerialPort::OneStop); //一位停止位
}

void MainWidget::on_pushButtonDisconnect_clicked()
{
    if (m_serialPort->isOpen())
    {
        m_serialPort->clear();
        m_serialPort->close();
        QMessageBox::information(this, "关闭", "关闭成功");
        ui->labelComText->setText("未连接");
        return;
    }
    QMessageBox::warning(this, "警告", "关闭失败");
}

void MainWidget::on_pushButtonChoose_clicked()
{
   QString filepath = QFileDialog::getOpenFileName(this, "open", "../");
   if(!filepath.isEmpty())
   {
//       只读方式打开文件
       file->setFileName(filepath);
//       获取文件信息
       QFileInfo fileinfo(filepath);
       filename = fileinfo.fileName();
       filesize = fileinfo.size();
       sendsize = 0;   //发送文件的大小
       if(file->open(QIODevice::ReadOnly))
       {
//           发送按钮亮起, 选择变灰
           ui->pushButtonChoose->setEnabled(false);
           ui->pushButtonSend->setEnabled(true);
       }
   }
}




void MainWidget::on_pushButtonSend_clicked()
{
    qint64 len = 0;
    do
    {
        //每次发送数据的大小
        char buf[4*1024] = {0};
        len = 0;
        //读取文件中的数据
        len = file->read(buf, sizeof (buf));
        //返回已经写入的数据
        len = m_serialPort->write(buf, len);
        //积累的已经发送的文件数据
        sendsize += len;

    } while(len > 0);   //只要写入的数据大于0, 就说明还没发完
    //判断文件是否发送完成
    if(filesize == sendsize)
    {
        QMessageBox::information(this, "完成", "文件发送完毕");
        //关闭文件
        file->close();

        ui->pushButtonChoose->setEnabled(true);
        ui->pushButtonSend->setEnabled(false);
    }


}
