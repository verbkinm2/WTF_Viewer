#ifndef CHART_H
#define CHART_H

#include <QValueAxis>
#include <QtCharts/QChart>

QT_BEGIN_NAMESPACE
class QGestureEvent;
QT_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE

//![1]
class Chart : public QChart
//![1]
{
public:
    explicit Chart(QGraphicsItem *parent = nullptr, Qt::WindowFlags wFlags = 0);
    ~Chart();

protected:
    bool sceneEvent(QEvent *event);

private:
    bool gestureEvent(QGestureEvent *event);

};

#endif // CHART_H
