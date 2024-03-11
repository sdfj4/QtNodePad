#include "myfilesystemmodel.h"

MyFileSystemModel::MyFileSystemModel(QObject *parent)
    : QFileSystemModel{parent}
{}

Qt::ItemFlags MyFileSystemModel::flags(const QModelIndex &index) const
{
        //返回三个模式;可编辑|可选择|可用的
    return Qt::ItemIsEditable|Qt::ItemIsSelectable|Qt::ItemIsEnabled;
}

int MyFileSystemModel::columnCount(const QModelIndex &parent) const
{
    //只有一列
    return 1;
}
