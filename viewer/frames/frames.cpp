#include "frames.h"
#include "../viewer_widget.h"

#include <QFile>
#include <QApplication>
#include <QFileInfo>
#include <QVector>

#include "iostream"

Frames::Frames(QObject *parent) : QObject(parent)
{

}

void Frames::addFrame(int number)
{
    OneFrame newFrame(number);
    list.append(newFrame);
}

void Frames::appendCluster()
{
    list.last().addCluster();
}
void Frames::appendEPoint(ePoint point)
{
    list.last().appendEPoint(point);
}

void Frames::appendEPoint(quint8 x, quint8 y, int tot)
{
    list.last().appendEPoint(x, y, tot);
}

int Frames::getFrameCount()
{
    return int(list.length());
}

int Frames::getClusterCount(int frameNumber)
{
    if(frameNumber > list.length() - 1)
    {
        std::cout << "error in " << Q_FUNC_INFO << __FILE__ << "line: " << __LINE__;
        exit(1);
    }

    OneFrame frame = list.at(int(frameNumber));
    return frame.getClusterCount();
}

int Frames::getClusterLenght(int frameNumber, int clusterNumber)
{
    OneFrame frame = list.at(int(frameNumber));
    return frame.getClusterLenght(clusterNumber);
}

int Frames::getEventCountInCluster(int frameNumber, int clusterNumber)
{
    OneFrame frame = list.at(int(frameNumber));
    return frame.getEventCountInCluster(clusterNumber);
}

ePoint Frames::getEPoint(int frameNumber, int clusterNumber, int eventNumber) const
{
    OneFrame frame = list.at(int(frameNumber));
    return frame.getEPoint(clusterNumber, eventNumber);
}

void Frames::setFile(QString path)
{
    QFile file(path);
    file.open(QFile::ReadOnly);
    QFileInfo fileInfo(path);

    QString     line;
    QStringList lineSplit;
    int         countLine = 0;

    while(!file.atEnd())
    {
        line = file.readLine();
        countLine++;
        QApplication::processEvents();

        if(line.startsWith("Frame"))
        {
            lineSplit = line.split(' ');
            addFrame(uint(lineSplit.at(1).toInt()));
            emit signalFrameCreated(countLine);

        }
        else if(line.startsWith("["))
        {
            //добавляем кластер
            appendCluster();
            line = line.remove(" ").remove("\r\n");
            QStringList lineList;
            foreach (QString str, line.split("]["))
                lineList << str.remove('[').remove(']');

            //заполняем кластер
            int i = 0;
            foreach (QString str, lineList)
            {
                i++;
                QStringList point = str.split(",");
                int tot = int(point.at(2).toInt());

                appendEPoint(quint8(point.at(0).toInt()),
                             quint8(point.at(1).toInt()),
                             tot);
            }
        }
    }
    file.close();

    emit signalFramesCreated();
}

//void Frames::resetClusterRange()
//{
//    clusterRangeBegin = 0;
//    clusterRangeEnd   = std::numeric_limits<quint8>::max();;
//}

//void Frames::resetTotRange()
//{
//    totRangeBegin   = 0;
//    totRangeEnd     = std::numeric_limits<int>::max();
//}

//void Frames::setMediPix(bool enable)
//{
//    if(enable){
//        mediPix = true;
//        timePix = false;
//    }
//    else {
//        mediPix = false;
//        timePix = true;
//    }
//}

//void Frames::setTimePix(bool enable)
//{
//    setMediPix(!enable);
//}

//bool Frames::isMediPix()
//{
//    if(mediPix)
//        return true;

//    return false;
//}

//bool Frames::isTimePix()
//{
//    if(timePix)
//        return true;

//    return false;
//}

void Frames::clear()
{
    foreach (OneFrame oneFrame, list)
        oneFrame.clear();

    list.clear();

//    resetClusterRange();
//    resetTotRange();
}

const QList<OneFrame> &Frames::getList()
{
    return list;
}

bool Frames::clusterInRange(int clusterLength, int clusterRangeBegin, int clusterRangeEnd)
{
    if(clusterLength >= clusterRangeBegin && clusterLength <= clusterRangeEnd)
        return true;

    return false;
}

bool Frames::totInRange(int frameNumber, int clusterNumber, int totRangeBegin, int totRangeEnd)
{
    OneFrame frame = list.at(int(frameNumber));
    foreach (ePoint point, frame.getList().at(int(clusterNumber)))
        if(point.tot >= totRangeBegin && point.tot <= totRangeEnd)
            return true;

    return false;
}

QList<ePoint> Frames::getListTotInRange(int frameNumber, int clusterNumber,
                                        int totRangeBegin, int totRangeEnd) const
{
    OneFrame frame = list.at(int(frameNumber));
    QList<ePoint> listePoint;
    listePoint.clear();

    foreach (ePoint point, frame.getList().at(int(clusterNumber)))
        if(point.tot >= totRangeBegin && point.tot <= totRangeEnd)
            listePoint << point;

    return listePoint;
}

//int Frames::getClusterRangeBegin() const
//{
//    return clusterRangeBegin;
//}

//void Frames::setClusterRangeBegin(const int &value)
//{
//    clusterRangeBegin = value;
//}

//int Frames::getClusterRangeEnd() const
//{
//    return clusterRangeEnd;
//}

//void Frames::setClusterRangeEnd(const int &value)
//{
//    clusterRangeEnd = value;
//}

//int Frames::getTotRangeBegin() const
//{
//    return totRangeBegin;
//}

//void Frames::setTotRangeBegin(const int &value)
//{
//    totRangeBegin = value;
//}

//int Frames::getTotRangeEnd() const
//{
//    return totRangeEnd;
//}

//void Frames::setTotRangeEnd(const int &value)
//{
//    totRangeEnd = value;
//}

QVector<int> Frames::getClustersLenghtList()
{
    QVector<int> lenghtList;
    for (int frameNumber = 0; frameNumber < getFrameCount(); ++frameNumber)
        for (int clusterNumber = 0; clusterNumber < getClusterCount(frameNumber); ++clusterNumber)
        {
            int lenght = getClusterLenght(frameNumber, clusterNumber);
            if(!lenghtList.contains(lenght))
                lenghtList << lenght;
        }

    std::sort(lenghtList.begin(), lenghtList.end());

    return lenghtList;
}

QVector<QPointF> Frames::getClusterVectorTot(int clusterLenght)
{
    QVector<QPointF> vector;
    //key = tot, value = count
    QMap<int, int> map;

    for (int frameNumber = 0; frameNumber < getFrameCount(); ++frameNumber)
        for (int clusterNumber = 0; clusterNumber < getClusterCount(frameNumber); ++clusterNumber)
            if(getClusterLenght(frameNumber, clusterNumber) == clusterLenght || clusterLenght == 0)
                for (int eventNumber = 0; eventNumber < getEventCountInCluster(frameNumber, clusterNumber); ++eventNumber)
                {
                    int key = getEPoint(frameNumber, clusterNumber, eventNumber).tot;
                    if(map.value(key) == 0)
                        map.insert(key, 1);
                    else
                        map[key] = map.value(key) + 1;

                }
    QMapIterator<int, int> i(map);

    while (i.hasNext())
    {
        i.next();
        vector.append(QPointF(i.key(), i.value()));
    }

    return vector;
}

QVector<QPointF> Frames::getClusterVector()
{
    QVector<QPointF> vector;
    //key = cluster, value = count
    QMap<int, int> map;

    for (int frameNumber = 0; frameNumber < getFrameCount(); ++frameNumber)
        for (int clusterNumber = 0; clusterNumber < getClusterCount(frameNumber); ++clusterNumber)
        {
            int key = getClusterLenght(frameNumber, clusterNumber);
            if(map.value(key) == 0)
                map.insert(key, 1);
            else
                map[key] = map.value(key) + 1;
        }

    QMapIterator<int, int> i(map);

    while (i.hasNext())
    {
        i.next();
        vector.append(QPointF(i.key(), i.value()));
    }

    return vector;
}

QVector<int> Frames::getTotLenghtList()
{
    QVector<int> lenghtList;
    for (int frameNumber = 0; frameNumber < getFrameCount(); ++frameNumber)
        for (int clusterNumber = 0; clusterNumber < getClusterCount(frameNumber); ++clusterNumber)
            for (int eventNumber = 0; eventNumber < getEventCountInCluster(frameNumber, clusterNumber); ++eventNumber) {
                int tot = getEPoint(frameNumber, clusterNumber, eventNumber).tot;
                if(!lenghtList.contains(tot))
                    lenghtList << tot;
            }

    std::sort(lenghtList.begin(), lenghtList.end());

    return lenghtList;
}

//bool Frames::getAllTotInCluster() const
//{
//    return allTotInCluster;
//}

//bool Frames::isAllTotInCluster() const
//{
//    return getAllTotInCluster();
//}

//void Frames::setAllTotInCluster(bool value)
//{
//    allTotInCluster = value;
//}

//void Frames::slotSetMediPix(bool checked)
//{
//    setMediPix(checked);
//}
