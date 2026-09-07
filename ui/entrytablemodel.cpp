#include "entrytablemodel.h"


EntryTableModel::EntryTableModel(std::vector<RowItem> *data, QObject *parent)
    : QAbstractTableModel{parent}, rowData(data)
{

}

bool EntryTableModel::insertData(int row, triplet<int> lIndex, WallFacing facing) {
    rowData->insert(rowData->begin() + row, {lIndex, facing});

    emit dataChanged(index(0, 0), index(rowCount()-1, columnCount()-1));
    return true;
}

bool EntryTableModel::appendData(triplet<int> lIndex, WallFacing facing) {
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    rowData->push_back({lIndex, facing});
    endInsertRows();

    //emit dataChanged(index(0, 0), index(rowCount()-1, columnCount()-1));
    return true;
}

Generator::IndexFacing EntryTableModel::eraseData(int row) {
    if (row >= rowCount()) {
        qFatal() << "Index exceeds row size";
        return (*rowData)[0];
    }
    RowItem data = (*rowData)[row];
    beginRemoveRows(QModelIndex(), rowCount(), rowCount());
    rowData->erase(rowData->begin()+row);
    endRemoveRows();

    //emit dataChanged(index(0, 0), index(rowCount()-1, columnCount()-1));
    return data;
}

QVariant EntryTableModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return {};
    if (role == Qt::DisplayRole) {
        if (index.column() < 0 || index.column() >= columnCount()) return {};
        if (index.row() < 0 || index.row() >= rowCount()) return {};

        switch (index.column()) {
        case 0: return QString::number(index.row()+1);
        case 1: return QString::fromStdString((*rowData)[index.row()].index.toString());
        case 2: return QString::fromStdString((*rowData)[index.row()].facing.toString());
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
