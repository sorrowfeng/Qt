#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QFile>
#include <QImage>
#include <QThread>
#include <QFileInfo>
#include "savethread.h"
#include <QFloat16>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWidget; }
QT_END_NAMESPACE

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    MainWidget(QWidget *parent = nullptr);
    ~MainWidget();
    void reboot();
    void paintEvent(QPaintEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mousePressEvent(QMouseEvent *e);
    qfloat16 deviceWidth;
    qfloat16 deviceHeight;
    QPoint p;

signals:
    void startDeal(QString, QImage*, int, int, quint8);

private slots:
    void on_pushButtonLoad_clicked();
    void on_pushButtonGenerate_clicked();
    void on_pushButtonExit_clicked();
    void on_sliderThreshold_sliderMoved(int position);
    void on_pushButtonSureImage_clicked();
    void on_sliderThreshold_actionTriggered();
    void on_pushButtonReset_clicked();

private:
    Ui::MainWidget *ui;
    QString filepath;
    QString filename;
    qint64 filesize;
    QImage* newimage;
    QThread *thread;
    SaveThread *savethread;
    quint8 threshold;
    QFileInfo *fio;
};
#endif // MAINWIDGET_H
