#include "vigemhintwidget.h"

VigemHintWidget::VigemHintWidget(QWidget *parent):
    QLabel(parent)
{
    setWindowTitle(tr("Hot keys"));
    setPixmap(QPixmap(":/vigem_resources/keys.jpeg"));
    resize(pixmap().size());
}
