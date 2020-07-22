#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QDomDocument>
#include <QFile>
#include <QMessageBox>
#include <QTreeWidget>
#include <QDebug>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();


private:
    Ui::Widget *ui;
    QTreeWidget *treeWidget;


    // Entrypoint for conversion: apply file-path or file-URL, 转换的入口点：应用文件路径或文件URL
    QDomDocument svgCode;
    bool importInMM = false;
};

#endif // WIDGET_H
