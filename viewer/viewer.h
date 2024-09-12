#ifndef VIEWER_H
#define VIEWER_H

#include <QWidget>
#include <QFile>
#include <QImage>
#include <QCloseEvent>
#include <QGraphicsScene>
#include <QSettings>
#include <QMenu>


//#include <QGraphicsPathItem>

#include "../eventfilter/fingerslide.h"
#include "frames/frames.h"


namespace Ui {
class Viewer;
}

class Viewer : public QWidget
{
    Q_OBJECT

public:

    const static int CLOG_SIZE = 256;

    //For cursor with Pen
    const static int X_HOT = 2;
    const static int Y_HOT = 23;

    explicit Viewer(QWidget *parent = nullptr);
    ~Viewer();

    QSettings*  pSettings = nullptr;
    //получение Qimage из txt файла
    QImage      getImageFromTxtFile     (QString fileName);
    //получение Qimage из clog файла
    QImage      getImageFromClogFile     (QString fileName);
    //установка картинки на виджет из файла
    void        setImageFile            (QString fileName);
    void        setImage                (QImage image);

    void        setSettings             (QSettings &settings);
    void        setScene                (QGraphicsScene* scene);
    void        setSceneDefault         ();

    QImage      getImage                ();
    QImage      getImageInversion       ();

    void        resetTransform          ();
    QWidget*    viewport                ();

    //объект сцены
    QGraphicsScene*                     currentScene = nullptr;
    QGraphicsScene                      defaultScene;

    QGraphicsScene*                     getScene();
    size_t      getColumnFromFile       (QString fileName);
    size_t      getRowFromFile          (QString fileName);

    void        setReadOnly             (bool value = true);
    void        hideAllPanel                 ();
    void        hideSettingsButton      (bool value = true);

    Frames* getFrames();

signals:


private:
    Ui::Viewer *ui;

    bool    readOnly = false;

    enum {PIX_AND_FILTER_PANEL, DATA_PANEL, BUTTONS_PANEL, INVERSION, SINGLE_WINDOW};
    /*
    [0 - pix. && filter pnael]
    [1 - data paNel]
    [2 - buttons panel]
    [3 - inversion]
    [4 - single window]
    видимость панелей
    */
    bool    action_array[5];

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
    double**       arrayOrigin             = nullptr;
    //массив для нанесения маски из настроек Settings -Image
    double**       arrayMask               = nullptr;

    //переменные для хранения кол-ва строк и столбцов файла
    size_t column  = 0;
    size_t row     = 0;

    //фильтр событий для сцены и представления
    FingerSlide* eventFilterScene = nullptr;

    //возвращает рисунок из файла или QImage::Format_Invalid
    QImage      createArrayImage        (const QString& fileName);
    double findMaxInArrayOrigin();

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

    QMenu*      pMenuFile               = nullptr;
    void        createButtonMenu        ();


public slots:
    void        slotSetImageFile(QString file);
    //сохранинеие в bmp
    void        slotSaveBMP             ();
    //сохранинеие в txt
    void        slotSaveTXT             ();

private slots:
    //поворот
    void        slotRotate              ();
    // отражение по горизонтали
    void        slotMirror              ();
    //сброс трансформации
    void        slotResetTransform      ();
    //масштаб при нажатии на виджете кнопок + и -
    void        slotScaled              ();
    // масштаб колёсиком мышки
    void        slotScaleWheel          (int);

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

//    [0 - pix. && filter pnael]
//                [1 - data pnael]
//                [2 - buttons panel]
//                [3 - inversion]
//                [4 - single window]
    //действия для каждого пункта меню кнопки button_settings
    void        slotPFP();
    void        slotDP();
    void        slotBP();
    void        slotI();
    void        slotSW();


protected:
//    virtual void closeEvent(QCloseEvent *event);
};

#endif // VIEWER_H
