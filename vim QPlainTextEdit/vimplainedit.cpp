#include "vimplainedit.h"
//#include <QPalette>
#include <QDebug>

vimPlainEdit::vimPlainEdit(QWidget *parent): QPlainTextEdit(parent)
{
    setPlainText("pliafewff\naflejwfkhvkn\n");			//测试用

    this->installEventFilter(this);						//安装事件过滤器

    theCursor = this->textCursor();
    backPen = this->palette();
    newCharFormat = defCharFormat = this->currentCharFormat();		//准备配色

    setProject(2);		//默认配色
    nowPos = 0;
    canLeft = canRight = canUp = canDown = false;
    nowLine = 0;
    this->setLineWrapMode(NoWrap);					//默认设为无自动换行
    this->setCurrentCharFormat(defCharFormat);
    theTabWidth = 24;
    this->setTabStopWidth(theTabWidth);
}

void vimPlainEdit::setFontSize(int size)	//用于增大字体
{
    this->setTabStopWidth(theTabWidth);
    this->zoomIn(size);
}

void vimPlainEdit::setProject(int Pro)
{
    int hasPro = 3;										//暂时有四种配色方案
    if(Pro > hasPro)
        return;

    //使用case选择，方便以后扩充
    switch (Pro) {
    case 0:		//设置背景色为黑色，字体为白色
        backPen.setColor(QPalette::Active, QPalette::Base, QColor("#333333"));
        backPen.setColor(QPalette::Active, QPalette::Text, Qt::white);
        this->setPalette(backPen);

        //设置Esc配色为白底黑字加粗
        newCharFormat.setBackground(QColor(Qt::white));
        newCharFormat.setForeground(Qt::black);
        //newCharFormat.setFontWeight(QFont::Bold);	//加粗
        //返回原来的颜色
        defCharFormat.setBackground(QColor("#333333"));
        defCharFormat.setForeground(Qt::white);
        //defCharFormat.setFontWeight(QFont::Thin);
        break;

    case 1:		//设置背景色为黑底（灰底），字体为绿色
        backPen.setColor(QPalette::Active, QPalette::Base, QColor("#333333"));
        backPen.setColor(QPalette::Active, QPalette::Text, QColor("#00FF00"));
        this->setPalette(backPen);

        //设置Esc配色为绿底黑字不加粗
        newCharFormat.setBackground(QColor("#00FF00"));
        newCharFormat.setForeground(Qt::black);
        //newCharFormat.setFontWeight(QFont::Thin);
        //返回原来的颜色黑底绿字不加粗
        defCharFormat.setBackground(QColor("#333333"));
        defCharFormat.setForeground(QColor("#00FF00"));
        //defCharFormat.setFontWeight(QFont::Thin);
        break;

    case 2:		//设置背景色为白底，字体为黑字
        backPen.setColor(QPalette::Active, QPalette::Base, Qt::white);
        backPen.setColor(QPalette::Active, QPalette::Text, Qt::black);
        this->setPalette(backPen);

        //设置Esc配色为黑底白字不加粗
        newCharFormat.setBackground(QColor(Qt::black));
        newCharFormat.setForeground(Qt::white);
        //newCharFormat.setFontWeight(QFont::Thin);
        //返回原来的颜色白底黑字不加粗
        defCharFormat.setBackground(QColor(Qt::white));
        defCharFormat.setForeground(Qt::black);
        //defCharFormat.setFontWeight(QFont::Thin);
        break;

    case 3:		//设置背景色为黄底，字体为黑字
        backPen.setColor(QPalette::Active, QPalette::Base, QColor("#FFEC8B"));
        backPen.setColor(QPalette::Active, QPalette::Text, Qt::black);
        this->setPalette(backPen);

        //设置Esc配色为黑底白字不加粗
        newCharFormat.setBackground(QColor(Qt::black));
        newCharFormat.setForeground(Qt::white);
        //newCharFormat.setFontWeight(QFont::Thin);
        //返回原来的颜色黄底黑字不加粗
        defCharFormat.setBackground(QColor("#FFEC8B"));
        defCharFormat.setForeground(Qt::black);
        //defCharFormat.setFontWeight(QFont::Thin);
        break;

    default:
        break;
    }
    this->appendPlainText(QString::number(Pro, 10));
}

void vimPlainEdit::setChar(uint pos, uint num)	//修改光标位置背景色及前景色，修改个数，修改为非高亮，用于恢复上一高亮字符
{
    uint markNowPos = this->textCursor().position();
    theCursor.movePosition(QTextCursor::Start);
    theCursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, pos-num);
    theCursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, num);
    this->setTextCursor(theCursor);
    this->setCurrentCharFormat(defCharFormat);

    //恢复到当前指向的光标位置	recover the TextCursor position
    theCursor.movePosition(QTextCursor::Start);
    theCursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor,markNowPos);
    this->setTextCursor(theCursor);
}

void vimPlainEdit::escPattern()		//转为Esc模式
{
    if(pattern)
        return;
    const QChar *fd;
    QChar fds;

    //设置新标记
    nowPos = this->textCursor().position();
    theCursor.movePosition(QTextCursor::Start);
    theCursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, nowPos-1);
    theCursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
    this->setTextCursor(theCursor);
    this->setCurrentCharFormat(newCharFormat);
    fd = theCursor.selectedText().unicode();		//检查左右是否可以移动
    fds = *fd;
    if(fds == QChar::ParagraphSeparator)
    {
        canLeft = canRight = false;
    }
    else {
        canLeft = canRight = true;
    }

    //nowPos = this->textCursor().position();
    whetherCanUpDown();		//检查上下是否可以移动，指针选择已被getLinePos()消除

    nowPos = this->textCursor().position();
    this->setReadOnly(true);
    pattern = true;
}
void vimPlainEdit::insertPattern(uint num, bool isBehind)	//转为写入模式
{
    if(!pattern)
        return;
    uint tmpPos = nowPos;
    setChar(tmpPos, num);	//清理旧标记
    this->setReadOnly(false);

    nowPos = this->textCursor().position();
    //插入方式在所选前面还是后面
    theCursor.movePosition(QTextCursor::Start);
    if(isBehind)	//在后面插入，用于a键
    {
        theCursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, nowPos);
        this->setTextCursor(theCursor);
    }
    else	//在前面插入，用于i键
    {
        theCursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, nowPos-1);
        this->setTextCursor(theCursor);
    }

    pattern = false;
}

void vimPlainEdit::moveCursorLeft()		//光标左移
{
    if(!canLeft)
        return;

    const QChar *fd;
    QChar fds;
    nowPos = this->textCursor().position();
    //chack highlight mark 检查是否可以左移，可以则高亮并消除原有字符
    theCursor.movePosition(QTextCursor::Start);
    theCursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, nowPos-2);
    theCursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
    this->setTextCursor(theCursor);
    fd = theCursor.selectedText().unicode();
    fds = *fd;
    if(fds == QChar::ParagraphSeparator || fds == QChar::LineSeparator || nowPos == 1)
    {
        canLeft = false;	//遇到分段符、换行符、文本开头，禁止左移
        theCursor.movePosition(QTextCursor::Start);
        theCursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, nowPos);
        this->setTextCursor(theCursor);
        return;
    }
    this->setCurrentCharFormat(newCharFormat);

    setChar(nowPos, 1);	//清理旧标记
    nowPos = this->textCursor().position();

    canRight = true;	//左移之后就可以右移	it can move right after move left
}
void vimPlainEdit::moveCursorRight()	//光标右移
{
    if(!canRight)
        return;

    const QChar *fd;
    QChar fds;
    nowPos = this->textCursor().position();
    //chack highlight mark 检查是否可以右移，可以则高亮并消除原有字符
    theCursor.movePosition(QTextCursor::Start);
    theCursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, nowPos);
    theCursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
    this->setTextCursor(theCursor);
    fd = theCursor.selectedText().unicode();
    fds = *fd;
    if(fds == QChar::ParagraphSeparator || fds == QChar::LineSeparator || theCursor.selectedText().isEmpty())
    {
        canRight = false;	//遇到分段符、换行符、文本开头，禁止右移
        theCursor.movePosition(QTextCursor::Start);
        theCursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, nowPos);
        this->setTextCursor(theCursor);
        return;
    }
    this->setCurrentCharFormat(newCharFormat);

    setChar(nowPos, 1);	//清理旧标记，此时光标的选择标记已消除
    nowPos = this->textCursor().position();

    canLeft = true;		//右移后就可以左移	it can move left after move right
}
void vimPlainEdit::moveCursorDown()	//光标下移
{
    if(!canDown)
        return;

    nowPos = this->textCursor().position();
    nowLine = getLinePos(nowPos);
    if(nowLine == linesNumber)
    {
        const QChar *fd;
        QChar fds;
        theCursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor);
        theCursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor);
        this->setTextCursor(theCursor);
        fd = theCursor.selectedText().unicode();
        fds = *fd;
        if(fds == QChar::ParagraphSeparator || fds == QChar::LineSeparator)		//判断是否是连续两个分段符
        {
            nowLine = linesNumber-1;
            theCursor.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor, 2);	//恢复指针位置 recover the point
            this->setTextCursor(theCursor);
        }
        else
        {
            theCursor.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor, 2);	//恢复指针位置 recover the point
            this->setTextCursor(theCursor);
            canDown = false;
            return;
        }
    }

    setChar(nowPos, 1);	//清理旧标记

    theCursor.movePosition(QTextCursor::Start);
    theCursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, --nowPos);
    theCursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor);
    theCursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
    this->setTextCursor(theCursor);
    this->setCurrentCharFormat(newCharFormat);
    nowPos = this->textCursor().position();

    theCursor.movePosition(QTextCursor::Start);
    theCursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, nowPos);
    this->setTextCursor(theCursor);

    canUp = canLeft = canRight = true;
}
void vimPlainEdit::moveCursorUp()	//光标上移
{
    if(!canUp)
        return;

    nowPos = this->textCursor().position();
    nowLine = getLinePos(nowPos);
    if(nowLine == 1)
    {
        canUp = false;
        return;
    }

    setChar(nowPos, 1);	//清理旧标记

    theCursor.movePosition(QTextCursor::Start);
    theCursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, --nowPos);
    theCursor.movePosition(QTextCursor::Up, QTextCursor::MoveAnchor);
    theCursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
    this->setTextCursor(theCursor);
    this->setCurrentCharFormat(newCharFormat);
    nowPos = this->textCursor().position();

    theCursor.movePosition(QTextCursor::Start);
    theCursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, nowPos);
    this->setTextCursor(theCursor);

    canDown = canLeft = canRight = true;
}
void vimPlainEdit::rowTo(int pos)	//列内移动
{
    uint tmpPos = nowPos;
    nowPos = this->textCursor().position();

    nowLine = getLinePos(nowPos);
    if(nowLine + pos <= 0)
        pos = 1 - nowLine;
    if(nowLine + pos > linesNumber)
        pos = linesNumber - nowLine;

    theCursor.movePosition(QTextCursor::Start);
    theCursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, --nowPos);
    if(pos <= 0)
    {
        theCursor.movePosition(QTextCursor::Up, QTextCursor::MoveAnchor, pos);
    }
    else
    {
        theCursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, pos);
    }
    theCursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
    this->setTextCursor(theCursor);
    this->setCurrentCharFormat(newCharFormat);

    nowPos = this->textCursor().position();
    theCursor.movePosition(QTextCursor::Start);
    theCursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, nowPos);
    this->setTextCursor(theCursor);

    setChar(tmpPos, 1);	//清理旧标记
}
void vimPlainEdit::lineTo(int pos)	//行内移动
{
    if(pos < 1)
        pos = 1;

    nowPos = this->textCursor().position();
    uint tmpPos = nowPos;
    const QChar *fd;
    QChar fds;

    theCursor.movePosition(QTextCursor::Start);
    theCursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, nowPos);
    if(pos < 32767)
    {
        theCursor.movePosition(QTextCursor::StartOfLine);
        theCursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, --pos);
    }
    else
    {
        theCursor.movePosition(QTextCursor::EndOfLine);
        theCursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor);
    }
    theCursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
    this->setTextCursor(theCursor);
    fd = theCursor.selectedText().unicode();
    fds = *fd;
    if(fds == QChar::ParagraphSeparator || fds == QChar::LineSeparator)
    {
        canRight = false;
        theCursor.movePosition(QTextCursor::EndOfLine);
        theCursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor);
        theCursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor);
        this->setTextCursor(theCursor);
        this->setCurrentCharFormat(newCharFormat);

        theCursor.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor);
        this->setTextCursor(theCursor);
        nowPos = this->textCursor().position();
        setChar(tmpPos, 1);
        return;
    }

    nowPos = this->textCursor().position();
    if(nowPos == 1)
        canLeft = false;

    this->setCurrentCharFormat(newCharFormat);

    setChar(tmpPos, 1);	//清理旧标记
    canRight = true;
}

bool vimPlainEdit::eventFilter(QObject *watch, QEvent *event)
{
    if(watch == this)
    {
        if(event->type() == QKeyEvent::KeyRelease)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if(keyEvent->key() == Qt::Key_Escape)
            {
                escPattern();
                pattern = true;
                return true;
            }

            if(pattern)
            {
                switch (keyEvent->key())
                {
                case Qt::Key_I:
                case Qt::Key_Insert:
                    insertPattern(1, false);
                    pattern = false;
                    break;
                case Qt::Key_A:
                    insertPattern(1, true);
                    pattern = false;
                    break;
                case Qt::Key_H:
                case Qt::Key_Left:
                    moveCursorLeft();
                    break;
                case Qt::Key_L:
                case Qt::Key_Right:
                    moveCursorRight();
                    break;
                case Qt::Key_J:
                case Qt::Key_Down:
                    moveCursorDown();
                    break;
                case Qt::Key_K:
                case Qt::Key_Up:
                    moveCursorUp();
                    break;
                case Qt::Key_Delete:
                    this->textCursor().deleteChar();
                    break;
                case Qt::Key_Backspace:
                    this->textCursor().deletePreviousChar();
                    break;
                case Qt::Key_Home:
                    lineTo(1);
                    break;
                case Qt::Key_End:
                    lineTo(32767);
                    break;
                default:
                    emit getText();
                    break;
                }
                return true;
            }

        }
        if(event->type() == QWheelEvent::Wheel)
        {
            QWheelEvent *wheel = static_cast<QWheelEvent *>(event);
            if(wheel->delta() > 10)
            {
                setFontSize(1);
                theTabWidth += 8;
                return true;
            }
            if(wheel->delta() < -10)
            {
                setFontSize(-1);
                if(theTabWidth >8)
                    theTabWidth -= 8;
                if(theTabWidth>0 && theTabWidth <= 8)
                    theTabWidth -= 2;
                else
                    theTabWidth = 1;
                return true;
            }
        }
    }
    return false;
}

void vimPlainEdit::chackLines()	//检查当前文本总行数
{
    linesNumber = this->document()->lineCount();
}
void vimPlainEdit::chackBlocks()	//以block方式检查当前文本总行数
{
    linesNumber = this->document()->blockCount();
}
void vimPlainEdit::chackRows()	//检查当前行总列数
{
    const QChar *fd;
    QChar fds;
    rowsNumber = 0;

    theCursor.movePosition(QTextCursor::Start);
    theCursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, nowPos);
    theCursor.movePosition(QTextCursor::StartOfLine);
    while (fds != QChar::ParagraphSeparator && fds != QChar::LineSeparator) {
        theCursor.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor);
        theCursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor);
        this->setTextCursor(theCursor);
        fd = theCursor.selectedText().unicode();
        fds = *fd;
        rowsNumber++;
        theCursor.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor);
        this->setTextCursor(theCursor);
    }
    rowsNumber--;

    theCursor.movePosition(QTextCursor::Start);
    theCursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, nowPos);
    this->setTextCursor(theCursor);
}
uint vimPlainEdit::getLinePos(uint tempPos)		//获取指定行位置 get the linePos at the tempPos
{
    uint markNowPos = this->textCursor().position();
    const QChar *fd;
    QChar fds;
    uint tempLine = 0;
    uint tempCharacter = 0;

    while (tempCharacter < tempPos) {
        theCursor.movePosition(QTextCursor::Start);
        theCursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, tempCharacter);
        theCursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
        this->setTextCursor(theCursor);
        fd = theCursor.selectedText().unicode();		//光标右移一位，左移框选一位，检查是否为换行或分段
        fds = *fd;
        tempCharacter++;
        if(fds == QChar::ParagraphSeparator)
            tempLine++;
        if(fds == QChar::LineSeparator)
        {
            tempLine++;
        }
        theCursor.movePosition(QTextCursor::PreviousCharacter);
        this->setTextCursor(theCursor);
    }

    theCursor.movePosition(QTextCursor::Start);
    theCursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, markNowPos);	//计算完成，跳到指定指针位置
    this->setTextCursor(theCursor);
    return ++tempLine;
}

void vimPlainEdit::setAutoWrap()	//设为自动换行模式
{
    this->setLineWidth(WidgetWidth);
}

void vimPlainEdit::whetherCanUpDown()	//检查是否可以上下移动
{
    if(this->lineWrapMode() == LineWrapMode::NoWrap)	//如果是无自动换行则行数为实际行数，否则为段数
        chackLines();
    else
        chackBlocks();

    nowLine = getLinePos(nowPos);	//获取当前行号
    if(nowLine == 1)
    {
        canUp = false;
    }
    else {
        canUp = true;
    }
    if(nowLine == linesNumber)
    {
        canDown = false;
    }
    else {
        canDown = true;
    }
}
