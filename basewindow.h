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

    QPushButton *entryAddButton, *entryRemoveButton, *generateButton, *exportButton;
    View3D *renderView;

    EntryTableModel::RowItem currentSelection = { {-1, -1, -1}, WallFacing::XPOS};

    void setupLayout();
    void setupLayoutConnections();
    void setupRendererConnections();

    inline void updateEntryButtons() {
        if (currentSelection.index.x != -1) {
            entryAddButton->setDisabled(false);
            entryAddButton->setText("Add to list");
        }
        else {
            entryAddButton->setDisabled(true);
            entryAddButton->setText("Select a quad");
        }

        // if list selected... (for remove)
    }

    inline void refreshView() {
        model->mesh->refresh();
        model->refreshData();
        renderView->updateBuffers();
    }

    // sets (updates) interaction planes for click registring
    inline void updateInteractionPlanes() {
        renderView->clickHandler->clearPlanes();
        QVector3D origin = gen->origin, x = QVector3D(gen->bounds.x(), 0, 0), y = QVector3D(0, gen->bounds.y(), 0), z = QVector3D(0, 0, gen->bounds.z());
        renderView->clickHandler->addPlane(origin, z + y, WallFacing::XNEG.getNormal());
        renderView->clickHandler->addPlane(origin + x, z + y, WallFacing::XPOS.getNormal());
        renderView->clickHandler->addPlane(origin, z + x, WallFacing::YNEG.getNormal());
        renderView->clickHandler->addPlane(origin + y, z + x, WallFacing::YPOS.getNormal());
        renderView->clickHandler->addPlane(origin, x + y, WallFacing::ZNEG.getNormal());
        renderView->clickHandler->addPlane(origin + z, x + y, WallFacing::ZPOS.getNormal());
    }
};
#endif // BASEWINDOW_H
