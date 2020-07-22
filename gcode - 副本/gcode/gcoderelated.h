#ifndef GCODERELATED_H
#define GCODERELATED_H

#include <QString>
#include <QPoint>
#include <QtMath>
#include <QFile>


class GcodeRelated
{
public:
    GcodeRelated();
    static void setup();
//    static void setDecimalPlaces(int num);
    static void MoveToRapid(QString gcodeString, QPoint coord, QString cmt = "");
    static void Move(QString gcodeString, int gnr, float x, float y, bool applyFeed, QString cmt);
    static void Move(QString gcodeString, int gnr, float x, float y, float z, bool applyFeed, QString cmt);
    static float fdistance(float x1, float y1, float x2, float y2);
    static float fvmag(float x, float y);
    static float fsqrt(float x);
    static QString frmtCode(int number);
    static QString frmtNum(float number);
    static QString frmtNum(double number);
    static void PenUp(QString gcodeString, QString cmto = "");
    static void SpindleOn(QString gcodeString, QString cmt = "");
    static void SpindleOff(QString gcodeString, QString cmt = "");
    static void PenDown(QString gcodeString, QString cmto = "");
    static void MoveTo(QString gcodeString, QPoint coord, QString cmt = "");
    static void MoveTo(QString gcodeString, float x, float y, QString cmt = "");
    static void MoveTo(QString gcodeString, float x, float y, float z, QString cmt = "");
    static void MoveSplit(QString gcodeString, int gnr, float x, float y, bool applyFeed, QString cmt);
    static void MoveSplit(QString gcodeString, int gnr, float finalx, float finaly, float z, bool applyFeed, QString cmt);
    static QPoint dragToolCompensation(QString gcodeString, float finalx, float final);
    static float* getAngle(float x1, float y1, float x2, float y2, float i, float j);
    static void MoveArc(QString gcodeString, int gnr, float x, float y, float i, float j, bool applyFeed, QString cmt = "", bool avoidG23 = false);
    static void splitArc(QString gcodeString, int gnr, float x1, float y1, float x2, float y2, float i, float j, bool applyFeed, QString cmt = "");
    static void Arc(QString gcodeString, int gnr, QPoint coordxy, QPoint coordij, QString cmt = "", bool avoidG23 = false);
    static void Arc(QString gcodeString, int gnr, float x, float y, float i, float j, QString cmt = "", bool avoidG23 = false);
    static void splitLine(QString gcodeString, int gnr, float x1, float y1, float x2, float y2, float maxStep, bool applyFeed, QString cmt = "");
    static void MoveToRapid(QString gcodeString, float x, float y, QString cmt = "");

    static bool lastMovewasG0;
    static float segFinalX, segFinalY, segLastFinalX, segLastFinalY;
    static float lastx, lasty, lastz, lastg, lastf, lasts;
    static float gcodeXYFeed;
    static bool applyXYFeedRate;
    static bool gcodeCompress;
    static float gcodeSpindleSpeed;
    static bool dragArc, drag1stMove;
    static float origLastX, origLastY, origFinalX, origFinalY;
    static QString gcodeSpindleCmdOn;
    static QString gcodeSpindleCmdOff;
    static float dragCompi, dragCompj;
    static bool gcodeDragCompensation;
    static float gcodeDragRadius;
    static float gcodeDragAngle;
    static bool gcodeNoArcs;
    static float gcodeAngleStep;
    static float remainingC;
    static int mDecimalPlaces;
};

#endif // GCODERELATED_H
