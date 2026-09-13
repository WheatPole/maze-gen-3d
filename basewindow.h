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
    void setupConnections();

    inline void refreshView() {
        model->mesh->refresh();
        model->refreshData();
        renderView->updateBuffers();
    }

    // sets (updates) interaction planes for click registring
    inline void setupInteractionPlanes() {
        renderView->clickHandler->clearPlanes();
        QVector3D origin = gen->origin, x = QVector3D(gen->bounds.x(), 0, 0), y = QVector3D(0, gen->bounds.y(), 0), z = QVector3D(0, 0, gen->bounds.z());
        renderView->clickHandler->addPlane(origin, x + y, WallFacing::ZNEG.getNormal());
        renderView->clickHandler->addPlane(origin + z, y + z, WallFacing::ZPOS.getNormal());
        renderView->clickHandler->addPlane(origin, z + y, WallFacing::XNEG.getNormal());
        renderView->clickHandler->addPlane(origin + x, z + y, WallFacing::XPOS.getNormal());
        renderView->clickHandler->addPlane(origin, z + x, WallFacing::YNEG.getNormal());
        renderView->clickHandler->addPlane(origin + y, x + z, WallFacing::YPOS.getNormal());
    }
};
#endif // BASEWINDOW_H
