#include "entrytable.h"
#include <QResizeEvent>

EntryTable::EntryTable() {
    m_model = new EntryTableModel(this);

    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    //horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    setModel(m_model);
}

/*void EntryTable::resizeEvent(QResizeEvent *event) {
    QTableView::resizeEvent(event);
    qDebug() << m_model->rowCount();
    int newWidth = event->size().width();
    double maxProp = 0;
    for (int p : proportions) maxProp += p;
    for (int col = 0; col < m_model->columnCount(); col++) {
        qDebug() << newWidth << (proportions[col] / maxProp * newWidth) << viewport()->width();
        setColumnWidth(col, proportions[col] / maxProp * newWidth);
    }
}*/

EntryTable::~EntryTable() {
    delete m_model;
}
