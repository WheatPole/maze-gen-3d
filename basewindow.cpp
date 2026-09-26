#include "basewindow.h"
#include <QLabel>
#include <QHeaderView>
#include <QFileDialog>
#include <fstream>
#include "parsing/objparser.h"
#include <QMessageBox>

QFrame* separatorLine() {
    QFrame* separatorLine = new QFrame();
    separatorLine->setFrameShape(QFrame::HLine);
    separatorLine->setFrameShadow(QFrame::Sunken);
    return separatorLine;
}

BaseWindow::BaseWindow(QWidget *parent)
    : QWidget(parent)
{
    this->setMinimumSize(600, 600);

    gen = new Generator(QVector3D(0, 0, 0), triplet<int>(3,3,3), QVector3D(10, 10, 10), 0.5);
    gen->addOpening({0, 1, 1}, WallFacing::XNEG);
    gen->addOpening({2,1,1}, WallFacing::XPOS);
    gen->generateWalls();

    qDebug() << "Finished generating!";
    //qDebug().noquote() << gen->toString();
    std::unique_ptr<Mesh> mesh = std::make_unique<Mesh>(gen);
    model = new OutlinedModel(std::move(mesh), nullptr);
    setupLayout();
    setupRendererConnections();
    setupLayoutConnections();
}

void BaseWindow::setupLayout() {
    QString axis[3] = { "X", "Y", "Z" };

    outerLayout = new QHBoxLayout(this);
    {
        renderView = new View3D(QVector3D(25, 25, 25), gen->bounds/2, this);
        renderView->appendModel(model);

        updateInteractionPlanes();

        //renderView->setFixedSize(100, 100);
        outerLayout->addWidget(renderView, 3);

        QFrame *parameterPanel = new QFrame(this);
        outerLayout->addWidget(parameterPanel, 1);
        parameterLayout = new QVBoxLayout(parameterPanel);
        parameterLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);
        {

            QLabel *roomLbl = new QLabel("Rooms per axis", this);
            parameterLayout->addWidget(roomLbl);

            QHBoxLayout *roomLyt = new QHBoxLayout(this);
            roomLyt->setAlignment(Qt::AlignLeft | Qt::AlignTop);

            parameterLayout->addLayout(roomLyt);
            {
                for (int i = 0; i < 3; i++) {
                    roomLyt->addWidget(new QLabel(axis[i], this));
                    roomBoxes[i] = new QSpinBox(this);
                    roomBoxes[i]->setMinimum(1);
                    roomBoxes[i]->setValue(3);
                    roomBoxes[i]->setMaximum(100);
                    roomBoxes[i]->setAlignment(Qt::AlignCenter);
                    roomLyt->addWidget(roomBoxes[i]);
                    roomLyt->setStretchFactor(roomBoxes[i], 1);
                }
            }

            parameterLayout->addSpacing(5);
            parameterLayout->addWidget(separatorLine());

            QHBoxLayout *wallPtgLyt = new QHBoxLayout(this);
            wallPtgLyt->setAlignment(Qt::AlignTop);
            parameterLayout->addLayout(wallPtgLyt);
            {
                QLabel *wallPtgLbl = new QLabel("Relative wall width", this);
                wallPtgLyt->addWidget(wallPtgLbl);

                wallPtg = new QDoubleSpinBox(this);
                wallPtg->setValue(0.5);
                wallPtg->setMinimum(0.01);
                wallPtg->setMaximum(5);
                wallPtg->setSingleStep(0.05);
                wallPtg->setAlignment(Qt::AlignRight);
                wallPtgLyt->addWidget(wallPtg);
            }

            parameterLayout->addSpacing(5);
            parameterLayout->addWidget(separatorLine());

            QLabel *openingsLbl = new QLabel("Openings", this);
            parameterLayout->addWidget(openingsLbl);

            entranceTable = new EntryTable(gen->openings, this);
            parameterLayout->addWidget(entranceTable);

            //openingList->setHorizontalHeader(new QHeaderView());

            //openingListModel->setHorizontalHeaderLabels(QStringList() << "Local index" << "Facing");

            //entranceTable->appendData({0,1,1}, WallFacing::XNEG);
            //entranceTable->appendData({2,1,1}, WallFacing::XPOS);

            QHBoxLayout *openingLyt = new QHBoxLayout(this);
            openingLyt->setAlignment(Qt::AlignTop);

            parameterLayout->addLayout(openingLyt);
            {
                entryAddButton = new QPushButton("Add opening", this);
                entryRemoveButton = new QPushButton("Remove", this);
                openingLyt->addWidget(entryAddButton);
                openingLyt->addWidget(entryRemoveButton);
                updateEntryButtons();

            }

            generateButton = new QPushButton("Generate");
            generateButton->setFixedHeight(40);
            parameterLayout->addWidget(generateButton);

            parameterLayout->addSpacing(5);
            parameterLayout->addWidget(separatorLine());

            QLabel *sizeLbl = new QLabel("Mesh size", this);
            parameterLayout->addWidget(sizeLbl);

            QHBoxLayout *sizeLyt = new QHBoxLayout(this);
            sizeLyt->setAlignment(Qt::AlignLeft | Qt::AlignTop);

            parameterLayout->addLayout(sizeLyt);
            {
                linkedSize = new QCheckBox(this);
                sizeLyt->addWidget(linkedSize);
                sizeLyt->addSpacing(20);

                for (int i = 0; i < 3; i++) {
                    sizeLyt->addWidget(new QLabel(axis[i], this));
                    sizeBoxes[i] = new QDoubleSpinBox(this);
                    sizeBoxes[i]->setMinimum(1);
                    sizeBoxes[i]->setValue(10);
                    sizeBoxes[i]->setMaximum(1000);
                    sizeBoxes[i]->setAlignment(Qt::AlignCenter);
                    sizeLyt->addWidget(sizeBoxes[i]);
                    sizeLyt->setStretchFactor(sizeBoxes[i], 1);
                }
            }

            exportButton = new QPushButton("Export as obj", this);
            parameterLayout->addWidget(exportButton);
        }
    }
}

double clamp(double val, double min, double max) {
    if (val < min) val = min;
    if (val > max) val = max;
    return val;
}

void BaseWindow::setupRendererConnections() {
    QObject::connect(renderView->clickHandler, &InteractionHandler::intersectedPlane, [&] (InteractionHandler::Plane plane, QVector3D rayHit) {
        // Ray hit without the extra wall edges (each outer rect on the mesh would be the same)
        QVector3D stabilizedHit = rayHit - gen->tileSize * gen->wallPtg / 2;
        QVector3D extendedTile = gen->tileSize * (1 + gen->wallPtg);
        QVector3D coordsF = stabilizedHit / extendedTile;
        int x = clamp( coordsF.x(), 0, gen->mazeDims.x-1 );
        int y = clamp( coordsF.y(), 0, gen->mazeDims.y-1 );
        int z = clamp( coordsF.z(), 0, gen->mazeDims.z-1 );

        currentSelection = {{x, y, z}, WallFacing::fromNormal(plane.normal)};

        // this is beautiful how it works
        Quad wall = gen->tileArray[gen->absoluteIndex(y, z, x)]->tBox->getQuad(WallFacing::fromNormal(plane.normal));

        renderView->setSelection(View3D::Selection(wall.origin, wall.right + wall.up, plane.normal));
        renderView->update();

        // "Add" button update
        updateEntryButtons();
        // Select the appropriate index
        int index = entranceTable->model()->find(currentSelection);
        if (index != -1)
            entranceTable->selectionModel()->setCurrentIndex(entranceTable->model()->index(index, 0), QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
        else if (entranceTable->size() > 0)
            entranceTable->selectionModel()->setCurrentIndex(entranceTable->model()->index(0, 0),  QItemSelectionModel::Clear);
    });

    QObject::connect(renderView->clickHandler, &InteractionHandler::noIntersections, [&] () {
        currentSelection = { {-1, -1, -1}, WallFacing::XPOS};;

        renderView->setSelection(View3D::NoSelection);
        renderView->update();

        // "Add" button update
        updateEntryButtons();
        // Select the appropriate index (none)
        if (entranceTable->size() > 0)
            entranceTable->selectionModel()->setCurrentIndex(entranceTable->model()->index(0, 0),  QItemSelectionModel::Clear);
    });

    QObject::connect(gen, &Generator::boundsChanged, [&] (QVector3D bounds) {
        sizeBoxes[0]->setValue(bounds.x());
        sizeBoxes[1]->setValue(bounds.y());
        sizeBoxes[2]->setValue(bounds.z());

        //recenter view
        QVector3D center = bounds/2;
        renderView->getCamera()->setPosition(center - renderView->getCamera()->getStaticDistance() * renderView->getCamera()->calcCameraFront());

        // recolour selection (if selected)
        if (currentSelection.index.x != -1) {
            Quad wall = gen->tileArray[gen->absoluteIndex(
                                           currentSelection.index.y,
                                           currentSelection.index.z,
                                           currentSelection.index.x)]->tBox->getQuad(currentSelection.facing);

            renderView->setSelection(View3D::Selection(wall.origin, wall.right + wall.up, currentSelection.facing.getNormal()));
        }
        updateInteractionPlanes();
    });
}

void BaseWindow::setupLayoutConnections() {

    QObject::connect(generateButton, &QPushButton::clicked, [&]() {
        gen->generateWalls();
        refreshView();
    });
    QObject::connect(wallPtg, &QDoubleSpinBox::valueChanged, [&](double val) {
        gen->setWallPtg(val);
        model->mesh->updateVbox();
        refreshView();
    });
    QObject::connect(entryAddButton, &QPushButton::clicked, [&](bool checked) {
        // assuming currentSelection is specified
        if (entranceTable->exists(currentSelection)) return;
        entranceTable->appendData(currentSelection.index, currentSelection.facing);
        entranceTable->selectionModel()->setCurrentIndex(entranceTable->model()->index(entranceTable->size()-1, 0), QItemSelectionModel::Select | QItemSelectionModel::Rows);

        gen->applyOpenings();
        model->mesh->updateVbox();
        refreshView();
    });

    QObject::connect(entryRemoveButton, &QPushButton::clicked, [&](bool checked) {
        // assuming currentSelection is specified
        if (entranceTable->selectionModel()->selectedRows().size() == 0) return;

        QModelIndexList rowList = entranceTable->selectionModel()->selectedRows();

        std::sort(rowList.begin(), rowList.end(), [](const QModelIndex& a, const QModelIndex& b) {
            return a.row() > b.row();
        });

        for (QModelIndex &index : rowList) {
            qDebug() << index;
            // directly editing the walls
            auto data = entranceTable->model()->getRow(index);
            gen->tileArray[gen->absoluteIndex(data.index)]->wall.add(data.facing);

            entranceTable->removeRows(index.row());
        }

        // unnecessary
        //gen->applyOpenings();
        model->mesh->updateVbox();
        refreshView();
    });

    // duplicate openings check?
    QObject::connect(roomBoxes[0], &QSpinBox::valueChanged, [&](int val) {
        auto newDims = gen->mazeDims;
        newDims.setX(val);
        gen->setDims(newDims);

        entranceTable->update();
        model->mesh->updateVbox();
        refreshView();
    });
    QObject::connect(roomBoxes[1], &QSpinBox::valueChanged, [&](int val) {
        auto newDims = gen->mazeDims;
        newDims.setY(val);
        gen->setDims(newDims);

        entranceTable->update();
        model->mesh->updateVbox();
        refreshView();
    });
    QObject::connect(roomBoxes[2], &QSpinBox::valueChanged, [&](int val) {
        auto newDims = gen->mazeDims;
        newDims.setZ(val);
        gen->setDims(newDims);

        entranceTable->update();
        model->mesh->updateVbox();
        refreshView();
    });

    QObject::connect(sizeBoxes[0], &QDoubleSpinBox::valueChanged, [&](double val) {
        auto newBounds = gen->bounds;
        // Cyclic calling, don't do anything
        if (val == newBounds.x()) {
            return;
        }
        if (linkedSize->isChecked()) {
            double rel = val/gen->bounds.x();
            newBounds *= rel;
        }
        else {
            newBounds.setX(val);
        }
        qDebug() << newBounds << gen->bounds << gen->tileSize;
        gen->setBounds(newBounds);
        qDebug() << gen->bounds << gen->tileSize;

        model->mesh->updateVbox();
        refreshView();
        updateInteractionPlanes();
    });
    QObject::connect(sizeBoxes[1], &QDoubleSpinBox::valueChanged, [&](double val) {
        auto newBounds = gen->bounds;
        // Cyclic calling, don't do anything
        if (val == newBounds.y()) {
            return;
        }
        if (linkedSize->isChecked()) {
            double rel = val/gen->bounds.y();
            newBounds *= rel;
        }
        else {
            newBounds.setY(val);
        }

        gen->setBounds(newBounds);

        model->mesh->updateVbox();
        refreshView();
        updateInteractionPlanes();
    });
    QObject::connect(sizeBoxes[2], &QDoubleSpinBox::valueChanged, [&](double val) {
        auto newBounds = gen->bounds;
        // Cyclic calling, don't do anything
        if (val == newBounds.z()) {
            return;
        }
        if (linkedSize->isChecked()) {
            double rel = val/gen->bounds.z();
            newBounds *= rel;
        }
        else {
            newBounds.setZ(val);
        }

        gen->setBounds(newBounds);

        model->mesh->updateVbox();
        refreshView();
        updateInteractionPlanes();
    });


    // Export button
    QObject::connect(exportButton, &QPushButton::clicked, [&](bool checked) {
        //QFileDialog::getSaveFileName(this, "Save as", QDir::currentPath(), tr("Obj file (*.obj)"))
        QFileDialog saveDialog(this, "Save as", QDir::currentPath());
        saveDialog.setAcceptMode(QFileDialog::AcceptSave);
        QStringList filters;
        filters << "Text Files (*.obj)" << "All Files (*.*)";
        saveDialog.setNameFilters(filters);

        saveDialog.setDefaultSuffix("obj");


        if (saveDialog.exec() == QDialog::Accepted) {
            QString fileName = saveDialog.selectedFiles().first();
            std::fstream stream(fileName.toStdString(), std::ios::out | std::ios::trunc);
            if (!stream.is_open()) {
                QMessageBox::critical(this, "Error", "Couldn't open file " + fileName);
                return;
            }
            stream << ObjParser::constructObjString(*renderView->modelList[0]->mesh);
            stream.close();

            QMessageBox::information(this, "Info", "Successfully exported to " + fileName);
        }

    });
}

BaseWindow::~BaseWindow() = default;
