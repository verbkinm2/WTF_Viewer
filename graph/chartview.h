#ifndef CHARTVIEW_H
#define CHARTVIEW_H

#include <QtCharts/QChartView>
#include <QtWidgets/QRubberBand>

#include <QValueAxis>

QT_CHARTS_USE_NAMESPACE

//![1]
class ChartView : public QChartView
//![1]
{
    Q_OBJECT
public:
    ChartView(QWidget *parent = nullptr);

    struct rangeAxis
    {
        rangeAxis(double min = std::numeric_limits<int>::min(), double max = 0) {
            this->min = min;
            this->max = max;

        }
        double min;
        double max;
    };

    struct rangeAxis rangeX, rangeY;

public slots:
    void slotResetZoomAndPosition();
//![2]
protected:
    bool vieportEvent(QEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void wheelEvent(QWheelEvent *event);
    bool event(QEvent* ev);

//![2]

private:
    //точка с координатами Charts, где была нажата правая кнопка
    QPointF lastMousePos;

signals:
    void signalMousePosition(QPointF);
};

#endif
