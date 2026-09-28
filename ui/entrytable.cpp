#include "entrytable.h"
#include <QResizeEvent>

EntryTable::EntryTable(std::vector<EntryTableModel::RowItem> *data, QWidget *parent)
    : QTableView(parent) {
    m_model = new EntryTableModel(data, this);

    setMouseTracking(true);

    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setSelectionBehavior(QAbstractItemView::SelectRows);

    setModel(m_model);

    m_delegate = new HoverDelegate(this);
    setItemDelegate(m_delegate);

    setStyleSheet(
        "QTableView::item:hover {"
        "background: none"
        "}"
        );

    /*QObject::connect(this, &QAbstractItemView::entered, this, [this](const QModelIndex& index) {
        hoverIndex = index;
    });*/
}

void EntryTable::mouseMoveEvent(QMouseEvent *event)
{
    QTableView::mouseMoveEvent(event);

    QModelIndex index = indexAt(event->pos());
    int oldHoverRow = m_hoverRow;
    int oldHoverColumn = m_hoverColumn;
    m_hoverRow = index.row();
    m_hoverColumn = index.column();

    if (selectionBehavior() == SelectRows && oldHoverRow != m_hoverRow) {
        for (int i = 0; i < model()->columnCount(); ++i)
            update(model()->index(m_hoverRow, i));
    }
    if (selectionBehavior() == SelectColumns && oldHoverColumn != m_hoverColumn) {
        for (int i = 0; i < model()->rowCount(); ++i) {
            update(model()->index(i, m_hoverColumn));
            update(model()->index(i, oldHoverColumn));
        }
    }
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
