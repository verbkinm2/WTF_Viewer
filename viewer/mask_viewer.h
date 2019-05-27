#ifndef MASK_VIEWER_H
#define MASK_VIEWER_H

#include <QWidget>
#include <QFile>
#include <QImage>
#include <QCloseEvent>
#include <QGraphicsScene>

#include "../eventfilter/fingerslide.h"

namespace Ui {
class Mask_Viewer;
}

class Mask_Viewer : public QWidget
{
    Q_OBJECT

public:
    explicit Mask_Viewer(QWidget *parent = nullptr);
    ~Mask_Viewer();

    //получение Qimage из txt файла
    QImage      getImageFromTxtFile     (QString fileName);
    //установка картинки на виджет из файла
    void        setImageFile            (QString fileName);
    void        setImage                (QImage image);

//    QImage      getImage                ();
//    QImage      getImageInversion       ();

    //объект сцены
    QGraphicsScene                      scene;

    int         getColumnFromFile       (QString fileName);
    int         getRowFromFile          (QString fileName);


private:
    Ui::Mask_Viewer *ui;

    //объект, который хранит сам рисунок
    QImage      imageOrigin;

    //указатели на item`ы
//    QGraphicsPixmapItem* itemBackground = nullptr;
    QGraphicsPixmapItem* itemForeground = nullptr;

    //двумерный массив с данными из файла
    int**       arrayOrigin             = nullptr;

    //переменные для хранения кол-ва строк и столбцов файла
    int column  = 0;
    int row     = 0;

    //фильтр событий для сцены и представления
    FingerSlide* eventFilterViewPort = nullptr;

    //очистка динамического массива arrayOrigin
    void        clearArrayOrigin        ();

    //возвращает рисунок из файла или QImage::Format_Invalid
    QImage      createArrayImage        (const QString& fileName);
    int         findMaxInArrayOrigin    ();

    //преобразование диапазонов
    double      convert                 (double value,
                                         double From1, double From2,
                                         double To1, double To2);

    void        selectFile();

public slots:
    void        slotOpenTXT(QString);

private slots:
    void        slotScaleWheel(int value);
};

#endif // MASK_VIEWER_H
