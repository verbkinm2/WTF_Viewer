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
#include "graph/centralwidget.h"

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
    QMenu*              pMenuGraph      = nullptr;

    QList<CentralWidget*> graphWindowList;

    void createMenu();

    QString             currentActiveFile;

protected:
    virtual bool        event(QEvent *event);

private slots:
    void slotSelectFile(const QModelIndex&);

    void slotAuthor();
    void slotPlotGraph();

    void slotExportFile();

    void slotCloseGraphWindow(QObject* obj);

    //при выборе типы данных для диаграммы по оси X, проверяем чтобы не было попытки добавить
    //новый график с одним типом к, существующим графикам с другим типом
    void slotGrapgWindowCheck(QString value);
};

#endif // MAINWINDOW_H
