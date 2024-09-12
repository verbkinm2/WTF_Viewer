#include "oneframe.h"

OneFrame::OneFrame() : empty_ePoint({0,0,0}), _number(-1), _threshold_energy(-1), _exposure_time(-1)
{

}

bool OneFrame::createFromStrings(QStringList buff)
{
    for(auto &string : buff)
    {
        if(string.startsWith("Frame"))
            setFrameProperties(string);
        else if(string.startsWith("["))
        {
            appendCluster();
            setClusterProperies(string);
        }
    }
    if(_number == -1)
    {
        _vectorOfCluster.clear();
        return false;
    }
    return  true;
}

void OneFrame::setThreshold_energy(double value) noexcept
{
    _threshold_energy = value;
}
void OneFrame::setExposure_time(double value) noexcept
{
    _exposure_time = value;
}
void OneFrame::setFrameNumber(int number) noexcept
{
    _number = number;
}

void OneFrame::appendEPoint(const size_t &clusterNumber, const ePoint &point)
{
    _vectorOfCluster.at(clusterNumber).push_back(point);
}

void OneFrame::appendCluster()
{
    _vectorOfCluster.push_back(cluster());
}

size_t OneFrame::getClusterCount() const
{
    return _vectorOfCluster.size();
}

size_t OneFrame::getClusterLenght(size_t clusterNumber) const
{
    try
    {
        return _vectorOfCluster.at(clusterNumber).size();
    }
    catch (std::out_of_range&)
    {
        return 0;
    }
}

size_t OneFrame::getEventCountInCluster(size_t clusterNumber) const
{
    try
    {
        return _vectorOfCluster.at(clusterNumber).size();
    }
    catch (std::out_of_range&)
    {
        return 0;
    }
}

const std::vector<OneFrame::cluster> &OneFrame::getClustersVector() const
{
    return _vectorOfCluster;
}

const OneFrame::ePoint &OneFrame::getEPoint(size_t clusterNumber, size_t eventNumber) const
{
    try
    {
        return _vectorOfCluster.at(clusterNumber).at(eventNumber);
    }
    catch (std::out_of_range&)
    {
        return empty_ePoint;
    }
}

OneFrame::ePoint &OneFrame::getPointer_to_EPoint(size_t clusterNumber, size_t eventNumber)
{
    return _vectorOfCluster[clusterNumber][eventNumber];
}

void OneFrame::setFrameProperties(QString &string)
{
    QStringList stringSplit = string.split(' ');
    try
    {
        setFrameNumber(stringSplit.at(1).toInt());
    }
    catch(std::out_of_range&)
    {
        return;
    }
}

void OneFrame::setClusterProperies(QString &string)
{
    string.remove(" ");
    string.remove("\r\n");
    string.remove(0, 1);
    string.chop(1);

    QStringList listFromString;
    for (auto &str : string.split("]["))
        listFromString << str;

    for(auto &str : listFromString)
    {
        QStringList point = str.split(",");
        if(point.length() == 3)
            appendEPoint(_vectorOfCluster.size() - 1, {point[0].toULongLong(), point[1].toULongLong(), point[2].toDouble()});
    }
}
