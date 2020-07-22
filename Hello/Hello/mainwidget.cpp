#include "mainwidget.h"
#include "ui_mainwidget.h"
#include <QMessageBox>
#include <QDebug>
#include <QRegExp>
#include <QRegularExpression>

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWidget)
{
    ui->setupUi(this);

    QRegularExpression qre("(?<=[\\)])");
    QRegularExpression qre1("[\\s,]|(?=(?<!e)-)");
    QRegularExpression qre2(".(?=[A-Za-z])");
    QString source = "scale(1.1) rotate(180) translate(1,0)";
    QString source1 = "8.7185884,-4.0017078";
    QString source2 = "M 8.7185878,4.0337352 L -2.2072895,0.016013256 L 8.7185884,-4.0017078 C 6.9730900,-1.6296469 6.9831476,1.6157441 8.7185878,4.0337352 z";
    QString source3 = "M 55,31.5 C 55,43.926407 44.926407,54 32.5,54 20.073593,54 10,43.926407 10,31.5 10,19.073593 20.073593,9 32.5,9 c 5.967371,0 11.690333,2.370529 15.909902,6.590097";
    qDebug() << source.split(qre);
    qDebug() << source1.split(qre1);
    qDebug() << source2.split(qre2);
    qDebug() << source3.split(qre2);
    QString temp = source3.split(qre2).at(2);
    qDebug() << temp.mid(1);
    QRegularExpression re("[\\s,]|(?=(?<!e)-)");       // 正则没问题
    QStringList splitArgs = temp.mid(1).split(re);
    splitArgs.removeAll("");
    qDebug() << "分割后: " << splitArgs;
}

MainWidget::~MainWidget()
{
    delete ui;
}


void MainWidget::on_pushButton_clicked()
{
    this->close();
}

void MainWidget::on_pushButton_2_clicked()
{

}
