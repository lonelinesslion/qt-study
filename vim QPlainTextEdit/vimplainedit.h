#ifndef VIMPLAINEDIT_H
#define VIMPLAINEDIT_H

#include <QPlainTextEdit>
#include <QWidget>
#include <QPalette>
#include <QTextCursor>
#include <QTextCharFormat>
#include <QEvent>

class vimPlainEdit : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit vimPlainEdit(QWidget *parent=0);

    uint nowPos;					//当前光标位置
    void escPattern();				//转为Esc模式
    void insertPattern(uint num, bool isBehind);	//转为写入模式，num为上一个需要清除的旧标记的长度,默认使用1
                                                    //isBehind为在前面插入还是在后面插入
    void moveCursorLeft();			//光标左移
    void moveCursorRight();			//光标右移
    void moveCursorUp();			//光标上移
    void moveCursorDown();			//光标下移

    void rowTo(int pos);			//列内移动	PS:这个基本是废的，因为需要计算行号什么的，会导致很大的麻烦
    void lineTo(int pos);			//行移动

    //可设置参数，放大缩小
    void setFontSize(int size);

    void setProject(int Pro);	//设置配色方案，共四种。0：黑白；1：黑绿；2：白黑；3：黄黑

    bool eventFilter(QObject *watch, QEvent *event);		//事件过滤器
    bool blow;		//放大缩小开关，为true时可以使用鼠标滚轮放大缩小字体

    void setAutoWrap();				//设为自动换行模式

    //Esc模式时光标的实际位置在高亮文本右边
    //使用QTextCursor选择文本的时候，选择方式为移动到字符串左边然后再移动到右边

signals:
    void getText();		//键盘按下的非快捷键而是其他，产生通知信号

private:
    QPalette backPen;			//设置背景色
    //先用QTextCursor选择特定字，然后用QTextCharFormat设置特定字体格式。分别使用setTextCursor(QTextCursor )函数
    //和setCurrentCharFormat(QTextCharFormat )函数
    QTextCursor theCursor;
    QTextCharFormat defCharFormat, newCharFormat;

    bool pattern;	//Esc与Insert模式标志

    void setChar(uint pos, uint num);	//修改指定位置背景色及前景色，修改个数，修改为非高亮，用于恢复指定位置的高亮字符
    int theTabWidth;	//Tab宽

    uint linesNumber;	//总行数
    uint nowLine;		//当前行位置
    uint rowsNumber;	//当前列总列数
    uint rowPos;		//当前光标所在列位置

    bool canLeft;		//可左移
    bool canRight;		//可右移
    bool canUp;			//可上移
    bool canDown;		//可下移

    void chackLines();		//检查文本总行数
    void chackBlocks();		//以block方式，即分段符检查文本总行数
    void chackRows();					//检查当前行总列数
    uint getLinePos(uint tempPos);		//获取指定位置的行号
    void whetherCanUpDown();			//检查是否可以上下移动
};

#endif // VIMPLAINEDIT_H
