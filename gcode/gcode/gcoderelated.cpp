#include "gcoderelated.h"
#include <QString>
#include <QFile>
#include <QDebug>
#define cout  qDebug() << "[" << __FILE__ << ":" << __LINE__ << "]"


int GcodeRelated::mDecimalPlaces = 2;
bool GcodeRelated::lastMovewasG0 = true;
bool GcodeRelated::applyXYFeedRate = true;
bool GcodeRelated::gcodeCompress = true;
bool GcodeRelated::dragArc = false;
bool GcodeRelated::drag1stMove = true;
bool GcodeRelated::gcodeDragCompensation = false;
bool GcodeRelated::gcodeNoArcs = false;
float GcodeRelated::segFinalX = 0;
float GcodeRelated::segFinalY = 0;
float GcodeRelated::segLastFinalX = 0;
float GcodeRelated::segLastFinalY = 0;
float GcodeRelated::lastx = 0;
float GcodeRelated::lasty = 0;
float GcodeRelated::lastz = 0;
float GcodeRelated::lastg = -1;
float GcodeRelated::lastf = 0;
float GcodeRelated::lasts = 0;
float GcodeRelated::gcodeXYFeed = 1999;
float GcodeRelated::gcodeSpindleSpeed = 999;
float GcodeRelated::origLastX = 0;
float GcodeRelated::origLastY = 0;
float GcodeRelated::origFinalX = 0;
float GcodeRelated::origFinalY = 0;
float GcodeRelated::dragCompi = 0;
float GcodeRelated::dragCompj = 0;
float GcodeRelated::gcodeDragRadius = 5;
float GcodeRelated::gcodeDragAngle = 30;
float GcodeRelated::gcodeAngleStep = 1.0f;
float GcodeRelated::remainingC = 10;
QString GcodeRelated::gcodeSpindleCmdOn = "M3";
QString GcodeRelated::gcodeSpindleCmdOff = "M4";
QString GcodeRelated::gcodeStrings = "";


GcodeRelated::GcodeRelated()
{

}


void GcodeRelated::setup()
{
//    setDecimalPlaces(mDecimalPlaces); // 设置小数位数

//    gcodeXYFeed = Settings.GetObject("GrayScaleConversion.VectorizeOptions.BorderSpeed", 1000);
//    gcodeSpindleSpeed = Settings.GetObject("GrayScaleConversion.Gcode.LaserOptions.PowerMax", 255);
//    // Smoothieware firmware need a value between 0.0 and 1.1
//    if (firmwareType == Firmware.Smoothie)
//        gcodeSpindleSpeed /= 255.0f;
//    gcodeSpindleCmdOn = Settings.GetObject("GrayScaleConversion.Gcode.LaserOptions.LaserOn", "M3");
//    gcodeSpindleCmdOff = Settings.GetObject("GrayScaleConversion.Gcode.LaserOptions.LaserOff", "M5");

    lastMovewasG0 = true;
    lastx = -1; lasty = -1; lastz = 0; lasts = -1 ; lastg = -1;
}

//void GcodeRelated::setDecimalPlaces(int num)
//{
//    formatNumber = "0.";
//    if (gcodeCompress)
//        formatNumber = formatNumber.PadRight(num + 2, '#'); //'0'
//    else
//        formatNumber = formatNumber.PadRight(num + 2, '0'); //'0'
//}

void GcodeRelated::MoveToRapid(QString gcodeString, float x, float y, QString cmt)
{ gcodeStrings.clear(); Move(gcodeString, 0, x, y, false, cmt); lastMovewasG0 = true; }

void GcodeRelated::MoveToRapid(QString gcodeString, QPoint coord, QString cmt)
{ gcodeStrings.clear(); Move(gcodeString, 0, (float)coord.x(), (float)coord.y(), false, cmt); lastMovewasG0 = true; }

void GcodeRelated::Move(QString gcodeString, int gnr, float x, float y, bool applyFeed, QString cmt)
{ Move(gcodeString, gnr, x, y, 0, applyFeed, cmt); }

void GcodeRelated::Move(QString gcodeString, int gnr, float x, float y, float z, bool applyFeed, QString cmt)
{
    if (gnr == 0)
    {
        segLastFinalX = segFinalX; segLastFinalY = segFinalY;
        segFinalX = x; segFinalY = y;
    }
    QString feed = "";
    QString gcodeTmp;
    bool isneeded = false;
    float x_relative = x - lastx;
    float y_relative = y - lasty;
    float z_relative = lastz;
    float tz = 0;

    float delta = fdistance(lastx, lasty, x, y);

    if (z != 0)
    {
        z_relative = (float)z - lastz;
        tz = (float)z;
    }

    if (applyFeed && (gnr > 0))
    {
        feed = QString("F%1").arg(gcodeXYFeed);
        applyXYFeedRate = false;                        // don't set feed next time
    }

    if (cmt.length() > 0) cmt = QString("(%1)").arg(cmt);

    if (gcodeCompress)
    {
        if (((gnr > 0) || (lastx != x) || (lasty != y) || (lastz != tz)))  // else nothing to do
        {
            // For Marlin, we must change this line to :
            // if (lastg != gnr || firmwareType == Firmware.Marlin) { gcodeTmp.AppendFormat("G{0}", frmtCode(gnr)); isneeded = true; }
            if (lastg != gnr) { gcodeTmp.append(QString("G%1").arg(frmtCode(gnr))); isneeded = true; }

            if (lastx != x) { gcodeTmp.append(QString("X%1").arg(frmtNum(x))); isneeded = true; }
            if (lasty != y) { gcodeTmp.append(QString("Y%1").arg(frmtNum(y))); isneeded = true; }
            if (z != 0)
            {
                if (lastz != z) { gcodeTmp.append(QString("Z%1").arg(frmtNum((float)z))); isneeded = true; }
            }


            if (((gnr == 1) && (lastf != gcodeXYFeed)) || applyFeed)
            {
                gcodeTmp.append(QString("F%1").arg(gcodeXYFeed));
                lastf = gcodeXYFeed;
                isneeded = true;
            }
            // Smothieware firmware need to know laserpower when G1 command is run
            if ((gnr == 1) && (lasts != gcodeSpindleSpeed) /*&& firmwareType == Firmware.Smoothie*/)    // 处于圆滑模式时
            {
                gcodeTmp.append(QString("S%1").arg(frmtNum(gcodeSpindleSpeed)));
                lasts = gcodeSpindleSpeed;
                isneeded = true;
            }
            gcodeTmp.append(QString("%1\r\n").arg(cmt));
            if (isneeded)
            {
//                gcodeString.append(gcodeTmp);
                gcodeStrings.append(gcodeTmp);
            }
        }
//        gcodeStrings.append(gcodeTmp);
    }
    else
    {
        if (z != 0)
        {
//            gcodeString.append(QString("G%1 X%2 Y%3 Z% %4 %5\r\n").arg(frmtCode(gnr)).arg(frmtNum(x)).arg(frmtNum(y)).arg(frmtNum((float)z)).arg(feed).arg(cmt));
            gcodeTmp.append(QString("G%1 X%2 Y%3 Z% %4 %5\r\n").arg(frmtCode(gnr)).arg(frmtNum(x)).arg(frmtNum(y)).arg(frmtNum((float)z)).arg(feed).arg(cmt));
        }
        else
        {
//            gcodeString.append(QString("G%1 X%2 Y%3 %4 %5\r\n").arg(frmtCode(gnr)).arg(frmtNum(x)).arg(frmtNum(y)).arg(feed).arg(cmt));
            gcodeTmp.append(QString("G%1 X%2 Y%3 %4 %5\r\n").arg(frmtCode(gnr)).arg(frmtNum(x)).arg(frmtNum(y)).arg(feed).arg(cmt));
        }
        gcodeStrings.append(gcodeTmp);
    }

    //gcodeTime += delta / gcodeXYFeed;
    lastx = x; lasty = y; lastg = gnr; // lastz = tz;
                                       //gcodeLines++;
    cout << gcodeTmp;
//    gcodeStrings.append(gcodeTmp);
}


float GcodeRelated::fdistance(float x1, float y1, float x2, float y2) { return fvmag(x2 - x1, y2 - y1); }
float GcodeRelated::fvmag(float x, float y) { return fsqrt(x * x + y * y); }
float GcodeRelated::fsqrt(float x) { return (float)qSqrt(x); }

QString GcodeRelated::frmtCode(int number)      // convert int to string using format pattern
{ return QString::number(number); }
QString GcodeRelated::frmtNum(float number)     // convert float to string using format pattern
{ return QString::number(number, 'f', 3); } // 保留三位有效数字
QString GcodeRelated::frmtNum(double number)     // convert double to string using format pattern
{ return QString::number(number, 'f', 3); }


void GcodeRelated::PenUp(QString gcodeString, QString cmto)
{
    gcodeStrings.clear();
    QString cmt = cmto;
    drag1stMove = true;
    origFinalX = lastx;
    origFinalY = lasty;

    // 如果传入的cmto长度大于0, 那么返回"(cmto)"字符串, 即给原字符串加上括号
    if (cmt.length() > 0) { cmt = QString("(%1)").arg(cmt); }
    SpindleOff(gcodeString, cmto);
    dragCompi = 0; dragCompj = 0;
}

void GcodeRelated::SpindleOff(QString gcodeString, QString cmt)
{
    if (cmt.length() > 0) cmt = QString("(%1)").arg(cmt);
    gcodeStrings.append(QString("%1 %2\r\n").arg(gcodeSpindleCmdOff).arg(cmt));
//    gcodeStrings.append(gcodeString);
    cout << "pen up:" << gcodeStrings;
}

void GcodeRelated::PenDown(QString gcodeString, QString cmto)
{
    gcodeStrings.clear();
    QString cmt = cmto;
    drag1stMove = true;
    origFinalX = lastx;
    origFinalY = lasty;
    if (cmt.length() > 0) { cmt = QString("(%1)").arg(cmt); }

    applyXYFeedRate = true;     // apply XY Feed Rate after each PenDown command (not just after Z-axis)

    SpindleOn(gcodeString, cmto);
}
void GcodeRelated::SpindleOn(QString gcodeString, QString cmt)
{
    if (cmt.length() > 0) cmt = QString("(%1)").arg(cmt);
    gcodeString.append(QString("%1 S%2 %3\r\n").arg(gcodeSpindleCmdOn).arg(gcodeSpindleSpeed).arg(cmt));
    gcodeStrings.append(gcodeString);
}


void GcodeRelated::MoveTo(QString gcodeString, QPoint coord, QString cmt)
{ gcodeStrings.clear(); MoveSplit(gcodeString, 1, (float)coord.x(), (float)coord.y(), applyXYFeedRate, cmt); }
void GcodeRelated::MoveTo(QString gcodeString, float x, float y, QString cmt)
{ gcodeStrings.clear(); MoveSplit(gcodeString, 1, x, y, applyXYFeedRate, cmt); }
void GcodeRelated::MoveTo(QString gcodeString, float x, float y, float z, QString cmt)
{ gcodeStrings.clear(); MoveSplit(gcodeString, 1, x, y, z, applyXYFeedRate, cmt); }

void GcodeRelated::MoveSplit(QString gcodeString, int gnr, float x, float y, bool applyFeed, QString cmt)
{ gcodeStrings.clear(); MoveSplit(gcodeString, gnr, x, y, 0, applyFeed, cmt); }

void GcodeRelated::MoveSplit(QString gcodeString, int gnr, float finalx, float finaly, float z, bool applyFeed, QString cmt)
{
    segLastFinalX = segFinalX; segLastFinalY = segFinalY;
    segFinalX = finalx; segFinalY = finaly;

    if (gcodeDragCompensation)  // start move with an arc and end with extended move
    {
        QPoint tmp = dragToolCompensation(gcodeString, finalx, finaly);
        finalx = (float)tmp.x(); finaly = (float)tmp.y();   // get extended final position
    }

    Move(gcodeString, 1, finalx, finaly, z, applyXYFeedRate, cmt);
    lastMovewasG0 = false;
}

QPoint GcodeRelated::dragToolCompensation(QString gcodeString, float finalx, float finaly)
{
    float dx = finalx - origFinalX;// lastx;       // remaining distance until full move, 剩余距离，直到全动
    float dy = finaly - origFinalY;// lasty;       // lastXY is global
    float moveLength = (float) qSqrt(dx * dx + dy * dy);
    if (moveLength == 0)
    {
        return QPoint(finalx, finaly);
    }

    // 计算最后移动和当前移动之间的弧角
    // calc arc angle between last move and current move, dragCompx y = lastrealpos, dragCompi j = centerofcircle
    float rx = origFinalX + gcodeDragRadius * dx / moveLength;     // calc end-pos of arc, 计算弧的最终位置
    float ry = origFinalY + gcodeDragRadius * dy / moveLength;
    float* angle = getAngle(lastx, lasty, rx, ry, dragCompi, dragCompj); // get start-,end- and diff-angle, 获取开始，结束和漫射角
    if (angle[2] > 180) { angle[2] = angle[2] - 360; }
    if (angle[2] < -180) { angle[2] = 360 + angle[2]; }
    int gcnr = 2;
    if (angle[2] > 0)
        gcnr = 3;
    dragArc = (qAbs(angle[2]) > gcodeDragAngle);

    // draw arc before move, 移动前画圆弧
    if (dragArc)        // add arc to connect next move, 添加弧线以连接下一步
    {
        origLastX = lastx; origLastY = lasty;
        if ((angle[2] != 0) && !drag1stMove)
        {
            MoveArc(gcodeString, gcnr, rx, ry, dragCompi, dragCompj, applyXYFeedRate, "Drag t. comp.");// + angle[2].ToString());
        }
        lastx = origLastX; lasty = origLastY;
    }
    else
    {               // connect first extend move with end of next extend move
    }
    // calc end-pos with added offset and center of new arc
    origLastX = lastx;
    origLastY = lasty;
    origFinalX = finalx;
    origFinalY = finaly;
    finalx = finalx + gcodeDragRadius * dx / moveLength;    // set new finalx for move command
    finaly = finaly + gcodeDragRadius * dy / moveLength;
    dragCompi = origFinalX - finalx;                              // calc center of arc to connect to next move
    dragCompj = origFinalY - finaly;

    drag1stMove = false;
    return QPoint(finalx, finaly);
}

float* GcodeRelated::getAngle(float x1, float y1, float x2, float y2, float i, float j)
{
    float* ret = new float;
    float radius = (float)qSqrt(i * i + j * j);             // get radius of circle

    float cos1 = i / radius;                                    // get start angle
    if (cos1 > 1) cos1 = 1;
    if (cos1 < -1) cos1 = -1;
    float a1 = 180 - 180 * (float)(qAcos(cos1) / M_PI);
    if (j > 0) { a1 = -a1; }

    float cos2 = (float)(x1 + i - x2) / radius;                 // get stop angle
    if (cos2 > 1) cos2 = 1;
    if (cos2 < -1) cos2 = -1;
    float a2 = 180 - 180 * (float)(qAcos(cos2) / M_PI);
    if ((y1 + j - y2) > 0) { a2 = -a2; }                        // get delta angle

    float da = -(360 + a1 - a2);
    if (da > 360) { da -= 360; }
    if (da < -360) { da += 360; }

    ret[0] = a1; ret[1] = a2; ret[2] = da;
    return ret;
}

void GcodeRelated::MoveArc(QString gcodeString, int gnr, float x, float y, float i, float j, bool applyFeed, QString cmt, bool avoidG23)
{
    QString feed = "";
    QString splindleSpeed = "";
    float x_relative = x - lastx;
    float y_relative = y - lasty;

    if (applyFeed)
    {
        feed = QString("F%1").arg(gcodeXYFeed);
        applyXYFeedRate = false;                        // don't set feed next time
    }
    if (cmt.length() > 0) cmt = QString("(%1)").arg(cmt);
    // 当运行G2-G3命令时，思慕雪固件需要了解激光功率
//    if (firmwareType == Firmware.Smoothie)  // 处于圆滑模式时
//    {
//        splindleSpeed = QString("S%1").arg(frmtNum(gcodeSpindleSpeed));
//    }
    if (gcodeNoArcs || avoidG23)
    {
        splitArc(gcodeString, gnr, lastx, lasty, x, y, i, j, applyFeed, cmt);
    }
    else
    {
        gcodeStrings.append(QString("G%1X%2Y%3I%4J%5%6%7%8\r\n").arg(frmtCode(gnr)).arg(frmtNum(x)).arg(frmtNum(y)).arg(frmtNum(i)).arg(frmtNum(j)).arg(feed).arg(splindleSpeed).arg(cmt));
//        gcodeStrings.append(gcodeString);
        lastg = gnr;
    }

    lastx = x; lasty = y; lastf = gcodeXYFeed;
}

void GcodeRelated::splitArc(QString gcodeString, int gnr, float x1, float y1, float x2, float y2, float i, float j, bool applyFeed, QString cmt)
{
    float segmentLength = 10;
    bool equidistance = false;

    float radius = (float)qSqrt(i * i + j * j);                 // get radius of circle, 得到圆的半径
    float cx = x1 + i, cy = y1 + j;                                 // get center point of circle, 得到圆心

    float* ret = getAngle(x1, y1, x2, y2, i, j);
    float a1 = ret[0], a2 = ret[1], da = ret[2];

    da = -(360 + a1 - a2);
    if (gnr == 3) { da = qAbs(360 + a2 - a1); }
    if (da > 360) { da -= 360; }
    if (da < -360) { da += 360; }

    if ((x1 == x2) && (y1 == y2))
    {
        if (gnr == 2) { da = -360; }
        else { da = 360; }
    }
    float step = (float)(qAsin((double)gcodeAngleStep / (double)radius) * 180 / M_PI);

    applyXYFeedRate = true;
    float moveLength = remainingC;
    int count;
    if (equidistance)
    {
        float circum = radius * da * (float)M_PI / 180;
        count = (int)qCeil(circum / segmentLength);
        segmentLength = circum / count;
        moveLength = 0;
    }
    count = 1;
    if (da > 0)                                             // if delta >0 go counter clock wise
    {
        for (float angle = (a1 + step); angle < (a1 + da); angle += step)
        {
            float x = cx + radius * (float)qCos(M_PI * angle / 180);
            float y = cy + radius * (float)qSin(M_PI * angle / 180);
            moveLength += fdistance(x, y, lastx, lasty);

            Move(gcodeString, 1, x, y, applyXYFeedRate, cmt);

            if (moveLength >= (count * segmentLength))
            {
                applyXYFeedRate = true;// insertSubroutine(gcodeString, lastx, lasty, lastz, applyXYFeedRate);
                count++;
            }
            if (cmt.length() > 1) cmt = "";
        }
    }
    else                                                       // else go clock wise
    {
        for (float angle = (a1 - step); angle > (a1 + da); angle -= step)
        {
            float x = cx + radius * (float)qCos(M_PI * angle / 180);
            float y = cy + radius * (float)qSin(M_PI * angle / 180);
            moveLength += fdistance(x, y, lastx, lasty);

            Move(gcodeString, 1, x, y, applyXYFeedRate, cmt);

            if (moveLength >= (count * segmentLength))
            {
                applyXYFeedRate = true;//insertSubroutine(gcodeString, lastx, lasty, lastz, applyXYFeedRate);
                count++;
            }
            if (cmt.length() > 1) cmt = "";
        }
    }
    Move(gcodeString, 1, x2, y2, applyXYFeedRate, "End Arc conversion");
    if ((moveLength >= (count * segmentLength)) || equidistance)
    {
        applyXYFeedRate = true; // insertSubroutine(gcodeString, lastx, lasty, lastz, applyXYFeedRate);
        moveLength = 0;
    }
}

void GcodeRelated::Arc(QString gcodeString, int gnr, QPoint coordxy, QPoint coordij, QString cmt, bool avoidG23)
{ gcodeStrings.clear(); MoveArc(gcodeString, gnr, (float)coordxy.x(), (float)coordxy.y(), (float)coordij.x(), (float)coordij.y(), applyXYFeedRate, cmt, avoidG23); }
void GcodeRelated::Arc(QString gcodeString, int gnr, float x, float y, float i, float j, QString cmt, bool avoidG23)
{ gcodeStrings.clear(); MoveArc(gcodeString, gnr, x, y, i, j, applyXYFeedRate, cmt, avoidG23); }

void GcodeRelated::splitLine(QString gcodeString, int gnr, float x1, float y1, float x2, float y2, float maxStep, bool applyFeed, QString cmt)
{
    gcodeStrings.clear();
    float dx = x2 - x1;
    float dy = y2 - y1;
    float c = (float)qSqrt(dx * dx + dy * dy);
    float tmpX, tmpY;
    int divid = (int)qCeil(c / maxStep);
    lastg = -1;
    for (int i = 1; i <= divid; i++)
    {
        tmpX = x1 + i * dx / divid;
        tmpY = y1 + i * dy / divid;
        if (i > 1) { applyFeed = false; cmt = ""; }
        if (gnr == 0)
        { Move(gcodeString, gnr, tmpX, tmpY, false, cmt); }
        else
        { Move(gcodeString, gnr, tmpX, tmpY, applyFeed, cmt); }
    }
}
