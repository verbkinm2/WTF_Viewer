#ifndef PANELWIDGET_H
#define PANELWIDGET_H

#include <QWidget>
#include <QLineSeries>
#include <QSplineSeries>
#include <QScatterSeries>

#include <QSlider>
#include <QVXYModelMapper>

#include "customtablemodel.h"
//#include <QXYSeries>


#include <QtCharts/QChartView>


QT_CHARTS_USE_NAMESPACE

namespace Ui {
class PanelWidget;
}

class PanelWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PanelWidget(QWidget *parent = nullptr);
    ~PanelWidget();

    void    addSeriesList(QXYSeries* series);
    int     getSeriesType();

private:
    Ui::PanelWidget *ui;

    QList<QXYSeries*> seriesList;
    QList<CustomTableModel*> modelList;

    QXYSeries*      currentSeries = nullptr;

//    CustomTableModel* model = nullptr;

    QVXYModelMapper mapper;

    void    createTableData();

    //отключение всех необходимых элементов, если список графов пуст!
    void    setDisableUnits(bool value);


signals:
    void    signalChangeTheme(int);
    void    signalSetLegendPosition(int);
    void    signalSetTitile(QString);

    void    signalAnimation(bool);
    void    signalAntialiasing(bool);

    void    signalTickCountChangeX(int);
    void    signalTickCountChangeY(int);

private slots:
    void    slotSetSeriesPropery(int value);
    void    slotSetPointVisible(bool value);

    void    slotSetSeriesColor  ();
    void    slotSetSeriesType   (int value);
    void    slotHideSeries      (bool value);
    void    slotSetSeriesPenWidth(int value);

    void    slotDeleteSeries();

    void    slotSaveToCSV();
};

#endif // PANELWIDGET_H
