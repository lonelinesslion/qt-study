#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    this->setWindowTitle("ToolBox 工具箱");
    this->setStyleSheet("QWidget#Widget{background-color: rgba(77, 77, 77, 255)}");

    layout = new QGridLayout(this);

    area = new QScrollArea(this);
    area->setWidgetResizable(true);

    layout->addWidget(area);

    QWidget *areacontain = new QWidget(area);
    area->setWidget(areacontain);
    BtnLayout = new QVBoxLayout(area);
    areacontain->setLayout(BtnLayout);

    //设置按钮
    QPushButton *Btn1 = new QPushButton(areacontain);
    QPushButton *Btn2 = new QPushButton(areacontain);
    QPushButton *Btn3 = new QPushButton(areacontain);
    QPushButton *Btn4 = new QPushButton(areacontain);
    QPushButton *Btn5 = new QPushButton(areacontain);
    QPushButton *Btn6 = new QPushButton(areacontain);
    QPushButton *Btn7 = new QPushButton(areacontain);
    QPushButton *Btn8 = new QPushButton(areacontain);
    QPushButton *Btn9 = new QPushButton(areacontain);
    QPushButton *Btn10 = new QPushButton(areacontain);

    setBtn(Btn1);
    setBtn(Btn2);
    setBtn(Btn3);
    setBtn(Btn4);
    setBtn(Btn5);
    setBtn(Btn6);
    setBtn(Btn7);
    setBtn(Btn8);
    setBtn(Btn9);
    setBtn(Btn10);

    //设置按钮文字
    Btn1->setText("批量整理文件名");
    Btn2->setText("图片隐藏注释");
    Btn3->setText("图片批量添加水印");

    //连接按钮与槽
    connect(Btn1, SIGNAL(clicked(bool)), this, SLOT(sortdir_slot()));			//批量整理文件名
    connect(Btn2, SIGNAL(clicked(bool)), this, SLOT(pictrueSeed_slot()));		//图片隐藏注释
    connect(Btn3, SIGNAL(clicked(bool)), this, SLOT(pictureNote_slot()));		//图片批量添加水印
}

Widget::~Widget()
{
    delete ui;
}

void Widget::setBtn(QPushButton *tempBtn)
{
    tempBtn->setStyleSheet("background-color: rgb(79, 148, 205)");
    tempBtn->setMinimumHeight(40);
    BtnLayout->addWidget(tempBtn);
    QFont btnFont;
    btnFont.setPixelSize(18);
    tempBtn->setFont(btnFont);
}

//弹窗槽
void Widget::sortdir_slot()		//整理文件名
{
    sortDir *N_sortDir = new sortDir;
    N_sortDir->show();
}

void Widget::pictrueSeed_slot()			//图片注释弹窗
{
    pictureSeed *N_pictureSeed = new pictureSeed;
    N_pictureSeed->show();
}

void Widget::pictureNote_slot()			//图片批量添加水印
{
    pictureNote *N_pictureNote = new pictureNote;
    N_pictureNote->show();
}
