#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSplitter>
#include <QFileSystemModel>
#include <QTreeView>
#include <QStatusBar>
#include <QMenuBar>
#include <QMenu>
#include <QEvent>
#include <QTreeWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>

#include "viewer\viewer_widget.h"
#include "eventfilter\eventfilter.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QSplitter*          pSplitter       = nullptr;
    QFileSystemModel*   pFSModel        = nullptr;
    QTreeView*          pTreeView       = nullptr;

    Viewer_widget*      pViewerWidget   = nullptr;
    EventFilter*        pEventFilter    = nullptr;

private:

    QMenu*              pMenuFile       = nullptr;
    QMenu*              pMenuAbout      = nullptr;

    void createMenu();

protected:
    virtual bool        event(QEvent *event);

private slots:
    void slotSelectFile(const QModelIndex&);

    void slotAuthor();

    void slotExportFile();
};

#endif // MAINWINDOW_H
