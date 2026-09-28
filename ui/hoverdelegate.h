#ifndef HOVERDELEGATE_H
#define HOVERDELEGATE_H

#include <QObject>
#include <QStyledItemDelegate>

class HoverDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit HoverDelegate(QObject* parent = nullptr);
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

    int hoveredRow = -1;
};

#endif // HOVERDELEGATE_H
