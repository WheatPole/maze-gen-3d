#ifndef BASEWINDOW_H
#define BASEWINDOW_H

#include <QMainWindow>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QListView>
#include <QTableView>
#include <QStandardItemModel>

#include "entrytable.h"
#include "engine/generator.h"
#include "parsing/mesh.h"
#include "parsing/objparser.h"
#include "view3d.h"

class BaseWindow : public QWidget
{
    Q_OBJECT

public:
    explicit BaseWindow(QWidget *parent = nullptr);
    ~BaseWindow() override;

    Generator *gen;
    QHBoxLayout *outerLayout;
    QVBoxLayout *parameterLayout;

    QCheckBox *linkedSize;
    QDoubleSpinBox* sizeBoxes[3];
    QSpinBox* roomBoxes[3];
    QDoubleSpinBox* wallPtg;
    EntryTable *entranceTable;

    View3D *renderView;
};
#endif // BASEWINDOW_H
