#ifndef MYCODEEDITOR_H
#define MYCODEEDITOR_H

#include "myhighlighter.h"

#include <QCompleter>
#include <QPlainTextEdit>
#include <QWidget>

class LineNumberWidget;

class MyCodeEditor : public QPlainTextEdit    //效率比textedit高
{
    Q_OBJECT
public:
    explicit MyCodeEditor(QWidget *parent = nullptr,QFont font=QFont("Consolas",17));

    ~MyCodeEditor();

    void lineNumberWidgetPaintEvent(QPaintEvent * event);
    void lineNumberWidgetMousePressEvent(QMouseEvent *event);
    void lineNumberWidgetWheelEvent(QWheelEvent *event);

    bool saveFile();
    bool saveAsFile();

    void setFileName(QString fileName);
    QString getFileName();

    void setAllFont(QFont font);
    //判断是否保存
    bool checkSaved();

private slots:
    void highlightCurrentLine();
    void updateLineNumberWidget(QRect rect, int dy);
    void updateLineNumberWidgetWidth();
    void updateSaveState();

    // void insertCompletion(const QString &completion);

protected:
    void resizeEvent(QResizeEvent *event) override;
    // void keyPressEvent(QKeyEvent * event) override;

private:
    void initConnection();
    void initHighlighter();
    // void initCompleter();
    int getLineNumberWidgetWidth();


    LineNumberWidget * lineNumberWidget;
    MyHighlighter * mHighlighter;

    QString mFileName;
    //默认不保存
    bool isSaved = false;
    // QCompleter * mCompleter;

signals:
};

//行高亮
class LineNumberWidget :public QWidget{
public:
    //初始化codeEdit
    explicit LineNumberWidget(MyCodeEditor *editor=nullptr):QWidget(editor){
        codeEditor=editor;
    }
protected:
    void  paintEvent(QPaintEvent *event) override{
        //把绘制任务提交给MyCodeEditor
        codeEditor->lineNumberWidgetPaintEvent(event);
    }
    void mousePressEvent(QMouseEvent *event) override{
        //把鼠标点击任务提交给MyCodeEditor
        codeEditor->lineNumberWidgetMousePressEvent(event);
    }

    void wheelEvent(QWheelEvent *event) override{
        //把滚轮任务提交给MyCodeEditor
        codeEditor->lineNumberWidgetWheelEvent(event);
    }

private:
    MyCodeEditor *codeEditor;
};
#endif // MYCODEEDITOR_H
