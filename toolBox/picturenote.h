#ifndef PICTURENOTE_H
#define PICTURENOTE_H

#include <QMainWindow>
#include <QToolBar>
#include <QImage>
#include <QPixmapCache>
#include <QPainter>
#include <QFile>
#include <QDataStream>
#include <QBuffer>
#include <QByteArray>
#include <QFileDialog>
#include <QMessageBox>
#include <QPushButton>

//宏定义几个位置量
#define LEFT_TOP 0
#define LEFT_BOTTOM 1
#define RIGHT_TOP 2
#define RIGHT_BOTTOM 3

namespace Ui {
class pictureNote;
}

class pictureNote : public QMainWindow
{
    Q_OBJECT

public:
    explicit pictureNote(QWidget *parent = 0);
    ~pictureNote();

    void paintEvent(QPaintEvent *event);				//重绘事件
    bool hasPic;
    bool hasWatermark;
    bool isWorking;

    void loadAndShow(QString str);						//加载并显示图片
    void addWatermark(QString picPath);					//给指定图片添加水印

private:
    Ui::pictureNote *ui;

    QString filePath;									//图片路径
    QString watermark;									//水印路径
    QPixmap picturePixmap;								//图片pixmap
    QPixmap watermarkPixmap;							//水印pixmap
    QPixmap tmpPix;										//用于双缓冲绘图的pix

    QStringList filesName;								//图片列表

    int waterPos;										//水印的位置，分别是左上、左下、右上、右下
    int waterX;											//水印离边缘的水平距离	0<waterX<30
    int waterY;											//水印离边缘的垂直距离	0<waterY<30

protected:
    bool saveApicWatermark(QString t_Path);			//用于保存某张图片

public slots:
    void waterPosChanged(int tmpPos);					//响应combobox的修改信号
    void openPic_eg();									//打开文件的槽
    void openPic_watermark();							//打开水印的槽
    void closePic_slot();								//关闭
    void help_slot();									//帮助对话框

    void openPics();									//打开指定的图片s

    void savePics();									//写入保存水印

    void changeWaterXY();				//修改水印添加位置
};

#endif // PICTURENOTE_H
