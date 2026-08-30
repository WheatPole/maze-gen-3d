#ifndef ENTRYTABLEMODEL_H
#define ENTRYTABLEMODEL_H

#include <QAbstractTableModel>
#include <QObject>
#include <QWidget>
#include <QMap>
#include "util/triplet.h"
#include "engine/wallfacing.h"

class EntryTableModel : public QAbstractTableModel
{
    Q_OBJECT
private:
    struct RowItem {
        triplet<int> localIndex;
        WallFacing facing;
        RowItem(triplet<int> _localIndex, WallFacing _facing) : localIndex(_localIndex), facing(_facing) {}
    };
public:
    explicit EntryTableModel(QObject *parent = nullptr);
    //QModelIndex	index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    bool insertData(int row, triplet<int> lIndex, WallFacing facing);
    bool appendData(triplet<int> lIndex, WallFacing facing);
    RowItem eraseData(int row);
    inline int rowCount(const QModelIndex &parent = QModelIndex()) const {
        return rowData.size();
    }
    constexpr inline int columnCount(const QModelIndex &parent = QModelIndex()) const {
        return 3;
    }
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

private:

    QList<RowItem> rowData;

signals:
    //void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int>());
};

#endif // ENTRYTABLEMODEL_H
