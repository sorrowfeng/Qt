#include "savefile.h"
#include "ui_mainwidget.h"
#include "mainwidget.h"
#include <QFile>
#include <iostream>
#include <QImage>
#include <QtMath>
#include <QRgb>
#include <QDebug>

SaveFile::SaveFile()
{
    GCODE_STD_HEADER = "G90;use absolute coordinates\nG0 X0 Y0 Z0;move back to origin\nM107\nM106 S0\n";
    GCODE_STD_FOOTER = "M106 S0\nM107\nG0 X0 Y0 Z0 ;move back to origin\nM106 S0\nM107\nM5\n";
    tempImage = nullptr;
    pixelUintWidth = 0.0f;
    pixelUintHeight = 0.0f;
}

void SaveFile::saveProgram(QString savepath, bool header, bool footer, bool between)
{
    try
    {
        if(!savepath.isEmpty())
        {
            QFile sw;
            //关联文件对象
            sw.setFileName(savepath);
            //以只写方式打开文件
            if(sw.open(QIODevice::WriteOnly))
            {
                if (header)
                    //写入文件头信息，
                    sw.write(GCODE_STD_HEADER.toUtf8());

                if (between)
                    this->writeBetweenGcode(sw);

                if (footer)
                    //写入文件尾部信息，
                    sw.write(GCODE_STD_FOOTER.toUtf8());
            }
           sw.close();
        }
    }
    catch(std::exception) { }
}

void SaveFile::getDeviceRect(qfloat16 deviceWidth, qfloat16 deviceHeight)
{
    this->deviceWidth = deviceWidth;
    this->deviceHeight = deviceHeight;
}
// 传进来之后 白色255, 黑色0
qfloat16 SaveFile::getSpeed(qfloat16 grayVal)
{
    return grayVal * 1.0 / 255.0;
}

void SaveFile::writeBetweenGcode(QFile &sw)
{
    tempImage = new QImage("save_temp.jpg");
//    int x = tempImage->width();
//    int y = tempImage->height();
    // 缩小图片
//    *tempImage = tempImage->scaled(deviceWidth*5, deviceHeight*5, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // 初始化每个像素对应的长度
    pixelUintWidth = deviceWidth/tempImage->width();
    pixelUintHeight = deviceHeight/tempImage->height();
    *tempImage = tempImage->mirrored(false, true);  // 设置水平翻转false, 垂直翻转true
    if(!tempImage->isNull())
    {
//        int whiteFlag = 0;
//        QRgb * line;
        // 回环逐行式
//        for (int y = tempImage->height()-1; y >= 0; y--)
        for (int y = 0; y < tempImage->height(); y++)
        {
//            do{
//                sw.write(QString("M106 S0\n").toUtf8());
//                whiteFlag = 0;
//                line = (QRgb *)tempImage->scanLine(y);
//                for (int i = 0; i < tempImage->width(); i++)
//                    if(qRed(line[i]) == 255) whiteFlag++;
//                if(y >= tempImage->width()-2)
//                {
//                    y = tempImage->width()-1;
//                    break;
//                }
//                if(whiteFlag >= tempImage->width()-1) y++;

//            }while(whiteFlag >= tempImage->width()-1);

            if (y == 0 ||(0 == y % 2)) //偶数行
            {
                for (int x = 0; x < tempImage->width(); x++)
                {
                    //QColor(tempImage->pixel(x, y)).red() 黑色是0, 白色255
                    grayVal = QColor(tempImage->pixel(x, y)).red();
                    speed = 1- getSpeed(grayVal);
                    qfloat16 tempX = x*pixelUintWidth;
                    qfloat16 tempY = y*pixelUintHeight;
                    if(grayVal == 255 && closeFlag == false) {    // 白色
                        sw.write(QString("M106 S0\n").toUtf8());
                        sw.write(QString("G0 X%1 Y%2\n").arg(QString::number(tempX, 'f', 2)).arg(QString::number(tempY, 'f', 2)).toUtf8());
                        closeFlag = true;
                    } else if(grayVal == 255 && closeFlag == true) {
//                        sw.write(QString("G0 X%1 Y%2\n").arg(QString::number(tempX, 'f', 2)).arg(QString::number(tempY, 'f', 2)).toUtf8());
                    } else {
                        closeFlag = false;
                        sw.write(QString("G1 X%1 Y%2 S%3\n").arg(QString::number(tempX, 'f', 2)).arg(QString::number(tempY, 'f', 2)).arg(QString::number(speed, 'f', 4)).toUtf8());
                    }
                }
            }
            else
            {
                for (int x = tempImage->width()-1; x >= 0; x--)
                {
                    grayVal = QColor(tempImage->pixel(x, y)).red();
                    speed = 1 - getSpeed(grayVal);
                    qfloat16 tempX = x*pixelUintWidth;
                    qfloat16 tempY = y*pixelUintHeight;
                    if(grayVal == 255 && closeFlag == false) {
                        sw.write(QString("M106 S0\n").toUtf8());
                        sw.write(QString("G0 X%1 Y%2\n").arg(QString::number(tempX, 'f', 2)).arg(QString::number(tempY, 'f', 2)).toUtf8());
                        closeFlag = true;
                    } else if(grayVal == 255 && closeFlag == true) {
//                        sw.write(QString("G0 X%1 Y%2\n").arg(QString::number(tempX, 'f', 2)).arg(QString::number(tempY, 'f', 2)).toUtf8());
                    } else {
                        closeFlag = false;
                        sw.write(QString("G1 X%1 Y%2 S%3\n").arg(QString::number(tempX, 'f', 2)).arg(QString::number(tempY, 'f', 2)).arg(QString::number(speed, 'f', 4)).toUtf8());
                    }
                }
            }
        }






    }
}
