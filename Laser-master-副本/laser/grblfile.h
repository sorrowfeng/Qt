#ifndef GRBLFILE_H
#define GRBLFILE_H

#include <QWidget>
#include <QList>
#include <iostream>
#include <QString>



class GrblFile : public QWidget
{
    Q_OBJECT
public:
    explicit GrblFile(QWidget *parent = nullptr);

signals:

public slots:

public:
//    void SaveProgram(QString filename, bool header, bool footer, bool between, int cycles);


private:
    QString GCODE_STD_HEADER;
    QString GCODE_STD_FOOTER;
};



#endif // GRBLFILE_H
