#ifndef SAVETHREAD_H
#define SAVETHREAD_H

#include <QObject>
#include <QImage>
#include "ui_mainwidget.h"

class SaveThread : public QObject
{
    Q_OBJECT
public:
    explicit SaveThread(QObject *parent = nullptr);
    ~SaveThread();
    void dealImage(QString, QImage*, int, int, quint8);
    QImage * greyScale(QImage * origin);

signals:
    void saveFinished(QImage *);

public slots:

private:
    quint8 threshold;
    QImage* newimage;
};

#endif // SAVETHREAD_H
