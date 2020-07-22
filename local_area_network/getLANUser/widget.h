#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QMap>
#include "ipscanthread.h"

QT_BEGIN_NAMESPACE
class QHostInfo;
QT_END_NAMESPACE

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
    QString getLocalHostIP();
    void lookUp(const QHostInfo &host);
    void btnClicked();
    void scanIP();
    void showEvent(QShowEvent *e);

signals:
    void startScanIP();

private slots:
    void on_comboBox_activated(const QString &arg1);

private:
    Ui::Widget *ui;
    QString ippath;
    QMap<QString,QString> ipMap;
    QThread *ipT;
    IPScanThread *ipthread;
};

#endif // WIDGET_H
