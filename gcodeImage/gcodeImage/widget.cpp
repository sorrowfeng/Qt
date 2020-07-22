#include "widget.h"
#include "ui_widget.h"
#include <QImage>
#include <QFile>
#include <QDebug>


Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    QFile file("../gcode.gcode");
    file.open(QIODevice::WriteOnly);

    QImage image("../sea.jpg");
    image = image.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation);  // 按比例缩放
    QImage *newImage = new QImage;
    newImage = greyScale(&image);

    newImage->save("../temp.png");
    for(int y=0; y<newImage->height(); y++)
    {
        for(int x=0; x<newImage->width(); x++)
        {
            int grayVal = QColor(newImage->pixel(x, y)).red();
            if(grayVal == 0)
                file.write(QString("G0 X Y \n").toUtf8());
            else {
                file.write(QString("G1 X Y S%1\n").arg(grayVal).toUtf8());
            }
        }
//        break;
    }
}

Widget::~Widget()
{
    delete ui;
}

// 图像灰度化处理
QImage * Widget::greyScale(QImage * origin)
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
