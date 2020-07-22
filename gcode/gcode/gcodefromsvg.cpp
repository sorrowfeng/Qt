#include "gcodefromsvg.h"
#include "gcoderelated.h"
#include <QRegularExpression>
#include <QtMath>
#include <QList>
#include <algorithm>
#include <QDebug>
#define cout qDebug() << "[" << __FILE__ << ":" << __LINE__ << "]"

// static 成员变量初始化要声明作用域
float GCodeFromSVG::factor_Mm2Px = 96.0 / 25.4;
float GCodeFromSVG::factor_In2Px = 96.0;
float GCodeFromSVG::factor_Cm2Px = 96.0 / 2.54;
float GCodeFromSVG::factor_Pt2Px = 96.0 / 72.0;
float GCodeFromSVG::factor_Pc2Px = 12 * 96.0 / 72.0;
float GCodeFromSVG::factor_Em2Px = 150.0;

GCodeFromSVG::GCodeFromSVG()
{
    sw = new QFile;
    //关联文件对象
    sw->setFileName("../GCODE.gcode");
    //以只写方式打开文件
    if(!sw->open(QIODevice::WriteOnly /*| QIODevice::Append*/)) cout << "open failed";
}
GCodeFromSVG::~GCodeFromSVG()
{
    delete sw;
}

//QString convertFromFile(QString file)
//{
//    QString xml = System.IO.File.ReadAllText(file);
//    return convertFromText(xml, true);
//}
//QString convertFromText(QString text, bool importMM = false)
//{
//    //fromText = true;
//    importInMM = importMM;

//    // From xml spec valid chars:
//    // #x9 | #xA | #xD | [#x20-#xD7FF] | [#xE000-#xFFFD] | [#x10000-#x10FFFF]
//    // any Unicode character, excluding the surrogate blocks, FFFE, and FFFF.
//    text = RemoveInvalidUnicode.Replace(text, string.Empty);
//    svgCode = XElement.Parse(text, LoadOptions.None);

//    return convertSVG(svgCode);
//}

QString GCodeFromSVG::convertSVG(QDomDocument* svgCode)
{
    GcodeRelated::setup();          // initialize GCode creation (get stored settings for export)
    QString header = "G90\r\nM107\r\nM106 S0\r\nG0X0Y0\r\n";
    QString footer = "M107\r\nM106 S0\r\nG0X0Y0\r\n";

    if (gcodeUseSpindle) GcodeRelated::SpindleOff(gcodeString, "Stop spindle - Option Z-Axis");
    sw->write(GcodeRelated::gcodeStrings.toUtf8());
    sw->write(header.toUtf8());

    startConvert(svgCode);
    //return header + gcodeString.ToString().Replace(',', '.') + footer;
    sw->write(footer.toUtf8());
//    return gcodeString.replace(',', '.');
    return "true";
}

//开始转换
void GCodeFromSVG::startConvert(QDomDocument* svgCode)
{
    countSubPath = 0;
    startFirstElement = true;
    gcodeScale = 1;
    svgWidthPx = 0; svgHeightPx = 0;
    currentX = 0; currentY = 0;
    offsetX = 0; offsetY = 0;
    firstX = 0;
    firstY = 0;
    lastX = 0;
    lastY = 0;

    // 將這個 Matrix 結構變更為單位矩陣。Qt貌似直接是单位矩阵, 不用转换

//    matrixElement.SetIdentity();
//    oldMatrixElement.SetIdentity();
//    for (int i = 0; i < matrixGroup.Length; i++)
//        matrixGroup[i].SetIdentity();
//    sw->write(";www.sorrowfeng.top\n");
    QDomElement* svgElement = new QDomElement;
    *svgElement =  svgCode->documentElement();
    //解析SVG尺寸（视图框，宽度，高度）
    parseGlobals(svgElement);
    if (!svgNodesOnly)
        parseBasicElements(svgElement, 1); // 解析基础元素, 如 矩形 圆 等 等
    parsePath(svgElement, 1);   // 解析path标签
    parseGroup(svgElement, 1);  // 解析g标签
    return;
}

// 全局解析
void GCodeFromSVG::parseGlobals(QDomElement* svgCode)
{
    //一个px单位定义为等于一个用户单位。因此，“ 5px ”的长度与“ 5 ”的长度相同。
    QMatrix *tmp = new QMatrix(1, 0, 0, 1, 0, 0); // m11, m12, m21, m22, offsetx, offsety
    svgWidthPx = 0;
    svgHeightPx = 0;
    float vbOffX = 0;
    float vbOffY = 0;
    float vbWidth = 0;
    float vbHeight = 0;
    float scale = 1;
    float M11Val=0, M12Val=0, M21Val=0, M22Val=0, OffsetXVal=0, OffsetYVal=0;
    QString tmpString = "";
    // 判断是否有xmlns属性
    if (svgCode->hasAttribute("xmlns"))
        nspace = svgCode->attributeNode("xmlns").value();
    else
        nspace = "";
//    sw->write(";by sorrowfeng\n");
//    cout << "nspace:" << nspace;

    // 是否存在viewBox属性
    if (svgCode->hasAttribute("viewBox"))   //viewBox单位始终以px为单位
    {
        QRegularExpression re("\\s+");
        QString viewbox = svgCode->attributeNode("viewBox").value();    // 返回标签为viewbox的值
        viewbox.replace(re, " ").replace(' ', '|');    // remove double space, 将viewbox中的\s+换为空格, 将空格换为|
        QStringList split = viewbox.split('|'); // 返回viewbox用|分割后的字符串

//        cout << "viwbox分割后的字符串:" << split;
        vbOffX = -convertToPixel(split[0]);
        vbOffY = -convertToPixel(split[1]);
        vbWidth = convertToPixel(split[2]);
        vbHeight = convertToPixel(split[3].remove(')'));  // TrimEnd(')')作用是删除后面的)字符, 这里使用remove来移除字符串中的 )
        M11Val = 1; M22Val = -1; OffsetYVal = vbHeight;  // 设置m11 = 1, m22 = -1, OffsetY = vbHeight // 翻转Y

//        cout << "viewbox转换后:" << vbOffX << " " << vbOffY << " " << vbWidth << " " << vbHeight;
//        tmp->m11 = 1;
//        tmp->m22 = -1;      // flip Y, 翻转Y
//        tmp->OffsetY = vbHeight;
    }
    scale = 1;
    if (svgConvertToMM)                 //从px转换回mm
        scale = (1 / factor_Mm2Px);
    else                                //从px转换回英寸
        scale = (1 / factor_In2Px);

    //如果有, 则取出width, 与 height 的值
    if (svgCode->hasAttribute("width"))
    {
        tmpString = svgCode->attributeNode("width").value();
        svgWidthPx = convertToPixel(tmpString); // convert in px

//        cout << "width:" << tmpString << "转换后:" << svgWidthPx;
        // M11获取tmp第一行与第一列交处的值
        M11Val = scale;
//        tmp.M11 = scale; // get desired scale, 得到指定的规模
                         // if (fromClipboard)
                         // tmp.M11 = 1 / factor_Mm2Px; // 3.543307;         // https://www.w3.org/TR/SVG/coords.html#Units
        if (vbWidth > 0)
        {
            M11Val = scale * svgWidthPx / vbWidth;
            OffsetXVal = vbOffX * scale;
//            tmp.M11 = scale * svgWidthPx / vbWidth;
//            tmp.OffsetX = vbOffX * scale;   // svgWidthUnit / vbWidth;
        }
    }
    if (svgCode->hasAttribute("height"))
    {
        tmpString = svgCode->attributeNode("height").value();
        svgHeightPx = convertToPixel(tmpString);

//        cout << "height:" << tmpString << "转换后:" << svgHeightPx;
        // M22获取第二行与第二列相交处的值
        M22Val = -scale;
        OffsetYVal = scale * svgHeightPx;
//        tmp.M22 = -scale;   // get desired scale and flip vertical, 获得所需的比例并垂直翻转
//        tmp.OffsetY = scale * svgHeightPx;  // svgHeightUnit;

        if (vbHeight > 0)
        {
            M22Val = -scale * svgHeightPx / vbHeight;
            OffsetYVal = -vbOffY * svgHeightPx / vbHeight + (svgHeightPx * scale);
//            tmp.M22 = -scale * svgHeightPx / vbHeight;
//            tmp.OffsetY = -vbOffY * svgHeightPx / vbHeight + (svgHeightPx * scale);
        }
    }

    //取最大值
    float newWidth = qMax(svgWidthPx, vbWidth);     // use value from 'width' or 'viewbox' parameter
    float newHeight = qMax(svgHeightPx, vbHeight);
    if ((newWidth > 0) && (newHeight > 0))
    {
        if (SvgScaleApplyfunc())
        {
            gcodeScale = SvgMaxSizefunc() / qMax(newWidth, newHeight);        // calc. factor to get desired max size, 获得所需最大尺寸的因素
            tmp->scale((double)gcodeScale, (double)gcodeScale);
            if (svgConvertToMM)                         // https://www.w3.org/TR/SVG/coords.html#Units
                tmp->scale(factor_Mm2Px, factor_Mm2Px);  // 3.543307, 3.543307);
            else
                tmp->scale(factor_In2Px, factor_In2Px);
        }
    }
    // 设置m11, m12, m21, m22, offsetx, offsety

//    cout << "最后的Matrix  " <<  "M11:" << M11Val << " M12:" << M12Val << " M21:" << M21Val << " M22:" << M22Val << " OffsetX:" << OffsetXVal << " OffsetY:" << OffsetYVal;
    tmp->setMatrix(M11Val, M12Val, M21Val, M22Val, OffsetXVal, OffsetYVal);
}

bool GCodeFromSVG::SvgScaleApplyfunc() { return svgScaleApply; }
bool GCodeFromSVG::SvgScaleApplyfunc(bool value) { svgScaleApply = value; return 0;}

float GCodeFromSVG::SvgMaxSizefunc() { return svgMaxSize; }
float GCodeFromSVG::SvgMaxSizefunc(float value) { svgMaxSize = value; return 0;}

// 解析float
float GCodeFromSVG::convertToPixel(QString str, float ext) {return floatParse(str, ext);}
float GCodeFromSVG::floatParse(QString str, float ext)
{
    bool percent = false;
    float factor = 1;   // 没有单位的话说明是px, factor因子为1
    // IndexOf 查找字符串value, 如果找到则返回索引, 没找到返回-1, 如果value为空返回0
    if (str.indexOf("mm") > 0) { factor = factor_Mm2Px; }               // Millimeter, 毫米
    else if (str.indexOf("cm") > 0) { factor = factor_Cm2Px; }          // Centimeter, 厘米
    else if (str.indexOf("in") > 0) { factor = factor_In2Px; }          // Inch    72, 90 or 96?, 英寸
    else if (str.indexOf("pt") > 0) { factor = factor_Pt2Px; }          // Point, 点
    else if (str.indexOf("pc") > 0) { factor = factor_Pc2Px; }          // Pica,
    else if (str.indexOf("em") > 0) { factor = factor_Em2Px; }          // Font size, 字体大小
    else if (str.indexOf("%") > 0) { percent = true; }
    QString nstr = removeUnit(str);
//    cout << "要转换的字符串:" << str << " 去除单位之后的字符串:" << nstr;
    str = nstr;

//    float test;
    if (str.length() > 0)
    {
        if (percent)
        {
            // TryParse将str转换成float类型, 成功返回true
//            if (float.TryParse(str, NumberStyles.Float, NumberFormatInfo.InvariantInfo, out test))
//            { return (test * ext / 100); }
            return str.toFloat() * ext / 100;
        }
        else
        {
//            if (float.TryParse(str, NumberStyles.Float, NumberFormatInfo.InvariantInfo, out test))
//            { return (test * factor); }
//            cout << "factor:" << factor << "str:" << str.toFloat() << "return :" << str.toFloat() * factor;
            return str.toFloat() * factor;
        }
    }

    //else error!
    return 0.0;
}
// 移除字符串  [^0-9.\-+] , 这里的作用把除了数字之外的单位匹配出来, 如226px 匹配出 px
QString GCodeFromSVG::removeUnit(QString str)
{
    QRegularExpression re("[^0-9.\\-+]");
    return str.replace(re, "");
}




// 转换基本形状（到目前为止：直线，矩形，圆形）查找: http://www.w3.org/2000/svg/rect等
// http://www.w3.org/TR/SVG/shape.html
void GCodeFromSVG::parseBasicElements(QDomNode* svgCode, int level)
{
//    sw->write(";1399600304\n");
    QStringList forms = { "rect", "circle", "ellipse", "line", "polyline", "polygon", "text", "image" };
    foreach (QString form , forms)
    {
//        foreach (QDomNode pathElement, svgCode->documentElement().elementsByTagName(form)) //返回集合中所有nspace + form的元素
        for (int i = 0; i < svgCode->toElement().elementsByTagName(form).count(); i++)
        {
//            cout << "找到:" << svgCode->toElement().elementsByTagName(form).item(i).nodeName();
            QDomNode * pathElement = new QDomNode();
            *pathElement = svgCode->toElement().elementsByTagName(form).item(i);
            if (!pathElement->isNull())
            {
                // 返回颜色
                QString myColor = getColor(pathElement);
//                cout << form << "的颜色:" << myColor;
                if (svgComments)    // svg注释, 默认值为false
                {
                    if (pathElement->toElement().hasAttribute("id"))
                        // 追加字符串
                        gcodeString.append("\r\n( Basic shape level:" + QString::number(level) + " id=" + pathElement->toElement().attribute("id") + " )\r\n");
                    // 追加颜色格式
                    gcodeString = QString("( SVG color=#%1)\r\n").arg(myColor);
                    sw->write(gcodeString.toUtf8());

                }


                if (startFirstElement)  // 开始第一个元素
                // 插入M4(1st shape)\r\n
                { gcodePenUp("1st shape"); startFirstElement = false; }

                offsetX = 0; offsetY = 0;

                oldMatrixElement = matrixElement;
                bool avoidG23 = false;

                parseTransform(pathElement, false, level);  // 转换将在gcodeMove中应用

                float x = 0, y = 0, x1 = 0, y1 = 0, x2 = 0, y2 = 0, width = 0, height = 0, rx = 0, ry = 0, cx = 0, cy = 0, r = 0;
                QStringList points = { "" };
                // 返回以上float的值并解析存放进去
                if (pathElement->toElement().hasAttribute("x")) x = floatParse(pathElement->toElement().attribute("x"));
                if (pathElement->toElement().hasAttribute("y")) y = floatParse(pathElement->toElement().attribute("y"));
                if (pathElement->toElement().hasAttribute("x1")) x1 = floatParse(pathElement->toElement().attribute("x1"));
                if (pathElement->toElement().hasAttribute("y1")) y1 = floatParse(pathElement->toElement().attribute("y1"));
                if (pathElement->toElement().hasAttribute("x2")) x2 = floatParse(pathElement->toElement().attribute("x2"));
                if (pathElement->toElement().hasAttribute("y2")) y2 = floatParse(pathElement->toElement().attribute("y2"));
                if (pathElement->toElement().hasAttribute("width")) width = floatParse(pathElement->toElement().attribute("width"), svgWidthPx);
                if (pathElement->toElement().hasAttribute("height")) height = floatParse(pathElement->toElement().attribute("height"), svgHeightPx);
                if (pathElement->toElement().hasAttribute("rx")) rx = floatParse(pathElement->toElement().attribute("rx"));
                if (pathElement->toElement().hasAttribute("ry")) ry = floatParse(pathElement->toElement().attribute("ry"));
                if (pathElement->toElement().hasAttribute("cx")) cx = floatParse(pathElement->toElement().attribute("cx"));
                if (pathElement->toElement().hasAttribute("cy")) cy = floatParse(pathElement->toElement().attribute("cy"));
                if (pathElement->toElement().hasAttribute("r")) r = floatParse(pathElement->toElement().attribute("r"));
                if (pathElement->toElement().hasAttribute("points")) points = pathElement->toElement().attribute("points"). split(' ');

//                cout << x << y << x1 << y1 << x2 << y2 << width << height << rx << ry << cx << cy << r << points;
                // 两个都是false ,  下面不执行
                if (svgPauseElement || svgPausePenDown) { /*gcode.Pause(gcodeString, "Pause before path");*/ }
                if (form == "rect")
                {
                    if (ry == 0) { ry = rx; }
                    else if (rx == 0) { rx = ry; }
                    else if (rx != ry) { rx = qMin(rx, ry); ry = rx; }   // only same r for x and y are possible, x和y只能使用相同的r
                    if (svgComments) gcodeString.append(QString("( SVG-Rect x:%1 y:%2 width:%3 height:%4 rx:%5 ry:%6)\r\n").arg(x).arg(y).arg(width).arg(height).arg(rx).arg(ry));
                    x += offsetX; y += offsetY;
                    gcodeStartPath(x + rx, y + height, form);   // TODO:
                    gcodeMoveTo(x + width - rx, y + height, form + " a1");
                    if (rx > 0) gcodeArcToCCW(x + width, y + height - ry, 0, -ry, form, avoidG23);  // +ry
                    gcodeMoveTo(x + width, y + ry, form + " b1");                        // upper right, 右上方
                    if (rx > 0) gcodeArcToCCW(x + width - rx, y, -rx, 0, form, avoidG23);
                    gcodeMoveTo(x + rx, y, form + " a2");                                // upper left, 左上方
                    if (rx > 0) gcodeArcToCCW(x, y + ry, 0, ry, form, avoidG23);                    // -ry
                    gcodeMoveTo(x, y + height - ry, form + " b2");                       // lower left, 左下方
                    if (rx > 0)
                    {
                        gcodeArcToCCW(x + rx, y + height, rx, 0, form, avoidG23);
                        gcodeMoveTo(x + rx, y + height, form);  // repeat first point to avoid back draw after last G3
                    }
                    gcodeStopPath(form);
                }
                else if (form == "circle")
                {
                    if (svgComments) gcodeString.append(QString("( circle cx:%1 cy:%2 r:%3 )\r\n").arg(cx).arg(cy).arg(r));
                    cx += offsetX; cy += offsetY;
                    gcodeStartPath(cx + r, cy, form);
                    gcodeArcToCCW(cx + r, cy, -r, 0, form, avoidG23);
                    gcodeStopPath(form);
                }
                else if (form == "ellipse")
                {
                    if (svgComments) gcodeString.append(QString("( ellipse cx:%1 cy:%2 rx:%3  ry:%4)\r\n").arg(cx).arg(cy).arg(rx).arg(ry));
                    cx += offsetX; cy += offsetY;
                    gcodeStartPath(cx + rx, cy, form);
                    isReduceOk = true;
                    calcArc(cx + rx, cy, rx, ry, 0, 1, 1, cx - rx, cy);
                    calcArc(cx - rx, cy, rx, ry, 0, 1, 1, cx + rx, cy);
                    gcodeStopPath(form);

                }
                else if (form == "line")
                {
                    if (svgComments) gcodeString.append(QString("( SVG-Line x1:%1 y1:%2 x2:%3 y2:%4 )\r\n").arg(x1).arg(y1).arg(x2).arg(y2));
                    x1 += offsetX; y1 += offsetY;
                    gcodeStartPath(x1, y1, form);
                    gcodeMoveTo(x2, y2, form);
                    gcodeStopPath(form);
                }
                else if ((form == "polyline") || (form == "polygon"))
                {
                    offsetX = 0;// (float)matrixElement.OffsetX;
                    offsetY = 0;// (float)matrixElement.OffsetY;
                    if (svgComments) gcodeString.append("( SVG-Polyline )\r\n");
                    int index = 0;
                    for (index = 0; index < points.length(); index++)
                    {
                        if (points[index].length() > 0)
                            break;
                    }
                    if (points[index].indexOf(",") >= 0)
                    {
                        QStringList coord = points[index].split(',');
                        x = floatParse(coord[0]); y = floatParse(coord[1]);
                        x1 = x; y1 = y;
                        gcodeStartPath(x, y, form);
                        isReduceOk = true;
                        for (int i = index + 1; i < points.length(); i++)
                        {
                            if (points[i].length() > 3)
                            {
                                coord = points[i].split(',');
                                x = floatParse(coord[0]); y = floatParse(coord[1]);
                                x += offsetX; y += offsetY;
                                gcodeMoveTo(x, y, form);
                            }
                        }
                        if (form == "polygon")
                            gcodeMoveTo(x1, y1, form);
                        gcodeStopPath(form);
                    }
                    else
                        gcodeString.append("( polygon coordinates - missing ',')");
                }
                else if ((form == "text") || (form == "image"))
                {
                    gcodeString.append("( +++++++++++++++++++++++++++++++++ )");
                    gcodeString.append("( ++++++ " + form + " is not supported ++++ )");
                    if (form == "text")
                    {
                        gcodeString.append("( ++ Convert Object to Path first + )");
                    }
                    gcodeString.append("( +++++++++++++++++++++++++++++++++ )");
                }
                else
                { if (svgComments) gcodeString.append("( ++++++ Unknown Shape: " + form + " )"); }

                matrixElement = oldMatrixElement;
            } 
        }
    }
    cout << "解析基础图形:" << gcodeString;
    sw->write(gcodeString.toUtf8());
    return;
}


//转换所有路径命令，查找：http://www.w3.org/TR/SVG/paths.html
//拆分命令标记
void GCodeFromSVG::parsePath(QDomNode* svgCode, int level)
{
    for (int i = 0; i < svgCode->toElement().elementsByTagName("path").count(); i++)
    {
//        cout << "找到:" << svgCode->toElement().elementsByTagName("path").item(i).toElement().attribute("id");
        QDomNode * pathElement = new QDomNode();
        *pathElement = svgCode->toElement().elementsByTagName("path").item(i);
       if (!pathElement->isNull())
       {
           offsetX = 0;// (float)matrixElement.OffsetX;
           offsetY = 0;// (float)matrixElement.OffsetY;
           currentX = offsetX; currentY = offsetX;
           firstX = 0; firstY = 0;
           startPath = true;
           startSubPath = true;
           lastX = offsetX; lastY = offsetY;
           QString d = pathElement->toElement().attribute("d");
//           cout << d;
           QString id = d;
           if (id.length() > 20)
               id = id.mid(0, 20); // 字符串截取
//           cout << id;
           QString myColor = getColor(pathElement);
//           cout << myColor;
           if (svgComments)
           {
               if (pathElement->toElement().hasAttribute("id"))
                   gcodeString.append("\r\n( Path level:" + QString::number(level) + " id=" + pathElement->toElement().attribute("id") + " )\r\n");
               else
                   gcodeString.append("\r\n( SVG path=" + id + " )\r\n");
               // 格式化 字符串
               gcodeString = QString("\r\n(SVG color=#%1)\r\n").arg(gcodeString);
           }

           cout << "解析path标签:" << gcodeString;
           sw->write(gcodeString.toUtf8());

           if (pathElement->toElement().hasAttribute("id"))
               id = pathElement->toElement().attribute("id");
//           cout << id;
           oldMatrixElement = matrixElement;
           // 解析transform属性中的内容
           parseTransform(pathElement, false, level);        // transform will be applied in gcodeMove, 转换将在gcodeMove中应用

           if (d.length() > 0)
           {
               // split complete path in to command-tokens, 将完整路径分割为命令令牌
               if (svgPauseElement || svgPausePenDown) { /*gcode.Pause(gcodeString, "Pause before path");*/ }
//               QStringList tokens = Regex.Split(d, separators).Where(t => !string.IsNullOrEmpty(t));
//               cout << "分割前:" << d;
//               QRegularExpression separators("(?=[A-Za-z-[e]])");  // 正则有问题, 作用是将各个命令分隔开来

               QRegularExpression separators(".(?=[A-Za-z])");
               QStringList tokens = d.split(separators);
               tokens.removeAll("");

//               cout << "传入的d属性分割成命令后:" <<  tokens;
               int objCount = 0;
               foreach (QString token , tokens)
                   objCount += parsePathCommand(token);
           }
           gcodePenUp("End path");

           matrixElement = oldMatrixElement;
       }
   }
   return;
}

// 得到颜色
QString GCodeFromSVG::getColor(QDomNode* pathElement)
{
    QString style = "";
    QString stroke_color = "000000";        // default=black
    if (pathElement->toElement().hasAttribute("style")) // 如果style不为空
    {
        int start, end;
        style = pathElement->toElement().attribute("style");
//        cout << style;
        start = style.indexOf("stroke:#"); // 返回第一个匹配项的索引     
        if (start >= 0)
        {
            end = style.indexOf(';', start); // start为搜索起始位置, 返回第一个匹配项的索引
//             cout << "start:" << start << "end:" << end;
            if (end > start)
                stroke_color = style.mid(start + 8, end - start - 8); // 检索子字符串, 从start+8开始, 指定长度为第二个参数
        }
        return stroke_color; // 返回颜色
    }
    return "";
}

// 解析组元素和包含的元素
void GCodeFromSVG::parseGroup(QDomNode* svgCode, int level)
{
//   foreach (QDomNode groupElement, svgCode->documentElement().elementsByTagName(nspace + "g"))
   for (int i = 0; i < svgCode->toElement().elementsByTagName("g").count(); i++)
   {
       QDomNode * groupElement = new QDomNode();
       *groupElement = svgCode->toElement().elementsByTagName("g").item(i);
       if (svgComments)
           if (groupElement->toElement().hasAttribute("id"))
               gcodeString.append("\r\n( Group level:" + QString::number(level) + " id=" + groupElement->toElement().attribute("id") + " )\r\n");
//       cout << "find g";
       parseTransform(groupElement, true, level);   // transform will be applied in gcodeMove
       if (!svgNodesOnly)
           parseBasicElements(groupElement, level);
       parsePath(groupElement, level);
       parseGroup(groupElement, level + 1);
   }
   return;
}



// 解析转换信息-此处提供更多信息：http://www.w3.org/TR/SVG/coords.html
// 转换将在gcodeMove中应用, 解析transform属性内的信息
bool GCodeFromSVG::parseTransform(QDomNode* element, bool isGroup, int level)   // isGroup 前面传的是false
{
    QMatrix *tmp = new QMatrix(1, 0, 0, 1, 0, 0); // m11, m12, m21, m22, offsetx, offsety
    float M11Val=0, M12Val=0, M21Val=0, M22Val=0, OffsetXVal=0, OffsetYVal=0;
    bool transf = false;
//    cout << "传参数之后的" << element->nodeName() << " 说明成功传进来了";
    if (element->toElement().hasAttribute("transform"))
    {
        transf = true;
        QString tstring = element->toElement().attribute("transform");
//        cout << "transform里的内容: " << tstring;
        if (!(tstring.isNull() || tstring.isEmpty()))
        {
            QRegularExpression re("(?<=[\\)]).");   // 正则没问题了
            QStringList tlist = tstring.split(re);  // 这里匹配的是字符串中间的空格
            std::reverse(tlist.begin(), tlist.end());   //反向, 有效
//            cout << "匹配出的内容反向后: " << tlist;
            foreach (QString tr, tlist)
            {
                if (!tr.isEmpty() && tr.trimmed().length() > 0) //  不为空, 并且删除前后空格后剩余的长度大于零
                {
                    if (tr.indexOf("translate") >= 0)
                    {
                        QString coord = getTextBetween(tr, "translate(", ")");
//                        cout << "获取中间的文本后: " << coord;
                        QStringList split = coord.split(',');   // 以逗号分隔取值
                        if (coord.indexOf(',') < 0)
                            split = coord.split(' ');   // 没有逗号, 就以空格分隔取值
//                        cout << split;
                        float ox = floatParse(split[0]);
                        float oy = (split.length() > 1) ? floatParse(split[1].remove(')')) : 0.0f;  // 这里删除尾部的)用了remove 代替
//                        cout << ox << " " << oy;
                        tmp->translate(ox, oy); // 移动中心 至 ox, oy

                        if (svgComments) gcodeString.append(QString("( SVG-Translate %1 %2 )\r\n").arg(ox).arg(oy));
                    }
                    else if (tr.indexOf("scale") >= 0)
                    {
                        QString coord = getTextBetween(tr, "scale(", ")");
                        QStringList split = coord.split(',');
                        if (coord.indexOf(',') < 0)
                            split = coord.split(' ');
                        M11Val = floatParse(split[0]);
//                        cout << "获取中间的文本后: " << split;
                        if (split.length() > 1)
                        { M22Val = floatParse(split[1]); }
                        else
                        {
                            M11Val = floatParse(coord);
                            M22Val = floatParse(coord);
//                            cout << M11Val << M22Val;
                        }
                        if (svgComments) gcodeString.append(QString("( SVG-Scale %1 %2 )\r\n").arg(tmp->m11()).arg(tmp->m22()));
                    }
                    else if (tr.indexOf("rotate") >= 0)
                    {
                        QString coord = getTextBetween(tr, "rotate(", ")");
                        QStringList split = coord.split(',');
                        if (coord.indexOf(',') < 0)
                            split = coord.split(' ');
//                        cout << split;
                        float angle = floatParse(split[0]); //no need to convert in radiant
                        float px = split.length() == 3 ? floatParse(split[1]) : 0.0f; //<--- this read rotation offset point x
                        float py = split.length() == 3 ? floatParse(split[2]) : 0.0f; //<--- this read rotation offset point y
//                        cout << angle << px << py;
                        tmp->translate(px, py); // 移动中心 至 px, py
                        tmp->rotate(angle); // <--- this apply RotateAt matrix, 旋转angle 角度

                        if (svgComments) gcodeString.append(QString("( SVG-Rotate %1 %2 %3 )\r\n").arg(angle).arg(px).arg(py));
                    }
                    else if (tr.indexOf("matrix") >= 0)
                    {
                        QString coord = getTextBetween(tr, "matrix(", ")");
                        QStringList split = coord.split(',');
                        if (coord.indexOf(',') < 0)
                            split = coord.split(' ');
//                        cout << split;
                        M11Val = floatParse(split[0]);     // a    scale x         a c e
                        M12Val = floatParse(split[1]);     // b                    b d f
                        M21Val = floatParse(split[2]);     // c                    0 0 1
                        M22Val = floatParse(split[3]);     // d    scale y
                        OffsetXVal = floatParse(split[4]); // e    offset x
                        OffsetYVal = floatParse(split[5]); // f    offset y
//                        cout << M11Val << M12Val << M21Val << M22Val << OffsetXVal << OffsetYVal;
                        if (svgComments) gcodeString.append(QString("\r\n( SVG-Matrix %1 %2 %3 )\r\n").arg(coord.replace(',', '|')).arg(level).arg(isGroup));
                    }
                }
            }
        }



        if (isGroup)
        {
//            matrixGroup[level].SetIdentity(); // 设置为单位 矩阵
            if (level > 0)
            {
                for (int i = level; i < 10 ; i++) //matrixGroup.Length, 这里QMatrix数组没有length属性, 直接用具体数字代替了
                { matrixGroup[i] = tmp->QMatrix::operator*(matrixGroup[level - 1]); }
            }
            else
            { matrixGroup[level].QMatrix::operator=(*tmp) ; }   // 赋值
            matrixElement = matrixGroup[level];

        }
        else
        {
            matrixElement = tmp->QMatrix::operator*( matrixGroup[level]);
        }

        if (svgComments && transf)
        {
            for (int i = 0; i <= level; i++)
                gcodeString.append(QString("( gc-Matrix level[%1] %2 )\r\n").arg(i).arg(
                     QString("%1%2%3%4%5").arg(matrixGroup[i].m11()).arg(matrixGroup[i].m12()).arg(matrixGroup[i].m21()).arg(matrixGroup[i].m22()).arg(matrixGroup[i].dx()).arg(matrixGroup[i].dy())));

            if (svgComments) gcodeString.append(QString("( gc-Scale %1 %2 )\r\n").arg(matrixElement.m11()).arg(matrixElement.m22()));
            if (svgComments) gcodeString.append(QString("( gc-Offset %1 %2 )\r\n").arg(matrixElement.dx()).arg(matrixElement.dy()));
        }
    }
    cout << "解析transform:" << gcodeString;
    sw->write(gcodeString.toUtf8());
    // 设置m11, m12, m21, m22, offsetx, offsety
    tmp->setMatrix(M11Val, M12Val, M21Val, M22Val, OffsetXVal, OffsetYVal);
//    cout << "最后:" << M11Val << M12Val << M21Val << M22Val << OffsetXVal << OffsetYVal;
    return transf;
}


QString GCodeFromSVG::getTextBetween(QString source, QString s1, QString s2)
{
    int start = source.indexOf(s1) + s1.length();
    QChar c;
    for (int i = start; i < source.length(); i++)
    {
        c = source.at(i);
        if (!(c.isNumber() || c == '.' || c == ',' || c == ' ' || c == '-' || c == 'e'))    // also exponent
            return source.mid(start, i - start);
    }
    return source.mid(start, source.length() - start - 1);
}


// 插入Pen-up gcode命令, 应该是插入了M4(cmt)\r\n字符串
void GCodeFromSVG::gcodePenUp(QString cmt)
{
    if (penIsDown)
        GcodeRelated::PenUp(gcodeString, cmt);    //TODO: gcoderelated.cs
    sw->write(GcodeRelated::gcodeStrings.toUtf8());
    penIsDown = false;
}


// 插入Pen-down gcode命令
// M3 S999 (cmt)\r\n
void GCodeFromSVG::gcodePenDown(QString cmt)
{
    if (!penIsDown)
        GcodeRelated::PenDown(gcodeString, cmt);
    sw->write(GcodeRelated::gcodeStrings.toUtf8());
    penIsDown = true;
}

// Insert G0 and Pen down gcode command,
// 插入G0和Pen down gcode命令

void GCodeFromSVG::gcodeStartPath(float x, float y, QString cmt)
{
    QPoint coord = translateXY(x, y);
    lastGCX = coord.x(); lastGCY = coord.y();
    lastSetGCX = coord.x(); lastSetGCY = coord.y();
    gcodePenUp(cmt);

    GcodeRelated::MoveToRapid(gcodeString, coord, cmt);   // TODO: gcoderelated.cs
    sw->write(GcodeRelated::gcodeStrings.toUtf8());
    if (svgPausePenDown) { /*gcode.Pause(gcodeString, "Pause before Pen Down");*/ }
    penIsDown = false;
    isReduceOk = false;
}

QPoint GCodeFromSVG::translateXY(QPoint pointStart)
{
    QPoint pointResult = matrixElement.map(pointStart); //map 代替   Transform 函数
    return pointResult;
}

// Transform XY coordinate using matrix and scale  ,
// 使用矩阵和比例变换X，Y坐标
QPoint GCodeFromSVG::translateXY(float x, float y)
{
    QPoint coord(x, y);
    return translateXY(coord);
}

// Insert G1 gcode command, 插入G1 gcode命令
void GCodeFromSVG::gcodeMoveTo(float x, float y, QString cmt)
{
    QPoint coord(x, y);
    gcodeMoveTo(coord, cmt);
}
// Insert G1 gcode command, 插入G1 gcode命令
void GCodeFromSVG::gcodeMoveTo(QPoint orig, QString cmt)
{
    QPoint coord = translateXY(orig);
    rejectPoint = false;
//    gcodePenDown(cmt);    // 避免重复暂时先注释了
    if (gcodeReduce && isReduceOk)
    {
        distance = qSqrt(((coord.x() - lastSetGCX) * (coord.x() - lastSetGCX)) + ((coord.y() - lastSetGCY) * (coord.y() - lastSetGCY)));
        if (distance < gcodeReduceVal)      // discard actual G1 movement, 放弃实际的G1运动
        {
            rejectPoint = true;
        }
        else
        {
            lastSetGCX = coord.x(); lastSetGCY = coord.y();
        }
    }
    if (!gcodeReduce || !rejectPoint)       // write GCode
    {
        GcodeRelated::MoveTo(gcodeString, coord, cmt); // TODO:  gcoderelated.cs
        sw->write(GcodeRelated::gcodeStrings.toUtf8());
    }
    lastGCX = coord.x(); lastGCY = coord.y();
}

// Insert G2/G3 gcode command, 插入G2/G3 gcode命令
void GCodeFromSVG::gcodeArcToCCW(float x, float y, float i, float j, QString cmt, bool avoidG23)
{
    QPoint coordxy = translateXY(x, y);
    QPoint coordij = translateIJ(i, j);
    gcodePenDown(cmt);
    if (gcodeReduce && isReduceOk)      // restore last skipped point for accurat G2/G3 use
    {
        if ((lastSetGCX != lastGCX) || (lastSetGCY != lastGCY))
            GcodeRelated::MoveTo(gcodeString, QPoint(lastGCX, lastGCY), cmt);
        sw->write(GcodeRelated::gcodeStrings.toUtf8());
    }
    GcodeRelated::Arc(gcodeString, 3, coordxy, coordij, cmt, avoidG23);       // TODO: gcoderelated.cs
    sw->write(GcodeRelated::gcodeStrings.toUtf8());
}


// 使用矩阵和比例变换I，J坐标
QPoint GCodeFromSVG::translateIJ(float i, float j)
{
    QPoint coord(i, j);
    return translateIJ(coord);
}
QPoint GCodeFromSVG::translateIJ(QPoint pointStart)
{
    QPoint pointResult = pointStart;
    double tmp_i = pointStart.x(), tmp_j = pointStart.y();
    pointResult.setX(tmp_i * matrixElement.m11() + tmp_j * matrixElement.m21());  // - tmp
    pointResult.setY(tmp_i * matrixElement.m12() + tmp_j * matrixElement.m22()); // tmp_i*-matrix     // i,j are relative - no offset needed, but perhaps rotation
    return pointResult;
}

// Insert Pen-up gcode command
void GCodeFromSVG::gcodeStopPath(QString cmt)
{
    if (gcodeReduce)
    {
        // TODO: gcoderelated.cs
        if ((lastSetGCX != lastGCX) || (lastSetGCY != lastGCY)) // restore last skipped point for accurat G2/G3 use
            GcodeRelated::MoveTo(gcodeString, QPoint(lastGCX, lastGCY), "restore Point");
        sw->write(GcodeRelated::gcodeStrings.toUtf8());
    }
    gcodePenUp(cmt);
}


// 计算路径弧命令-代码,     从https://github.com/vvvv/SVG/blob/master/Source/Paths/SvgArcSegment.cs
void GCodeFromSVG::calcArc(float StartX, float StartY, float RadiusX, float RadiusY, float Angle, float Size, float Sweep, float EndX, float EndY)
{
    if (RadiusX == 0.0f && RadiusY == 0.0f)
    {
        //              graphicsPath.AddLine(this.Start, this.End);
        return;
    }
    double sinPhi = qSin(Angle * M_PI / 180.0);
    double cosPhi = qCos(Angle * M_PI / 180.0);
    double x1dash = cosPhi * (StartX - EndX) / 2.0 + sinPhi * (StartY - EndY) / 2.0;
    double y1dash = -sinPhi * (StartX - EndX) / 2.0 + cosPhi * (StartY - EndY) / 2.0;
    double root;
    double numerator = RadiusX * RadiusX * RadiusY * RadiusY - RadiusX * RadiusX * y1dash * y1dash - RadiusY * RadiusY * x1dash * x1dash;
    float rx = RadiusX;
    float ry = RadiusY;
    if (numerator < 0.0)
    {
        float s = (float)qSqrt(1.0 - numerator / (RadiusX * RadiusX * RadiusY * RadiusY));

        rx *= s;
        ry *= s;
        root = 0.0;
    }
    else
    {
        root = ((Size == 1 && Sweep == 1) || (Size == 0 && Sweep == 0) ? -1.0 : 1.0) * qSqrt(numerator / (RadiusX * RadiusX * y1dash * y1dash + RadiusY * RadiusY * x1dash * x1dash));
    }
    double cxdash = root * rx * y1dash / ry;
    double cydash = -root * ry * x1dash / rx;
    double cx = cosPhi * cxdash - sinPhi * cydash + (StartX + EndX) / 2.0;
    double cy = sinPhi * cxdash + cosPhi * cydash + (StartY + EndY) / 2.0;
    double theta1 = CalculateVectorAngle(1.0, 0.0, (x1dash - cxdash) / rx, (y1dash - cydash) / ry);
    double dtheta = CalculateVectorAngle((x1dash - cxdash) / rx, (y1dash - cydash) / ry, (-x1dash - cxdash) / rx, (-y1dash - cydash) / ry);
    if (Sweep == 0 && dtheta > 0)
    {
        dtheta -= 2.0 * M_PI;
    }
    else if (Sweep == 1 && dtheta < 0)
    {
        dtheta += 2.0 * M_PI;
    }
    int segments = (int)qCeil((double)qAbs(dtheta / (M_PI / 2.0)));
    double delta = dtheta / segments;
    double t = 8.0 / 3.0 * qSin(delta / 4.0) * qSin(delta / 4.0) / qSin(delta / 2.0);

    double startX = StartX;
    double startY = StartY;

    for (int i = 0; i < segments; ++i)
    {
        double cosTheta1 = qCos(theta1);
        double sinTheta1 = qSin(theta1);
        double theta2 = theta1 + delta;
        double cosTheta2 = qCos(theta2);
        double sinTheta2 = qSin(theta2);

        double endpointX = cosPhi * rx * cosTheta2 - sinPhi * ry * sinTheta2 + cx;
        double endpointY = sinPhi * rx * cosTheta2 + cosPhi * ry * sinTheta2 + cy;

        double dx1 = t * (-cosPhi * rx * sinTheta1 - sinPhi * ry * cosTheta1);
        double dy1 = t * (-sinPhi * rx * sinTheta1 + cosPhi * ry * cosTheta1);

        double dxe = t * (cosPhi * rx * sinTheta2 + sinPhi * ry * cosTheta2);
        double dye = t * (sinPhi * rx * sinTheta2 - cosPhi * ry * cosTheta2);

//        points = new QPoint[4];
        points[0] = QPoint(startX, startY);
        points[1] = QPoint((startX + dx1), (startY + dy1));
        points[2] = QPoint((endpointX + dxe), (endpointY + dye));
        points[3] = QPoint(endpointX, endpointY);
        QPoint* b = GetBezierApproximation(points, svgBezierAccuracy);

        for (int k = 1; k < b->manhattanLength(); k++)
            gcodeMoveTo(b[k], "arc");

        theta1 = theta2;
        startX = (float)endpointX;
        startY = (float)endpointY;
    }
}

double GCodeFromSVG::CalculateVectorAngle(double ux, double uy, double vx, double vy)
{
    double ta = qAtan2(uy, ux);
    double tb = qAtan2(vy, vx);
    if (tb >= ta)
    { return tb - ta; }
    return M_PI * 2 - (ta - tb);
}

QPoint * GCodeFromSVG::GetBezierApproximation(QPoint * controlPoints, int outputSegmentCount)
{
//    QPoint points[outputSegmentCount + 1];
    QPoint * points = new QPoint[outputSegmentCount + 1];
    for (int i = 0; i <= outputSegmentCount; i++)
    {
        double t = (double)i / outputSegmentCount;
        points[i] = GetBezierPoint(t, controlPoints, 0, 4); // 4 原本是 controlPoints.Length
    }
    return points;  //  可能有点问题
}

QPoint GCodeFromSVG::GetBezierPoint(double t, QPoint* controlPoints, int index, int count)
{
    if (count == 1)
        return controlPoints[index];
    QPoint P0 = GetBezierPoint(t, controlPoints, index, count - 1);
    QPoint P1 = GetBezierPoint(t, controlPoints, index + 1, count - 1);
    double x = (1 - t) * P0.x() + t * P1.x();
    return QPoint(x, (1 - t) * P0.y() + t * P1.y());
}


// 转换所有路径命令，请检查：http://www.w3.org/TR/SVG/paths.html
// 转换命令令牌, 这里传入的是一个个命令, 如 L 8.7185884,-4.0017078
int GCodeFromSVG::parsePathCommand(QString svgPath)
{
//    var command = svgPath.Take(1).Single(); // take(1), 返回第一个, 这里的作用就是取出第一个字符
//    char cmd = char.ToUpper(command);
    QChar command = svgPath.at(0);
    QChar cmd = command.toUpper();
    bool absolute = (cmd == command);
//    cout << "开头字母:" << command << cmd;
//    cout << "传进来的字符串" << svgPath;

    QString remainingargs = svgPath.mid(2);
//    cout << "命令去除字母后:" << remainingargs;
    if (remainingargs.isNull()||remainingargs.isEmpty()) return 0;
    QRegularExpression re("[\\s,]|(?=(?<!e)-)");       // 正则没问题
    QStringList splitArgs = remainingargs.split(re);
    splitArgs.removeAll("");
//    cout << "将各个命令分割后: " << splitArgs;
//    cout << splitArgs.length();

    // get command coordinates, 获取命令坐标
//    float[] floatArgs = splitArgs.Select(arg => floatParse(arg)).ToArray();
    float floatArgs[splitArgs.length()];
    for (int i = 0; i < splitArgs.length(); i++)
    {
        QString tempstring = splitArgs.at(i);
        floatArgs[i] = floatParse(tempstring);;
//        cout << floatArgs[i];
    }
    int objCount = 0;
    int floatArgsSize = sizeof (floatArgs)/sizeof (floatArgs[0]);

    switch (cmd.unicode())  // 将QChar 转换为 char
    {
        case 'M':       // Start a new sub-path at the given (x,y) coordinate, 在给定的（x，y）坐标处开始一个新的子路径
            for (int i = 0; i < floatArgsSize; i += 2)
            {
                objCount++;
                if (absolute || startPath)
                { currentX = floatArgs[i] + offsetX; currentY = floatArgs[i + 1] + offsetY; }
                else
                { currentX = floatArgs[i] + lastX; currentY = floatArgs[i + 1] + lastY; }
                if (startSubPath)
                {
                    if (svgComments) { gcodeString.append(QString("( Start new subpath at %1 %2 )\r\n").arg(floatArgs[i]).arg(floatArgs[i + 1])); }
                    //                            pathCount = 0;

                    if (countSubPath++ > 0)
                        gcodeStopPath("Stop Path");
                    if (svgNodesOnly)
                        gcodeDotOnly(currentX, currentY, QString(command));
                    else
                        gcodeStartPath(currentX, currentY, QString(command));
                    isReduceOk = true;
                    firstX = currentX; firstY = currentY;
                    startPath = false;
                    startSubPath = false;

                }
                else
                {
                    if (svgNodesOnly)
                        gcodeDotOnly(currentX, currentY, QString(command));
                    else if (i <= 1) // amount of coordinates, 坐标量
                    { gcodeStartPath(currentX, currentY, QString(command)); }//gcodeMoveTo(currentX, currentY, command.ToString());  // G1
                    else
                        gcodeMoveTo(currentX, currentY, QString(command));  // G1
                }
                if (firstX == 0) { firstX = currentX; }
                if (firstY == 0) { firstY = currentY; }
                lastX = currentX; lastY = currentY;
            }
            cxMirror = currentX; cyMirror = currentY;
//            cout << cxMirror << cyMirror;
            break;

        case 'Z':       // Close the current subpath, 关闭当前子路径
            if (!svgNodesOnly)
            {
                if (firstX == 0) { firstX = currentX; }
                if (firstY == 0) { firstY = currentY; }
                gcodeMoveTo((float)firstX, (float)firstY, QString(command));    // G1
            }
            lastX = (float)firstX; lastY = (float)firstY;
            firstX = 0; firstY = 0;
            startSubPath = true;
            if ((svgClosePathExtend) && (!svgNodesOnly))
            { gcodeString.append(secondMove); }
            gcodeStopPath("Z");
            break;

        case 'L':       // Draw a line from the current point to the given (x,y) coordinate, 从当前点到给定的（x，y）坐标画一条线
            for (int i = 0; i < floatArgsSize; i += 2)
            {
                objCount++;
                if (absolute)
                { currentX = floatArgs[i] + offsetX; currentY = floatArgs[i + 1] + offsetY; }
                else
                { currentX = lastX + floatArgs[i]; currentY = lastY + floatArgs[i + 1]; }
                if (svgNodesOnly)
                    gcodeDotOnly(currentX, currentY, QString(command));
                else
                    gcodeMoveTo(currentX, currentY, QString(command));
                lastX = currentX; lastY = currentY;
                cxMirror = currentX; cyMirror = currentY;
            }
            startSubPath = true;
            break;

        case 'H':       // Draws a horizontal line from the current point (cpx, cpy) to (x, cpy), 从当前点（cpx，cpy）到（x，cpy）画一条水平线
            for (int i = 0; i < floatArgsSize; i++)
            {
                objCount++;
                if (absolute)
                { currentX = floatArgs[i] + offsetX; currentY = lastY; }
                else
                { currentX = lastX + floatArgs[i]; currentY = lastY; }
                if (svgNodesOnly)
                    gcodeDotOnly(currentX, currentY, QString(command));
                else
                    gcodeMoveTo(currentX, currentY, QString(command));
                lastX = currentX; lastY = currentY;
                cxMirror = currentX; cyMirror = currentY;
            }
            startSubPath = true;
            break;

        case 'V':       // Draws a vertical line from the current point (cpx, cpy) to (cpx, y), 从当前点（cpx，cpy）到（cpx，y）画一条垂直线
            for (int i = 0; i < floatArgsSize; i++)
            {
                objCount++;
                if (absolute)
                { currentX = lastX; currentY = floatArgs[i] + offsetY; }
                else
                { currentX = lastX; currentY = lastY + floatArgs[i]; }
                if (svgNodesOnly)
                    gcodeDotOnly(currentX, currentY, QString(command));
                else
                    gcodeMoveTo(currentX, currentY, QString(command));
                lastX = currentX; lastY = currentY;
                cxMirror = currentX; cyMirror = currentY;
            }
            startSubPath = true;
            break;

        case 'A':       // Draws an elliptical arc from the current point to (x, y), 从当前点到（x，y）绘制一个椭圆弧
            if (svgComments) { gcodeString.append(QString("( Command %1 %2 )\r\n").arg(QString(command)).arg(((absolute == true) ? "absolute" : "relative"))); }
            for (int rep = 0; rep < floatArgsSize; rep += 7)
            {
                objCount++;
                if (svgComments) { gcodeString.append(QString("( draw arc nr. %1 )\r\n").arg(1 + rep / 6)); }
                float rx, ry, rot, large, sweep, nx, ny;
                rx = floatArgs[rep]; ry = floatArgs[rep + 1];
                rot = floatArgs[rep + 2];
                large = floatArgs[rep + 3];
                sweep = floatArgs[rep + 4];
                if (absolute)
                {
                    nx = floatArgs[rep + 5] + offsetX; ny = floatArgs[rep + 6] + offsetY;
                }
                else
                {
                    nx = floatArgs[rep + 5] + lastX; ny = floatArgs[rep + 6] + lastY;
                }
                if (svgNodesOnly)
                    gcodeDotOnly(currentX, currentY, QString(command));
                else
                    calcArc(lastX, lastY, rx, ry, rot, large, sweep, nx, ny);
                lastX = nx; lastY = ny;
            }
            startSubPath = true;
            break;

        case 'C':       // Draws a cubic Bézier curve from the current point to (x,y), 从当前点到（x，y）绘制三次贝塞尔曲线
            if (svgComments) { gcodeString.append(QString("( Command %1 %2 )\r\n").arg(QString(command)).arg((absolute == true) ? "absolute" : "relative")); }
            for (int rep = 0; rep < floatArgsSize; rep += 6)
            {
                objCount++;
                if (svgComments) { gcodeString.append(QString("( draw curve nr. %1 )\r\n").arg(1 + rep / 6)); }
                if ((rep + 5) < floatArgsSize)
                {
                    float cx1, cy1, cx2, cy2, cx3, cy3;
                    if (absolute)
                    {
                        cx1 = floatArgs[rep] + offsetX; cy1 = floatArgs[rep + 1] + offsetY;
                        cx2 = floatArgs[rep + 2] + offsetX; cy2 = floatArgs[rep + 3] + offsetY;
                        cx3 = floatArgs[rep + 4] + offsetX; cy3 = floatArgs[rep + 5] + offsetY;
                    }
                    else
                    {
                        cx1 = lastX + floatArgs[rep]; cy1 = lastY + floatArgs[rep + 1];
                        cx2 = lastX + floatArgs[rep + 2]; cy2 = lastY + floatArgs[rep + 3];
                        cx3 = lastX + floatArgs[rep + 4]; cy3 = lastY + floatArgs[rep + 5];
                    }
                    QPoint *points = new QPoint[4];
                    points[0] = QPoint(lastX, lastY);
                    points[1] = QPoint(cx1, cy1);
                    points[2] = QPoint(cx2, cy2);
                    points[3] = QPoint(cx3, cy3);
                    QPoint * b = GetBezierApproximation(points, svgBezierAccuracy);
                    if (svgNodesOnly)
                    {
                        gcodeDotOnly(cx3, cy3, QString(command));
                    }
                    else
                    {
                        for (int i = 1; i < b->manhattanLength(); i++)
                            gcodeMoveTo((float)b[i].x(), (float)b[i].y(), QString(command));
                    }
                    cxMirror = cx3 - (cx2 - cx3); cyMirror = cy3 - (cy2 - cy3);
                    lastX = cx3; lastY = cy3;
                }
                else
                { gcodeString.append(QString("( Missing argument after %1 )\r\n").arg(rep)); }
            }
            startSubPath = true;
            break;

        case 'S':       // Draws a cubic Bézier curve from the current point to (x,y), 从当前点到（x，y）绘制三次贝塞尔曲线
            if (svgComments) { gcodeString.append(QString("( Command %1 %2 )\r\n").arg(QString(command).arg((absolute == true) ? "absolute" : "relative"))); }
            for (int rep = 0; rep < floatArgsSize; rep += 4)
            {
                objCount++;
                if (svgComments) { gcodeString.append(QString("( draw curve nr. {0} )\r\n").arg(1 + rep / 4)); }
                float cx2, cy2, cx3, cy3;
                if (absolute)
                {
                    cx2 = floatArgs[rep] + offsetX; cy2 = floatArgs[rep + 1] + offsetY;
                    cx3 = floatArgs[rep + 2] + offsetX; cy3 = floatArgs[rep + 3] + offsetY;
                }
                else
                {
                    cx2 = lastX + floatArgs[rep]; cy2 = lastY + floatArgs[rep + 1];
                    cx3 = lastX + floatArgs[rep + 2]; cy3 = lastY + floatArgs[rep + 3];
                }
                QPoint points[4];
                points[0] = QPoint(lastX, lastY);
                points[1] = QPoint(cxMirror, cyMirror);
                points[2] = QPoint(cx2, cy2);
                points[3] = QPoint(cx3, cy3);
                QPoint * b = GetBezierApproximation(points, svgBezierAccuracy);
                if (svgNodesOnly)
                {
                    gcodeDotOnly(cx3, cy3, QString(command));
                }
                else
                {
                    for (int i = 1; i < b->manhattanLength(); i++)
                        gcodeMoveTo((float)b[i].x(), (float)b[i].y(), QString(command));
                }
                cxMirror = cx3 - (cx2 - cx3); cyMirror = cy3 - (cy2 - cy3);
                lastX = cx3; lastY = cy3;
            }
            startSubPath = true;
            break;

        case 'Q':       // Draws a quadratic Bézier curve from the current point to (x,y), 从当前点到（x，y）绘制二次贝塞尔曲线
            if (svgComments) { gcodeString.append(QString("( Command {0} {1} )\r\n").arg(QString(command)).arg((absolute == true) ? "absolute" : "relative")); }
            for (int rep = 0; rep < floatArgsSize; rep += 4)
            {
                objCount++;
                if (svgComments) { gcodeString.append(QString("( draw curve nr. {0} )\r\n").arg(1 + rep / 4)); }
                float cx2, cy2, cx3, cy3;
                if (absolute)
                {
                    cx2 = floatArgs[rep] + offsetX; cy2 = floatArgs[rep + 1] + offsetY;
                    cx3 = floatArgs[rep + 2] + offsetX; cy3 = floatArgs[rep + 3] + offsetY;
                }
                else
                {
                    cx2 = lastX + floatArgs[rep]; cy2 = lastY + floatArgs[rep + 1];
                    cx3 = lastX + floatArgs[rep + 2]; cy3 = lastY + floatArgs[rep + 3];
                }

                float qpx1 = (cx2 - lastX) * 2 / 3 + lastX;     // shorten control points to 2/3 length to use , 将控制点缩短为使用的2/3长度
                float qpy1 = (cy2 - lastY) * 2 / 3 + lastY;     // qubic function, 三次函数
                float qpx2 = (cx2 - cx3) * 2 / 3 + cx3;
                float qpy2 = (cy2 - cy3) * 2 / 3 + cy3;
                QPoint points[4];
                points[0] = QPoint(lastX, lastY);
                points[1] = QPoint(qpx1, qpy1);
                points[2] = QPoint(qpx2, qpy2);
                points[3] = QPoint(cx3, cy3);
                cxMirror = cx3 - (cx2 - cx3); cyMirror = cy3 - (cy2 - cy3);
                lastX = cx3; lastY = cy3;
                QPoint * b = GetBezierApproximation(points, svgBezierAccuracy);
                if (svgNodesOnly)
                {
                    gcodeDotOnly(cx3, cy3, QString(command));
                }
                else
                {
                    for (int i = 1; i < b->manhattanLength(); i++)
                        gcodeMoveTo((float)b[i].x(), (float)b[i].y(), QString(command));
                }
            }
            startSubPath = true;
            break;

        case 'T':       // Draws a quadratic Bézier curve from the current point to (x,y), 从当前点到（x，y）绘制二次贝塞尔曲线
            if (svgComments) { gcodeString.append(QString("( Command {0} {1} )\r\n").arg(QString(command)).arg((absolute == true) ? "absolute" : "relative")); }
            for (int rep = 0; rep < floatArgsSize; rep += 2)
            {
                objCount++;
                if (svgComments) { gcodeString.append(QString("( draw curve nr. {0} )\r\n").arg(1 + rep / 2)); }
                float cx3, cy3;
                if (absolute)
                {
                    cx3 = floatArgs[rep] + offsetX; cy3 = floatArgs[rep + 1] + offsetY;
                }
                else
                {
                    cx3 = lastX + floatArgs[rep]; cy3 = lastY + floatArgs[rep + 1];
                }

                float qpx1 = (cxMirror - lastX) * 2 / 3 + lastX;     // shorten control points to 2/3 length to use
                float qpy1 = (cyMirror - lastY) * 2 / 3 + lastY;     // qubic function
                float qpx2 = (cxMirror - cx3) * 2 / 3 + cx3;
                float qpy2 = (cyMirror - cy3) * 2 / 3 + cy3;
                QPoint points[4];
                points[0] = QPoint(lastX, lastY);
                points[1] = QPoint(qpx1, qpy1);
                points[2] = QPoint(qpx2, qpy2);
                points[3] = QPoint(cx3, cy3);
                cxMirror = cx3; cyMirror = cy3;
                lastX = cx3; lastY = cy3;
                QPoint * b = GetBezierApproximation(points, svgBezierAccuracy);
                if (svgNodesOnly)
                {
                    gcodeDotOnly(cx3, cy3, QString(command));
                }
                else
                {
                    for (int i = 1; i < b->manhattanLength(); i++)
                        gcodeMoveTo((float)b[i].x(), (float)b[i].y(), QString(command));
                }
            }
            startSubPath = true;
            break;

        default:
            if (svgComments) gcodeString.append(QString("( *********** unknown: %1 ***** )\r\n").arg(command));
            break;
    }
    cout << "解析令牌命令:" << gcodeString;
    sw->write(gcodeString.toUtf8());
    return objCount;
}


void GCodeFromSVG::gcodeDotOnly(float x, float y, QString cmt)
{
    gcodeStartPath(x, y, cmt);
    gcodePenDown(cmt);
    gcodePenUp(cmt);
}
