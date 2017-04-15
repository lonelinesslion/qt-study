#ifndef PICTURESEED_H
#define PICTURESEED_H

#include <QMainWindow>
#include <QToolBar>
#include <QByteArray>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QImage>
#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QPaintEvent>
#include <QThread>

#include "cryption.h"								//使用异或加密解密

namespace Ui {
class pictureSeed;
}

class pictureSeed : public QMainWindow
{
    Q_OBJECT

public:
    explicit pictureSeed(QWidget *parent = 0);
    ~pictureSeed();

    bool eventFilter(QObject *watched, QEvent *event);		//事件过滤器

    QString modelDialog(QString m_title, QString m_label);	//用于输入的模式对话框
    void warningMsg(QString m_title, QString m_text);		//用于提醒的模式对话框

protected:
    QString fileDirPath;									//存储文件夹路径
    QString thisFilePath;							//当前图片路径
    QString lastPicPath;							//暂存原有图片路径
    QImage thisPicImage;							//当前图片的image
    qint64 filelongSize;							//当前图片的真实长度
    QStringList filesList;							//当前文件夹内图片列表

    bool isEnlarge;									//正在放大
    int scaleSize;									//放大值
    int PicEuclidW, PicEuclidH;						//当前图片的image最简比例 用于放大缩小图片
    void getPicEuclid(QImage picImage);				//用于计算图片的image的最简比例

    QStringList readIndex(QString dirPath);				//获取指定文件夹内图片列表
    int thisFileNumber;								//当前图片在列表内的位置
    int getThisFileNumber(QString filePath, QStringList dirList);		//计算当前图片在列表内的位置

private:
    Ui::pictureSeed *ui;

    bool isLocked;										//是否锁住注释修改
    QString lockKey;									//锁定注释修改权限的密码
    QString tmpText;									//用于转存注释
    bool isEncrypt;										//是否加密
    bool hasNotes;										//是否有注释
    bool isScaling;									//是否正在进行缩放

    void openAndShow(QString filePath);					//打开图片并显示注释

public slots:
    void openFile_slot();								//打开图片
    void saveFile_slot();								//保存注释
    void saveAs_slot();									//另存为
    void clearNotes_slot();								//清除注释

    void lastFile_slot();								//上一张
    void nextFile_slot();								//下一张

    void encryptNotes_slot();							//加密
    void decryptNotes_slot();							//解密

    void lockNotes_slot();								//锁定修改权限
    void unlockNotes_slot();							//解锁修改权限

    void enlarge_slot();								//放大
    void reduce_slot();									//缩小

    void hideTextEdit_slot();							//隐藏输入框
    void showTextEdit_slot();							//显示输入框
};

#endif // PICTURESEED_H
