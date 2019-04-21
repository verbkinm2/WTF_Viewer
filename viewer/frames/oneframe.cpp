#include "oneframe.h"

OneFrame::OneFrame(int number)
{
    this->number = number;
}

void OneFrame::setThreshold_energy(double value)
{
    threshold_energy = value;
}

void OneFrame::setExposure_time(double value)
{
    exposure_time = value;
}

void OneFrame::appendEPoint(ePoint point)
{
    list.last().append(point);
}

void OneFrame::appendEPoint(int x, int y, int tot)
{
    appendEPoint(ePoint(x, y, tot));
}

void OneFrame::addCluster()
{
    cluster newClaster;
    list.append(newClaster);

//    return list.last();
}

void OneFrame::addEPoint(cluster &inClaster, int x, int y, int tot)
{
    inClaster.append(ePoint(x,y,tot));

//    return inClaster.last();
}

int OneFrame::getClusterCount()
{
    return int(list.length());
}

int OneFrame::getClusterLenght(int clusterNumber)
{
    return  int(list.at(int(clusterNumber)).length());
}

int OneFrame::getEventCountInCluster(int clusterNumber)
{
    if(clusterNumber > list.length() - 1)
    {
        qDebug() << "error in " << Q_FUNC_INFO << __FILE__ << "line: " << __LINE__;
        exit(1);
    }

    return int(list.at(int(clusterNumber)).length());
}

const QList<cluster> &OneFrame::getList()
{
    return list;
}

const ePoint &OneFrame::getEPoint(int clusterNumber, int eventNumber)
{
    if(clusterNumber > list.length() - 1 ||
           eventNumber > list.at(int(clusterNumber)).length())
    {
        qDebug() << "error in " << Q_FUNC_INFO << __FILE__ << "line: " << __LINE__;
        exit(1);
    }

    return list.at(int(clusterNumber)).at(int(eventNumber));
}

void OneFrame::clear()
{
    foreach (cluster clust, list)
        clust.clear();
}
