#ifndef ENTRYTABLEMODEL_H
#define ENTRYTABLEMODEL_H

#include <QAbstractTableModel>
#include <QObject>
#include <QWidget>
#include <QMap>
#include "../util/triplet.h"
#include "../engine/generator.h"

class EntryTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    using RowItem = Generator::IndexFacing;
    /*struct RowItem {
        triplet<int> localIndex;
        WallFacing facing;
        RowItem(triplet<int> _localIndex, WallFacing _facing) : localIndex(_localIndex), facing(_facing) {}
    };*/
    explicit EntryTableModel(std::vector<RowItem> *data, QObject *parent = nullptr);
    //QModelIndex	index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    bool insertData(int row, triplet<int> lIndex, WallFacing facing);
    bool appendData(triplet<int> lIndex, WallFacing facing);
    bool removeRows(int row, int count = 1, const QModelIndex &parent = QModelIndex()) override;
    inline int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        return rowData->size();
    }
    inline int columnCount(const QModelIndex &parent = QModelIndex()) const override {
        return 3;
    }
    inline int find(RowItem &data) const {
        for (int i = 0;  i < rowData->size(); i++) {
            if ((*rowData)[i].facing == data.facing && (*rowData)[i].index == data.index) {
                return i;
            }
        }
        return -1;
    }
    bool exists(const RowItem &item) const;
    RowItem getRow(const QModelIndex &ind) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

private:

    std::vector<RowItem> *rowData;

signals:
    //void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int>());
};

#endif // ENTRYTABLEMODEL_H
