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
    EntryTable();
    //void resizeEvent(QResizeEvent *event) override;
    inline EntryTableModel* model() const {
        return m_model;
    }
    ~EntryTable();
private:
    EntryTableModel *m_model;
    double proportions[3] = { 1, 5, 3 };
};

#endif // ENTRYTABLE_H
