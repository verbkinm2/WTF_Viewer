#ifndef VIEWER_WIDGET_H
#define VIEWER_WIDGET_H

#include <QWidget>
#include <QFile>
#include <QImage>
#include <QCloseEvent>
#include <QGraphicsScene>
#include <QSettings>

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

    const static int CLOG_SIZE = 256;

    const static int X_HOT = 2;
    const static int Y_HOT = 23;

    explicit Viewer_widget(QSettings& setting, QWidget *parent = nullptr);
    ~Viewer_widget();

    QSettings*  pSettings = nullptr;
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
    enum tabName  {PIX_PROPERTY_TAB, CLOG_FILTER_TAB};
    //текущий тип файла
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
    int**       arrayOrigin             = nullptr;
    int**       arrayMask               = nullptr;

    //переменные для хранения кол-ва строк и столбцов файла
    int column  = 0;
    int row     = 0;

    //фильтр событий для сцены и представления
    FingerSlide* eventFilterScene = nullptr;

    //возвращает рисунок из файла или QImage::Format_Invalid
    QImage      createArrayImage        (const QString& fileName);
    int         findMaxInArrayOrigin    ();

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
    void        applyClogFilterAdditionalFunction(const ePoint & point);

    void        imageSettingsForArray();
    void        imageSettingsForImage(QImage& image);
    //создаёт рамку согласно настройкам
    void        createFrameInArray();
    //маскируем выбранные пиксели
    void        createMaskInArray();


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
    void        slotDrawPoint           (QPointF point);

    //действия при нажатии кнопки на панели инструментов
    void        slotSelectionFrame      (bool value);
    void        slotPen                 (bool value);
    void        slotCut                 ();

    //действия при окончании выделения(отпускание кнопки мышки)
    void        slotFinishSelection     ();

    // изменение выделения с помощью спинбоксов на панели
    void        slotMoveRectFromKey     ();

    void        slotCreateRectItem(QGraphicsRectItem* item);

//    void        slotClogFilterRangeChange(QObject* obj, int value);

    void        slotApplyClogFilter     ();

//    void        slotSetAllTotInCluster(bool value);

    void        slotRepaint             ();

protected:
//    virtual void closeEvent(QCloseEvent *event);
};

#endif // VIEWER_WIDGET_H
