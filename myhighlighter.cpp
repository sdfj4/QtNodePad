#include "myhighlighter.h"

#include <QTextStream>

MyHighlighter::MyHighlighter(QTextDocument *parent, QString fontFamily, int fontSize)
    : QSyntaxHighlighter(parent)
{
    mFontFamily =fontFamily;
    mFontSize = fontSize;

    //普通文本
    addNormalTextFormat();
    //数字
    addNumberFormat();
    //字符串
    addStringFormat();
    //注释
    addCommentFormat();
    //对于关键字/保留字
    addKeywordsFromat();
    //类名
    addClassNameFormat();
    //方法名
    addFunctionFormat();
}

void MyHighlighter::setFont(QFont font)
{
    mFontFamily = font.family();
    mFontSize = font.pointSize();
}

void MyHighlighter::highlightBlock(const QString &text)
{
    QRegExp regExp;
    foreach (const HighlightRule &rule, highlightRules) {
        regExp=rule.pattern;
        int index = regExp.indexIn(text);
        while(index>=0){
            int length =regExp.matchedLength();
            QTextCharFormat format =  rule.format;
            format.setFont(QFont(mFontFamily,mFontSize));
            setFormat(index,length,format);
            index = regExp.indexIn(text,index+length);
        }
    }

    //调用多行注释
    addMultiLineCommentFormat(text);
}

void MyHighlighter::addNormalTextFormat()
{
    HighlightRule rule;
    rule.pattern = QRegExp("[a-z0-9A-Z]+");  //通过正则表达式获取指定的匹配a-z,0-9,A-Z，加号表示可重复

    QTextCharFormat normalTextFormat;

    normalTextFormat.setFont(QFont(mFontFamily,mFontSize));
    normalTextFormat.setForeground(QColor(0,0,139));

    rule.format = normalTextFormat;

    highlightRules.append(rule);
}

void MyHighlighter::addNumberFormat()
{
    HighlightRule rule;
    rule.pattern = QRegExp("\\b\\d+|\\d+\\.+\\d+\\b");  //数字,整数和小数点

    QTextCharFormat numberTextFormat;

    numberTextFormat.setFont(QFont(mFontFamily,mFontSize));
    numberTextFormat.setForeground(QColor(250,80,50));

    rule.format = numberTextFormat;

    highlightRules.append(rule);
}

void MyHighlighter::addStringFormat()
{
    QTextCharFormat stringFormat;

    stringFormat.setFont(QFont(mFontFamily,mFontSize));
    stringFormat.setForeground(QColor(0,180,180));


    HighlightRule rule;

    rule.format = stringFormat;

    //''
    rule.pattern = QRegExp("'[^']*'");
    highlightRules.append(rule);

    //""
    rule.pattern = QRegExp("\"[^\"]*\"");
    highlightRules.append(rule);

    //``
    rule.pattern = QRegExp("`[^`]*`");
    highlightRules.append(rule);
}

void MyHighlighter::addCommentFormat()
{

    QTextCharFormat commnetFormat;
    commnetFormat.setFont(QFont(mFontFamily,mFontSize));
    commnetFormat.setForeground(Qt::darkGreen);
    commnetFormat.setFontItalic(true);   //斜体

    HighlightRule rule;

    rule.format = commnetFormat;

    // //dada
    rule.pattern = QRegExp("\\/\\/.*$");
    highlightRules.append(rule);

    // /*xxx*/
    // rule.pattern = QRegExp("\\/\\*[^*]*\\*+(?:[^/*][^*]*\\*+)*\\/");
    // highlightRules.append(rule);
}

void MyHighlighter::addMultiLineCommentFormat(const QString &text)
{
    //对于多行注释
    setCurrentBlockState(0);   //默认高亮一行

    // /*
    QRegExp commentStratRegExp("\\/\\*");   //开始的位置
    // */
    QRegExp commentEndRegExp("\\*\\/");     //结束的位置

    //高亮格式
    QTextCharFormat multiLineCommentFormat ;
    multiLineCommentFormat.setFont(QFont(mFontFamily,mFontSize));
    multiLineCommentFormat.setForeground(Qt::darkGreen);
    multiLineCommentFormat.setFontItalic(true);
    int startIndex = 0;
    //前一行
    if(previousBlockState()!=1)
        //中间行
        startIndex = commentStratRegExp.indexIn(text);
    while(startIndex >= 0 ){
        int endIndex = commentEndRegExp.indexIn(text,startIndex);
        int commentLength = 0;
        if(endIndex == -1){    //没有结束位置
            setCurrentBlockState(1);   //标记改行--高亮
            commentLength = text.length()-startIndex;
        }else{
            commentLength = endIndex-startIndex+commentEndRegExp.matchedLength();
        }
        //设置格式
        setFormat(startIndex,
                  commentLength,
                  multiLineCommentFormat);
        startIndex = commentStratRegExp.indexIn(text,commentLength+startIndex);
    }
}

void MyHighlighter::addKeywordsFromat()
{
    //通过文件保存关键字
    QFile file(":/keyword/keyword.txt");
    QTextStream keywordsStream(&file);

    HighlightRule rule ;
    QTextCharFormat keywordsFormat;
    keywordsFormat.setFont(QFont(mFontFamily,mFontSize));
    keywordsFormat.setForeground(Qt::darkYellow);
    rule.format = keywordsFormat;

    if(file.open(QIODevice::ReadOnly|QIODevice::Text)){
        keywordsStream.seek(0);
        QString line ;
        while (!keywordsStream.atEnd()) {
            line = keywordsStream.readLine();
            if(line!=""){
                rule.pattern = QRegExp("\\b"+line+"\\b");
                highlightRules.append(rule);
            }
        }
        file.close();
    }
}

void MyHighlighter::addClassNameFormat()
{
    HighlightRule rule;

    QTextCharFormat classNameFormat;
    classNameFormat.setFont(QFont(mFontFamily,mFontSize));
    classNameFormat.setForeground(QColor(150,20,100));
    classNameFormat.setFontWeight(99);

    rule.format = classNameFormat;

    rule.pattern = QRegExp("\\b[A-Z]+\\w*");
    highlightRules.append(rule);
}

void MyHighlighter::addFunctionFormat()
{
    HighlightRule rule;

    QTextCharFormat functionFormat;
    functionFormat.setFont(QFont(mFontFamily,mFontSize));
    functionFormat.setForeground(QColor(200,100,150));

    rule.format = functionFormat;

    rule.pattern = QRegExp("\\w+\\(");
    highlightRules.append(rule);

    rule.pattern = QRegExp("\\)");
    highlightRules.append(rule);
}
