#ifndef BASEWINDOW_H
#define BASEWINDOW_H

#include <QMainWindow>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "engine/generator.h"
#include "parsing/mesh.h"
#include "parsing/objparser.h"

class BaseWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit BaseWindow(QWidget *parent = nullptr);
    ~BaseWindow() override;

    Generator *gen;
    QHBoxLayout *outerLayout;
    QVBoxLayout *parameterLayout;
};
#endif // BASEWINDOW_H
