#include "basewindow.h"
#include <QLabel>
#include <QHeaderView>

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
    setupConnections();
}

void BaseWindow::setupLayout() {
    QString axis[3] = { "X", "Y", "Z" };

    outerLayout = new QHBoxLayout(this);
    {
        renderView = new View3D(QVector3D(25, 25, 25), gen->bounds/2, this);
        renderView->appendModel(model);

        setupInteractionPlanes();

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

            entranceTable = new EntryTable(&(gen->openings), this);
            parameterLayout->addWidget(entranceTable);

            //openingList->setHorizontalHeader(new QHeaderView());

            //openingListModel->setHorizontalHeaderLabels(QStringList() << "Local index" << "Facing");

            //entranceTable->appendData({0,1,1}, WallFacing::XNEG);
            //entranceTable->appendData({2,1,1}, WallFacing::XPOS);

            QHBoxLayout *openingLyt = new QHBoxLayout(this);
            openingLyt->setAlignment(Qt::AlignTop);

            parameterLayout->addLayout(openingLyt);
            {
                entryButton = new QPushButton("Add opening", this);
                removeButton = new QPushButton("Remove", this);
                openingLyt->addWidget(entryButton);
                openingLyt->addWidget(removeButton);

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

            exportButton = new QPushButton("Export as", this);
            parameterLayout->addWidget(exportButton);
        }
    }
}

double clamp(double val, double min, double max) {
    if (val < min) val = min;
    if (val > max) val = max;
    return val;
}

void BaseWindow::setupConnections() {
    QObject::connect(renderView->clickHandler, &InteractionHandler::intersectedPlane, [&] (InteractionHandler::Plane plane, QVector3D rayHit) {
        // todo: parse down to specific index and facing (and append to table)
        int x = (plane.size.x() == 0)? 0 : clamp( (rayHit.x() * gen->mazeDims.x) / plane.size.x(), 0, gen->mazeDims.x-1 );
        int y = (plane.size.y() == 0)? 0 : clamp( (rayHit.y() * gen->mazeDims.y) / plane.size.y(), 0, gen->mazeDims.y-1 );
        int z = (plane.size.z() == 0)? 0 : clamp( (rayHit.z() * gen->mazeDims.z) / plane.size.z(), 0, gen->mazeDims.z-1 );
        qDebug() << plane.origin << plane.size << rayHit << x << y << z;
        // this is beautiful how it works
        Quad wall = gen->tileArray[gen->absoluteIndex(y, z, x)]->tBox->getQuad(WallFacing::fromNormal(plane.normal));
        // it's not working on POS direction
        qDebug() << wall.origin << wall.right << wall.up;
        renderView->setSelection(View3D::Selection(wall.origin, wall.right + wall.up, plane.normal));
        renderView->update();
    });

    QObject::connect(gen, &Generator::boundsInvolutarelyChanged, [&] (QVector3D bounds) {
        sizeBoxes[0]->setValue(bounds.x());
        sizeBoxes[1]->setValue(bounds.y());
        sizeBoxes[2]->setValue(bounds.z());

        //recenter view
        QVector3D center = bounds/2;
        renderView->getCamera()->setPosition(center - renderView->getCamera()->getStaticDistance() * renderView->getCamera()->calcCameraFront());

        setupInteractionPlanes();
    });

    QObject::connect(generateButton, &QPushButton::clicked, [&]() {
        gen->generateWalls();
        refreshView();
    });
// TODO: not change global size
    QObject::connect(wallPtg, &QDoubleSpinBox::valueChanged, [&](double val) {
        qDebug() << gen->bounds;
        gen->setWallPtg(val);
        qDebug() << gen->bounds;
        model->mesh->updateVbox();
        qDebug() << gen->bounds;
        refreshView();
    });
    // duplicate openings check?
    QObject::connect(roomBoxes[0], &QSpinBox::valueChanged, [&](int val) {
        auto newDims = gen->mazeDims;
        newDims.setX(val);
        gen->setDims(newDims);

        model->mesh->updateVbox();
        refreshView();
        entranceTable->update();
    });
    QObject::connect(roomBoxes[1], &QSpinBox::valueChanged, [&](int val) {
        auto newDims = gen->mazeDims;
        newDims.setY(val);
        gen->setDims(newDims);

        model->mesh->updateVbox();
        refreshView();
        entranceTable->update();
    });
    QObject::connect(roomBoxes[2], &QSpinBox::valueChanged, [&](int val) {
        auto newDims = gen->mazeDims;
        newDims.setZ(val);
        gen->setDims(newDims);

        model->mesh->updateVbox();
        refreshView();
        entranceTable->update();
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
        qDebug() << newBounds;
        gen->setBounds(newBounds);

        model->mesh->updateVbox();
        refreshView();
        setupInteractionPlanes();
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
        setupInteractionPlanes();
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
        setupInteractionPlanes();
    });
}

BaseWindow::~BaseWindow() = default;
