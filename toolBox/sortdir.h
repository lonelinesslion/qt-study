#ifndef SORTDIR_H
#define SORTDIR_H

#include <QWidget>
#include <QFile>
#include <QFileDialog>
#include <QDateTime>
#include <QDate>
#include <QTime>

namespace Ui {
class sortDir;
}

class sortDir : public QWidget
{
    Q_OBJECT

public:
    explicit sortDir(QWidget *parent = 0);
    ~sortDir();

    QString fileDir;							//记录文件夹路径
    QString fileEndAdd;							//记录文件后缀
    QString fileFormat;							//记录文件名格式

    QStringList fNameExplain(QString str, int num);		//文件名格式解释器。参数：文件格式，文件个数

private slots:
    void on_fileDir_clicked();

    void on_startSort_clicked();

private:
    Ui::sortDir *ui;

    QString addEndadd(QString from, QString to);		//将from追加到to并检查是否重复，用于将后缀加入后缀列表
    QString addEndadd(QString from, QString to, QString before);
};

#endif // SORTDIR_H
