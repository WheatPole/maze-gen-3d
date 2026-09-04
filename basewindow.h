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
#include <QPushButton>

#include "ui/entrytable.h"
#include "engine/generator.h"
#include "parsing/mesh.h"
#include "parsing/objparser.h"
#include "rendering/view3d.h"
#include "rendering/outlinedmodel.h"

class BaseWindow : public QWidget
{
    Q_OBJECT

public:
    explicit BaseWindow(QWidget *parent = nullptr);
    ~BaseWindow() override;

private:
    Generator *gen;
    OutlinedModel *model;
    QHBoxLayout *outerLayout;
    QVBoxLayout *parameterLayout;

    QCheckBox *linkedSize;
    QDoubleSpinBox* sizeBoxes[3];
    QSpinBox* roomBoxes[3];
    QDoubleSpinBox* wallPtg;
    EntryTable *entranceTable;

    QPushButton *entryButton, *removeButton, *generateButton, *exportButton;
    View3D *renderView;

    void setupLayout();
};
#endif // BASEWINDOW_H
