#include "entrytable.h"
#include <QResizeEvent>

EntryTable::EntryTable(std::vector<EntryTableModel::RowItem> *data, QWidget *parent)
    : QTableView(parent) {
    m_model = new EntryTableModel(data, this);

    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setSelectionBehavior(QAbstractItemView::SelectRows);

    setModel(m_model);
}

void EntryTable::resizeEvent(QResizeEvent *event) {
    QTableView::resizeEvent(event);
    int newWidth = event->size().width();
    double maxProp = 0;
    for (int p : proportions) maxProp += p;
    for (int col = 0; col < m_model->columnCount(); col++) {
        setColumnWidth(col, proportions[col] / maxProp * newWidth);
    }
}
