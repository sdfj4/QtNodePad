#include "myhighlighter.h"
#include "mytextedit.h"
#include "ui_mytextedit.h"
#include <QDebug>
MyTextEdit::MyTextEdit(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MyTextEdit)
{
    ui->setupUi(this);
    //绑定滚动条
    initConnect();
    //初始化字体
    initFont();
    //高亮
    initHighlighter();
}

MyTextEdit::~MyTextEdit()
{
    delete ui;
}

void MyTextEdit::initConnect()
{
    connect(ui->textEdit,SIGNAL(textChanged()),this,SLOT(onTextChanged()));
    //滑块绑定
    connect(ui->textEdit->horizontalScrollBar(),SIGNAL(valueChanged(int)),this,SLOT(textEditHScrollBarChanged()));
    connect(ui->horizontalScrollBar,SIGNAL(valueChanged(int)),this,SLOT(scrollBarChanged()));
    //行号绑定
    //滑块绑定
    connect(ui->textEdit->verticalScrollBar(),SIGNAL(valueChanged(int)),this,SLOT(textEditVScrollBarChanged()));
    connect(ui->textBrowser->verticalScrollBar(),SIGNAL(valueChanged(int)),this,SLOT(textBrowserVScrollBarChanged()));
}

void MyTextEdit::initFont()
{
    mFont =QFont("Consolas",14);
    ui->textEdit->setFont(mFont);
    ui->textBrowser->setFont(mFont);
}

void MyTextEdit::initHighlighter()
{
    //new MyHighlighter(ui->textEdit->document(),"Times New Roman",18);
    new MyHighlighter(ui->textEdit->document());
}

void MyTextEdit::textEditHScrollBarChanged()
{   //两个滑块的最大最小值同步
    ui->horizontalScrollBar->setMinimum(ui->textEdit->horizontalScrollBar()->minimum());
    ui->horizontalScrollBar->setMaximum(ui->textEdit->horizontalScrollBar()->maximum());
    //设置滑块长度
    ui->horizontalScrollBar->setPageStep(ui->textEdit->horizontalScrollBar()->pageStep());
    //textedit的滑块与horizontalScrollBar滑块一起移动
    ui->horizontalScrollBar->setValue(ui->textEdit->horizontalScrollBar()->value());
}

void MyTextEdit::scrollBarChanged()
{
    ui->textEdit->horizontalScrollBar()->setValue(ui->horizontalScrollBar->value());
}

void MyTextEdit::onTextChanged()
{
    int lineCountOfTextEdit = ui->textEdit->document()->lineCount();
    QString text = ui->textBrowser->toPlainText();    //获取当前行数
    int lineCountOfTextBrowser = text.trimmed().split("\n").length();  //trimmed默认删除空格

    //判断行数大小
    if(lineCountOfTextBrowser>lineCountOfTextEdit){
        for(int i = lineCountOfTextBrowser;i>lineCountOfTextEdit;i--){
            text.chop((QString::number(i)+"\n").length());    //删除字段末
        }
    }else if(lineCountOfTextBrowser==1&&text.length()<1){
        text+= "1\n";
    }else if(lineCountOfTextBrowser<lineCountOfTextEdit){
        for(int i = lineCountOfTextBrowser;i<lineCountOfTextEdit;i++){
            text+= QString::number(i+1)+"\n";
        }
    }

    ui->textBrowser->setMaximumWidth(25+QString::number(lineCountOfTextEdit).length()*10);
    ui->textBrowser->setText(text);
}

void MyTextEdit::textEditVScrollBarChanged()
{
    ui->textBrowser->verticalScrollBar()->setValue(ui->textEdit->verticalScrollBar()->value());
}

void MyTextEdit::textBrowserVScrollBarChanged()
{
    ui->textEdit->verticalScrollBar()->setValue(ui->textBrowser->verticalScrollBar()->value());
}
