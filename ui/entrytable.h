#ifndef ENTRYTABLE_H
#define ENTRYTABLE_H

#include <QObject>
#include <QTableView>
#include <QWidget>
#include <QHeaderView>
#include "entrytablemodel.h"
#include "hoverdelegate.h"

class EntryTable : public QTableView
{
    Q_OBJECT
public:
    EntryTable(std::vector<EntryTableModel::RowItem> *data, QWidget *parent = nullptr);
    void resizeEvent(QResizeEvent *event) override;

    inline bool insertData(int row, triplet<int> lIndex, WallFacing facing) {
        return m_model->insertData(row, lIndex, facing);
    }
    inline bool exists(const EntryTableModel::RowItem &data) const {
        return m_model->exists(data);
    }
    inline bool appendData(triplet<int> lIndex, WallFacing facing) {
        return m_model->appendData(lIndex, facing);
    }
    inline bool removeRows(int row) {
        return m_model->removeRows(row);
    }
    inline int size() const {
        return m_model->rowCount();
    }
    inline EntryTableModel* model() {
        return m_model;
    }
    inline QModelIndex hoverIndex() const { return m_model->index(m_hoverRow, m_hoverColumn); }
    void mouseMoveEvent(QMouseEvent *event) override;
private:
    int m_hoverRow = -1, m_hoverColumn = -1;
    EntryTableModel *m_model;
    HoverDelegate *m_delegate;
    double proportions[3] = { 1, 5, 3 };
};

#endif // ENTRYTABLE_H
