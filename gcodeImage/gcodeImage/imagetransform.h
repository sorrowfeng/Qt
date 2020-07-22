#ifndef IMAGETRANSFORM_H
#define IMAGETRANSFORM_H

#include <QBitmap>
#include <QString>

enum Direction
{ Horizontal, Vertical, Diagonal, None };

enum Firmware
{ Grbl, Smoothie, Marlin };

class L2LConf
{
public:
    double res;
    float oX;
    float oY;
    int markSpeed;
    int travelSpeed;
    int borderSpeed;
    int minPower;
    int maxPower;
    QString lOn;
    QString lOff;
    Direction dir;
    bool pwm;
    double fres;
    bool vectorfilling;
    Firmware firmwareType;
};

class ImageTransform
{
public:
    ImageTransform();
    void LoadImagePotrace(QBitmap bmp, QString filename, bool UseSpotRemoval, int SpotRemoval, bool UseSmoothing, int Smoothing, bool UseOptimize, int Optimize, bool useOptimizeFast, L2LConf c, bool append);

};

#endif // IMAGETRANSFORM_H
