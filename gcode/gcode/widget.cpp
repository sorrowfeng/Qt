#include "widget.h"
#include "ui_widget.h"
#include "gcodefromsvg.h"
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QSvgRenderer>
#include <QPainter>
#include <QDebug>
#define cout  qDebug() << "[" << __FILE__ << ":" << __LINE__ << "]"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
}

Widget::~Widget()
{
    delete ui;
}


void Widget::on_pushButtonLoad_clicked()
{
    // 读取svg图片
    QString filepath = QFileDialog::getOpenFileName(this, "open", "../");
    QFile file(filepath);
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

    QSvgRenderer* svgRender = new QSvgRenderer();
    svgRender->load(filepath);

    QPixmap* pixmap = new QPixmap(32,32);
    pixmap->fill(Qt::transparent);//设置背景透明
    QPainter p(pixmap);
    svgRender->render(&p);
    ui->label->setPixmap(*pixmap);
}

void Widget::on_pushButtonSave_clicked()
{
    GCodeFromSVG gfs;
    if(gfs.convertSVG(&svgCode) == "true")
        QMessageBox::information(this, "save", "save success");
}
