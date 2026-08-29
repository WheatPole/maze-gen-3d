#include "entrytablemodel.h"

EntryTableModel::EntryTableModel(QObject *parent)
    : QAbstractTableModel{parent}
{

}

bool EntryTableModel::insertData(int row, triplet<int> lIndex, WallFacing facing) {
    rowData.insert(rowData.begin() + row, {lIndex, facing});

    emit dataChanged(index(0, 0), index(rows()-1, columns()-1));
    return true;
}

EntryTableModel::RowItem EntryTableModel::eraseData(int row) {
    if (row >= rowData.size()) {
        qFatal() << "Index exceeds row size";
        return rowData[0];
    }
    RowItem data = rowData[row];
    rowData.erase(rowData.begin()+row);

    emit dataChanged(index(0, 0), index(rows()-1, columns()-1));
    return data;
}