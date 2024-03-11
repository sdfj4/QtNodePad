#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QFontDialog>
// #include "mytextedit.h"
// #include "mytexteditbycode.h"
#include <QDebug>
#include <QDateTime>
// #include <mycodeeditor.h>
//满足条件导入打印机需要的头文件
#if defined(QT_PRINTSUPPORT_LIB)
#include <QtPrintSupport/qtprintsupportglobal.h>
#if QT_CONFIG(printer)
#if QT_CONFIG(printdialog)
#include <QPrintDialog>
#endif
#include <QPrinter>
#endif
#endif

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //组件布满全局
    // this->setCentralWidget(ui->tabWidget);
    mSettings = new QSettings("setting.ini",QSettings::IniFormat);

    //设置treeview和filesystem的比例
    QList<int> sizeList;
    sizeList<<1000<<4000;
    ui->splitter->setSizes(sizeList);

    //初始菜单
    initMenu();
    //初始字体
    initFont();
    //启动程序自动打开一个
    initAction();
    //初始化文件目录
    initTreeView();
#if !QT_CONFIG(printer)
    ui->printf->setEnabled(false);
#endif

}

MainWindow::~MainWindow()
{
    delete ui;
}



//初始化菜单
void MainWindow::initMenu()
{
    //获取Menu
    QMenu *recent = this->findChild<QMenu*>("recent");
    //获取Action
    QList<QObject *> chLists =recent->children();
    foreach(QObject * ch,chLists){
        QAction *action = (QAction *)ch;
        //清空子菜单Action
        recent->removeAction(action);
    }

    QList<QString> lists = getHistory();


    for(int i=lists.size()-1 ;i>=0;i--){
        //生成子菜单Action
        recent->addAction(lists[i],this,&MainWindow::on_open_rencent_file);
    }

    if(lists.size()>0){
        recent->addAction("清除历史记录",this,&MainWindow::on_clear_history_triggered,QKeySequence("Ctrl+Alt+Shift+C"));
    }

}

//初始化字体
void MainWindow::initFont()
{
    mFontFamily = mSettings->value("font_family","Consolas").toString();
    mFontSize = mSettings->value("font_size",14).toInt();
}

//初始化之后动作
void MainWindow::initAction()
{
    bool valid =ui->tabWidget->count()>0;
    ui->save_file->setEnabled(valid);
    ui->save_as->setEnabled(valid);
    ui->copy->setEnabled(valid);
    ui->paste->setEnabled(valid);
    ui->cut->setEnabled(valid);
    ui->undo->setEnabled(valid);
    ui->redo->setEnabled(valid);
}

//打开最近文件
void MainWindow::on_open_rencent_file()
{
    //打开最近文件
    QAction *action =(QAction*)sender();
    QString fileName = action->text();
    createTab(fileName);
}

//获取历史记录
QList<QString> MainWindow::getHistory(){
    //打开开始读
    int size =mSettings->beginReadArray("history");

    //创建返回对象
    QList<QString> lists;
    for(int i = 0;i<size;i++){
        mSettings->setArrayIndex(i);
        QString path = mSettings->value("path").toString();
        lists.append(path);
    }
    //关闭读
    mSettings->endArray();

    return lists;
}

//保存历史记录
void MainWindow::saveHistory(QString path){

    //获取历史
    QList<QString> lists = getHistory();
    foreach(QString str,lists){
        if(str==path){
            lists.removeOne(str);
        }
    }
    lists.append(path);

    //打开开始写入
    mSettings->beginWriteArray("history");
    for(int i =0;i<lists.size();i++){
        mSettings->setArrayIndex(i);
        //保存字符串
        mSettings->setValue("path",lists[i]);
    }

    //关闭开始写入
    mSettings->endArray();
}

//新建文件
void MainWindow::on_new_file_triggered()
{
    // curreFile.clear();
    // ui->textEdit->clear();
    //alt+enter快速添加头文件

    // MyTextEdit *myTextEdit = new MyTextEdit(this);
    // ui->tabWidget->addTab(new  MyTextEditByCode(),"new file 01");
     ui->tabWidget->addTab(new  MyCodeEditor(this,QFont(mFontFamily,mFontSize)),"new file 01");
    initAction();

}

//打开文件
void MainWindow::on_open_file_triggered()
{
    QString filePath =QFileDialog::getOpenFileName(this,"打开文件");
    createTab(filePath);

}

//创建tab
void MainWindow::createTab(QString filePath)
{
    QFile file(filePath);
    if(!file.open(QIODevice::ReadOnly|QFile::Text)){
        QMessageBox::warning(this,"警告","无法打开文件:"+file.errorString());
        return ;
    }
    curreFile  = filePath;
    setWindowTitle(filePath);
    QTextStream in(&file);
    QString text =in.readAll();

    //创建一个codeedit
    MyCodeEditor *codeEdit = new MyCodeEditor(this,QFont(mFontFamily,mFontSize));
    codeEdit->setPlainText(text);
    // ui->textEdit->setText(text);
    //添加新的分页
    ui->tabWidget->addTab(codeEdit,curreFile);
    initAction();
    //为当前的分页添加修饰(获取对应的下标)
    ui->tabWidget->setCurrentIndex(ui->tabWidget->count()-1);
    file.close();

    saveHistory(curreFile);
    initMenu();
}

//文件目录
void MainWindow::initTreeView()
{
    //创建一个文件模型
    mFileSystemModel = new MyFileSystemModel();
    //设置文件模型的根目录|.//表示当前文件
    mFileSystemModel->setRootPath(".//");
    //treeview添加模型(默认不可以编写)
    ui->treeView->setModel(mFileSystemModel);
    //treeview添加根目录
    ui->treeView->setRootIndex(mFileSystemModel->index(".//"));
    //隐藏列
    // ui->treeView->setColumnHidden(1,true);
    // ui->treeView->setColumnHidden(2,true);
    // ui->treeView->setColumnHidden(3,true);
    //隐藏标题
    ui->treeView->setHeaderHidden(true);
    //添加鼠标右键菜单
    ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    //添加右键信号事件
    connect(ui->treeView,SIGNAL(customContextMenuRequested(const QPoint &)),this,SLOT(showCustomContextMenu(const QPoint &)));
    //取消双击编辑
    ui->treeView->setEditTriggers(QTreeView::NoEditTriggers);
    //双击打开事件
    connect(ui->treeView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(onDoubleTreeViewClicked(const QModelIndex&)));
}

void MainWindow::showCustomContextMenu(const QPoint &pos)
{
    //创建菜单类
    QModelIndex index =  ui->treeView->indexAt(pos);
    QMenu menu(this);
    //获取当前右键的东西的属性
    if(mFileSystemModel->isDir(index)){    //如果是文件夹
        QString filePath = mFileSystemModel->filePath(index);
        if(filePath.isNull())  filePath = ".//";
        menu.addAction("创建文件(New File)",[=](){
            //文件路径
            QString fileName = filePath+"/新建文件"+QDateTime::currentDateTime().toString("yyyy-MM-dd hh-mm-ss")+".sdj4";
            QFile file(fileName);
            if(!file.open(QIODevice::ReadWrite)){
                QMessageBox::critical(this,"失败","创建新文件失败!可能没有权限或文件已经存在！");
            }
            file.close();
        });
        menu.addAction("新建文件夹(New Folder)",[=](){
            QDir dir(filePath);
            QString dirStr ="新建文件夹"+QDateTime::currentDateTime().toString("yyyy-MM-dd hh-mm-ss");
            if(!dir.exists(dirStr)){   //判断文件路径是否存在
                dir.mkdir(dirStr);
            }
        });
        menu.addAction("重命名文件夹(ReName File)",[=](){
            ui->treeView->edit(index);
        });
    }else{
        menu.addAction("重命名(Rename File)",[=](){
            //编辑节点
            ui->treeView->edit(index);
        });
        menu.addAction("删除文件(Remove File)",[=](){
            //删除文件
            //文件名称
            QString fileName = mFileSystemModel->fileName(index);
            //文件路径
            QString filePath = mFileSystemModel->filePath(index);
            if(QMessageBox::Yes==QMessageBox::question(this,"是否删除","请确认是否删除文件：\n   "+fileName+"\n此操作不可恢复!",QMessageBox::Yes|QMessageBox::No)){
                //删除
                if(!QFile::remove(filePath))
                    QMessageBox::critical(this,"失败","删除文件：\n   "+fileName+"\n失败!可能没有权限或文件不存在！");
            }
        });
    }
    menu.exec(QCursor::pos());
}
//双击打开文件夹
void MainWindow::onDoubleTreeViewClicked(const QModelIndex &index)
{
    //首先判断是不是文件
    if(!mFileSystemModel->isDir(index)){
        QString filePath = mFileSystemModel->filePath(index);
        createTab(filePath);
    }
}

//保存文件
void MainWindow::on_save_file_triggered()
{
    //把文件保存交给mycodeedit
    MyCodeEditor *codeEditWidget = (MyCodeEditor*) ui->tabWidget->currentWidget();
    if(codeEditWidget){
        if(codeEditWidget->saveFile()){
         saveSuccessAction(codeEditWidget);
        }
    }

}

//另存文件
void MainWindow::on_save_as_triggered()
{
    //把文件保存交给mycodeedit
    MyCodeEditor *codeEditWidget = (MyCodeEditor*) ui->tabWidget->currentWidget();
    if(codeEditWidget){
        if(codeEditWidget->saveAsFile()){
            saveSuccessAction(codeEditWidget);
        }
    }
}

//粘贴
void MainWindow::on_paste_triggered()
{
    //把粘贴交给CodeEditor
    MyCodeEditor *codeEditor = (MyCodeEditor* )ui->tabWidget->currentWidget();
    if(codeEditor){
        codeEditor->paste();
    }
}

//剪切
void MainWindow::on_cut_triggered()
{
    //把剪切交给CodeEditor
    MyCodeEditor *codeEditor = (MyCodeEditor* )ui->tabWidget->currentWidget();
    if(codeEditor){
        codeEditor->cut();
    }
}

//复制
void MainWindow::on_copy_triggered()
{
    //把复制交给CodeEditor
    MyCodeEditor *codeEditor = (MyCodeEditor* )ui->tabWidget->currentWidget();
    if(codeEditor){
        codeEditor->copy();
    }
}

//加粗
// void MainWindow::on_border_triggered(bool checked)
// {
//     ui->textEdit->setFontWeight(checked?QFont::Bold:QFont::Normal);
// }

//斜体
// void MainWindow::on_italic_triggered(bool checked)
// {
//     ui->textEdit->setFontItalic(checked);
// }

//下划线
// void MainWindow::on_underline_triggered(bool checked)
// {
//     ui->textEdit->setFontUnderline(checked);
// }

//撤销
void MainWindow::on_undo_triggered()
{
    //撤销
    MyCodeEditor *codeEditor = (MyCodeEditor* )ui->tabWidget->currentWidget();
    if(codeEditor){
        codeEditor->undo();
    }
}

//取消
void MainWindow::on_redo_triggered()
{
    //取消
    MyCodeEditor *codeEditor = (MyCodeEditor* )ui->tabWidget->currentWidget();
    if(codeEditor){
        codeEditor->redo();
    }
}

//打印机
void MainWindow::on_printf_triggered()
{
    MyCodeEditor *codeEditor = (MyCodeEditor* )ui->tabWidget->currentWidget();
    if(codeEditor){
#if defined(QT_PRINTSUPPORT_LIB) && QT_CONFIG(printer)
        QPrinter printDev;
#if QT_CONFIG(printdialog)
        QPrintDialog dialog(&printDev,this);
        if(dialog.exec()==QDialog::Rejected)
            return;
#endif
        // ui->textEdit->print(&printDev);
        //把打印交个codeEdit
        codeEditor->print(&printDev);
#endif
    }
}

//退出
void MainWindow::on_exit_triggered()
{
    QCoreApplication::exit();
}

//字体
void MainWindow::on_font_triggered()
{
     bool fontSelected;
    QFont font = QFontDialog::getFont(&fontSelected,QFont(mFontFamily,mFontSize),this);
    if(fontSelected){
        //设置字体
        MyCodeEditor *codeEditor = (MyCodeEditor* )ui->tabWidget->currentWidget();
        if(codeEditor){
            codeEditor->setAllFont(font);
        }
        mSettings->setValue("font_family",font.family());
        mSettings->setValue("font_size",font.pointSize());
    }
}

//关于
void MainWindow::on_about_triggered()
{
    QMessageBox::about(this,"这是我的Notepad！","这是我的Notepad，欢迎学习和使用！");
}


void MainWindow::on_clear_history_triggered()
{
    mSettings->remove("history");
    initMenu();
}

//关闭tab标签页
void MainWindow::on_tabWidget_tabCloseRequested(int index)
{
    //关闭前看有没有保存|可以用QMessageBox::Rejected获取点击后的信息
    MyCodeEditor *codeEdit =(MyCodeEditor*)ui->tabWidget->widget(index);   //获取当前的窗口
    if(!codeEdit->checkSaved()){
        QMessageBox::StandardButton btn = QMessageBox::question(this,"警告","您还没有保存文档!是否保存?",QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);
        if(btn == QMessageBox::Yes){
            //如果点yes就保存
            if(codeEdit->saveFile()){
                saveSuccessAction(codeEdit);
                return;
            }
        }else if(btn == QMessageBox::Cancel){
            return;
        }
    }
    ui->tabWidget->removeTab(index);
    delete codeEdit;
    initAction();
}



//保存
void MainWindow::saveSuccessAction(MyCodeEditor *codeEdit)
{
    QString filename =codeEdit->getFileName();
    //保存历史
    saveHistory(codeEdit->getFileName());
    //设置tab标题
    ui->tabWidget->setTabText(ui->tabWidget->currentIndex(),filename);
    //初始化菜单
    initMenu();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(ui->tabWidget->count()>0){
        QMessageBox::StandardButton btn = QMessageBox::question(this,"警告","还有文件是否继续关闭",QMessageBox::Yes|QMessageBox::No);
        if(btn ==QMessageBox::Yes){
            event->accept();
        } else {
            event->ignore();
        }
    }
}

