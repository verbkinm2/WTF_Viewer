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

Viewer_widget::Viewer_widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Viewer_widget)
{
    this->ui->setupUi(this);

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

    connect(ui->selection_button, SIGNAL(clicked()), this, SLOT(slotSelectionFrame()));
    connect(ui->cut_button, SIGNAL(clicked()), this, SLOT(slotCut()));

    //изменение выделения с помощью спинбоксов
    connect(ui->x_selection, SIGNAL(valueChanged(int)), this, SLOT(slotMoveRectFromKey()) );
    connect(ui->y_selection, SIGNAL(valueChanged(int)), this, SLOT(slotMoveRectFromKey()) );
    connect(ui->width_selection, SIGNAL(valueChanged(int)), this, SLOT(slotMoveRectFromKey()) );
    connect(ui->heigth_selection, SIGNAL(valueChanged(int)), this, SLOT(slotMoveRectFromKey()) );

    //
    connect(ui->clogFilterPanel, SIGNAL(signalRangeChanged(QObject*, quint16)),   this, SLOT(slotClogFilterRangeChange(QObject*, quint16)));
    connect(ui->clogFilterPanel, SIGNAL(signalApplyFilter()),       this, SLOT(slotApplyClogFilter()));
    connect(ui->clogFilterPanel, SIGNAL(signalRangeEnabled(QObject*)), this, SLOT(slotClogFilterRangeEnabled(QObject*)));
    connect(ui->clogFilterPanel, SIGNAL(signalRangeDisabled(QObject*)), this, SLOT(slotClogFilterRangeDisabled(QObject*)));

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
QImage Viewer_widget::getImageFromTxtFile(QString fileName)
{
    return createArrayImage(fileName);
}

QImage Viewer_widget::getImageFromClogFile(QString fileName)
{
//    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QFile file(fileName);
    file.open(QFile::ReadOnly);
    uint lines = 0;

    while (!file.atEnd()) {
        file.readLine();
        lines++;
    }
    file.close();

    ProgressBar progressBar(lines);
    connect(&frames, SIGNAL(signalFramesCreated()), &progressBar, SLOT(close()));
    connect(&frames, SIGNAL(signalFrameCreated(int)), &progressBar, SLOT(slotSetValue(int)));
    progressBar.show();

    frames.setFile(fileName);

    column  = 256;
    row     = 256;

    QImage image(column, row, QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::black);

    //выделяем память для основного массива
    arrayOrigin = new quint16 *[column];
    for (quint16 i = 0; i < column; ++i)
        arrayOrigin[i] = new quint16[row];


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
    ui->cut_button->setEnabled(state);

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

    ui->tabWidgetRight->setEnabled(state);

    if(fType == TXT)
        ui->tabWidgetRight->setTabEnabled(1, false);
    else if(fType == CLOG)
    {
        ui->tabWidgetRight->setTabEnabled(1, true);

        ui->clogFilterPanel->setClusterRangeMaximum(frames.getMaxCluster());
        if( !(ui->clogFilterPanel->isClusterEnable()))
            ui->clogFilterPanel->setClusterEnd(frames.getMaxCluster());

        ui->clogFilterPanel->setTotRangeMaximum(frames.getMaxTot());
        if( !(ui->clogFilterPanel->isTotEnable()))
            ui->clogFilterPanel->setTotEnd(frames.getMaxTot());
    }


//    ui->data_panel->setEnabled(state);
//    ui->edit_panel->setEnabled(state);

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

//        QApplication::restoreOverrideCursor();
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
//    qDebug() << frames.getClusterRangeBegin() << frames.getClusterRangeEnd();

    //обнуляем основной массив
   for (quint16 x = 0; x < column; ++x)
       for (quint16 y = 0; y < row; ++y)
           arrayOrigin[x][y] = 0;

    quint16 max = 0;
    for (quint16 frameNumber = 0; frameNumber < frames.getFrameCount(); ++frameNumber)
        for (quint16 clusterNumber = 0; clusterNumber < frames.getClusterCount(frameNumber); ++clusterNumber) {
            if( frames.clusterInRange(frames.getClusterLenght(frameNumber, clusterNumber)) &&
                frames.totInRange(frameNumber, clusterNumber))
                if(frames.isMediPix())
                {
                    for (uint eventNumber = 0; eventNumber < frames.getEventCountInCluster(frameNumber, clusterNumber); ++eventNumber) {
                        ePoint point = frames.getEPoint(frameNumber, clusterNumber, eventNumber);
                        arrayOrigin[point.x][point.y] += 1;
                        if(max < arrayOrigin[point.x][point.y])
                        {
                            max = arrayOrigin[point.x][point.y];
                        }
                    }

                }
        }

    //наполнение объекта QImage
    for (quint16 x = 0; x < column; ++x)
        for (quint16 y = 0; y < row; ++y) {
            quint16 value = quint16(convert(double(arrayOrigin[x][y]), \
                                            double(0), \
                                            double(max), \
                                            double(0), \
                                            double(255) ) + 0.5);
            QColor color(value, value, value);
            image.setPixelColor(x, y, color);
        }

    ui->clogFilterPanel->setLabelMax(frames.getMaxCluster());
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

void Viewer_widget::slotClogFilterRangeChange(QObject *obj, quint16 value)
{
    if(obj->objectName() == "clusterRangeBegin")
        frames.setClusterRangeBegin(value);
    if(obj->objectName() == "clusterRangeEnd")
        frames.setClusterRangeEnd(value);

    if(obj->objectName() == "totRangeBegin")
        frames.setTotRangeBegin(value);
    if(obj->objectName() == "totrRangeBegin")
        frames.setTotRangeBegin(value);
}

void Viewer_widget::slotApplyClogFilter()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    QImage image(column, row, QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::white);

    applyClogFilter(image);

    setImage(image);

    QApplication::restoreOverrideCursor();
}

void Viewer_widget::slotClogFilterRangeEnabled(QObject *obj)
{
    QGroupBox* groupBox = static_cast<QGroupBox*>(obj);

    if(groupBox->objectName() == "clusterRangeGroup")
    {
        frames.setClusterRangeBegin(ui->clogFilterPanel->getClusterBegin());
        frames.setClusterRangeEnd(ui->clogFilterPanel->getClusterEnd());
    }
    else if(groupBox->objectName() == "totRangeGroup")
    {
        frames.setTotRangeBegin(ui->clogFilterPanel->getTotBegin());
        frames.setTotRangeEnd(ui->clogFilterPanel->getTotEnd());
    }
}

void Viewer_widget::slotClogFilterRangeDisabled(QObject *obj)
{
    QGroupBox* groupBox = static_cast<QGroupBox*>(obj);

    if(groupBox->objectName() == "clusterRangeGroup"){
        frames.setClusterRangeBegin(1);
        frames.setClusterRangeEnd(frames.getMaxCluster());
    }
    else if(groupBox->objectName() == "totRangeGroup")
    {
        frames.setTotRangeBegin(1);
        frames.setTotRangeEnd(frames.getMaxTot());
    }
}

//void Viewer_widget::slotOpened(int tab)
//{
//    if( tab == 1){
//        ui->graphicsView_Origin->fitInView(imageOrigin.rect(), Qt::KeepAspectRatio);
//        disconnect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(slotOpened(int)));
//    }

//}
void Viewer_widget::slotViewSelectionMovePos(QPoint point)
{
    disconnectSelectionSpinBox();

    ui->x_selection->setValue(point.x());
    ui->y_selection->setValue(point.y());

    connectSelectionSpinBox();
}
void Viewer_widget::slotViewSelectionPos(QRect rect)
{
    qint16 x = qint16(rect.x());
    qint16 y = qint16(rect.y());

    qint16 width = qint16(rect.width());
    qint16 height = qint16(rect.height());

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
    ui->selection_button->setChecked(false);
    ui->cut_button->setEnabled(true);
}
QImage Viewer_widget::createArrayImage(const QString& fileName)
{
    ListData data(fileName);

    column  = quint16(data.column);
    row     = quint16(data.row);

    QImage image(column, row, QImage::Format_ARGB32_Premultiplied);

    if(column == 0 || row == 0 || column-1 > 65535 || row-1 > 65535)
        return QImage(QSize(0,0),QImage::Format_Invalid);

    //выделяем память для основного массива
    arrayOrigin = new quint16 *[column];
    for (quint16 i = 0; i < column; ++i)
        arrayOrigin[i] = new quint16[row];

    //max необходима для дальнейшего преобразования диапазонов
    int max = 0;
    //Заполнение матрицы данными из файла
    quint16 iterrator = 0;
    quint16 value = 0;
    for (quint16 y = 0; y < row; ++y)
        for (quint16 x = 0; x < column; ++x) {
            value =  data.list.at(iterrator++);
            arrayOrigin[x][y] = value;
            max < value ? max = value : NULL ;
        }

    //наполнение объекта QImage
    for (quint16 x = 0; x < column; ++x)
        for (quint16 y = 0; y < row; ++y) {
            quint16 value = quint16(convert(double(arrayOrigin[x][y]), \
                                            double(0), \
                                            double(max), \
                                            double(0), \
                                            double(255) ) + 0.5);
            QColor color(value, value, value);
            image.setPixelColor(x, y, color);
        }

    //удаление временного массива
//    for (int i = 0; i < column; ++i)
//        delete[] array[i];
//    delete[] array;

    return image;
}
double Viewer_widget::convert(double value, double From1, double From2, double To1, double To2)
{
    if( (quint16(From1) == quint16(From2)) ||  (quint16(To1) == quint16(To2)))
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
void Viewer_widget::slotSelectionFrame()
{
    if(ui->selection_button->isChecked())
    {
        setEnableDataPanelSelection(true);

//        if(scene.items().length() > 2)
//            scene.removeItem(scene.items().at(0));
        if(itemRect){
            scene.removeItem(itemRect);
            itemRect = nullptr;
        }
        ui->graphicsView->setCursor(QCursor(Qt::CrossCursor));
    }
    else
    {
        ui->graphicsView->unsetCursor();
        setEnableDataPanelSelection(false);
    }
}
void Viewer_widget::slotInversionCheckBox(int state)
{
    switch (state)
    {
    case (Qt::Unchecked):
        imageBackground.fill(Qt::black);
        itemBackground->setPixmap(QPixmap::fromImage(imageBackground));
        itemForeground->setPixmap(QPixmap::fromImage(imageOrigin));
//        if(scene.items().length() > 2)
//            scene.removeItem(scene.items().at(2));
//        else
//            scene.clear();

//        imageBackground.fill(Qt::black);
//        scene.addPixmap(QPixmap::fromImage(imageBackground));
//        scene.addPixmap(QPixmap::fromImage(imageOrigin));
        break;
    case (Qt::Checked):
        imageBackground.fill(Qt::white);
        itemBackground->setPixmap(QPixmap::fromImage(imageBackground));
        QImage inversion(imageOrigin);
        inversion.invertPixels(QImage::InvertRgb);
        itemForeground->setPixmap(QPixmap::fromImage(inversion));

//        if(scene.items().length() > 2)
//            scene.removeItem(scene.items().at(2));
//        else
//            scene.clear();
//        QImage inversion(imageOrigin);
//        inversion.invertPixels(QImage::InvertRgb);
//        imageBackground.fill(Qt::white);
////        scene.addPixmap(QPixmap::fromImage(imageBackground));
//        scene.addPixmap(QPixmap::fromImage(inversion));
        break;
    }

//    qDebug() << scene.items();
}
void Viewer_widget::slotCut()
{
    if(!itemRect)
    {
        QMessageBox::critical(this, "Error", "Please, select an area!");
        return;
    }
    quint16 column  = quint16(ui->width_selection->value());
    quint16 row     = quint16(ui->heigth_selection->value());

    QImage image(column, row, QImage::Format_ARGB32_Premultiplied);

//    if(column == 0 || row == 0 || column-1 > 65535 || row-1 > 65535)
//        setImage(QImage(QSize(0,0),QImage::Format_Invalid));

    //Временный массив для данных преобразованного диапазона
    quint16** array;

    //выделяем память для временного массива
    array = new quint16 *[column];
    for (quint16 i = 0; i < column; ++i)
        array[i] = new quint16[row];

    //Заполнение временного массива данными из выделенной области
    quint16 value = 0;
    for (qint16 x = qint16(ui->x_selection->value()), tmpX = 0; tmpX < column; ++x, ++tmpX)
        for (qint16 y = qint16(ui->y_selection->value()), tmpY = 0; tmpY < row; ++y, ++tmpY) {
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
    arrayOrigin = new quint16 *[column];
    for (quint16 i = 0; i < column; ++i)
        arrayOrigin[i] = new quint16[row];

    //max необходима для дальнейшего преобразования диапазонов
    int max = 0;
    value = 0;
    //копируем временный массив в основной
    for (quint16 x = 0; x < column; ++x)
        for (quint16 y = 0; y < row; ++y) {
            value = array[x][y];
            arrayOrigin[x][y] = value;
            max < value ? max = value : NULL ;
        }

    // преобразование диапазонов
    for (quint16 x = 0; x < column; ++x)
        for (quint16 y = 0; y < row; ++y) {
            quint16 value = quint16(convert(double(arrayOrigin[x][y]), \
                                            double(0), \
                                            double(max), \
                                            double(0), \
                                            double(255) ) + 0.5);
            array[x][y] = value;
        }

    //наполнение объекта QImage
    for (quint16 x = 0; x < column; ++x)
        for (quint16 y = 0; y < row; ++y) {
            quint16 value = array[x][y];
            QColor color(value, value, value);
            image.setPixelColor(x, y, color);
        }

    //удаление временного массива
    for (int i = 0; i < column; ++i)
        delete[] array[i];
    delete[] array;

    setImage(image);
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
    quint16 width = quint16(scene.sceneRect().width());
    quint16 height = quint16(scene.sceneRect().height());

    if(pos.x() < 0 || pos.x() > width || pos.y() < 0 || pos.y() > height)
    {
        ui->x->setValue(0);
        ui->y->setValue(0);
        ui->data->setValue(0);
    }
    else if(arrayOrigin != nullptr)
    {
        quint16 x = quint16(pos.x());
        quint16 y = quint16(pos.y());

        if( (x < column) && (y < row) ){
            quint16 data = arrayOrigin[x][y];
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
                                                    ("Save files"),
                                                    QDir::rootPath(),
                                                    "Images (*.bmp);;All files (*.*)");

    QImage image(column, row, QImage::Format_RGB32);

    if(ui->inversion->isChecked())
        image = this->getImageInversion();
    else
        image = this->getImage();

    image.save(fileName, "BMP");
}
//void Viewer_widget::closeEvent(QCloseEvent *event)
//{
//    qDebug() << "close";
//}
Viewer_widget::~Viewer_widget()
{
    delete ui;
}
