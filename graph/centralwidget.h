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
    QWidget        centralWidget;// = nullptr;
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

    QMenu         menuFile;//       = nullptr;

    //ось X и Y на всем полотне
    void XYDefault();

    void createMenu();

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


public slots:

    // QWidget interface
protected:
    virtual void closeEvent(QCloseEvent *event);

signals:
    void signalCloseWindow(QObject*);
};

#endif // CENTRALWIDGET_H
