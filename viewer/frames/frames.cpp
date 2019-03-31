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

void Frames::addFrame(uint number)
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

void Frames::appendEPoint(quint8 x, quint8 y, quint16 tot)
{
    list.last().appendEPoint(x, y, tot);
}

quint16 Frames::getFrameCount()
{
    return quint16(list.length());
}

quint16 Frames::getClusterCount(quint16 frameNumber)
{
    if(frameNumber > uint(list.length() - 1))
    {
        std::cout << "error in " << Q_FUNC_INFO << __FILE__ << "line: " << __LINE__;
        exit(1);
    }

    OneFrame frame = list.at(int(frameNumber));
    return frame.getClusterCount();
}

quint16 Frames::getClusterLenght(quint16 frameNumber, quint16 clusterNumber)
{
    OneFrame frame = list.at(int(frameNumber));
    return frame.getClusterLenght(clusterNumber);
}

quint16 Frames::getEventCountInCluster(quint16 frameNumber, quint16 clusterNumber)
{
    OneFrame frame = list.at(int(frameNumber));
    return frame.getEventCountInCluster(clusterNumber);
}

ePoint Frames::getEPoint(quint16 frameNumber, quint16 clusterNumber, quint16 eventNumber) const
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
            quint16 i = 0;
            foreach (QString str, lineList)
            {
                i++;
                QStringList point = str.split(",");
                quint16 tot = quint16(point.at(2).toInt());

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
//    totRangeEnd     = std::numeric_limits<quint16>::max();
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

bool Frames::clusterInRange(quint16 clusterLength, quint16 clusterRangeBegin, quint16 clusterRangeEnd)
{
    if(clusterLength >= clusterRangeBegin && clusterLength <= clusterRangeEnd)
        return true;

    return false;
}

bool Frames::totInRange(uint frameNumber, quint16 clusterNumber, quint16 totRangeBegin, quint16 totRangeEnd)
{
    OneFrame frame = list.at(int(frameNumber));
    foreach (ePoint point, frame.getList().at(int(clusterNumber)))
        if(point.tot >= totRangeBegin && point.tot <= totRangeEnd)
            return true;

    return false;
}

QList<ePoint> Frames::getListTotInRange(quint16 frameNumber, quint16 clusterNumber,
                                        quint16 totRangeBegin, quint16 totRangeEnd) const
{
    OneFrame frame = list.at(int(frameNumber));
    QList<ePoint> listePoint;
    listePoint.clear();

    foreach (ePoint point, frame.getList().at(int(clusterNumber)))
        if(point.tot >= totRangeBegin && point.tot <= totRangeEnd)
            listePoint << point;

    return listePoint;
}

//quint16 Frames::getClusterRangeBegin() const
//{
//    return clusterRangeBegin;
//}

//void Frames::setClusterRangeBegin(const quint16 &value)
//{
//    clusterRangeBegin = value;
//}

//quint16 Frames::getClusterRangeEnd() const
//{
//    return clusterRangeEnd;
//}

//void Frames::setClusterRangeEnd(const quint16 &value)
//{
//    clusterRangeEnd = value;
//}

//quint16 Frames::getTotRangeBegin() const
//{
//    return totRangeBegin;
//}

//void Frames::setTotRangeBegin(const quint16 &value)
//{
//    totRangeBegin = value;
//}

//quint16 Frames::getTotRangeEnd() const
//{
//    return totRangeEnd;
//}

//void Frames::setTotRangeEnd(const quint16 &value)
//{
//    totRangeEnd = value;
//}

QVector<quint16> Frames::getClustersLenghtList()
{
    QVector<quint16> lenghtList;
    for (quint16 frameNumber = 0; frameNumber < getFrameCount(); ++frameNumber)
        for (quint16 clusterNumber = 0; clusterNumber < getClusterCount(frameNumber); ++clusterNumber)
        {
            quint16 lenght = getClusterLenght(frameNumber, clusterNumber);
            if(!lenghtList.contains(lenght))
                lenghtList << lenght;
        }

    std::sort(lenghtList.begin(), lenghtList.end());

    return lenghtList;
}

QVector<QPointF> Frames::getClusterVectorTot(quint16 cluserLenght)
{
    QVector<QPointF> vector;
    //key = tot, value = count
    QMap<quint16, quint16> map;

    for (quint16 frameNumber = 0; frameNumber < getFrameCount(); ++frameNumber)
        for (quint16 clusterNumber = 0; clusterNumber < getClusterCount(frameNumber); ++clusterNumber)
            if(getClusterLenght(frameNumber, clusterNumber) == cluserLenght)
                for (quint16 eventNumber = 0; eventNumber < getEventCountInCluster(frameNumber, clusterNumber); ++eventNumber)
                {
                    quint16 key = getEPoint(frameNumber, clusterNumber, eventNumber).tot;
                    if(map.value(key) == 0)
                        map.insert(key, 1);
                    else
                        map[key] = map.value(key) + 1;

                }
    QMapIterator<quint16, quint16> i(map);

    while (i.hasNext())
    {
        i.next();
        vector.append(QPointF(i.key(), i.value()));
    }

    return vector;
}

QVector<quint16> Frames::getTotLenghtList()
{
    QVector<quint16> lenghtList;
    for (quint16 frameNumber = 0; frameNumber < getFrameCount(); ++frameNumber)
        for (quint16 clusterNumber = 0; clusterNumber < getClusterCount(frameNumber); ++clusterNumber)
            for (quint16 eventNumber = 0; eventNumber < getEventCountInCluster(frameNumber, clusterNumber); ++eventNumber) {
                quint16 tot = getEPoint(frameNumber, clusterNumber, eventNumber).tot;
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
