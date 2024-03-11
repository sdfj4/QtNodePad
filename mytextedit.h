#ifndef MYTEXTEDIT_H
#define MYTEXTEDIT_H

#include <QWidget>

namespace Ui {
class MyTextEdit;
}

class MyTextEdit : public QWidget
{
    Q_OBJECT

public:
    explicit MyTextEdit(QWidget *parent = nullptr);
    ~MyTextEdit();

private:
    Ui::MyTextEdit *ui;
    QFont mFont;

    void initConnect();   //绑定滚动条
    void initFont();     //修改字体
    void initHighlighter(); //高亮
private slots:
    void textEditHScrollBarChanged();  //textedit的滚动条
    void scrollBarChanged();  //horizontalScrollBar滑块
    void onTextChanged();    //显示行号
    void textEditVScrollBarChanged();   //行号与文本一起变化
    void textBrowserVScrollBarChanged();   //文本滑块滑动,行号滑块变化
};

#endif // MYTEXTEDIT_H
