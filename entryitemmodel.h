#ifndef ENTRYITEMMODEL_H
#define ENTRYITEMMODEL_H

#include <QAbstractTableModel>
#include <QObject>
#include <QWidget>
#include <QMap>
#include "util/triplet.h"
#include "engine/wallfacing.h"

class EntryItemModel : public QAbstractTableModel
{
    Q_OBJECT
private:
    struct RowItem {
        triplet<int> localIndex;
        WallFacing facing;
        RowItem(triplet<int> _localIndex, WallFacing _facing) : localIndex(_localIndex), facing(_facing) {}
    };
public:
    explicit EntryItemModel(QObject *parent = nullptr);
    QModelIndex	index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    bool insertData(int row, triplet<int> index, WallFacing facing);
    RowItem eraseData(int row);


    inline int rows() const noexcept {
        return rowData.size();
    }
    constexpr inline int columns() const {
        return 2;
    }
private:

    QList<RowItem> rowData;

signals:
    //void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int>());
};

#endif // ENTRYITEMMODEL_H
