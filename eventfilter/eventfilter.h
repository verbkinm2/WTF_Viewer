#ifndef EVENTFILTER_H
#define EVENTFILTER_H

#include <QObject>

class EventFilter : public QObject
{
public:
    EventFilter(QObject* parent = nullptr);

protected:
    virtual bool eventFilter(QObject*, QEvent*);
};


#endif // EVENTFILTER_H
