#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "mycodeeditor.h"
#include "QSettings"  //存储配置文件
#include <QMainWindow>
#include <QList>
#include <QFileSystemModel>
#include "myfilesystemmodel.h"
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    //保存文件的步骤单独拿出来封装
    void saveSuccessAction(MyCodeEditor *codeEdit);

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void on_new_file_triggered();

    void on_open_file_triggered();

    void on_save_file_triggered();

    void on_save_as_triggered();

    void on_paste_triggered();

    void on_cut_triggered();

    void on_copy_triggered();

    // void on_border_triggered(bool checked);

    // void on_italic_triggered(bool checked);

    // void on_underline_triggered(bool checked);

    void on_undo_triggered();

    void on_redo_triggered();

    void on_printf_triggered();

    void on_exit_triggered();

    void on_font_triggered();

    void on_about_triggered();

    void on_clear_history_triggered();

    void on_tabWidget_tabCloseRequested(int index);

    void showCustomContextMenu(const QPoint &pos);

    void onDoubleTreeViewClicked(const QModelIndex &index);
private:
    Ui::MainWindow *ui;
    QString curreFile;
    QString mFontFamily;
    int mFontSize;
    QSettings *mSettings;
    MyFileSystemModel  *mFileSystemModel;

    QList<QString> getHistory();
    void initMenu();
    void initFont();
    void on_open_rencent_file();
    void saveHistory(QString path);
    void initAction();
    void createTab(QString fileName);
    void initTreeView();
};
#endif // MAINWINDOW_H
