#ifndef SAVEFILE_H
#define SAVEFILE_H

#include <QObject>
#include <QString>
#include <QImage>
#include <QFile>
#include <QFloat16>
#define MACHINE_SPEED 1

class SaveFile
{
public:
    SaveFile();
    void saveProgram(QString filename, bool header, bool footer, bool between);
    void writeBetweenGcode( QFile &sw);
    void getDeviceRect(qfloat16 deviceWidth, qfloat16 deviceHeight);
    qfloat16 getSpeed(qfloat16 grayVal);

private:
    QString GCODE_STD_HEADER;
    QString GCODE_STD_FOOTER;
    QString filepath;
    QImage * tempImage;

//    每个像素对应单位的宽度
    qfloat16 pixelUintWidth;
    qfloat16 pixelUintHeight;
//    设备的宽度高度
    qfloat16 deviceWidth;
    qfloat16 deviceHeight;
    qfloat16 grayVal;
    qfloat16 speed;
    bool closeFlag = false;
};

#endif // SAVEFILE_H
