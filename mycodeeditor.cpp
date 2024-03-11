//对基于代码的界面优化
#include "mycodeeditor.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QPainter>
#include <QScrollBar>
#include <QTextStream>
MyCodeEditor::MyCodeEditor(QWidget *parent,QFont font)
    : QPlainTextEdit{parent}
{
    lineNumberWidget = new LineNumberWidget(this);
    //绑定
    initConnection();
    //高亮
    initHighlighter();
    //设置字体
    setAllFont(font);
    //行高亮
    highlightCurrentLine();
    //改变行边距
    updateLineNumberWidgetWidth();
    //不自动换行
    setLineWrapMode(QPlainTextEdit::NoWrap);
    //去掉边框
    setFrameShape(QPlainTextEdit::NoFrame);
}

MyCodeEditor::~MyCodeEditor()
{
    delete lineNumberWidget;
}

void MyCodeEditor::initConnection()
{
    //cursor
    connect(this,SIGNAL(cursorPositionChanged()),this,SLOT(highlightCurrentLine()));

    //更新保存状态
    connect(this,SIGNAL(textChanged()),this,SLOT(updateSaveState()));

    //blockCount--获取行号边距
    connect(this,SIGNAL(blockCountChanged(int)),this,SLOT(updateLineNumberWidgetWidth()));

    //updateRequest--行号刷新
    connect(this,SIGNAL(updateRequest(QRect,int)),this,SLOT(updateLineNumberWidget(QRect,int)));
}

void MyCodeEditor::initHighlighter()
{
    mHighlighter = new MyHighlighter(document());
        // mCompleter = new QCompleter();

    // QStringList list;
    // list<<"int"<<"init"<<"void"<<"while"<<"completer"<<"abstract"<<"apple";
    // list.sort(Qt::CaseInsensitive);

    // QStringListModel * model = new QStringListModel(list,mCompleter);

    // mCompleter->setModel(model);
    // mCompleter->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
    // mCompleter->setWrapAround(false);

    // mCompleter->setWidget(this);
    // mCompleter->setCompletionMode(QCompleter::PopupCompletion);

    // connect(mCompleter,SIGNAL(activated(const QString &)),this,SLOT(insertCompletion(const QString &)));
}

int MyCodeEditor::getLineNumberWidgetWidth()
{
    //获取宽度（合适）|fontMetrics().horizontalAdvance可以获取字符长度|
    return 8+QString::number(blockCount()+1).length()*fontMetrics().horizontalAdvance(QChar('0'));
}

void MyCodeEditor::setAllFont(QFont font)
{
    this->setFont(font);
    mHighlighter->setFont(font);
    // mHighlighter->rehighlight();
    // mCompleter->popup()->setFont(font);
    // viewport()->update();
    // lineNumberWidget->update();
    updateLineNumberWidgetWidth();
}

bool MyCodeEditor::checkSaved()
{
    return isSaved;
}

void MyCodeEditor::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    QTextEdit::ExtraSelection selection;
    selection.format.setBackground(QColor(0,100,100,20));
    selection.format.setProperty(QTextFormat::FullWidthSelection,true);
    selection.cursor =textCursor();

    extraSelections.append(selection);
    setExtraSelections(extraSelections);
}

void MyCodeEditor::updateLineNumberWidget(QRect rect, int dy)
{
    if(dy)
        lineNumberWidget->scroll(0,dy);
    else
        lineNumberWidget->update(0,rect.y(),getLineNumberWidgetWidth(),rect.height());
}

void MyCodeEditor::updateLineNumberWidgetWidth()
{
    //设置边距
    setViewportMargins(getLineNumberWidgetWidth(),0,0,0);
}

void MyCodeEditor::updateSaveState()
{
    //更新保存状态
    isSaved = false;
}

void MyCodeEditor::resizeEvent(QResizeEvent *event)
{
    QPlainTextEdit::resizeEvent(event);
    lineNumberWidget->setGeometry(0,0,getLineNumberWidgetWidth(),contentsRect().height());
}

void MyCodeEditor::lineNumberWidgetPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberWidget);
    //绘制行号区域
    painter.fillRect(event->rect(),QColor(100,100,100,100));

    //拿到block(本质是列表)
    QTextBlock block = firstVisibleBlock();

    //拿到行号
    int blockNumber =block.blockNumber();

    //拿到当前的block的top
    int cursorTop = blockBoundingGeometry(textCursor().block()).translated(contentOffset()).top();

    //拿到block的top
    int top = blockBoundingGeometry(block).translated(contentOffset()).top();

    //拿到block的bottom
    int bottom = top +blockBoundingRect(block).height();

    while(block.isValid()&&top<=event->rect().bottom()){
        //设置画笔颜色
        painter.setPen(cursorTop==top?Qt::black:Qt::gray);
        //绘制文字,参数为x,y,宽度,高,对齐方式,文字
        painter.drawText(0,top,getLineNumberWidgetWidth()-3,bottom-top,Qt::AlignRight,QString::number(blockNumber+1));

        //拿到下一个block
        block = block.next();

        top = bottom;
        bottom =  top +blockBoundingRect(block).height();
        blockNumber++;
    }
}

void MyCodeEditor::lineNumberWidgetMousePressEvent(QMouseEvent *event)
{
    //获取鼠标点击时的行号,知道距离,再除以高度就可以获得,setTextCursor设置光标,需要加上之前被隐藏的行号(与textedit的滑块值一致)
   setTextCursor(QTextCursor(document()->findBlockByLineNumber
                              (event->y()/fontMetrics().height()+verticalScrollBar()->value())));
}

void MyCodeEditor::lineNumberWidgetWheelEvent(QWheelEvent *event)
{
  //滚轮事件|event.delta获取滚轮距离|
    if(event->orientation()==Qt::Horizontal){     //水平滚动条
        horizontalScrollBar()->setValue(horizontalScrollBar()->value()-event->delta());
    }else {   //垂直滚动条
        verticalScrollBar()->setValue(verticalScrollBar()->value()-event->delta());
    }
    event->accept();
}

bool MyCodeEditor::saveFile()
{
    QString fileName;
    if(mFileName.isEmpty()){
        fileName = QFileDialog::getSaveFileName(this,"保存文件");
        mFileName = fileName;
    }else{
        fileName = mFileName;
    }
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly|QFile::Text)){
        QMessageBox::warning(this,"警告","文件无法打开"+file.errorString());
        return false;
    }
    QTextStream out(&file);
    // QString text = ui->textEdit->toHtml();
    // out<<text;
    out<<toPlainText();
    file.close();
    isSaved = true;
    return true;
}

bool MyCodeEditor::saveAsFile()
{
    QString fileName=QFileDialog::getSaveFileName(this,"另存文件");
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly|QFile::Text)){
        QMessageBox::warning(this,"警告","无法保存文件:"+file.errorString());
        return false;
    }
    mFileName =fileName;
    QTextStream out(&file);
    QString text =toPlainText();
    out<<text;
    file.close();

    isSaved = true;
    return true;
}

void MyCodeEditor::setFileName(QString fileName)
{
    mFileName = fileName;
}

QString MyCodeEditor::getFileName()
{
    return mFileName;
}
