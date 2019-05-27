#include "mask_viewer.h"
#include "ui_mask_viewer.h"
#include "checkFile/checkfile.h"

#include <QFileInfo>
#include <QDebug>
#include <QApplication>

Mask_Viewer::Mask_Viewer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Mask_Viewer)
{
    ui->setupUi(this);

    ui->graphicsView->setScene(&scene);

    scene.setObjectName("scene");

    //представление
    ui->graphicsView->viewport()->setObjectName("viewport");

    //фильтр событий для представления
    eventFilterViewPort = new FingerSlide(ui->graphicsView->viewport());
    ui->graphicsView->viewport()->installEventFilter(eventFilterViewPort);
    connect(eventFilterViewPort, SIGNAL(signalWheel(int)), this, SLOT(slotScaleWheel(int)));
}
void Mask_Viewer::selectFile()
{
    scene.clear();

    ui->graphicsView->resetTransform();

    scene.setSceneRect(QRect(0,0, 50,50));
    scene.addText("Select file!");
}
void Mask_Viewer::slotScaleWheel(int value)
{
    if(value > 0)
    {
        ui->graphicsView->scale(1.1, 1.1);
    }
    else if(value < 0)
    {
        ui->graphicsView->scale(1 / 1.1, 1 / 1.1);
    }
}
Mask_Viewer::~Mask_Viewer()
{
    delete ui;

    delete eventFilterViewPort;
}

QImage Mask_Viewer::getImageFromTxtFile(QString fileName)
{
    return createArrayImage(fileName);
}

void Mask_Viewer::setImageFile(QString fileName)
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QFileInfo file(fileName);

    clearArrayOrigin();

    setImage(getImageFromTxtFile(fileName));

    QApplication::restoreOverrideCursor();
}

void Mask_Viewer::setImage(QImage image)
{
    imageOrigin = image;

    if(image.format() != QImage::Format_Invalid)
    {
        ui->graphicsView->resetTransform();
        scene.setSceneRect(image.rect());

        ui->graphicsView->fitInView(image.rect(), Qt::KeepAspectRatio);

        scene.clear();

        itemForeground = scene.addPixmap(QPixmap::fromImage(imageOrigin));
    }
}

int Mask_Viewer::getColumnFromFile(QString fileName)
{
    return ListData(fileName).column;
}

int Mask_Viewer::getRowFromFile(QString fileName)
{
    return ListData(fileName).row;
}

void Mask_Viewer::clearArrayOrigin()
{
    if(arrayOrigin != nullptr){
        for (int i = 0; i < column; ++i)
            delete[] arrayOrigin[i];
        delete[] arrayOrigin;
        arrayOrigin = nullptr;
        column = 0;
        row = 0;

        itemForeground = nullptr;
    }
}

QImage Mask_Viewer::createArrayImage(const QString &fileName)
{
    ListData data(fileName);

    column  = int(data.column);
    row     = int(data.row);

    QImage image(column, row, QImage::Format_ARGB32_Premultiplied);

    if(column == 0 || row == 0)
        return QImage(QSize(0,0),QImage::Format_Invalid);

    //выделяем память для основного массива
    arrayOrigin = new int *[column];
    for (int i = 0; i < column; ++i)
        arrayOrigin[i] = new int[row];

    //Заполнение матрицы данными из файла
    int iterrator = 0;
    int value = 0;
    for (int y = 0; y < row; ++y)
        for (int x = 0; x < column; ++x) {
            value =  data.list.at(iterrator++);
            arrayOrigin[x][y] = value;
        }


    int max = findMaxInArrayOrigin();

    //наполнение объекта QImage
    for (int x = 0; x < column; ++x)
        for (int y = 0; y < row; ++y) {
            int value = int(convert(double(arrayOrigin[x][y]), \
                                            double(0), \
                                            double(max), \
                                            double(0), \
                                            double(255) ) + 0.5);
            QColor color(value, value, value);
            image.setPixelColor(x, y, color);
        }


    return image;
}

int Mask_Viewer::findMaxInArrayOrigin()
{
    int max = 0;
    int value = 0;

    for (int y = 0; y < row; ++y)
        for (int x = 0; x < column; ++x) {
            value = arrayOrigin[x][y];
            if(max < value) max = value;
        }

    return max;
}

double Mask_Viewer::convert(double value, double From1, double From2, double To1, double To2)
{
    if( (int(From1) == int(From2)) ||  (int(To1) == int(To2)))
        return 0.0;

    return (value-From1)/(From2-From1)*(To2-To1)+To1;
}

void Mask_Viewer::slotOpenTXT(QString file)
{
    setImageFile(file);
}
