#ifndef ONEFRAME_H
#define ONEFRAME_H

#include <QList>
#include <QDebug>

struct evPoint
{
    int x = 0;
    int y = 0;
    int tot = 0;

    evPoint(int x, int y, int tot) {
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
    OneFrame(int number);

    void setThreshold_energy        (double value);
    void setExposure_time           (double value);

    void appendEPoint(ePoint point);
    void appendEPoint(int x, int y, int tot);

    void addCluster();
    void addEPoint(cluster &inClaster, int x, int y, int tot);

    int getClusterCount();
    int getClusterLenght(int clusterNumber);
    int getEventCountInCluster(int clusterNumber);
    const QList<cluster>&
         getList();

    const ePoint &getEPoint(int clusterNumber, int eventNumber);

    void    clear();

private:
    int number              = 0;
    double threshold_energy = 0;
    double exposure_time    = 0; //спросить у Влада за параметры в скобках

    QList<cluster> list;

};

#endif // ONEFRAME_H
