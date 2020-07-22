#include "widget.h"
#include "ui_widget.h"
#include <QHostAddress>
#include <QHostInfo>
#include <QDebug>
#include <QNetworkInterface>
#include <QRegularExpression>
#include <QMap>
#include <QThread>
#include "ipscanthread.h"



Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

//    ui->label->setText(this->getLocalHostIP());

//    this->scanIP();

//    qDebug() << ipMap.size();

//    QMap<QString,QString>::iterator it; //遍历map
//    for ( it = ipMap.begin(); it != ipMap.end(); ++it ) {
//        qDebug() << it.key() << it.value();
//    }




    ipthread = new IPScanThread;
    ipT = new QThread(this);
    ipthread->moveToThread(ipT);

    connect(this, &Widget::startScanIP, ipthread, &IPScanThread::scanALL);

    void (IPScanThread::*func)(QMap<QString,QString>) = &IPScanThread::isFindOne;
    connect(ipthread, func,
        [=](QMap<QString,QString> ipMap)
        {
            this->ipMap = ipMap;
            ui->comboBox->clear();
            ui->comboBox->addItems(this->ipMap.keys());
//            ui->comboBox->addItem(this->ipMap.end().key());
        }
    );

}

Widget::~Widget()
{
    delete ui;
    delete ipthread;
    if(ipT->isRunning())
    {
        //退出线程
        ipT->quit();
        ipT->wait();
    }
}


QString Widget::getLocalHostIP()
{
    QList<QHostAddress> AddressList= QNetworkInterface::allAddresses();
    foreach (QHostAddress address, AddressList)
    {
        if(address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress::Null && address!=QHostAddress::LocalHost)
        {
            qDebug() << address.toString();
            QRegularExpression re("\\.\\d{1,3}$");
            ippath = address.toString().remove(re);
            return address.toString();
        }
    }
    return 0;
}

void Widget::scanIP()
{
    for(int i = 1 ; i < 256; i++)
    {
        QHostInfo::lookupHost(QString("%1.%2").arg(ippath).arg(i), this, &Widget::lookUp);
    }
}

int useRow = 0;
void Widget::lookUp(const QHostInfo &host)
{
    if(host.error() != QHostInfo::NoError){
        qDebug() << "Lookup failed:" << host.errorString();
        return;
    }

    if(host.addresses()[0].toString() == host.hostName())
        return;

    qDebug() << host.hostName() << host.addresses()[0].toString();

    ipMap[host.hostName()] = host.addresses()[0].toString();

    qDebug() << ipMap.size();

    ui->comboBox->addItem(host.hostName());
    useRow++;
}




void Widget::on_comboBox_activated(const QString &arg1)
{
    ui->label->setText(ipMap[arg1]);
}


void Widget::showEvent(QShowEvent *e)
{
    ipT->start();
    emit startScanIP();
}
