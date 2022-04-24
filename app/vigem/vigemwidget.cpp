#include "vigemwidget.h"
#include "vigemhintwidget.h"

VigemWidget::VigemWidget(QWidget *parent):
    QLabel(parent)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setPixmap(QPixmap(":/vigem_resources/pzjoystick-icon.png"));

    hint_widget = nullptr;
    vigem_client = nullptr;

    QMenu * menu = new QMenu(this);

    startupAction = menu->addAction(tr("Startup"), this, &VigemWidget::callVigemStartup);
    shutdownAction = menu->addAction(tr("Shutdown"), this, &VigemWidget::callVigemShutdown);
    keysAction = menu->addAction(tr("Show keys"), this, &VigemWidget::callKeys);
    quitAction = menu->addAction(tr("Quit"), qApp, &QApplication::quit);

    startupAction->setEnabled(false);
    shutdownAction->setEnabled(false);

    trayIcon = new QSystemTrayIcon(QIcon(QPixmap(":/vigem_resources/pzjoystick-icon.png")), this);
    trayIcon->show();
    trayIcon->setContextMenu(menu);

    key_mapper = Vigem::VigemKeyMapper::createFromFile(getKeymapFilename());

    openVigem();

    if (startupAction->isEnabled()) {
        // callVigemStartup();
    }
}

VigemWidget::~VigemWidget()
{
    key_mapper.writeToFile(getKeymapFilename());
}

void VigemWidget::openVigem()
{
    QString filename = getLibraryFilename();

    if (!filename.isEmpty()) {
        vigem_client = new Vigem::VigemClient(filename, this);
        vigem_client->setKeyMapper(key_mapper);

        connect(vigem_client, &Vigem::VigemClient::activated, this, &VigemWidget::enableVigem);
        connect(vigem_client, &Vigem::VigemClient::deactivated, this, &VigemWidget::disableVigem);

        if (vigem_client->loadLibrary()) {
            startupAction->setEnabled(true);
        }
    }
}

void VigemWidget::callVigemStartup()
{
    if (!vigem_client->startup()) {
        QMessageBox::critical(this, tr("Error"), tr("vigem_startup failed"));
    }
}

void VigemWidget::callVigemShutdown()
{
    if (!vigem_client->shutdown()) {
        QMessageBox::critical(this, tr("Error"), tr("vigem_shutdown failed"));
    }
}

void VigemWidget::callKeys()
{
    if (!hint_widget) {
        hint_widget = new VigemHintWidget();
    }

    hint_widget->show();
    hint_widget->raise();
    hint_widget->activateWindow();
}

void VigemWidget::callNoKeys()
{
    if (hint_widget) {
        hint_widget->hide();
    }
}

void VigemWidget::enableVigem()
{
    shutdownAction->setEnabled(true);
    startupAction->setEnabled(false);

    moveToBottomRight();
    show();
    activateWindow();
    setFocus(Qt::OtherFocusReason);

    callKeys();
}

void VigemWidget::disableVigem()
{
    shutdownAction->setEnabled(false);
    startupAction->setEnabled(true);
    hide();
}

void VigemWidget::keyPressEvent(QKeyEvent *evt)
{
    if (vigem_client) {
        vigem_client->pressKey((Qt::Key) evt->key());

        evt->accept();
    }
}

void VigemWidget::keyReleaseEvent(QKeyEvent *evt)
{
    if (vigem_client) {
        vigem_client->releaseKey((Qt::Key) evt->key());

        evt->accept();
    }
}

void VigemWidget::closeEvent(QCloseEvent *evt)
{
    evt->ignore();
}

void VigemWidget::focusOutEvent(QFocusEvent *evt)
{
    activateWindow();
    setFocus(Qt::OtherFocusReason);
}

void VigemWidget::moveToBottomRight()
{
    QRect rec = QApplication::primaryScreen()->geometry();
    int h = rec.height();
    int w = rec.width();

    setGeometry(w - pixmap().width(), h - pixmap().height(), pixmap().width(), pixmap().height());
}

QString VigemWidget::getKeymapFilename()
{
    return QDir::homePath() + QDir::separator() + QString(".partyzone") + QDir::separator() + QString("vigem_keys.json");
}

QString VigemWidget::getLibraryFilename()
{
    return QString("vigemclient.dll");
}
