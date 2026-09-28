#include "hoverdelegate.h"
#include <QPainter>
#include <QTableView>
#include "entrytable.h"

HoverDelegate::HoverDelegate(QObject* parent) : QStyledItemDelegate(parent) {}

void HoverDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    QStyleOptionViewItem opt(option);
    opt.state &= ~QStyle::State_HasFocus;

    EntryTable *view = qobject_cast<EntryTable *>(opt.styleObject);
    QTableView::SelectionBehavior behavior = view->selectionBehavior();
    QModelIndex hoverIndex = view->hoverIndex();

    if (!(option.state & QStyle::State_Selected) && behavior != QTableView::SelectItems) {
        if (behavior == QTableView::SelectRows && hoverIndex.row() == index.row())
            opt.state |= QStyle::State_MouseOver;
        if (behavior == QTableView::SelectColumns && hoverIndex.column() == index.column())
            opt.state |= QStyle::State_MouseOver;
    }
    QStyledItemDelegate::paint(painter, opt, index);
}
