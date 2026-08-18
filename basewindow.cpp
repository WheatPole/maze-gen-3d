#include "basewindow.h"

BaseWindow::BaseWindow(QWidget *parent)
    : QMainWindow(parent)
{
    qDebug() << "Hello!";

    gen = new Generator(QVector3D(0, 0, 0), triplet<int>(4, 1, 4), QVector3D(20, 20, 20), 0.5);
    gen->generateWalls(triplet<int>{0, 0, 0}, WallFacing::XNEG, triplet<int> {3, 0, 3}, WallFacing::ZPOS);

    qDebug() << "Finished generating!";
    //qDebug().noquote() << gen->toString();
    Mesh mesh(gen);
    ObjParser::parse(mesh);

    outerLayout = new QHBoxLayout(this);

}

BaseWindow::~BaseWindow() = default;
