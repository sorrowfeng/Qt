#include "ipscanthread.h"
#include <QHostAddress>
#include <QHostInfo>
#include <QDebug>
#include <QNetworkInterface>
#include <QRegularExpression>
#include <QMap>

IPScanThread::IPScanThread(QObject *parent) : QObject(parent)
{

}

void IPScanThread::scanALL()
{
    qDebug() << getLocalHostIP();
    scanIP();
}

QString IPScanThread::getLocalHostIP()
{
    QList<QHostAddress> AddressList= QNetworkInterface::allAddresses();
    foreach (QHostAddress address, AddressList)
    {
        if(address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress::Null && address!=QHostAddress::LocalHost)
        {
            QRegularExpression re("\\.\\d{1,3}$");
            ippath = address.toString().remove(re);
            return address.toString();
        }
    }
    return 0;
}

void IPScanThread::scanIP()
{
    for(int i = 1 ; i < 256; i++)
    {
        QHostInfo::lookupHost(QString("%1.%2").arg(ippath).arg(i), this, &IPScanThread::lookUp);
    }
}


void IPScanThread::lookUp(const QHostInfo &host)
{
    if(host.error() != QHostInfo::NoError){
        qDebug() << "Lookup failed:" << host.errorString();
        return;
    }
    if(host.addresses()[0].toString() == host.hostName())
        return;

    qDebug() << host.hostName() << host.addresses()[0].toString();
    ipMap[host.hostName()] = host.addresses()[0].toString();

    qDebug() << "map size:" << ipMap.size();
    useRow++;

    emit isFindOne(ipMap);
}
