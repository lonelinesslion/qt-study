#include "pictureseed.h"
#include "ui_pictureseed.h"

pictureSeed::pictureSeed(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::pictureSeed)
{
    ui->setupUi(this);

    this->setWindowState(Qt::WindowMaximized);
    this->setWindowTitle("Picture Note 图片隐藏注释");
    this->setStyleSheet("background-color: rgba(238, 220, 82, 220)");

    ui->textEdit->setStyleSheet("background-color: rgb(255, 255, 255)");
//    ui->picArea->setAlignment(Qt::AlignVCenter);
    ui->label->setAlignment(Qt::AlignCenter);

    //初始化参数
    isLocked = false;
    lockKey = "<LOCK></LOCK>";
    isEncrypt = false;
    scaleSize = 0;	//放大值为0

    //安装事件过滤器
    ui->picArea->installEventFilter(this);
    //ui->textEdit->installEventFilter(this);

    //添加工具栏
    QToolBar *toolBar = addToolBar("&File");
    toolBar->addAction(ui->openFile);
    toolBar->addAction(ui->saveFile);
    toolBar->addAction(ui->saveAs);
    toolBar->addAction(ui->clearNotes);
    toolBar->addAction(ui->lastPic);
    toolBar->addAction(ui->nextPic);
    toolBar->addAction(ui->scrollBig);
    toolBar->addAction(ui->scrollSmall);
    toolBar->addAction(ui->encryptNotes);
    toolBar->addAction(ui->decryptNotes);
    toolBar->addAction(ui->lockChange);
    toolBar->addAction(ui->unlockChange);
    toolBar->addAction(ui->quitApp);
    toolBar->addAction(ui->showTextEdit);
    toolBar->addAction(ui->hideTextEdit);

    //链接按钮与槽
    connect(ui->openFile, SIGNAL(triggered(bool)), this, SLOT(openFile_slot()));	//打开文件
    connect(ui->saveFile, SIGNAL(triggered(bool)), this, SLOT(saveFile_slot()));	//保存文件
    connect(ui->saveAs, SIGNAL(triggered(bool)), this, SLOT(saveAs_slot()));		//另存为
    connect(ui->clearNotes, SIGNAL(triggered(bool)), this, SLOT(clearNotes_slot()));	//清除注释

    connect(ui->lastPic, SIGNAL(triggered(bool)), this, SLOT(lastFile_slot()));		//上一张
    connect(ui->nextPic, SIGNAL(triggered(bool)), this, SLOT(nextFile_slot()));		//下一张

    connect(ui->encryptNotes, SIGNAL(triggered(bool)), this, SLOT(encryptNotes_slot()));	//加密
    connect(ui->decryptNotes, SIGNAL(triggered(bool)), this, SLOT(decryptNotes_slot()));	//解密
    connect(ui->lockChange, SIGNAL(triggered(bool)), this, SLOT(lockNotes_slot()));			//锁定注释修改权限
    connect(ui->unlockChange, SIGNAL(triggered(bool)), this, SLOT(unlockNotes_slot()));		//解锁注释修改权限

    connect(ui->scrollBig, SIGNAL(triggered(bool)), this, SLOT(enlarge_slot()));	//放大
    connect(ui->scrollSmall, SIGNAL(triggered(bool)), this, SLOT(reduce_slot()));	//缩小

    connect(ui->hideTextEdit, SIGNAL(triggered(bool)), this, SLOT(hideTextEdit_slot()));	//隐藏编辑框
    connect(ui->showTextEdit, SIGNAL(triggered(bool)), this, SLOT(showTextEdit_slot()));	//显示编辑框
}

pictureSeed::~pictureSeed()
{
    delete ui;
}

void pictureSeed::openFile_slot()					//打开图片
{
    lastPicPath = thisFilePath;
    thisFilePath = QFileDialog::getOpenFileName(this, "Choose Image", "x:/image", ("Images (*.*)"));

    if(thisFilePath.isEmpty())
    {
        if(lastPicPath.isEmpty())
            return;
        else
            thisFilePath = lastPicPath;
    }

    //isScaling = false;
    openAndShow(thisFilePath);

    int i=0;
    while (i<thisFilePath.length() && thisFilePath.indexOf("/", i)>0) {
        i++;
    }
    i = thisFilePath.length() - i;

    //获取当前文件夹下的图片列表
    fileDirPath = thisFilePath;
    fileDirPath.chop(i);
    filesList = readIndex(fileDirPath);
    //计算当前图片在列表内的位置
    thisFileNumber = getThisFileNumber(thisFilePath, filesList);
}
void pictureSeed::openAndShow(QString filePath)		//打开并显示注释
{
    hasNotes = false;		//暂时记为无注释
    scaleSize = 0;			//打开一张图片，放大倍数为0
    isScaling = false;
    isEncrypt = false;

    thisPicImage.load(filePath);						//载入图片

    QPixmap pixmap(QPixmap::fromImage(thisPicImage));
    //QPixmap truePixmap = pixmap.scaled(200, 200, Qt::IgnoreAspectRatio, Qt::FastTransformation);
    QPixmap truePixmap;
    //如果pixmap不能完全放入label
    if(pixmap.width() > ui->scrollAreaWidgetContents->width() || pixmap.height() > ui->scrollAreaWidgetContents->height())
    {
        if(ui->scrollAreaWidgetContents->width() < ui->scrollAreaWidgetContents->height())
            truePixmap = pixmap.scaledToWidth(ui->scrollAreaWidgetContents->width() - 50, Qt::FastTransformation);
        else
            truePixmap = pixmap.scaledToHeight(ui->scrollAreaWidgetContents->height() - 50, Qt::FastTransformation);
    }
    else {
        truePixmap = pixmap;
    }

    //打印图片
    ui->label->setPixmap(truePixmap);
    this->setWindowTitle(filePath);

    //显示注释
    ui->textEdit->clear();

    QFile thePicFile(filePath);
    filelongSize = thePicFile.size();

    if(!thePicFile.open(QIODevice::ReadOnly))
        return;

    thePicFile.seek(filelongSize - 1);
    QByteArray sizeLong = thePicFile.read(1);		//读取文件真实长度记录值字符串的长度

    bool ok = true;
    int fileRealLong = sizeLong.toInt(&ok, 10);

    fileRealLong += 7;								//最后一个字节 + </CLS> = 7
    thePicFile.seek(filelongSize - fileRealLong);
    QByteArray signAndLong = thePicFile.read(fileRealLong-1);
    if(!signAndLong.contains("</CLS>"))
    {
        isLocked = false;
        lockKey = "<LOCK></LOCK>";
        isEncrypt = false;
        hasNotes = false;
        thePicFile.close();
        return;
    }

    hasNotes = true;

    signAndLong.remove(0, 6);
    filelongSize = signAndLong.toInt(&ok, 10);		//获取文件真实长度
    thePicFile.seek(filelongSize);					//跳到图片结束位置

    QTextStream out(&thePicFile);
    QString text = out.readAll();
    thePicFile.close();

    text.remove(0, 5);
    text.chop(fileRealLong);						//解析出注释数据

    if(!text.startsWith("<LOCK>"))
    {
        isLocked = false;
        isEncrypt = false;
        lockKey = "<LOCK></LOCK>";
        ui->textEdit->setText(text);
        return;
    }
    fileRealLong = text.indexOf("</LOCK>");			//暂用fileRealLong来检查锁定标志的密码
    if(fileRealLong != 6)
    {
        isLocked = true;
        lockKey = text.left(fileRealLong+7);
    }
    else {
        lockKey = "<LOCK></LOCK>";
        isLocked = false;
    }
    text.remove(0, fileRealLong+7);

    if(text.contains("<CRY>") && text.contains("</CRY>"))
    {
        text.remove(0, 5);
        text.chop(6);
        tmpText = text;
        ui->textEdit->setText(tr("该图片注释已加密，请解密"));
        isEncrypt = true;
        return;
    }

    ui->textEdit->setText(text);
    isEncrypt = false;
}

void pictureSeed::saveFile_slot()					//保存注释
{
    if(thisFilePath.isEmpty())
        return;

    QFile tempFile(thisFilePath);
    if(!tempFile.exists())
    {
        warningMsg(tr("错误"), tr("文件不存在"));
        return;
    }

    //如果没有注释则停止执行
    QString textData = ui->textEdit->toPlainText();
    if(textData.isEmpty())
        return;

    //如果不具有修改权限则停止执行
    if(isLocked)
    {
        warningMsg(tr("错误"), tr("您不具备修改权限，\n原文件已锁定，如需修改，请先解锁"));
        return;
    }

    tempFile.resize(filelongSize);					//删除原有注释

    if(!tempFile.open(QIODevice::Append))
    {
        warningMsg(tr("错误"), tr("文件打开失败"));
        return;
    }

    QTextStream in(&tempFile);
    in << "<CLS>";
    in << lockKey;

    //检查是否加密
    if(isEncrypt)
    {
        in << "<CRY>";
        in << tmpText;
        in << "</CRY>";
    }
    else {
        tmpText = textData;
        in << tmpText;
    }
    in << "</CLS>";

    QString tmpNum = QString::number(filelongSize, 10);
    in << tmpNum;
    in << tmpNum.size();

    tempFile.close();
}

void pictureSeed::saveAs_slot()						//另存为
{
    if(thisFilePath.isEmpty())
        return;

    QFile tempFile(thisFilePath);
    if(!tempFile.exists())
    {
        warningMsg(tr("错误"), tr("文件不存在"));
        return;
    }

    //如果没有注释则停止执行
    QString textData = ui->textEdit->toPlainText();
    if(textData.isEmpty())
        return;

    //如果不具有修改权限则停止执行
    if(isLocked)
    {
        warningMsg(tr("错误"), tr("您不具备修改权限，\n原文件已锁定，如需修改，请先解锁"));
        return;
    }

    QString SecFileName = QFileDialog::getSaveFileName(this, tr("另存为"), "x:/image", "Images (*.jpg *.png *.bmp)");
    if(SecFileName.isEmpty())
        return;

    tempFile.copy(SecFileName);

    //更新列表
    filesList = readIndex(fileDirPath);
    //计算当前图片在列表内的位置
    thisFileNumber = getThisFileNumber(thisFilePath, filesList);

    QFile newFile(SecFileName);
    //添加注释
    newFile.resize(filelongSize);					//删除原有注释

    if(!newFile.open(QIODevice::Append))
    {
        warningMsg(tr("错误"), tr("文件打开失败"));
        return;
    }

    QTextStream in(&newFile);
    in << "<CLS>";
    in << lockKey;

    //检查是否加密
    if(isEncrypt)
    {
        in << "<CRY>";
        in << tmpText;
        in << "</CRY>";
    }
    else {
        tmpText = textData;
        in << tmpText;
    }
    in << "</CLS>";

    QString tmpNum = QString::number(filelongSize, 10);
    in << tmpNum;
    in << tmpNum.size();

    newFile.close();
}
void pictureSeed::clearNotes_slot()					//清除注释
{
    if(thisFilePath.isEmpty())
        return;

    QFile tempFile(thisFilePath);
    if(!tempFile.exists())
    {
        warningMsg(tr("错误"), tr("文件不存在"));
        return;
    }

    //如果不具有修改权限则停止执行
    if(isLocked)
    {
        warningMsg(tr("错误"), tr("您不具备修改权限，\n原文件已锁定，如需修改，请先解锁"));
        return;
    }

    tempFile.resize(filelongSize);					//删除原有注释
    ui->textEdit->clear();
    warningMsg(tr("完成"), tr("注释清除完成"));
}
QStringList pictureSeed::readIndex(QString dirPath)		//获取指定文件夹内图片列表
{
    QStringList tempList;		//临时字符串列表，先用于过滤图片，后用于返回文件列表
    tempList << "*.jpg" << "*.jpeg" << "*.png" << "*.bmp" << "*.gif";

    QDir filesDir(dirPath);
    filesDir.setNameFilters(tempList);

    return filesDir.entryList();
}
int pictureSeed::getThisFileNumber(QString filePath, QStringList dirList)	//获取当前图片在列表内的位置
{
    int i=0;
    while (i<filePath.length() && filePath.indexOf("/", i)>0) {
        i++;
    }
    i = filePath.length() - i;

    QString tempStr = filePath.right(i);
    int p = 0;
    while (p < dirList.length() && tempStr != dirList[p]) {
        p++;
    }
    if(p == dirList.length())
        return 0;
    return p;
}

void pictureSeed::lastFile_slot()					//上一张
{
    if(thisFilePath.isEmpty())
        return;

    //isScaling = false;
    QString tempFilePath = thisFilePath;

    thisFileNumber--;
    if(thisFileNumber == -1)
        thisFileNumber = filesList.length()-1;
    thisFilePath = fileDirPath + filesList[thisFileNumber];

    QFile checkExist(thisFilePath);
    if(!checkExist.exists())
    {
        filesList = readIndex(fileDirPath);
        thisFileNumber = getThisFileNumber(tempFilePath, filesList);
        lastFile_slot();
    }
    openAndShow(thisFilePath);
}
void pictureSeed::nextFile_slot()					//下一张
{
    if(thisFilePath.isEmpty())
        return;

    QString tempFilePath = thisFilePath;

    thisFileNumber++;
    if(thisFileNumber == filesList.length())
        thisFileNumber = 0;
    thisFilePath = fileDirPath + filesList[thisFileNumber];

    QFile checkExist(thisFilePath);
    if(!checkExist.exists())
    {
        filesList = readIndex(fileDirPath);
        thisFileNumber = getThisFileNumber(tempFilePath, filesList);
        nextFile_slot();
    }
    openAndShow(thisFilePath);

}

void pictureSeed::encryptNotes_slot()				//加密
{
    if(thisFilePath.isEmpty())
        return;

    if(isEncrypt)
    {
        warningMsg("warning", tr("原文件已加密，请先解密"));
        return;
    }

    //获取密码
    QString tmpKey = modelDialog(tr("加密"), tr("请输入纯数字密码\n中文结尾的注释可能因为在加密时丢失最后一个字符"));
    bool ok = true;
    qint64 intKey = tmpKey.toInt(&ok, 10);

    cryption tmpCrypt;
    //消除中文误差
    tmpKey = ui->textEdit->toPlainText() + "人";				//用于消除中文误差
    tmpText = tmpCrypt.encryption(tmpKey, intKey);
    //ui->textEdit->setText(tmpKey);

    isEncrypt = true;

    warningMsg(tr("注意"), tr("您需要手动保存"));
}
void pictureSeed::decryptNotes_slot()				//解密
{
    if(thisFilePath.isEmpty())
        return;

    if(tmpText.isEmpty())
        return;

    //如本来没有加密则不必解密
    if(!isEncrypt)
        return;

    QString tmpKey = modelDialog(tr("解密"), tr("请输入密码"));
    bool ok = true;
    qint64 intKey = tmpKey.toInt(&ok, 10);

    cryption tmpCrypt;
    QString tmpText_2;
    tmpText_2 = tmpCrypt.decryption(tmpText, intKey);
    tmpText_2.chop(1);

    ui->textEdit->setText(tmpText_2);

    isEncrypt = false;

    warningMsg(tr("注意"), tr("您需要手动保存"));
}
void pictureSeed::lockNotes_slot()					//锁定修改权限
{
    if(thisFilePath.isEmpty())
        return;

    //如果已有密码，需清除原有密码
    if(isLocked)
    {
        warningMsg(tr("注意"), tr("原文件已锁定修改权限，\n如需重新锁定请先解锁"));
        return;
    }

    QString tmpKey = modelDialog(tr("锁定修改权限"), tr("请输入数字或字符串作为修改权限密码"));

    if(tmpKey.isEmpty())
    {
        warningMsg(tr("错误"), tr("锁定失败，您输入的密码为空"));
        return;
    }

    lockKey = "<LOCK></LOCK>";
    lockKey.insert(6, tmpKey);

    warningMsg(tr("注意"), tr("您需要手动保存"));
}
void pictureSeed::unlockNotes_slot()				//解锁修改权限
{
    if(thisFilePath.isEmpty())
        return;

    //排除错按
    if(!isLocked)
        return;

    QString tmpKey = modelDialog(tr("解锁修改权限"), tr("请输入权限修改密码"));
    tmpKey.insert(0, "<LOCK>");
    tmpKey.append("</LOCK>");

    if(tmpKey != lockKey)
    {
        warningMsg(tr("错误"), tr("您输入的密码错误，无法解锁权限修改"));
        return;
    }

    lockKey = "<LOCK></LOCK>";
    isLocked = false;
}

void pictureSeed::enlarge_slot()					//放大
{
    if(thisFilePath.isEmpty())
        return;

    isScaling = true;
    getPicEuclid(thisPicImage);

    QPixmap pixmap(QPixmap::fromImage(thisPicImage));
    QPixmap truePixmap;

    int tempW = pixmap.width() + PicEuclidW*scaleSize;
    int tempH = pixmap.height() + PicEuclidH*scaleSize;

    truePixmap = pixmap.scaled(tempW, tempH, Qt::IgnoreAspectRatio, Qt::FastTransformation);

    //打印图片
    ui->label->setPixmap(truePixmap);

    scaleSize++;
}
void pictureSeed::reduce_slot()						//缩小
{
    if(thisFilePath.isEmpty())
        return;

    isScaling = true;
    getPicEuclid(thisPicImage);

    QPixmap pixmap(QPixmap::fromImage(thisPicImage));
    QPixmap truePixmap;

    int tempW = pixmap.width() + PicEuclidW*scaleSize;
    int tempH = pixmap.height() + PicEuclidH*scaleSize;

    if(tempW < 0 || tempH < 0)
        return;

    truePixmap = pixmap.scaled(tempW, tempH, Qt::IgnoreAspectRatio, Qt::FastTransformation);

    //打印图片
    ui->label->setPixmap(truePixmap);

    scaleSize--;
}
void pictureSeed::getPicEuclid(QImage picImage)		//通过辗转相除法计算最大公约数从而计算最简比例
{
        //约分，求最简分数
        int a, b, x;
        a = picImage.width();
        b = picImage.height();

        //求最大公约数
        while (b != 0) {
            x = a % b;
            a = b;
            b = x;
        }
        //a 即为最大公约数
        x = a;

        //计算最简分数
        PicEuclidW = picImage.width() / x;
        PicEuclidH = picImage.height() / x;
        //最简分数 a/b
}

void pictureSeed::hideTextEdit_slot()				//隐藏输入框
{
    ui->textEdit->hide();
}
void pictureSeed::showTextEdit_slot()				//显示输入框
{
    ui->textEdit->show();
}

bool pictureSeed::eventFilter(QObject *watched, QEvent *event)		//事件过滤器
{
    if(event->type() == QEvent::KeyRelease)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

        if(watched == ui->picArea)
        {
            switch (keyEvent->key()) {
            case Qt::Key_Left:
            case Qt::Key_A:
            case Qt::Key_H:
                lastFile_slot();
                break;
            case Qt::Key_Right:
            case Qt::Key_D:
            case Qt::Key_L:
                nextFile_slot();
                break;
            case Qt::Key_Up:
            case Qt::Key_K:
            case Qt::Key_W:
                enlarge_slot();
                break;
            case Qt::Key_Down:
            case Qt::Key_J:
            case Qt::Key_S:
                reduce_slot();
                break;
            case Qt::Key_Escape:
                this->close();
                break;
            case Qt::Key_F5:
                scaleSize = 0;
                openAndShow(thisFilePath);
                break;
            default:
                break;
            }
        }
    }

    if(event->type() == QEvent::Wheel)
    {
        if(watched == ui->picArea && !isScaling)
        {
            QWheelEvent *tmpWheelEvent = static_cast<QWheelEvent *>(event);
            if(tmpWheelEvent->delta()>0)
                lastFile_slot();
            else
                nextFile_slot();
        }
    }
    return true;
}

QString pictureSeed::modelDialog(QString m_title, QString m_label)		//用于输入的模式对话框
{
    //弹出输入框
    QDialog *tmpDialog = new QDialog(this);
    tmpDialog->setModal(true);								//设为模式对话框
    tmpDialog->setWindowTitle(m_title);

    QVBoxLayout *tmpVBlayout = new QVBoxLayout(tmpDialog);

    QLabel *tmpLabel = new QLabel(tmpDialog);
    tmpLabel->setText(m_label);
    QLineEdit *tmpLineEdit = new QLineEdit(tmpDialog);
    QPushButton *tmpPBtn = new QPushButton(tr("确定"), tmpDialog);

    tmpVBlayout->addWidget(tmpLabel);
    tmpVBlayout->addWidget(tmpLineEdit);
    tmpVBlayout->addWidget(tmpPBtn);

    connect(tmpPBtn, SIGNAL(clicked(bool)), tmpDialog, SLOT(close()));
    tmpDialog->exec();

    QString tmpStr = tmpLineEdit->text();
    return tmpStr;
}
void pictureSeed::warningMsg(QString m_title, QString m_text)			//用于提示信息的模式对话框
{
    QMessageBox *MsgBox = new QMessageBox(this);
    MsgBox->setWindowTitle(m_title);
    MsgBox->setModal(true);
    MsgBox->setText(m_text);
    MsgBox->exec();
}
