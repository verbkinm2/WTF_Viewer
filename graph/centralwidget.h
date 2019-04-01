#ifndef CENTRALWIDGET_H
#define CENTRALWIDGET_H

#include "chart.h"
#include "chartview.h"
#include "panelwidget.h"

#include <QMainWindow>
#include <QLineSeries>
#include <QScatterSeries>
#include <QSplineSeries>
#include <QXYSeries>
#include <QValueAxis>
#include <QChart>
#include <QChartView>
#include <QValueAxis>
#include <QMenu>
#include <QMenuBar>
#include <QLabel>

#include <QHBoxLayout>

QT_CHARTS_USE_NAMESPACE

class CentralWidget : public QMainWindow
{
    Q_OBJECT
public:
    CentralWidget(QWidget *parent = nullptr);
    ~CentralWidget();

    void addSeries(QVector<QPointF> pointVector, QXYSeries::SeriesType type = QXYSeries::SeriesTypeLine,
                   QString legendTitle = "", QString axsisX_Title = "X", QString axsisY_Title = "Y");
    void addSeries(QVector<QPointF> pointVector, QString legendTitle = "",
                   QString axsisX_Title = "X", QString axsisY_Title = "Y");

    QString getTitle();
    QString getDataXType();

private:
    QWidget        centralWidget;
    QLabel         statusBarWidget;

    ChartView      chartView;
    Chart          chart;

    QLineSeries    lineSeriesX;
    QLineSeries    lineSeriesY;

    QHBoxLayout    layout;

    PanelWidget    panelWidget;

    double maxX = 0;
    double maxY = 0;
    double minX = std::numeric_limits<int>::max();
    double minY = std::numeric_limits<int>::max();

    QMenu         menuFile, menuView;

    //ось X и Y на всем полотне
    void XYDefault();

    void createMenu();

    double findMaxX(QXYSeries* series);
    double findMinX(QXYSeries* series);
    double findMaxY(QXYSeries* series);
    double findMinY(QXYSeries* series);

signals:

private slots:
    void    slotSetTheme            (int theme);
    void    slotSetLegentPosition   (int position);
    void    slotSetTitle            (QString title);
    void    slotAnimation           (bool value);
    void    slotAntialiasing        (bool value);

    void    slotSetTcickCountX      (int value);
    void    slotSetTcickCountY      (int value);

    void    slotSaveBMP             ();

    void    slotReRange             ();

    void    slotResetZoomAndPosition();

    void    slotViewXYCoordinate    (QPointF point);
public slots:

protected:
    virtual void closeEvent(QCloseEvent *event);

signals:
    void signalCloseWindow(QObject*);
};

#endif // CENTRALWIDGET_H
