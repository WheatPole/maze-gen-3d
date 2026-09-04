#ifndef ENTRYTABLE_H
#define ENTRYTABLE_H

#include <QObject>
#include <QTableView>
#include <QWidget>
#include <QHeaderView>
#include "entrytablemodel.h"

class EntryTable : public QTableView
{
    Q_OBJECT
public:
    EntryTable(QWidget *parent = nullptr);
    void resizeEvent(QResizeEvent *event) override;

    inline bool insertData(int row, triplet<int> lIndex, WallFacing facing) {
        return m_model->insertData(row, lIndex, facing);
    }
    inline bool appendData(triplet<int> lIndex, WallFacing facing) {
        return m_model->appendData(lIndex, facing);
    }
    inline EntryTableModel::RowItem eraseData(int row) {
        return m_model->eraseData(row);
    }
private:
    EntryTableModel *m_model;
    double proportions[3] = { 1, 5, 3 };
};

#endif // ENTRYTABLE_H
