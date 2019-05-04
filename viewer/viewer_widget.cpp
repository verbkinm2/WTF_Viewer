#include "viewer_widget.h"
#include "ui_viewer_widget.h"
#include "checkFile/checkfile.h"
#include "../progressbar.h"


#include <QPushButton>
#include <QTransform>
#include <QFileDialog>
#include <QTabBar>
#include <QPixmap>
#include <QMessageBox>
#include <QFile>
#include <QFileInfo>

#include <QDebug>


Viewer_widget::Viewer_widget(QSettings &setting, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Viewer_widget)
{
    this->ui->setupUi(this);

    pSettings = &setting;

    //повороты
    connect(ui->rotate_plus, SIGNAL(clicked()), this, SLOT(slotRotate()));
    connect(ui->rotate_minus, SIGNAL(clicked()), this, SLOT(slotRotate()));
    connect(ui->angle, SIGNAL(valueChanged(double)), this, SLOT(slotRotate()));

    //сброс трансформации
    connect(ui->reset_transform, SIGNAL(clicked()), this, SLOT(slotResetTransform()));

    //масштаб
    connect(ui->scaled_plus, SIGNAL(clicked()), this, SLOT(slotScaled()));
    connect(ui->scaled_minus, SIGNAL(clicked()), this, SLOT(slotScaled()));

    //сохранение в bmp
    connect(ui->save_bmp, SIGNAL(clicked()), this, SLOT(slotSaveBMP()));
    //сохранение в txt
    connect(ui->save_txt, SIGNAL(clicked()), this, SLOT(slotSaveTXT()));

    //инверсия цвета
    connect(ui->inversion, SIGNAL(stateChanged(int)), this, SLOT(slotInversionCheckBox(int)));

    //установка сцены
    scene.setObjectName("scene");
    ui->graphicsView->setScene(&scene);
    ui->graphicsView_Origin->setScene(&scene);
    //фильтр событий для сцены
    eventFilterScene = new FingerSlide(&scene);
    scene.installEventFilter(eventFilterScene);

    //представление
    ui->graphicsView->viewport()->setObjectName("viewport");
    ui->graphicsView_Origin->viewport()->setObjectName("viewport");

    //фильтр событий для представления
    FingerSlide* eventFilterViewPort = new FingerSlide(ui->graphicsView->viewport());
    ui->graphicsView->viewport()->installEventFilter(eventFilterViewPort);
    connect(eventFilterViewPort, SIGNAL(signalWheel(int)), this, SLOT(slotScaleWheel(int)));
    //вторая вкладка
    FingerSlide* eventFilterViewPort2 = new FingerSlide(ui->graphicsView_Origin->viewport());
    ui->graphicsView_Origin->viewport()->installEventFilter(eventFilterViewPort);
    connect(eventFilterViewPort2, SIGNAL(signalWheel(int)), this, SLOT(slotScaleWheel(int)));

    connect(ui->edit_panel, SIGNAL(signalSelectionToggle(bool)), this, SLOT(slotSelectionFrame(bool)));
    connect(ui->edit_panel, SIGNAL(signalPenToggle(bool)), this, SLOT(slotPen(bool)));
    connect(ui->edit_panel, SIGNAL(signalCutClicked(bool)), this, SLOT(slotCut()));
    connect(ui->edit_panel, SIGNAL(signalRepaint()), this, SLOT(slotRepaint()));

    //изменение выделения с помощью спинбоксов
    connect(ui->x_selection, SIGNAL(valueChanged(int)), this, SLOT(slotMoveRectFromKey()) );
    connect(ui->y_selection, SIGNAL(valueChanged(int)), this, SLOT(slotMoveRectFromKey()) );
    connect(ui->width_selection, SIGNAL(valueChanged(int)), this, SLOT(slotMoveRectFromKey()) );
    connect(ui->heigth_selection, SIGNAL(valueChanged(int)), this, SLOT(slotMoveRectFromKey()) );

    //
//    connect(ui->clogFilterPanel, SIGNAL(signalRangeChanged(QObject*, int)),   this, SLOT(slotClogFilterRangeChange(QObject*, int)));
    connect(ui->clogFilterPanel, SIGNAL(signalApplyFilter()),       this, SLOT(slotApplyClogFilter()));
//    connect(ui->clogFilterPanel, SIGNAL(signalPixGroupMidiPixSet(bool)), &frames, SLOT(slotSetMediPix(bool)));
//    connect(ui->clogFilterPanel, SIGNAL(signalAllTotInClusterToggled(bool)), this, SLOT(slotSetAllTotInCluster(bool)));

    //при первом запуске - вывести на экран надпись и отключить всё не нужное
    incorrectFile();
}
void Viewer_widget::selectFile()
{
    scene.clear();

    ui->graphicsView->resetTransform();
    ui->graphicsView_Origin->resetTransform();

    scene.setSceneRect(QRect(0,0, 50,50));
    scene.addText("Select file!");
}
int Viewer_widget::getColumnFromFile(QString fileName)
{
    return ListData(fileName).column;
}
int Viewer_widget::getRowFromFile(QString fileName)
{
    return ListData(fileName).row;
}

Frames *Viewer_widget::getFrames()
{
    return &frames;
}

QImage Viewer_widget::getImageFromTxtFile(QString fileName)
{
    return createArrayImage(fileName);
}

QImage Viewer_widget::getImageFromClogFile(QString fileName)
{
    QFile file(fileName);
    file.open(QFile::ReadOnly);
    int lines = 0;

    while (!file.atEnd())
    {
        file.readLine();
        lines++;
    }
    file.close();

    ProgressBar progressBar(lines);
    connect(&frames, SIGNAL(signalFramesCreated()), &progressBar, SLOT(close()));
    connect(&frames, SIGNAL(signalFrameCreated(int)), &progressBar, SLOT(slotSetValue(int)));
    progressBar.show();

    frames.setFile(fileName);
    setEnableButtonPanel(true);

    column  = CLOG_SIZE;
    row     = CLOG_SIZE;

    QImage image(column, row, QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::black);

    //выделяем память для основного массива
    arrayOrigin = new int *[column];
    for (int i = 0; i < column; ++i)
        arrayOrigin[i] = new int[row];


    //наполняем основной массив данными согласно установленному фильтру
    applyClogFilter(image);

    return image;
}
void Viewer_widget::clearArrayOrigin()
{
    if(arrayOrigin != nullptr){
        for (int i = 0; i < column; ++i)
            delete[] arrayOrigin[i];
        delete[] arrayOrigin;
        arrayOrigin = nullptr;
        column = 0;
        row = 0;

        itemBackground = nullptr;
        itemForeground = nullptr;
        itemRect       = nullptr;
    }
}
void Viewer_widget::setEnableDataPanelSelection(bool state)
{
    disconnectSelectionSpinBox();

    ui->data_panel_selection->setEnabled(state);
    ui->edit_panel->buttonCutDisable(!state);

    ui->x_selection->setValue(0);
    ui->y_selection->setValue(0);
    ui->width_selection->setValue(0);
    ui->heigth_selection->setValue(0);

    setReadOnlyDataPanelSelection(true);

    connectSelectionSpinBox();
}
void Viewer_widget::setReadOnlyDataPanelSelection(bool state)
{
    ui->x_selection->setReadOnly(state);
    ui->y_selection->setReadOnly(state);
    ui->width_selection->setReadOnly(state);
    ui->heigth_selection->setReadOnly(state);
}
void Viewer_widget::setEnableButtonPanel(bool state)
{
    ui->button_panel->setEnabled(state);
    ui->inversion->setEnabled(state);
    ui->data_panel->setEnabled(state);

    ui->tabWidgetRight->setEnabled(state);
    ui->edit_panel->setToggleButtons(false);

    if(fType == TXT)
        ui->tabWidgetRight->setTabEnabled(CLOG_FILTER_TAB, false);
    else if(fType == CLOG)
    {
        ui->tabWidgetRight->setTabEnabled(CLOG_FILTER_TAB, true);

        ui->clogFilterPanel->setClusterRange(frames.getClustersLenghtList());
        ui->clogFilterPanel->setTotRange(frames.getTotLenghtList());
    }


    //state
    setEnableDataPanelSelection(false);
}
void Viewer_widget::setImage(QImage image)
{
    imageOrigin = image;
    imageBackground = image;

    if(image.format() != QImage::Format_Invalid)
    {
        setEnableButtonPanel(true);

        angle = 0;
        ui->angle->setValue(0);
        ui->graphicsView->resetTransform();
        ui->graphicsView_Origin->resetTransform();
        scene.setSceneRect(image.rect());
        ui->width->setValue(int(scene.width()) );
        ui->heigth->setValue(int(scene.height()) );

//переключаемся на 1-ю вкладку tabWidget и растягиваем рисунок по graphicsView
//потом на 2-ю и растягиваем рисунок по graphicsView_Origin
//а потом переключаемся на вкладку, которая была изначально
        int currentTab = ui->tabWidget->currentIndex();
        ui->tabWidget->setCurrentIndex(0);
        ui->graphicsView->fitInView(image.rect(), Qt::KeepAspectRatio);
        ui->tabWidget->setCurrentIndex(1);
        ui->graphicsView_Origin->fitInView(image.rect(), Qt::KeepAspectRatio);
        ui->tabWidget->setCurrentIndex(currentTab);

        scene.clear();

        itemBackground = scene.addPixmap(QPixmap::fromImage(imageBackground));
        itemBackground->setZValue(0);
        itemForeground = scene.addPixmap(QPixmap::fromImage(imageOrigin));
        itemForeground->setZValue(1);

        slotInversionCheckBox(ui->inversion->checkState());

        //Отображение на панели координаты курсора (x,y) относительно graphicsView
        connect(eventFilterScene, SIGNAL(signalMousePos(QPointF)), this, SLOT(slotViewPosition(QPointF)));
        //Отображение на панели данных о выделении(x,y,width,height)
        connect(eventFilterScene, SIGNAL(siganlRect(QRect)), this, SLOT(slotViewSelectionPos(QRect)));
        //Отображение на панели координат выделения при перемещении(x,y)
        connect(eventFilterScene, SIGNAL(signalRectMove(QPoint)), this, SLOT(slotViewSelectionMovePos(QPoint)));
        //Изображение курсора - стрелка, выключение кнопки selection_button
        connect(eventFilterScene, SIGNAL(signalRelease()), this, SLOT(slotFinishSelection()));

        connect(eventFilterScene, SIGNAL(signalCreateRectItem(QGraphicsRectItem*)), this, SLOT(slotCreateRectItem(QGraphicsRectItem*)));
        connect(eventFilterScene, SIGNAL(signalDrawPoint(QPointF)), this, SLOT(slotDrawPoint(QPointF)));


    }
    else
        incorrectFile();
}
void Viewer_widget::setImageFile(QString fileName)
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QFileInfo file(fileName);

    frames.clear();
    clearArrayOrigin();

    if(file.suffix() == "txt")
    {
        fType = TXT;
        setImage(getImageFromTxtFile(fileName));
    }
    else if(file.suffix() == "clog")
    {
        fType = CLOG;
        setImage(getImageFromClogFile(fileName));
    }
    else {
        fType = UNDEFINED;
        incorrectFile();
    }

    QApplication::restoreOverrideCursor();
}
void Viewer_widget::disconnectSelectionSpinBox()
{
    disconnect(ui->x_selection, SIGNAL(valueChanged(int)), this, SLOT(slotMoveRectFromKey()) );
    disconnect(ui->y_selection, SIGNAL(valueChanged(int)), this, SLOT(slotMoveRectFromKey()) );
    disconnect(ui->width_selection, SIGNAL(valueChanged(int)), this, SLOT(slotMoveRectFromKey()) );
    disconnect(ui->heigth_selection, SIGNAL(valueChanged(int)), this, SLOT(slotMoveRectFromKey()) );
}
void Viewer_widget::connectSelectionSpinBox()
{
    connect(ui->x_selection, SIGNAL(valueChanged(int)), this, SLOT(slotMoveRectFromKey()) );
    connect(ui->y_selection, SIGNAL(valueChanged(int)), this, SLOT(slotMoveRectFromKey()) );
    connect(ui->width_selection, SIGNAL(valueChanged(int)), this, SLOT(slotMoveRectFromKey()) );
    connect(ui->heigth_selection, SIGNAL(valueChanged(int)), this, SLOT(slotMoveRectFromKey()) );
}

void Viewer_widget::applyClogFilter(QImage& image)
{
    //обнуляем основной массив
   for (int x = 0; x < column; ++x)
       for (int y = 0; y < row; ++y)
           arrayOrigin[x][y] = 0;

    for (int frameNumber = 0; frameNumber < frames.getFrameCount(); ++frameNumber)
        for (int clusterNumber = 0; clusterNumber < frames.getClusterCount(frameNumber); ++clusterNumber)
        {
            if( frames.clusterInRange(frames.getClusterLenght(frameNumber, clusterNumber),
                                      ui->clogFilterPanel->getClusterBegin(),
                                      ui->clogFilterPanel->getClusterEnd())
                                      &&
                frames.totInRange(frameNumber, clusterNumber,
                                  ui->clogFilterPanel->getTotBegin(),
                                  ui->clogFilterPanel->getTotEnd()) )
            {
                if(ui->clogFilterPanel->isAllTotInCluster())
                    for (int eventNumber = 0; eventNumber < frames.getEventCountInCluster(frameNumber, clusterNumber); ++eventNumber)
                    {
                        ePoint point = frames.getEPoint(frameNumber, clusterNumber, eventNumber);
                        applyClogFilterAdditionalFunction(point);
                    }
                else
                {
                    QList<ePoint> listePoint = frames.getListTotInRange(frameNumber, clusterNumber,
                                                                        ui->clogFilterPanel->getTotBegin(), ui->clogFilterPanel->getTotEnd());
                    foreach (ePoint point, listePoint)
                        applyClogFilterAdditionalFunction(point);
                }
            }
        }

    //проверяем настройки для картинки такие как рамка и маскирование пикселей
    imageSettingsForArray();

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
    //цвет пикселей, к которым применилась маска
    imageSettingsForImage(image);
}
//для меньшего кол-ва строк исполбзуем эту функцию
void Viewer_widget::applyClogFilterAdditionalFunction(const ePoint &point)
{
    //если координаты точек выходят за границы - это просто игнорируется
    if(point.x >= column || point.y >= row)
        return;
    //Выбор режима - MediPix or TimePix
    if(ui->clogFilterPanel->isMediPix())
        arrayOrigin[point.x][point.y] = arrayOrigin[point.x][point.y] + 1;
    else
        arrayOrigin[point.x][point.y] = arrayOrigin[point.x][point.y] + point.tot;
}

void Viewer_widget::imageSettingsForImage(QImage &image)
{
    if(pSettings->value("SettingsImage/MasquradingGroupBox").toBool())
    {
        //рисуем маскированые пиксели выбраным цветом
        for (int x = 0; x < column; ++x)
            for (int y = 0; y < row; ++y)
                if(arrayMask[x][y] > 0)
                    image.setPixelColor(x, y, QColor(pSettings->value("SettingsImage/maskColor").toString()));
        //удаляем массив для маскирования пикселей
        for (int i = 0; i < column; ++i)
            delete[] arrayMask[i];
        delete[] arrayMask;
        arrayMask = nullptr;
    }
}

void Viewer_widget::imageSettingsForArray()
{
    pSettings->beginGroup("SettingsImage");

    //если в настройка включено рисование рамки то рисуем её
    if(pSettings->value("FrameGroupBox").toBool())
        createFrameInArray();

    //если в настройка включено маскирование пискелей то маскируем их
    if(pSettings->value("MasquradingGroupBox").toBool())
        createMaskInArray();

    pSettings->endGroup();
}

void Viewer_widget::createFrameInArray()
{
    int width = pSettings->value("frameWidth").toInt();
    if(width > column || width > row) width = 0;

    int value = pSettings->value("frameValue").toInt();

    //верх
    for (int x = 0; x < column; ++x)
        for (int y = 0; y < width; ++y)
            arrayOrigin[x][y] = value;
    //низ
    for (int x = 0; x < column; ++x)
        for (int y = row - 1; y >= row - width; --y)
            arrayOrigin[x][y] = value;
    //лево
    for (int y = 0; y < row; ++y)
        for (int x = 0; x < width; ++x)
            arrayOrigin[x][y] = value;
    //право
    for (int y = 0; y < row; ++y)
        for (int x = column - 1; x >= column - width; --x)
            arrayOrigin[x][y] = value;
}

void Viewer_widget::createMaskInArray()
{
    //выделяем память для массива маскирования пикселей, удаляем его в функции imageSettingsForImage
    arrayMask = new int *[column];
    for (int i = 0; i < column; ++i)
        arrayMask[i] = new int[row];
    //обнуляем его
    for (int x = 0; x < column; ++x)
        for (int y = 0; y < row; ++y)
            arrayMask[x][y] = 0;

    int value       = pSettings->value("maskValue").toInt();
    int newValue    = pSettings->value("maskNewValue").toInt();
    bool after      = pSettings->value("maskAfter").toBool();

    //пробегаемся по всему массиву
    for (int x = 0; x < column; ++x)
        for (int y = 0; y < row; ++y)
        {
            if(after)
            {
                if(arrayOrigin[x][y] >= value)
                {
                    arrayOrigin[x][y] = newValue;
                    arrayMask[x][y] = 1;
                }
            }
            else if(arrayOrigin[x][y] <= value)
            {
                arrayOrigin[x][y] = newValue;
                arrayMask[x][y] = 1;
            }
        }
}
// !!!
void Viewer_widget::slotMoveRectFromKey()
{
    QGraphicsRectItem* rectItem = static_cast<QGraphicsRectItem*>(scene.items().at(0));

    rectItem->setPos(0, 0);
    rectItem->setRect(ui->x_selection->value(), ui->y_selection->value(),
                      ui->width_selection->value(), ui->heigth_selection->value());
}

void Viewer_widget::slotCreateRectItem(QGraphicsRectItem * item)
{
    itemRect = item;
}

void Viewer_widget::slotApplyClogFilter()
{
//    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    QImage image(column, row, QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::white);

    applyClogFilter(image);

    setImage(image);

//    QApplication::restoreOverrideCursor();
}

void Viewer_widget::slotRepaint()
{
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
            imageOrigin.setPixelColor(x, y, color);
        }
    itemForeground->setPixmap(QPixmap::fromImage(imageOrigin));
}

void Viewer_widget::slotViewSelectionMovePos(QPoint point)
{
    disconnectSelectionSpinBox();

    ui->x_selection->setValue(point.x());
    ui->y_selection->setValue(point.y());

    connectSelectionSpinBox();
}

void Viewer_widget::slotDrawPoint(QPointF point)
{
    int x = int(point.x());
    int y = int(point.y());

    if(x >= 0 && x < column && y >= 0 && y < row)
    {
        imageOrigin.setPixelColor(x, y, ui->edit_panel->getPenColor());
        slotInversionCheckBox(ui->inversion->checkState());
        arrayOrigin[x][y] = ui->edit_panel->getPenValue();
    }
}
void Viewer_widget::slotViewSelectionPos(QRect rect)
{
    int x = rect.x();
    int y = rect.y();

    int width = rect.width();
    int height = rect.height();

    if(height < 0 && width < 0)
    {
        x = x + width;
        y = y - qAbs(height);
        width = qAbs(width);
        height = qAbs(height);
    }
    else if(height < 0 && width > 0)
    {
        y = y - qAbs(height);
        width = qAbs(width);
        height = qAbs(height);
    }
    else if(height > 0 && width < 0  )
    {
        x = x + width;
        width = qAbs(width);
        height = qAbs(height);
    }
    else if(height > 0 && width > 0  )
    {
        ;
    }

    ui->x_selection->setValue(x);
    ui->y_selection->setValue(y);
    ui->width_selection->setValue(width);
    ui->heigth_selection->setValue(height);
}
void Viewer_widget::slotFinishSelection()
{
    ui->graphicsView->unsetCursor();
    setReadOnlyDataPanelSelection(false);
    ui->edit_panel->finishSelection();
    ui->edit_panel->buttonCutDisable(false);
}
QImage Viewer_widget::createArrayImage(const QString& fileName)
{
    ListData data(fileName);

    column  = int(data.column);
    row     = int(data.row);

    QImage image(column, row, QImage::Format_ARGB32_Premultiplied);

    if(column == 0 || row == 0 || column-1 > 65535 || row-1 > 65535)
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

    //проверяем настройки для картинки такие как рамка и маскирование пикселей
    imageSettingsForArray();

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

    imageSettingsForImage(image);

    return image;
}

int Viewer_widget::findMaxInArrayOrigin()
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
double Viewer_widget::convert(double value, double From1, double From2, double To1, double To2)
{
    if( (int(From1) == int(From2)) ||  (int(To1) == int(To2)))
        return 0.0;

    return (value-From1)/(From2-From1)*(To2-To1)+To1;
}
void Viewer_widget::incorrectFile()
{
    setEnableButtonPanel(false);
    selectFile();
    //Отображение на панели координаты курсора (x,y) относительно graphicsView
    disconnect(eventFilterScene, SIGNAL(signalMousePos(QPointF)), this, SLOT(slotViewPosition(QPointF)));
    //Отображение на панели данных о выделении(x,y,width,height)
    disconnect(eventFilterScene, SIGNAL(siganlRect(QRect)), this, SLOT(slotViewSelectionPos(QRect)));
    //Отображение на панели координат выделения при перемещении(x,y)
    disconnect(eventFilterScene, SIGNAL(signalRectMove(QPoint)), this, SLOT(slotViewSelectionMovePos(QPoint)));
    //Изображение курсора - стрелка, выключение кнопки edit
    disconnect(eventFilterScene, SIGNAL(signalRelease()), this, SLOT(slotFinishSelection()));

    disconnect(eventFilterScene, SIGNAL(signalCreateRectItem(QGraphicsRectItem*)), this, SLOT(slotCreateRectItem(QGraphicsRectItem*)));
    disconnect(eventFilterScene, SIGNAL(signalDrawPoint(QPointF)), this, SLOT(slotDrawPoint(QPointF)));

    ui->x->setValue(0);
    ui->y->setValue(0);
    ui->data->setValue(0);
    ui->width->setValue(0);
    ui->heigth->setValue(0);

    clearArrayOrigin();
}
QImage Viewer_widget::getImage()
{
    return imageOrigin;
}
QImage Viewer_widget::getImageInversion()
{
    QImage image(imageOrigin);
    image.invertPixels();
    return image;
}
void Viewer_widget::slotScaleWheel(int value)
{
    if(value > 0)
    {
        ui->graphicsView->scale(1.1, 1.1);
        ui->graphicsView_Origin->scale(1.1, 1.1);
    }
    else if(value < 0)
    {
        ui->graphicsView->scale(1 / 1.1, 1 / 1.1);
        ui->graphicsView_Origin->scale(1 / 1.1, 1 / 1.1);
    }
}
void Viewer_widget::slotSelectionFrame(bool value)
{
    if(value)
    {
        setEnableDataPanelSelection(true);

        if(itemRect){
            scene.removeItem(itemRect);
            itemRect = nullptr;
        }
        ui->graphicsView->setCursor(QCursor(Qt::CrossCursor));
    }
    if(!value && itemRect == nullptr)
    {
        ui->graphicsView->unsetCursor();
        setEnableDataPanelSelection(false);
    }
}

void Viewer_widget::slotPen(bool value)
{
    if(value)
       ui->graphicsView->setCursor(QCursor(QPixmap(":/pen").scaled(24,24, Qt::KeepAspectRatio, Qt::SmoothTransformation), X_HOT ,Y_HOT));
    else
       ui->graphicsView->unsetCursor();
}
void Viewer_widget::slotInversionCheckBox(int state)
{
    switch (state)
    {
    case (Qt::Unchecked):
        imageBackground.fill(Qt::black);
        itemBackground->setPixmap(QPixmap::fromImage(imageBackground));
        itemForeground->setPixmap(QPixmap::fromImage(imageOrigin));
        break;
    case (Qt::Checked):
        imageBackground.fill(Qt::white);
        itemBackground->setPixmap(QPixmap::fromImage(imageBackground));
        QImage inversion(imageOrigin);
        inversion.invertPixels(QImage::InvertRgb);
        itemForeground->setPixmap(QPixmap::fromImage(inversion));
        break;
    }
}
void Viewer_widget::slotCut()
{
    if(!itemRect)
    {
        QMessageBox::critical(this, "Error", "Please, select an area!");
        return;
    }


    int column  = int(ui->width_selection->value());
    int row     = int(ui->heigth_selection->value());

    QImage image(column, row, QImage::Format_ARGB32_Premultiplied);

//    if(column == 0 || row == 0 || column-1 > 65535 || row-1 > 65535)
//        setImage(QImage(QSize(0,0),QImage::Format_Invalid));

    //Временный массив для данных преобразованного диапазона
    int** array;

    //выделяем память для временного массива
    array = new int *[column];
    for (int i = 0; i < column; ++i)
        array[i] = new int[row];

    //Заполнение временного массива данными из выделенной области
    int value = 0;
    for (int x = int(ui->x_selection->value()), tmpX = 0; tmpX < column; ++x, ++tmpX)
        for (int y = int(ui->y_selection->value()), tmpY = 0; tmpY < row; ++y, ++tmpY) {
            if( (x < 0) || (x >= this->column) || (y < 0) || (y >= this->row) )
                value = 0;
            else
                value = arrayOrigin[x][y];
            array[tmpX][tmpY] = value;
        }

    //очищаем основной массив и переписываем переменные column и row
    clearArrayOrigin();
    this->column= column;
    this->row   = row;

    //выделяем память для основного массива
    arrayOrigin = new int *[column];
    for (int i = 0; i < column; ++i)
        arrayOrigin[i] = new int[row];

    value = 0;
    //копируем временный массив в основной
    for (int x = 0; x < column; ++x)
        for (int y = 0; y < row; ++y) {
            value = array[x][y];
            arrayOrigin[x][y] = value;
        }

    int max = findMaxInArrayOrigin();

    // преобразование диапазонов
    for (int x = 0; x < column; ++x)
        for (int y = 0; y < row; ++y) {
            int value = int(convert(double(arrayOrigin[x][y]), \
                                            double(0), \
                                            double(max), \
                                            double(0), \
                                            double(255) ) + 0.5);
            array[x][y] = value;
        }

    //наполнение объекта QImage
    for (int x = 0; x < column; ++x)
        for (int y = 0; y < row; ++y) {
            int value = array[x][y];
            QColor color(value, value, value);
            image.setPixelColor(x, y, color);
        }

    //удаление временного массива
    for (int i = 0; i < column; ++i)
        delete[] array[i];
    delete[] array;

    setImage(image);

    if(fType == CLOG)
        ui->tabWidgetRight->setTabEnabled(CLOG_FILTER_TAB, false);
}
void Viewer_widget::slotRotate()
{
    if( sender()->objectName() == "rotate_plus" ){
        ui->angle->setValue(ui->angle->value() + 90);
    }
    else if ( sender()->objectName() == "rotate_minus" ){
        ui->angle->setValue(ui->angle->value() - 90);
    }
    else
    {        
        itemForeground->setTransformOriginPoint(column / 2, row / 2);
//        itemForeground->setRotation(ui->angle->value());
        QTransform transform;
//        transform = transform.translate(500,500);
        transform.rotate(ui->angle->value(), Qt::ZAxis);
        QImage img = imageOrigin.transformed(transform, Qt::FastTransformation);
        itemForeground->setPixmap(QPixmap::fromImage(img));

    }

    angle = ui->angle->value();
}
void Viewer_widget::slotResetTransform()
{
    ui->graphicsView->resetTransform();
    ui->graphicsView->fitInView(imageOrigin.rect(), Qt::KeepAspectRatio);

    ui->graphicsView_Origin->resetTransform();
    ui->graphicsView_Origin->fitInView(imageOrigin.rect(), Qt::KeepAspectRatio);

    angle = 0;
    ui->angle->setValue(0);
}
void Viewer_widget::slotViewPosition(QPointF pos)
{
    int width = int(scene.sceneRect().width());
    int height = int(scene.sceneRect().height());

    if(pos.x() < 0 || pos.x() > width || pos.y() < 0 || pos.y() > height)
    {
        ui->x->setValue(0);
        ui->y->setValue(0);
        ui->data->setValue(0);
    }
    else if(arrayOrigin != nullptr)
    {
        int x = int(pos.x());
        int y = int(pos.y());

        if( (x < column) && (y < row) ){
            int data = arrayOrigin[x][y];
            ui->x->setValue(x);
            ui->y->setValue(y);
            ui->data->setValue(data);
        }
    }
}
void Viewer_widget::slotScaled()
{
    if( sender()->objectName() == "scaled_plus" )
    {
        ui->graphicsView->scale(1.1, 1.1);
        ui->graphicsView_Origin->scale(1.1, 1.1);
    }
    else if ( sender()->objectName() == "scaled_minus" )
    {
        ui->graphicsView->scale(1 / 1.1, 1 / 1.1);
        ui->graphicsView_Origin->scale(1 / 1.1, 1 / 1.1);
    }
}
void Viewer_widget::slotSaveBMP()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    ("Save file"),
                                                    QDir::rootPath(),
                                                    "Images (*.bmp);;All files (*.*)");


    QImage image(column, row, QImage::Format_RGB32);

    if(ui->inversion->isChecked())
        image = this->getImageInversion();
    else
        image = this->getImage();

    image.save(fileName, "BMP");
}

void Viewer_widget::slotSaveTXT()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    ("Save file"),
                                                    QDir::rootPath(),
                                                    "TXT (*.txt);;All files (*.*)");

    QFile file(fileName);
    QTextStream writeStrime(&file);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    for (int x = 0; x < row; ++x)
    {
        for (int y = 0; y < column; ++y) {
            if(y != 0)
                writeStrime << " ";
            writeStrime << QString::number(arrayOrigin[y][x]);
        }
        writeStrime << "\n";
        writeStrime.flush();
    }
    file.close();
}
//void Viewer_widget::closeEvent(QCloseEvent *event)
//{
//    qDebug() << "close";
//}
Viewer_widget::~Viewer_widget()
{
    delete ui;
}
