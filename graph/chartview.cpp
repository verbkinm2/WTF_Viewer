#include "chartview.h"
#include <QtGui/QMouseEvent>

#include <QDebug>

ChartView::ChartView(QWidget *parent) :
    QChartView(parent)
{
//    this->setMouseTracking(true);
    setRubberBand(QChartView::RectangleRubberBand);
//    setDragMode(QGraphicsView::ScrollHandDrag);
}

void ChartView::slotResetZoomAndPosition()
{
    chart()->zoomReset();
    chart()->axisX()->setRange(rangeX.min, rangeX.max);
    chart()->axisY()->setRange(rangeY.min, rangeY.max);
}
void ChartView::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::RightButton)
    {
        lastMousePos = event->pos();
//        chart()->setAnimationOptions(QChart::NoAnimation);
    }

    QChartView::mousePressEvent(event);
}

void ChartView::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::RightButton)
    {
        auto dPos = event->pos() - lastMousePos;
        chart()->scroll(-dPos.x(), dPos.y());

        lastMousePos = event->pos();
        event->accept();
    }
    emit signalMousePosition(chart()->mapToValue(event->pos()));

    QChartView::mouseMoveEvent(event);
}

void ChartView::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::RightButton)
    {
//        chart()->setAnimationOptions(QChart::SeriesAnimations);
        return;
    }

    QChartView::mouseReleaseEvent(event);
}

//![1]
void ChartView::keyPressEvent(QKeyEvent *event)
{
        switch (event->key()) {
    case Qt::Key_Plus:
        chart()->zoom(0.51);
        chart()->zoomIn();
        break;
    case Qt::Key_Minus:
        chart()->zoom(1.95);
        chart()->zoomOut();
        break;
//![1]
    case Qt::Key_Left:
        chart()->scroll(-10, 0);
        break;
    case Qt::Key_Right:
        chart()->scroll(10, 0);
        break;
    case Qt::Key_Up:
        chart()->scroll(0, 10);
        break;
    case Qt::Key_Down:
        chart()->scroll(0, -10);

        break;
    case Qt::Key_Escape:
        slotResetZoomAndPosition();
        break;
    default:
        QGraphicsView::keyPressEvent(event);
        break;
        }
}

void ChartView::wheelEvent(QWheelEvent *event)
{
    if(event->angleDelta().y() > 0){
        chart()->zoom(0.7);
        chart()->zoomIn();
    }
    else if(event->angleDelta().y() < 0)
    {
        chart()->zoom(1.5);
        chart()->zoomOut();
    }

    QChartView::wheelEvent(event);
}

bool ChartView::event(QEvent *ev)
{
    if(ev->type() == QEvent::Enter)
        this->setFocus();

    return QChartView::event(ev);
}
