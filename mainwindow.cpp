#include "mainwindow.h"
#include "export\export.h"

#include <QApplication>
#include <QHeaderView>
#include <QDebug>

#ifdef Q_OS_Linux
    #define SPLITTER_PATH "/"
#endif
#ifdef Q_OS_WIN
    #define SPLITTER_PATH "\\"
#endif

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    createMenu();

    pSplitter       = new QSplitter(Qt::Horizontal);
    pFSModel        = new QFileSystemModel;
    pTreeView       = new QTreeView;
    pViewerWidget   = new Viewer_widget;
    pEventFilter    = new EventFilter(pTreeView);
    pTreeView->installEventFilter(pEventFilter);


    pFSModel->setRootPath(QDir::rootPath());
    QStringList filter;
    filter << "*.txt" << "*.clog";
    pFSModel->setNameFilters(filter);
    pFSModel->setNameFilterDisables(false);


    pTreeView->setModel(pFSModel);
    pTreeView->header()->hideSection(1);
    pTreeView->header()->hideSection(2);
    pTreeView->header()->hideSection(3);


    pSplitter->addWidget(pTreeView);
    pSplitter->addWidget(pViewerWidget);

    this->setCentralWidget(pSplitter);

    connect(pTreeView, SIGNAL(clicked(const QModelIndex&)), this, SLOT(slotSelectFile(const QModelIndex&)));
    connect(pTreeView, SIGNAL(activated(const QModelIndex&)), this, SLOT(slotSelectFile(const QModelIndex&)));

    this->setWindowIcon(QIcon(":/atom"));

}
void MainWindow::createMenu()
{
    pMenuFile = new QMenu("File");
    pMenuFile->addAction(QIcon(":/save_as"), "Export files", this, SLOT(slotExportFile()));

    pMenuFile->addSeparator();

    pMenuFile->addAction(QIcon(":/exit"), "Exit", QApplication::instance(), SLOT(quit()));

    pMenuAbout= new QMenu("About");
    pMenuAbout->addAction(QIcon(":/author"),"Author", this, SLOT(slotAuthor()));
    pMenuAbout->addAction(QIcon(":/qt_logo"), "About Qt", QApplication::instance(), SLOT(aboutQt()));

    this->menuBar()->addMenu(pMenuFile);
    this->menuBar()->addMenu(pMenuAbout);
}
void MainWindow::slotExportFile()
{
    QFileInfo file(pFSModel->filePath(pTreeView->currentIndex()));
    QString path;
    if(file.isDir())
        path = file.absoluteFilePath();
    else if(file.isFile())
        path = file.absolutePath();

    Export exportWindow(path, this);
    if(exportWindow.exec() == QDialog::Accepted)
    {
        QApplication::setOverrideCursor(Qt::WaitCursor);

        int correct = 0;
        int error   = 0;
        QStringList listFiles = exportWindow.getFileNames();

        foreach (QString fileName, listFiles) {
            QImage image(pViewerWidget->getImageFromTxtFile(fileName));
            if(image.format() != QImage::Format_Invalid)
            {
                QFileInfo fileInfo(fileName);
                QString fullName = exportWindow.getPath() + SPLITTER_PATH + fileInfo.baseName();
                switch (exportWindow.getOption())
                {
                case (Export::BW) :
                    if(image.save(fullName+".bmp", "BMP"))
                        correct++;
                    else
                        error++;

                    break;
                case (Export::WB) :
                    image.invertPixels();
                    if(image.save(fullName+"_INVERSION.bmp", "BMP"))
                        correct++;
                    else
                        error++;

                    break;
                case (Export::BW_AND_WB) :
                    if(image.save(fullName+".bmp", "BMP"))
                        correct++;

                    image.invertPixels();

                    if(image.save(fullName+"_INVERSION.bmp", "BMP"))
                        correct++;

                    break;
                }
            }
            else
                error++;
        }
        QApplication::restoreOverrideCursor();QApplication::restoreOverrideCursor();

        QMessageBox::information(this, "Export", "Export " + QString::number(correct) + " file(s)<br>"
                                                                                        "Error export " + QString::number(error) + " file(s)");
    }
}
void MainWindow::slotAuthor()
{
    QString text = "<h3>WTF_Viewer 0.7.1 </h3> <br>"
                   "WTF(What flies?)<br>"
                   "Author: Verbkin Mikhail <br>"
                   "Email: <a href=\"mailto:verbkinm@yandex.ru\" >verbkinm@yandex.ru</a> <br>"
                   "Source code: <a href='https://github.com/verbkinm/wtf_viewer'>github.com</a> <br><br>"
                   "The program for my dear friend! =))";

    QMessageBox::about(this, "Author", text);
}
void MainWindow::slotSelectFile(const QModelIndex& index)
{
    this->statusBar()->showMessage(pFSModel->filePath(index));
    pViewerWidget->setImageFile(pFSModel->filePath(index));
    if(pTreeView->isExpanded(index))
        pTreeView->collapse(index);
    else
        pTreeView->expand(index);
}

bool MainWindow::event(QEvent *event)
{
//    qDebug() << event->type();
    return QWidget::event(event);
}
MainWindow::~MainWindow()
{
    delete pViewerWidget;
    delete pFSModel;
    delete pEventFilter;
    delete pTreeView;
    delete pSplitter;

    delete pMenuFile;
    delete pMenuAbout;
}
