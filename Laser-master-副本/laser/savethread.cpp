#include "savethread.h"
#include "ui_mainwidget.h"
#include "savefile.h"
#include <QImage>
#include <QDebug>
#include <QFileInfo>
#include <QPainter>
//方便调试时的打印输出
#define cout qDebug() << "[" << __FILE__ << ":" << __LINE__ << "] "

SaveThread::SaveThread(QObject *parent) : QObject(parent)
{

}
SaveThread::~SaveThread()
{
    delete newimage;
}
// 图像灰度化处理
QImage * SaveThread::greyScale(QImage * origin)
{
    QImage * newImage = new QImage(origin->width(), origin->height(), QImage::Format_ARGB32);

    for(int y = 0; y<newImage->height(); y++)
    {
        QRgb * line = (QRgb *)origin->scanLine(y);

        for(int x = 0; x<newImage->width(); x++)
        {
            // 灰度 - 取R,G,B值为三者的算数平均数;
            int average = (qRed(line[x]) + qGreen(line[x]) + qRed(line[x]))/3;
            newImage->setPixel(x,y, qRgb(average, average, average));
        }
    }
    return newImage;
}

void SaveThread::dealImage(QString filepath, QImage* newimage, int width, int height, quint8 threshold)
{
    this->newimage = newimage;
    this->threshold = threshold;
    // 如果文件存在就删了
    if(QFileInfo(filepath).exists()) QFile("save_temp.jpg").remove();
    QImage image(filepath);

//            图片适应label大小
//    image = image.scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);  // 按比例缩放
//            newimage为灰度化后的图像
    newimage = this->greyScale(&image);

// 二值化
//    cout << QColor(newimage->pixel(3, 3)).red();

//    if(!newimage->isNull())
//    {
//        for (int x = 0; x < newimage->width(); x++)
//        {
//            for (int y = 0; y < newimage->height(); y++)
//            {
//                if(QColor(newimage->pixel(x, y)).red() >= threshold)
//                    newimage->setPixel(x, y, qRgb(255,255,255));   //设为白色
//                else
//                    newimage->setPixel(x, y, qRgb(0,0,0));   //设为黑色
//            }
//        }
//    }

    if(!newimage->save("save_temp.jpg"))
    {
        qDebug() << "save fault";
        return;
    }
    qDebug() << "save success";

//    处理完发送信号回主线程
    emit saveFinished(newimage);

}
