#include "dockwidget.h"

DockWidget::DockWidget(QWidget* parent):QDockWidget(parent)
{
 ui.setupUi(this);

 this->setFixedWidth(200);
}

