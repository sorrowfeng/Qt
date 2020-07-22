#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QFile>
#include <QWidget>
#include <QSerialPort>
#include <QSerialPortInfo>

namespace Ui {
class MainWidget;
}

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = nullptr);
    ~MainWidget();
    void receiveInfo();
    void sendInfo(char* info,int len);
    void sendInfo(const QString &info);
    void convertStringToHex(const QString &str, QByteArray &byteData);
    char convertCharToHex(char ch);
    QStringList getPortNameList();

private slots:
    void on_comboBoxCom_activated(int index);

    void on_pushButtonLink_clicked();

    void on_pushButtonDisconnect_clicked();

    void on_pushButtonChoose_clicked();

    void on_pushButtonSend_clicked();

private:
    Ui::MainWidget *ui;
    QStringList  m_serialPortName;
    QSerialPort* m_serialPort; //串口类
    int com_index=0;
    QString filename;
    quint64 filesize;
    quint64 sendsize;
    QFile *file;
};

#endif // MAINWIDGET_H
