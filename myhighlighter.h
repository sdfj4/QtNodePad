#ifndef MYHIGHLIGHTER_H
#define MYHIGHLIGHTER_H

#include <QObject>
#include <QSyntaxHighlighter>
class MyHighlighter:public QSyntaxHighlighter
{
public:
    explicit MyHighlighter(QTextDocument  *parent = nullptr,QString fontFamily="Consolas",int fontSize=14);

    void setFont(QFont font);
protected:
    void highlightBlock(const QString &text);

private:

    QString mFontFamily ;   //字体
    int mFontSize ;   //字体大小

    struct HighlightRule{
        QRegExp pattern;
        QTextCharFormat format;
    };

    QVector<HighlightRule> highlightRules;

    void addNormalTextFormat();   //添加字体高光
    void addNumberFormat();     //数字高光
    void addStringFormat();     //字符串高光
    void addCommentFormat();    //注释(难点--正则表达式匹配注释)
    void addMultiLineCommentFormat(const QString &text);   //多行注释
    void addKeywordsFromat();  //关键字
    void addClassNameFormat(); //类名高亮
    void addFunctionFormat();   //函数高亮
signals:
};

#endif // MYHIGHLIGHTER_H
