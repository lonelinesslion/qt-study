#include "picturenote.h"
#include "ui_picturenote.h"

pictureNote::pictureNote(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::pictureNote)
{
    ui->setupUi(this);

    QToolBar *toolBar = addToolBar("&File");

    toolBar->addAction(ui->action_open);
    toolBar->addAction(ui->action_Insert);
    toolBar->addAction(ui->action_opens);
    toolBar->addAction(ui->action_Save);
    toolBar->addAction(ui->action_Close);
    toolBar->addAction(ui->action_Help);
    toolBar->addAction(ui->action_Quit);

    ui->comboBox->addItem("左上");
    ui->comboBox->addItem("左下");
    ui->comboBox->addItem("右上");
    ui->comboBox->addItem("右下");
    connect(ui->comboBox, SIGNAL(activated(int)), this, SLOT(waterPosChanged(int)));

    //连接action与槽
    connect(ui->action_open, SIGNAL(triggered(bool)), this, SLOT(openPic_eg()));
    connect(ui->action_Insert, SIGNAL(triggered(bool)), this, SLOT(openPic_watermark()));
    connect(ui->action_Close, SIGNAL(triggered(bool)), this, SLOT(closePic_slot()));
    connect(ui->action_Help, SIGNAL(triggered(bool)), this, SLOT(help_slot()));
    connect(ui->action_opens, SIGNAL(triggered(bool)), this, SLOT(openPics()));
    connect(ui->action_Save, SIGNAL(triggered(bool)), this, SLOT(savePics()));

    //初始化
    waterPos = 0;
    waterX = 10;
    waterY = 10;
    hasPic = false;
    hasWatermark = false;
    isWorking = false;

    ui->thePic->setAlignment(Qt::AlignCenter);
    ui->theWater->setAlignment(Qt::AlignCenter);

    ui->spinX->setMinimum(0);
    ui->spinX->setMaximum(50);
    ui->spinY->setMinimum(0);
    ui->spinY->setMaximum(50);
}

pictureNote::~pictureNote()
{
    delete ui;
}

void pictureNote::paintEvent(QPaintEvent *event)
{
    if(!hasPic || !hasWatermark)
        return;

    if(isWorking)
        return;

    tmpPix = picturePixmap;
    QPainter tmpPainter(&tmpPix);
    switch (waterPos) {
    case LEFT_TOP:
        waterX = ui->spinX->value();
        waterY = ui->spinY->value();
        break;
    case LEFT_BOTTOM:
        waterX = ui->spinX->value();
        waterY = tmpPix.height() - ui->spinY->value() - watermarkPixmap.height();
        break;
    case RIGHT_TOP:
        waterX = tmpPix.width() - ui->spinX->value() - watermarkPixmap.width();
        waterY = ui->spinY->value();
        break;
    default:
        waterX = tmpPix.width() - ui->spinX->value() - watermarkPixmap.width();
        waterY = tmpPix.height() - ui->spinY->value() - watermarkPixmap.height();
        break;
    }
    tmpPainter.drawPixmap(waterX, waterY, watermarkPixmap);
    ui->thePic->setPixmap(tmpPix);
}

void pictureNote::addWatermark(QString picPath)
{
    QPixmap reTmpPix;
    reTmpPix.load(picPath);
    tmpPix = reTmpPix;
    QPainter painter(&tmpPix);

    switch (waterPos) {
    case LEFT_TOP:
        waterX = ui->spinX->value();
        waterY = ui->spinY->value();
        break;
    case LEFT_BOTTOM:
        waterX = ui->spinX->value();
        waterY = tmpPix.height() - ui->spinY->value() - watermarkPixmap.height();
        break;
    case RIGHT_TOP:
        waterX = tmpPix.width() - ui->spinX->value() - watermarkPixmap.width();
        waterY = ui->spinY->value();
        break;
    default:
        waterX = tmpPix.width() - ui->spinX->value() - watermarkPixmap.width();
        waterY = tmpPix.height() - ui->spinY->value() - watermarkPixmap.height();
        break;
    }
    painter.drawPixmap(waterX, waterY, watermarkPixmap);
    ui->thePic->setPixmap(tmpPix);
}

bool pictureNote::saveApicWatermark(QString t_Path)	//用于保存指定图片的水印
{
    QFile file(t_Path);
    if(!file.exists())
        return false;

    if(!file.open(QIODevice::WriteOnly))
        return false;

    QByteArray bytes;
    QBuffer buffer(&bytes);
    buffer.open(QIODevice::WriteOnly);
    tmpPix.save(&buffer, "jpg");

    file.write(bytes);
    file.close();

    return true;
}

void pictureNote::loadAndShow(QString str)				//加载并显示图片
{
    this->setWindowTitle(str);
    picturePixmap.load(str);
    tmpPix = picturePixmap;
    ui->thePic->setPixmap(picturePixmap);
}

//槽函数
void pictureNote::waterPosChanged(int tmpPos)	//更新修改水印标注方位的槽
{
    waterPos = tmpPos;							//获取水印标注方位
}
void pictureNote::openPic_eg()					//打开图片的槽
{
    QString fileDir = "x:/";
    filePath = QFileDialog::getOpenFileName(this, tr("打开示例图片"), fileDir, tr("image (*.jpg *.jpeg *.png *.bmp)"));

    loadAndShow(filePath);

    hasPic = true;
}
void pictureNote::openPic_watermark()			//打开水印的槽
{
    QString fileDir = "x:/";
    watermark = QFileDialog::getOpenFileName(this, tr("打开水印"), fileDir, tr("image (*.jpg *.jpeg *.png *.bmp)"));

    watermarkPixmap.load(watermark);
    ui->theWater->setPixmap(watermarkPixmap);

    hasWatermark = true;
}

void pictureNote::closePic_slot()				//关闭
{
    ui->thePic->clear();
    ui->theWater->clear();
    hasPic = false;
    hasWatermark = false;
}

void pictureNote::help_slot()					//帮助对话框
{
    QString help = "这个功能可以批量添加水印，\n具体操作为：\n打开一张图片，打开要作为水印的图片，选择想要批量修改的图片，\n修改水印位置，执行\n";
    help += "\n因为虽然加入另存为的功能并不是很麻烦，\n但是使用起来还是比较复杂，所以追终还是没添加\n如果不想在原有图片上添加水印，请记得备份\n";
    help += "\n\n是否将水印的添加范围提高到2000？\n";

    QMessageBox *helpMsgB = new QMessageBox(this);
    helpMsgB->setModal(true);
    helpMsgB->setWindowTitle(tr("关于"));
    helpMsgB->setText(help);

    QPushButton *cancelPbt;
    QPushButton *enterPbt;

    cancelPbt = helpMsgB->addButton(tr("取消"), QMessageBox::NoRole);
    enterPbt = helpMsgB->addButton(tr("确定"), QMessageBox::YesRole);

    connect(cancelPbt, SIGNAL(clicked(bool)), helpMsgB, SLOT(close()));
    connect(enterPbt, SIGNAL(clicked(bool)), this, SLOT(changeWaterXY()));

    helpMsgB->exec();
}

void pictureNote::openPics()					//打开指定的图片s
{
    //如无示例图片和水印则不进入执行
    if(!hasPic || !hasWatermark)
        return;

    QString tmpPath = "x:/";
    filesName = QFileDialog::getOpenFileNames(this, tr("指定要添加水印的图片"), tmpPath, tr("images (*.jpg *.jpeg *.png *.bmp)"));
}

void pictureNote::savePics()					//写入保存水印
{
    //如无示例图片和水印则不进入执行
    if(!hasPic || !hasWatermark)
        return;

    //如果没有指定要添加水印的图片，则不予执行
    if(filesName.isEmpty())
        return;

    bool OKorFAIL = true;
    int i;

    isWorking = true;
    for(i=0; i<filesName.length(); i++)
    {
        addWatermark(filesName[i]);
        OKorFAIL = saveApicWatermark(filesName[i]);
        if(!OKorFAIL)
            break;
    }
    isWorking = false;

    filePath = filesName[--i];
    loadAndShow(filePath);
    update();

    QMessageBox *MsgBox = new QMessageBox(this);
    MsgBox->setModal(true);
    MsgBox->setWindowTitle(tr("注意"));
    if(OKorFAIL)
        MsgBox->setText(tr("添加成功"));
    else {
        MsgBox->setText(tr("添加失败"));
    }
    MsgBox->exec();
}

void pictureNote::changeWaterXY()		//修改水印添加范围
{
    int xyRange = 2000;
    ui->spinX->setMaximum(xyRange);
    ui->spinY->setMaximum(xyRange);
}
