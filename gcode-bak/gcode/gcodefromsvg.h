#ifndef GCODEFROMSVG_H
#define GCODEFROMSVG_H

#include <QWidget>
#include <QDomDocument>
#include <QFile>
#include <QMessageBox>
#include <QTreeWidget>
#include <QDebug>
#include <QMatrix>
#include <QRegExp>
#include <QXmlNamespaceSupport>

class GCodeFromSVG
{
public:
    GCodeFromSVG();
    QString convertSVG(QDomDocument* svgCode);
    void startConvert(QDomDocument* svgCode);
    void parseGlobals(QDomElement* svgCode);

    bool SvgScaleApplyfunc();
    bool SvgScaleApplyfunc(bool value);
    float SvgMaxSizefunc();
    float SvgMaxSizefunc(float value);

    float convertToPixel(QString str, float ext = 1);
    float floatParse(QString str, float ext = 1);
    static QString removeUnit(QString str);
    void parseBasicElements(QDomNode* svgCode, int level);
    void parsePath(QDomNode* svgCode, int level);
    void parseGroup(QDomNode* svgCode, int level);
    bool parseTransform(QDomNode* element, bool isGroup, int level);
    QString getColor(QDomNode* pathElement);
    void gcodePenUp(QString cmt);
    void gcodePenDown(QString cmt);
    QString getTextBetween(QString source, QString s1, QString s2);
    void gcodeStartPath(float x, float y, QString cmt);
    QPoint translateXY(QPoint pointStart);
    QPoint translateXY(float x, float y);
    void gcodeMoveTo(float x, float y, QString cmt);
    void gcodeMoveTo(QPoint orig, QString cmt);
    void gcodeArcToCCW(float x, float y, float i, float j, QString cmt, bool avoidG23 = false);
    QPoint translateIJ(float i, float j);
    QPoint translateIJ(QPoint pointStart);
    void gcodeStopPath(QString cmt);
    void calcArc(float StartX, float StartY, float RadiusX, float RadiusY, float Angle, float Size, float Sweep, float EndX, float EndY);
    double CalculateVectorAngle(double ux, double uy, double vx, double vy);
    QPoint * GetBezierApproximation(QPoint * controlPoints, int outputSegmentCount);
    QPoint GetBezierPoint(double t, QPoint* controlPoints, int index, int count);
    int parsePathCommand(QString svgPath);
    void gcodeDotOnly(float x, float y, QString cmt);

    static float factor_Mm2Px;
    static float factor_In2Px;
    static float factor_Cm2Px;
    static float factor_Pt2Px;
    static float factor_Pc2Px;
    static float factor_Em2Px;


private:
    QDomDocument* svgCode;
    int countSubPath = 0;
    bool startFirstElement = true;
    bool svgConvertToMM = true;
    bool svgScaleApply = false;
    bool svgNodesOnly = false;
    bool startPath = true;
    bool startSubPath = true;
    bool svgComments = false;
    bool svgPauseElement = false;
    bool svgPausePenDown = false;
    bool penIsDown = true;
    bool isReduceOk = false;
    bool rejectPoint = false;
    bool gcodeReduce = false;
    bool svgClosePathExtend = true;
    bool gcodeUseSpindle = false;

    float svgMaxSize = 100.0;
    float gcodeScale = 1.0;
    float svgWidthPx, svgHeightPx;
    float offsetX, offsetY;
    float currentX, currentY;
    float firstX, firstY;
    float lastX, lastY;
    float cxMirror = 0, cyMirror = 0;
    double lastGCX = 0, lastGCY = 0, lastSetGCX = 0, lastSetGCY = 0, distance;
    float gcodeReduceVal = 0.1f;
    int svgBezierAccuracy = 12;

    QMatrix matrixElement;
    QMatrix oldMatrixElement;
    QMatrix matrixGroup[10];
    // XML名称空间, 这里没找到, 先用string
    QString nspace = "http://www.w3.org/2000/svg";
    QString gcodeString;
    QPoint points[4];
    QString secondMove;

};

#endif // GCODEFROMSVG_H
