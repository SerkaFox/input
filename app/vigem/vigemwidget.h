#ifndef VIGEMWIDGET_H
#define VIGEMWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QMenu>
#include <QSystemTrayIcon>
#include <QApplication>
#include <QMessageBox>
#include <QDesktopWidget>
#include <QKeyEvent>
#include <QFileDialog>
#include <QDir>
#include <QScreen>
#include <QCloseEvent>

#include "vigemclient.h"
#include "vigemhintwidget.h"

class VigemWidget : public QLabel
{
    Q_OBJECT
public:
    explicit VigemWidget(QWidget *parent = nullptr);

    virtual ~VigemWidget();

public slots:
    void openVigem();

    void callVigemStartup();
    void callVigemShutdown();
    void callKeys();
    void callNoKeys();

    void enableVigem();
    void disableVigem();

protected:
    virtual void keyPressEvent(QKeyEvent * evt);
    virtual void keyReleaseEvent(QKeyEvent * evt);

    virtual void focusOutEvent(QFocusEvent * evt);
    virtual void closeEvent(QCloseEvent * evt);

private:
    void moveToBottomRight();
    QString getKeymapFilename();
    QString getLibraryFilename();

    Vigem::VigemKeyMapper key_mapper;

    Vigem::VigemClient * vigem_client;

    QSystemTrayIcon * trayIcon;

    QAction * startupAction;
    QAction * shutdownAction;
    QAction * keysAction;
    QAction * quitAction;

    VigemHintWidget * hint_widget;
};

#endif // VIGEMWIDGET_H
