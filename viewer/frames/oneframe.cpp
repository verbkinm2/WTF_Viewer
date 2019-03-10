#include "oneframe.h"

OneFrame::OneFrame(uint number)
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

void OneFrame::appendEPoint(quint8 x, quint8 y, quint16 tot)
{
    appendEPoint(ePoint(x, y, tot));
}

void OneFrame::addCluster()
{
    cluster newClaster;
    list.append(newClaster);

//    return list.last();
}

void OneFrame::addEPoint(cluster &inClaster, quint8 x, quint8 y, quint16 tot)
{
    inClaster.append(ePoint(x,y,tot));

//    return inClaster.last();
}

quint16 OneFrame::getClusterCount()
{
    return quint16(list.length());
}

quint16 OneFrame::getClusterLenght(uint clusterNumber)
{
    return  quint16(list.at(int(clusterNumber)).length());
}

uint OneFrame::getEventCountInCluster(uint clusterNumber)
{
    if(clusterNumber > uint(list.length() - 1)){
        qDebug() << "error in " << Q_FUNC_INFO << __FILE__ << "line: " << __LINE__;
        exit(1);
    }

    return uint(list.at(int(clusterNumber)).length());
}

const QList<cluster> &OneFrame::getList()
{
    return list;
}

const ePoint &OneFrame::getEPoint(uint clusterNumber, uint eventNumber)
{
    if(clusterNumber > uint(list.length() - 1) ||
           eventNumber > uint(list.at(int(clusterNumber)).length()))
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
