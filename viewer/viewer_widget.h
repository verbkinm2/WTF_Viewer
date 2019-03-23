#ifndef VIEWER_WIDGET_H
#define VIEWER_WIDGET_H

#include <QWidget>
#include <QFile>
#include <QImage>
#include <QCloseEvent>
#include <QGraphicsScene>
//#include <QGraphicsPathItem>

#include "../eventfilter/fingerslide.h"
#include "frames/frames.h"

namespace Ui {
class Viewer_widget;
}

class Viewer_widget : public QWidget
{
    Q_OBJECT

public:
    explicit Viewer_widget(QWidget *parent = nullptr);
    ~Viewer_widget();

    //получение Qimage из txt файла
    QImage      getImageFromTxtFile     (QString fileName);
    //получение Qimage из clog файла
    QImage      getImageFromClogFile     (QString fileName);
    //установка картинки на виджет из файла
    void        setImageFile            (QString fileName);
    void        setImage                (QImage image);

    QImage      getImage                ();
    QImage      getImageInversion       ();

    //объект сцены
    QGraphicsScene                      scene;

    int         getColumnFromFile       (QString fileName);
    int         getRowFromFile          (QString fileName);

    Frames* getFrames();

signals:


private:
    Ui::Viewer_widget *ui;



    enum fileType {UNDEFINED, TXT, CLOG};
    fileType fType;

//используется для нормального вращения QGraphicsView без сложных(для меня) модификаций QTransform
    double angle = 0;

//объект, который хранит сам рисунок
    QImage      imageOrigin;
//фон
    QImage      imageBackground;

//указатели на item`ы
    QGraphicsPixmapItem* itemBackground = nullptr;
    QGraphicsPixmapItem* itemForeground = nullptr;
    //рамка при выделении
    QGraphicsRectItem*   itemRect       = nullptr;

    //объект для хранения данных для работы с файлами clog
    Frames frames;

    //двумерный массив с данными из файла
    quint16**       arrayOrigin             = nullptr;

    //переменные для хранения кол-ва строк и столбцов файла
    quint16 column  = 0;
    quint16 row     = 0;

    //фильтр событий для сцены и представления
    FingerSlide* eventFilterScene = nullptr;

    //возвращает рисунок из файла или QImage::Format_Invalid
    QImage      createArrayImage        (const QString& fileName);

    //преобразование диапазонов
    double      convert                 (double value,
                                         double From1, double From2,
                                         double To1, double To2);

    //действия при не правильном файле
    void        incorrectFile           ();

    //вывести вместо изображения надпись - "Select file!"
    void        selectFile              ();

    //включени\отключение кнопок на панелях
    void        setEnableButtonPanel    (bool);

    //очистка динамического массива arrayOrigin
    void        clearArrayOrigin        ();

    //включени\отключение кнопок на панели Data
    void        setEnableDataPanelSelection(bool);

    //состояние readonly на панели Data
    void        setReadOnlyDataPanelSelection(bool);

    void        disconnectSelectionSpinBox ();
    void        connectSelectionSpinBox ();

    void        applyClogFilter(QImage& image);
    void        applyClogFilterAdditionalFunction(const ePoint & point, quint16 & max);

private slots:
    //поворот
    void        slotRotate              ();
    //сброс трансформации
    void        slotResetTransform      ();
    //масштаб при нажатии на виджете кнопок + и -
    void        slotScaled              ();
    // масштаб колёсиком мышки
    void        slotScaleWheel          (int);
    //сохранинеие в bmp
    void        slotSaveBMP             ();
    //сохранинеие в txt
    void        slotSaveTXT             ();
    //отрисовка imageOrigin в зависимости от значения checkBox'а Inversion
    void        slotInversionCheckBox   (int state);

    //позиция курсора и значение "пикселя" на сцене
    void        slotViewPosition        (QPointF);
    //позиция выделения
    void        slotViewSelectionPos    (QRect);
    //позиция выделения при перетаскивании мышкой
    void        slotViewSelectionMovePos(QPoint);

    //действия при нажатии кнопки selection_button
    void        slotSelectionFrame      ();
    void        slotCut                 ();

    //действия при окончании выделения(отпускание кнопки мышки)
    void        slotFinishSelection     ();

    // изменение выделения с помощью спинбоксов на панели
    void        slotMoveRectFromKey     ();

    void        slotCreateRectItem(QGraphicsRectItem* item);

//    void        slotClogFilterRangeChange(QObject* obj, quint16 value);

    void        slotApplyClogFilter     ();

//    void        slotSetAllTotInCluster(bool value);

protected:
//    virtual void closeEvent(QCloseEvent *event);
};

#endif // VIEWER_WIDGET_H
