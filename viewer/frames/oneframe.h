#ifndef ONEFRAME_H
#define ONEFRAME_H

#include <QList>
#include <QDebug>

struct evPoint
{
    quint8 x = 0;
    quint8 y = 0;
    quint16 tot = 0;

    evPoint(quint8 x, quint8 y, quint16 tot) {
        this->x = x;
        this->y = y;
        this->tot = tot;
    }

};

typedef struct evPoint ePoint;
typedef QList<ePoint> cluster;

class OneFrame
{
public:
    OneFrame(uint number);

    void setThreshold_energy        (double value);
    void setExposure_time           (double value);

    void appendEPoint(ePoint point);
    void appendEPoint(quint8 x, quint8 y, quint16 tot);

    void addCluster();
    void addEPoint(cluster &inClaster, quint8 x, quint8 y, quint16 tot);

    quint16 getClusterCount();
    quint16 getClusterLenght(uint clusterNumber);
    uint getEventCountInCluster(uint clusterNumber);
    const QList<cluster>&
         getList();

    const ePoint &getEPoint(uint clusterNumber, uint eventNumber);

    void    clear();

private:
    uint number              = 0;
    double threshold_energy = 0;
    double exposure_time    = 0; //спросить у Влада за параметры в скобках

    QList<cluster> list;

};

#endif // ONEFRAME_H
