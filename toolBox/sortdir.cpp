#include "sortdir.h"
#include "ui_sortdir.h"

sortDir::sortDir(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::sortDir)
{
    ui->setupUi(this);

    this->setWindowTitle("Dir sort 批量整理文件名");

    fileDir = "C:/";
    fileEndAdd = "jpg, jpeg, png";
    fileFormat = "pic--yy--mm--dd--hh-";

    ui->fileDirEdit->setText(fileDir);
    ui->endAddEdit->setText(fileEndAdd);
    ui->nameFormatEdit->setText(fileFormat);
}

sortDir::~sortDir()
{
    delete ui;
}

void sortDir::on_fileDir_clicked()
{
    fileDir = QFileDialog::getExistingDirectory(this, tr("choose directory"), fileDir, QFileDialog::ShowDirsOnly|QFileDialog::DontResolveSymlinks);
    ui->fileDirEdit->setText(fileDir);
}

void sortDir::on_startSort_clicked()
{
    fileDir = ui->fileDirEdit->text();
    fileEndAdd = ui->endAddEdit->text();
    fileFormat = ui->nameFormatEdit->text();

    if(ui->choosePic->isChecked())
    {
        fileEndAdd = addEndadd("jpg", fileEndAdd, ", ");
        fileEndAdd = addEndadd("jpeg", fileEndAdd, ", ");
        fileEndAdd = addEndadd("png", fileEndAdd, ", ");
        fileEndAdd = addEndadd("bmp", fileEndAdd, ", ");
        fileEndAdd = addEndadd("gif", fileEndAdd, ", ");
        ui->endAddEdit->setText(fileEndAdd);
    }

    if(ui->chooseTxt->isChecked())
    {
        fileEndAdd = addEndadd("txt", fileEndAdd, ", ");
        ui->endAddEdit->setText(fileEndAdd);
    }

    fileEndAdd.remove(" ", Qt::CaseInsensitive);			//整理去除后缀名中的空格

    QStringList tempEndAdd;												//临时后缀名过滤列表
    tempEndAdd = fileEndAdd.split(",", QString::SkipEmptyParts);		//生成临时后缀名过滤列表

    for(int i=0; i<tempEndAdd.length(); i++)				//添加，整理成后缀名可过滤格式
        tempEndAdd[i].insert(0, "*.");

    QDir dir(fileDir);		//指向文件夹路径

    dir.setNameFilters(tempEndAdd);							//过滤文件格式

    QStringList fileNames = dir.entryList();				//导出临时文件名列表

    QStringList tempfNlist = fNameExplain(fileFormat, fileNames.length());		//调用文件名格式解释器生成文件名列表


    QString tempFileName;									//临时文件名
    QString tempFileEnd;									//临时文件名后缀，用于暂时存储当前文件的后缀

    //拼合产生要修改的文件名列表 并 修改文件名
    for(int i=0; i<fileNames.length(); i++)
    {
        tempFileName = fileNames[i];
        int l = (tempFileName.count(".", Qt::CaseInsensitive));
        int k=0;
        if(l > 1)
        {
            for(;k<tempFileName.length() && tempFileName.indexOf(".", k)>0; k++)
            {}
            k--;
        }
        else
        {
            k = tempFileName.indexOf(".");
        }
        tempFileEnd = tempFileName.right(tempFileName.size() - k);
        dir.rename(fileNames[i], tempfNlist[i] + tempFileEnd);
    }
}


QString sortDir::addEndadd(QString from, QString to)		//将from追加到to并检查是否重复，用于将后缀加入后缀列表
{
    if(to.contains(from, Qt::CaseInsensitive))
        return to;
    to.append(from);
    return to;
}
QString sortDir::addEndadd(QString from, QString to, QString before)
{
    if(to.contains(from, Qt::CaseInsensitive))
        return to;
    to.append(before);
    to.append(from);
    return to;
}

QStringList sortDir::fNameExplain(QString str, int num)				//文件名格式解释器
{
    QStringList tempList;

    str.remove("\\");
    str.remove(QRegExp("[:/*?\"<>|]"));				//清除不可用的文件名符号

    QDateTime dateTime = QDateTime::currentDateTime();

    //解析文件名格式
    if(str.indexOf("-yy")>0)
        str.replace(str.indexOf("-yy"), 3, dateTime.toString("yyyy"));
    if(str.indexOf("-mm")>0)
        str.replace(str.indexOf("-mm"), 3, dateTime.toString("MM"));
    if(str.indexOf("-dd")>0)
        str.replace(str.indexOf("-dd"), 3, dateTime.toString("dd"));
    if(str.indexOf("-hh")>0)
        str.replace(str.indexOf("-hh"), 3, dateTime.toString("hh"));

    QString tempNum;							//临时记录编号

    for(int i=0; i<num; i++)
    {
        tempNum = QString::number(i, 10);
        tempList << str + tempNum;
    }

    return tempList;
}
