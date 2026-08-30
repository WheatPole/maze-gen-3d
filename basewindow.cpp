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
    gen->generateWalls(triplet<int>{0, 0, 0}, WallFacing::XNEG, triplet<int> {2,2,2}, WallFacing::ZPOS);

    qDebug() << "Finished generating!";
    //qDebug().noquote() << gen->toString();
    Mesh mesh(gen);
    ObjParser::parse(mesh);
    Model *model = new Model(&mesh, nullptr);

    QString axis[3] = { "X", "Y", "Z" };

    outerLayout = new QHBoxLayout(this);
    {
        renderView = new View3D(this);
        renderView->appendModel(model);
        //renderView->setFixedSize(100, 100);
        outerLayout->addWidget(renderView, 3);

        QFrame *parameterPanel = new QFrame(this);
        outerLayout->addWidget(parameterPanel, 1);

        parameterLayout = new QVBoxLayout(parameterPanel);
        parameterLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);
        {
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
            parameterLayout->addSpacing(10);
            parameterLayout->addWidget(separatorLine());

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

            parameterLayout->addSpacing(10);
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
                wallPtg->setAlignment(Qt::AlignRight);
                wallPtgLyt->addWidget(wallPtg);
            }

            parameterLayout->addSpacing(10);
            parameterLayout->addWidget(separatorLine());

            QLabel *openingsLbl = new QLabel("Openings", this);
            parameterLayout->addWidget(openingsLbl);

            entranceTable = new EntryTable();
            parameterLayout->addWidget(entranceTable);

            //openingList->setHorizontalHeader(new QHeaderView());

            //openingListModel->setHorizontalHeaderLabels(QStringList() << "Local index" << "Facing");

            entranceTable->model()->appendData({0,0,0}, WallFacing::XNEG);
            entranceTable->model()->appendData({2,2,2}, WallFacing::ZPOS);

            QHBoxLayout *openingLyt = new QHBoxLayout(this);
            openingLyt->setAlignment(Qt::AlignTop);

        }
    }
}

BaseWindow::~BaseWindow() = default;
