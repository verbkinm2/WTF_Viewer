#include "centralwidget.h"

#include <QPushButton>
#include <QFileDialog>
#include <QFileInfo>
#include <QDebug>

#include <QApplication>

CentralWidget::CentralWidget(QWidget *parent) : QMainWindow(parent)
{
    createMenu();

    XYDefault();

    chart.setTitle("Title");
    QFont font = chart.titleFont();
    font.setPixelSize(18);
    chart.setTitleFont(QFont(font));

    chart.createDefaultAxes();

    chart.axisX()->setTitleText("X");
    chart.axisY()->setTitleText("Y");
    chart.setDropShadowEnabled(true);

    chartView.setChart(&chart);

    layout.addWidget(&chartView);
    layout.addWidget(&panelWidget);

    layout.setStretch(0, 1);
//    centralWidget = new QWidget();
    centralWidget.setLayout(&layout);
    setCentralWidget(&centralWidget);
    chartView.setFocus();

    connect(&panelWidget,    SIGNAL(signalChangeTheme(int)), this,
                            SLOT(slotSetTheme(int)));

    connect(&panelWidget,    SIGNAL(signalSetLegendPosition(int)), this,
                            SLOT(slotSetLegentPosition(int)));

    connect(&panelWidget,    SIGNAL(signalSetTitile(QString)), this,
                            SLOT(slotSetTitle(QString)));

    connect(&panelWidget,    SIGNAL(signalAnimation(bool)), this,
                            SLOT(slotAnimation(bool)));

    connect(&panelWidget,    SIGNAL(signalAntialiasing(bool)), this,
                            SLOT(slotAntialiasing(bool)));

    connect(&panelWidget,    SIGNAL(signalTickCountChangeX(int)), this,
                            SLOT(slotSetTcickCountX(int)));
    connect(&panelWidget,    SIGNAL(signalTickCountChangeY(int)), this,
            SLOT(slotSetTcickCountY(int)));

//    this->setWindowFlag(Qt::Widget);
}

CentralWidget::~CentralWidget()
{
//    delete pMenuFile;
}

void CentralWidget::addSeries(QVector<QPointF> pointVector, QXYSeries::SeriesType type, QString legendTitle, QString axsisX_Title, QString axsisY_Title)
{
    QXYSeries* series = nullptr;

    switch (type) {
    case QXYSeries::SeriesTypeLine:
        series = new QLineSeries();
        break;
    case QXYSeries::SeriesTypeSpline:
        series = new QSplineSeries();
        break;
    case QXYSeries::SeriesTypeScatter:
        series = new QScatterSeries();
        break;
    default:
        break;
    }

    foreach (QPointF point, pointVector)
    {
        *series << point;

        if(point.x() > maxX) maxX = point.x();
        if(point.y() > maxY) maxY = point.y();

        if(point.x() < minX) minX = point.x();
        if(point.y() < minY) minY = point.y();
    }

    series->setName(legendTitle);
    chart.addSeries(series);
    panelWidget.addSeriesList(series);

//Что-бы правильно работало нажатие Esc >>
    chartView.rangeX.min = minX;
    chartView.rangeX.max = maxX;

    chartView.rangeY.min = minY;
    chartView.rangeY.max = maxY;
//Что-бы правильно работало нажатие Esc <<

    chart.createDefaultAxes();
    chart.axisX()->setRange(minX, maxX);
    chart.axisX()->setTitleText(axsisX_Title);
    chart.axisY()->setRange(minY, maxY);
    chart.axisY()->setTitleText(axsisY_Title);
}

void CentralWidget::addSeries(QVector<QPointF> pointVector, QString legendTitle, QString axsisX_Title, QString axsisY_Title)
{
    addSeries(pointVector, QXYSeries::SeriesType(panelWidget.getSeriesType()), legendTitle, axsisX_Title, axsisY_Title);
}

QString CentralWidget::getTitle()
{
    return chart.title();
}

QString CentralWidget::getDataXType()
{
    return chart.axisX()->titleText();
}
void CentralWidget::XYDefault()
{
    lineSeriesX.setPen(QPen(QColor(Qt::black)));
    lineSeriesX << QPointF( -(chart.maximumWidth() / 2), 0) << QPointF(chart.maximumWidth() / 2, 0);
    lineSeriesX.setName("X");
    chart.addSeries(&lineSeriesX);

    lineSeriesY.setPen(QPen(QColor(Qt::black)));
    lineSeriesY << QPointF( 0, -(chart.maximumWidth() / 2)) << QPointF(0, chart.maximumWidth() / 2);
    lineSeriesY.setName("Y");

    chart.addSeries(&lineSeriesY);
}

void CentralWidget::createMenu()
{
//    pMenuFile = new QMenu("File");
    menuFile.setTitle("File");
    menuFile.addAction(QIcon(":/save_as"), "save as BMP", this, SLOT(slotSaveBMP()));

    menuFile.addSeparator();

//    menuFile.addAction(QIcon(":/exit"), "Exit", QApplication::instance(), SLOT(close()));

//    pMenuAbout= new QMenu("About");
//    pMenuAbout->addAction(QIcon(":/author"),"Author", this, SLOT(slotAuthor()));
//    pMenuAbout->addAction(QIcon(":/qt_logo"), "About Qt", QApplication::instance(), SLOT(aboutQt()));

//    pMenuGraph = new QMenu("Graph");
//    pMenuGraph->addAction(QIcon(":/graph"), "Plot the graph", this, SLOT(slotPlotGraph()));
//    pMenuGraph->setDisabled(true);

    this->menuBar()->addMenu(&menuFile);
//    this->menuBar()->addMenu(pMenuGraph);
//    this->menuBar()->addMenu(pMenuAbout);
}

void CentralWidget::slotSetTheme(int theme)
{
    chartView.chart()->setTheme(QChart::ChartTheme(theme));
    lineSeriesX.setPen(QPen(QColor(Qt::black)));
    lineSeriesY.setPen(QPen(QColor(Qt::black)));
    chart.setDropShadowEnabled(true);
}

void CentralWidget::slotSetLegentPosition(int position)
{
    switch (position) {
    case 0:
        chart.legend()->setAlignment(Qt::AlignTop);
        break;
    case 1:
        chart.legend()->setAlignment(Qt::AlignRight);
        break;
    case 2:
        chart.legend()->setAlignment(Qt::AlignBottom);
        break;
    case 3:
        chart.legend()->setAlignment(Qt::AlignLeft);
        break;
    default:
        break;
    }
}

void CentralWidget::slotSetTitle(QString title)
{
    chart.setTitle(title);
}

void CentralWidget::slotAnimation(bool value)
{
    if(value)
        chart.setAnimationOptions(QChart::AllAnimations);
    else
        chart.setAnimationOptions(QChart::NoAnimation);
}

void CentralWidget::slotAntialiasing(bool value)
{
    if(value)
        chartView.setRenderHint(QPainter::HighQualityAntialiasing);
    else
        chartView.setRenderHint(QPainter::HighQualityAntialiasing, false);
}

void CentralWidget::slotSetTcickCountY(int value)
{
    QValueAxis* axis = static_cast<QValueAxis*>(chart.axisY());
    axis->setTickCount(value);
}

void CentralWidget::slotSaveBMP()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    ("Save as BMP"),
                                                    QDir::rootPath() + chart.title(),
                                                    "Images (*.bmp);;All files (*.*)");
    if(fileName != "")
    {
        QPixmap pix = chartView.grab();
        pix.toImage().save(fileName, "BMP");
    }
}

void CentralWidget::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    emit signalCloseWindow(this);
}

void CentralWidget::slotSetTcickCountX(int value)
{
    QValueAxis* axis = static_cast<QValueAxis*>(chart.axisX());
    axis->setTickCount(value);
}
