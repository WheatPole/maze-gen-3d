#include "entrytablemodel.h"

EntryTableModel::EntryTableModel(QObject *parent)
    : QAbstractTableModel{parent}
{

}

bool EntryTableModel::insertData(int row, triplet<int> lIndex, WallFacing facing) {
    rowData.insert(rowData.begin() + row, {lIndex, facing});

    emit dataChanged(index(0, 0), index(rowCount()-1, columnCount()-1));
    return true;
}

bool EntryTableModel::appendData(triplet<int> lIndex, WallFacing facing) {
    beginInsertRows(QModelIndex(), rowData.size(), rowData.size());
    rowData.push_back({lIndex, facing});
    endInsertRows();

    //emit dataChanged(index(0, 0), index(rowCount()-1, columnCount()-1));
    return true;
}

EntryTableModel::RowItem EntryTableModel::eraseData(int row) {
    if (row >= rowData.size()) {
        qFatal() << "Index exceeds row size";
        return rowData[0];
    }
    RowItem data = rowData[row];
    beginRemoveRows(QModelIndex(), rowData.size(), rowData.size());
    rowData.erase(rowData.begin()+row);
    endRemoveRows();

    //emit dataChanged(index(0, 0), index(rowCount()-1, columnCount()-1));
    return data;
}

QVariant EntryTableModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return {};
    if (role == Qt::DisplayRole) {
        if (index.column() < 0 || index.column() >= columnCount()) return {};
        if (index.row() < 0 || index.column() >= rowCount()) return {};
        switch (index.column()) {
        case 0: return QString::number(index.row()+1);
        case 1: return QString::fromStdString(rowData[index.row()].localIndex.toString());
        case 2: return QString::fromStdString(rowData[index.row()].facing.toString());
        }

    }
    else if (role == Qt::TextAlignmentRole) {
        return QVariant(Qt::AlignVCenter | Qt::AlignHCenter);
    }
    return {};
}

QVariant EntryTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole) {
        if (orientation == Qt::Horizontal) {
            switch (section) {
            case 0:
                return QString("Id");
            case 1:
                return QString("Local index");
            case 2:
                return QString("Facing");
            }
        }
    }
    return {};
}

Qt::ItemFlags EntryTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}
