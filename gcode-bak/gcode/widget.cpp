#include "widget.h"
#include "ui_widget.h"
#include "gcodefromsvg.h"
#include <QFile>
#include <QDebug>
#define cout  qDebug() << "[" << __FILE__ << ":" << __LINE__ << "]"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);


    /*
     * xml文档顺序      ---所有东西都是元素 (element)
     * 根节点  (documentElement)获取根节点, 这里函数返回直接就是一个元素(element)
     * --子节点       节点(node)
     * -----属性      (attribute获取内容, attributeNode获取属性对象)
     * -----内容      (text)
     */

    // 读取svg图片
    QFile file("../test.svg");
    // 打开文件
    if(!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::information(nullptr, QString("title"),QString("open error!"));
        return;
    }
    // 绑定文件
    QString error;
    int row = 0, column = 0;
    if(!svgCode.setContent(&file, false, &error, &row, &column)) { //形参2，是否创建命名空间
        QMessageBox::information(nullptr, QString("Error"),
                                 QString("parse file failed at line row and column") +
                                 QString::number(row, 10) + QString(",") +
                                 QString::number(column, 10));
        file.close();
        return;
    }
    // 如果内容为空
    if(svgCode.isNull()) {
       QMessageBox::information(nullptr, QString("title"), QString("document is null!"));
       file.close();
       return;
    }




    GCodeFromSVG gfs;
    QString gcode = gfs.convertSVG(&svgCode);

    cout << gcode.left(10);



//    // 获取根节点, 这里是svg
//    QDomElement root=svgCode.documentElement();
//    // root节点里是否含有属性"xmlns"
//    qDebug() << root.nodeName() << root.hasAttribute("xmlns") ;
//    // 打印该节点中含有"xmlns"属性的值
//    qDebug() << root.attributeNode("xmlns").value();
//    // 获取第一个子节点
//    QDomNode node = root.firstChild();
//    // 打印子节点的名字
//    qDebug() << node.nodeName();
//    // 注意节点要转换为Element才能得到其中value的值, 直接nodeValue是空的

//    // 得到value的两种方法
//    // 1. attribute, 直接一字符串返回value
//    QString id = node.toElement().attribute("id");
//    // 2. attributeNode, 返回的是个QDomAttr对象(属性), 通过value再得到里面的值
//    id = node.toElement().attributeNode("id").value();
//    // 获取元素内的内容text
//    qDebug() << node.toElement().text();

//    // 也可以来到子节点的下一个节点
//    node.nextSibling();

//    // 获取了root的所有子节点, 即svg的所有子节点
//    QDomNodeList list = root.childNodes();
//    for (int i = 0; i < list.length(); i++)
//    {
//        QDomNode node = list.at(i);
//        qDebug() << node.nodeName();
//    }

//    // 指针来到下一个节点, 这里是来到和root并行的下一个节点
//    QDomNode nextnode = root.nextSibling();
}

Widget::~Widget()
{
    delete ui;

}

