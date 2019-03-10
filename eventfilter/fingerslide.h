#ifndef FINGERSLIDE_H
#define FINGERSLIDE_H

#include <QObject>

#include <QEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <QGraphicsSceneMoveEvent>
#include <QGraphicsSceneMouseEvent>
//#include <QGraphicsSceneWheelEvent>

class FingerSlide : public QObject
{
    Q_OBJECT
public:
    FingerSlide(QObject *parent = nullptr);
    ~FingerSlide();

private:
//координаты курсора
    int x, y;

    int preX, preY;
    int stepX, stepY;

    bool eventFilterScene(QObject* object, QEvent* event);
    bool eventFilterViewport(QObject* object, QEvent* event);

    QPointF point, rectPoint;
protected:
    virtual bool eventFilter(QObject* object, QEvent* event);

//имитация touchpada'а
    void slide(QObject* object, QEvent* event);


signals:
    void signalRelease();

    void signalWheel(int);

    void signalMousePos(QPointF);
    void siganlRect(QRect);
    void signalRectMove(QPoint);

    void signalCreateRectItem(QGraphicsRectItem*);

};

#endif // FINGERSLIDE_H
