#ifndef DOCKWIDGET_H
#define DOCKWIDGET_H
#include <QDockWidget>
#include "ui_dockwidget.h"
class DockWidget:public QDockWidget
{
    Q_OBJECT
public:
    DockWidget(QWidget *parent = nullptr);
public:
    Ui::DockWidget ui;


signals:

public slots:
};

#endif // DOCKWIDGET_H
