#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QBoxLayout>
#include <QScrollArea>
#include <QPushButton>
#include <QMouseEvent>
#include <QLabel>

#include "sortdir.h"
#include "pictureseed.h"
#include "picturenote.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

    QScrollArea *area;
    QGridLayout *layout;
    QVBoxLayout *BtnLayout;

    void setBtn(QPushButton *tempBtn);

private:
    Ui::Widget *ui;

    QPushButton *pBt0;

public slots:
    void sortdir_slot();					//整理文件名弹窗
    void pictrueSeed_slot();				//图片注释弹窗
    void pictureNote_slot();				//图片批量添加水印
};

#endif // WIDGET_H
