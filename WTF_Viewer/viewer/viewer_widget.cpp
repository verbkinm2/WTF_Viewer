#include "viewer_widget.h"
#include "ui_viewer_widget.h"
#include "checkFile/checkfile.h"

#include <QPushButton>
#include <QTransform>
#include <QFileDialog>
#include <QTabBar>
#include <QPixmap>

#include <QDebug>

Viewer_widget::Viewer_widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Viewer_widget)
{
    this->ui->setupUi(this);

    //повороты
    connect(ui->rotate_plus, SIGNAL(clicked()), this, SLOT(slotRotate()));
    connect(ui->rotate_minus, SIGNAL(clicked()), this, SLOT(slotRotate()));

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

    //изменение выделения с помощью спинпоксов
    connect(ui->x_selection, SIGNAL(valueChanged(int)), this, SLOT(slotMoveRectFromKey()) );
    connect(ui->y_selection, SIGNAL(valueChanged(int)), this, SLOT(slotMoveRectFromKey()) );
    connect(ui->width_selection, SIGNAL(valueChanged(int)), this, SLOT(slotMoveRectFromKey()) );
    connect(ui->heigth_selection, SIGNAL(valueChanged(int)), this, SLOT(slotMoveRectFromKey()) );

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
    clearArrayOrigin();
    QImage image = createArrayImage(fileName);

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
    ui->data_panel->setEnabled(state);
    ui->edit_panel->setEnabled(state);

    setEnableDataPanelSelection(false);
}
void Viewer_widget::setImage(QImage image)
{
    imageOrigin = image;

    if(image.format() != QImage::Format_Invalid)
    {
        setEnableButtonPanel(true);

        ui->graphicsView->resetTransform();
        ui->graphicsView_Origin->resetTransform();
        scene.setSceneRect(image.rect());
        ui->width->setValue(int(scene.width()) );
        ui->heigth->setValue(int(scene.height()) );

        ui->graphicsView->fitInView(image.rect(), Qt::KeepAspectRatio);
        ui->graphicsView_Origin->fitInView(image.rect(), Qt::KeepAspectRatio);

        scene.clear();
        slotInversionCheckBox(ui->inversion->checkState());


        //Отображение на панели координаты курсора (x,y) относительно graphicsView
        connect(eventFilterScene, SIGNAL(signalMousePos(QPointF)), this, SLOT(slotViewPosition(QPointF)));
        //Отображение на панели данных о выделении(x,y,width,height)
        connect(eventFilterScene, SIGNAL(siganlRect(QRect)), this, SLOT(slotViewSelectionPos(QRect)));
        //Отображение на панели координат выделения при перемещении(x,y)
        connect(eventFilterScene, SIGNAL(signalRectMove(QPoint)), this, SLOT(slotViewSelectionMovePos(QPoint)));
        //Изображение курсора - стрелка, выключение кнопки selection_button
        connect(eventFilterScene, SIGNAL(signalRelease()), this, SLOT(slotFinishSelection()));
    }
    else
        incorrectFile();
}
void Viewer_widget::setImageFile(QString fileName)
{
    setImage(getImageFromTxtFile(fileName));
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
// !!!
void Viewer_widget::slotMoveRectFromKey()
{
    QGraphicsRectItem* rectItem = static_cast<QGraphicsRectItem*>(scene.items().at(0));

    rectItem->setPos(0, 0);
    rectItem->setRect(ui->x_selection->value(), ui->y_selection->value(),
                      ui->width_selection->value(), ui->heigth_selection->value());
}
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

    //Временный массив для данных преобразованного диапазона
    quint16** array;

    //выделяем память для временного массива
    array = new quint16 *[column];
    for (quint16 i = 0; i < column; ++i)
        array[i] = new quint16[row];

    //выделяем память для основного массива
    arrayOrigin = new quint16 *[column];
    for (quint16 i = 0; i < column; ++i)
        arrayOrigin[i] = new quint16[row];

    //max необходима для дальнейшего преобразования диапазонов
    int max = 0;
    //Заполнение матрицы данными из файла
    quint16 iterrator = 0;
    quint16 value = 0;
    for (quint16 x = 0; x < row; ++x)
        for (quint16 y = 0; y < column; ++y) {
            value =  data.list.at(iterrator++);
            arrayOrigin[y][x] = value;
            max < value ? max = value : NULL ;
        }

    // преобразование диапазонов
    for (quint16 x = 0; x < row; ++x)
        for (quint16 y = 0; y < column; ++y) {
            quint16 value = quint16(convert(double(arrayOrigin[y][x]), \
                                            double(0), \
                                            double(max), \
                                            double(0), \
                                            double(255) ) + 0.5);
            array[y][x] = value;
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

        if(scene.items().length() > 1)
            scene.removeItem(scene.items().at(0));
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
        if(scene.items().length() > 1)
            scene.removeItem(scene.items().at(1));
        else
            scene.clear();
        scene.addPixmap(QPixmap::fromImage(imageOrigin));
        break;
    case (Qt::Checked):
        if(scene.items().length() > 1)
            scene.removeItem(scene.items().at(1));
        else
            scene.clear();
        QImage inversion(imageOrigin);
        inversion.invertPixels(QImage::InvertRgb);
        scene.addPixmap(QPixmap::fromImage(inversion));
        break;
    }
}
void Viewer_widget::slotCut()
{
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
            if( (x < 0) || (x > this->column) || (y < 0) || (y > this->row) || \
                (tmpX > this->column) || (tmpY > this->row)   )
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
        ui->graphicsView->rotate(90);
        ui->graphicsView_Origin->rotate(90);
    }
    else if ( sender()->objectName() == "rotate_minus" ){
        ui->graphicsView->rotate(-90);
        ui->graphicsView_Origin->rotate(-90);
    }
}
void Viewer_widget::slotResetTransform()
{
    ui->graphicsView->resetTransform();
    ui->graphicsView->fitInView(imageOrigin.rect(), Qt::KeepAspectRatio);

    ui->graphicsView_Origin->resetTransform();
    ui->graphicsView_Origin->fitInView(imageOrigin.rect(), Qt::KeepAspectRatio);
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
