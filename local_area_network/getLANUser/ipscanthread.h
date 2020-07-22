#ifndef IPSCANTHREAD_H
#define IPSCANTHREAD_H

#include <QObject>
#include <QThread>
#include <QMap>
#include <QHostInfo>

class IPScanThread : public QObject
{
    Q_OBJECT
public:
    explicit IPScanThread(QObject *parent = nullptr);
    QString getLocalHostIP();
    void lookUp(const QHostInfo &host);
    void scanIP();
    void scanALL();

signals:
    void isFindOne(QMap<QString,QString>);

private:
    QString ippath;
    QMap<QString,QString> ipMap;
    int useRow = 0;
};

#endif // IPSCANTHREAD_H
