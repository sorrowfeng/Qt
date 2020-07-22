#include "mainwidget.h"
#include "ui_mainwidget.h"
#include "savefile.h"
#include <QAction>
#include <QProcess>
#include <QFileDialog>
#include <QFileInfo>
#include <QImage>
#include <QPixmap>
#include <QBitmap>
#include <QImage>
#include <QImageReader>
#include <QPainter>
#include <QSize>
#include <QRgb>
#include <QFileDevice>
#include <QThread>
#include <QMessageBox>
#include <QMouseEvent>
#include <QDebug>
//方便调试时的打印输出
#define cout qDebug() << "[" << __FILE__ << ":" << __LINE__ << "] "

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWidget)
{
    ui->setupUi(this);
    this->setWindowTitle("Laser");
    this->setWindowIcon(QIcon("://image/favicon.ico"));

    ui->label_9->setVisible(false);
    ui->sliderThreshold->setVisible(false);
    ui->thresholdNum->setVisible(false);
    ui->pushButtonSureImage->setVisible(false);


//    美化
    QFile qssFile("://style/myStyle.qss");
    qssFile.open(QFile::ReadOnly);
    QString qss = qssFile.readAll();
    this->setStyleSheet(qss);
    qssFile.close();
////    去边框
//    this->setWindowFlag(Qt::FramelessWindowHint); // | windowFlags()
////    把窗口背景设置为透明
//    this->setAttribute(Qt::WA_TranslucentBackground);

// 如果文件存在就删了
    if(QFileInfo("save_temp.jpg").exists()) QFile("save_temp.jpg").remove();

//    初始化
    filepath = "";
    filename = "";
    filesize = 0;
    newimage = nullptr;
    //设置按钮状态
    ui->pushButtonLoad->setEnabled(true);
    ui->pushButtonGenerate->setEnabled(false);
// 初始化材料的宽度和高度, 默认是100mm
//    设置材料默认大小
    ui->lineEditWidth->setText("100");
    ui->lineEditHeight->setText("100");
    deviceWidth = ui->lineEditWidth->text().toFloat();
    deviceHeight = ui->lineEditHeight->text().toFloat();
//    设置滑块范围与默认值 显示数字同步
    ui->sliderThreshold->setMaximum(255);
    ui->sliderThreshold->setMinimum(0);
    ui->sliderThreshold->setValue(125);
    ui->thresholdNum->setNum(ui->sliderThreshold->value());
    //分配内存, 注意不能指定父对象, 指定了就不能将对象移入myThread中了
    savethread = new SaveThread;
    //创建子线程
    thread = new QThread(this);
    //把自定义的线程加入到子线程中
    savethread->moveToThread(thread);



/*
 * 信号与槽的连接
 */
//    响应图片保存完成信号
    void (SaveThread::*func)(QImage *) = &SaveThread::saveFinished;
    connect(savethread, func,
            [=](QImage * newimage)
            {
                this->newimage = newimage;
                this->update();
//                thread->quit();
//                thread->wait();
            }
    );
//    发送开始处理图片信号
    connect(this, &MainWidget::startDeal, savethread, &SaveThread::dealImage, Qt::QueuedConnection);
}

MainWidget::~MainWidget()
{
//    释放内存
    delete ui;
    delete newimage;
    delete savethread;
    delete fio;
    if(thread->isRunning())
    {
        thread->quit();
        thread->wait();
    }
//    删除临时保存文件
    if(QFileInfo("save_temp.jpg").exists()) QFile("save_temp.jpg").remove();
}

void MainWidget::reboot()
{
    QString program = QApplication::applicationFilePath();
    QStringList arguments = QApplication::arguments();
    QString workingDirectory = QDir::currentPath();
    QProcess::startDetached(program, arguments, workingDirectory);
    QApplication::exit();
}

/*
 * 事件函数
 */

// 绘图事件
void MainWidget::paintEvent(QPaintEvent *e)
{
    if(QFileInfo("save_temp.jpg").exists())
    {
        QPixmap img = QPixmap("save_temp.jpg");

    //        显示图片
        if(!img.isNull())
        {
            //            图片适应label大小
            img = img.scaled(ui->imgshowzoom->width(), ui->imgshowzoom->height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);  // 按比例缩放
            ui->imgshowzoom->setPixmap(img);
    //        ui->imgshowzoom->setScaledContents (true); // 让图片自适应label大小
        }
    }
}


// 实现左键移动窗口
void MainWidget::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        //求坐标差值
        //当前点击坐标 - 窗口左上角坐标
        p = e->globalPos() - this->frameGeometry().topLeft();
    }
}
void MainWidget::mouseMoveEvent(QMouseEvent *e)
{
    //实现鼠标左键拖动
    if(e->buttons() & Qt::LeftButton)
    {
        this->move(e->globalPos() - p);
    }
}




/*
 * 按钮
 */


// 选择图片按钮
void MainWidget::on_pushButtonLoad_clicked()
{
//    打开文件
    filepath = QFileDialog::getOpenFileName(this, "open", "../", "IMG(*.jpg);;ALL(*.*)");
    cout << filepath;
    if(filepath.isNull())
    {
        QMessageBox::warning(this, "警告", "请选择图片");
    }
    else
    {
//        //三次后重启
//        static int i = 0;
//        if(i++ == 3)  this->reboot();
        //    显示文件信息
        if(fio != nullptr) fio = nullptr;
        fio = new QFileInfo(filepath);
        filesize = fio->size()/1024;
        filename = fio->fileName();
        cout << filesize << filename;

        ui->labelFileName->setText(filename);
        ui->labelFileSize->setText(QString("%1  KB").arg(filesize));
        ui->labelImageWH->setText(QString("%1 * %2").arg(QImage(filepath).width()).arg(QImage(filepath).height()));

        // 导入图片之后设置默认成品大小为  图片分辨率/10
        ui->lineEditWidth->setText(QString::number(QImage(filepath).width()/10));
        ui->lineEditHeight->setText(QString::number(QImage(filepath).height()/10));

        ui->pushButtonSureImage->setEnabled(true);
        ui->pushButtonLoad->setEnabled(false);
        ui->pushButtonGenerate->setEnabled(false);
        on_pushButtonSureImage_clicked();
    }

}


//确认按钮
void MainWidget::on_pushButtonSureImage_clicked()
{
    //启动线程, 但没有启动线程函数
    if(thread->isRunning())
    {
        thread->quit();
        thread->wait();
    }
    thread->start();
    int width = ui->imgshowzoom->width();
    int height = ui->imgshowzoom->height();
    this->threshold = ui->sliderThreshold->value();
    // 发送开始处理图片信号
    emit startDeal(filepath, newimage, width, height, threshold);

    ui->pushButtonGenerate->setEnabled(true);
    ui->pushButtonSureImage->setEnabled(false);
    ui->lineEditWidth->setEnabled(true);
    ui->lineEditHeight->setEnabled(true);
}

//保存gcode
void MainWidget::on_pushButtonGenerate_clicked()
{
    //    再次配置材料大小
    deviceWidth = ui->lineEditWidth->text().toFloat();
    deviceHeight = ui->lineEditHeight->text().toFloat();
    QString savepath = QFileDialog::getSaveFileName(this, "save", "../gcode", "TXT(*.gcode *.nc);;ALL(*.*)");
    if(!savepath.isEmpty())
    {
        SaveFile gFile;
        gFile.getDeviceRect(deviceWidth, deviceHeight);
        gFile.saveProgram(savepath, true, true, true);
        ui->lineEditWidth->setEnabled(false);
        ui->lineEditHeight->setEnabled(false);
        ui->pushButtonLoad->setEnabled(true);
        ui->pushButtonGenerate->setEnabled(false);
        QMessageBox::information(this, "保存", "保存完成");
        QFile("save_temp.jpg").remove();
    }
    else QMessageBox::warning(this, "警告", "保存失败");
}

// 关闭按钮
void MainWidget::on_pushButtonExit_clicked()
{
    this->close();
    this->~MainWidget();
}
// 同步滑块与上方的数字
void MainWidget::on_sliderThreshold_sliderMoved(int position)
{
    ui->thresholdNum->setNum(position);
}
void MainWidget::on_sliderThreshold_actionTriggered()
{
    ui->thresholdNum->setNum( ui->sliderThreshold->value() );
}

// 重置所有按钮状态
void MainWidget::on_pushButtonReset_clicked()
{
    this->reboot();
    ui->pushButtonLoad->setEnabled(true);
    ui->pushButtonGenerate->setEnabled(true);
    ui->pushButtonSureImage->setEnabled(true);
    ui->lineEditWidth->setEnabled(true);
    ui->lineEditHeight->setEnabled(true);
    // 如果文件存在就删了
    if(QFileInfo("save_temp.jpg").exists()) QFile("save_temp.jpg").remove();
}
