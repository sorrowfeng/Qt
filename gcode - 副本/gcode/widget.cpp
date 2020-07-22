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

//    cout << gcode.left(10);


}

Widget::~Widget()
{
    delete ui;

}

