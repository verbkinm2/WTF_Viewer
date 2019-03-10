#include "frames.h"

#include <QFile>
#include <QApplication>
#include <QFileInfo>

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

uint Frames::getEventCountInCluster(uint frameNumber, uint clusterNumber)
{
    OneFrame frame = list.at(int(frameNumber));
    return frame.getEventCountInCluster(clusterNumber);
}

//const quint16 &Frames::getMax()
//{
//    return max;
//}

//const quint16 &Frames::getMin()
//{
//    return min;
//}

ePoint Frames::getEPoint(uint frameNumber, uint clusterNumber, uint eventNumber) const
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
                if(maxCluster < i) maxCluster = i;
                if(maxTot < tot) maxTot = tot;
            }
        }
    }
    file.close();
//    qDebug() << "MIN = " << min;
//    qDebug() << "MAX = " << max;
    emit signalFramesCreated();
}

void Frames::setClusterRange(quint16 begin, quint16 end)
{
    clusterRangeBegin = begin;
    clusterRangeEnd   = end;
}

void Frames::setTotRange(quint16 begin, quint16 end)
{
    totRangeBegin   = begin;
    totRangeEnd     = end;
}

void Frames::resetClusterRange()
{
    clusterRangeBegin = 0;
    clusterRangeEnd   = std::numeric_limits<quint8>::max();;
}

void Frames::resetTotRange()
{
    totRangeBegin   = 0;
    totRangeEnd     = std::numeric_limits<quint16>::max();
}

void Frames::setMediPix(bool enable)
{
    if(enable){
        mediPix = true;
        timePix = false;
    }
    else {
        mediPix = false;
        timePix = true;
    }
}

void Frames::setTimePix(bool enable)
{
    setMediPix(!enable);
}

bool Frames::isMediPix()
{
    if(mediPix)
        return true;

    return false;
}

bool Frames::isTimePix()
{
    if(timePix)
        return true;

    return false;
}

void Frames::clear()
{
    foreach (OneFrame oneFrame, list)
        oneFrame.clear();

    list.clear();
    maxCluster  = 0;
    maxTot      = 0;
}

const QList<OneFrame> &Frames::getList()
{
    return list;
}

bool Frames::clusterInRange(quint16 clusterLength)
{
    if(clusterLength >= clusterRangeBegin && clusterLength <= clusterRangeEnd)
        return true;

    return false;
}

bool Frames::totInRange(uint frameNumber, quint16 clusterNumber)
{
    OneFrame frame = list.at(int(frameNumber));
    foreach (ePoint point, frame.getList().at(int(clusterNumber)))
        if(point.tot >= totRangeBegin && point.tot <= totRangeEnd)
            return true;

    return false;
}

quint16 Frames::getClusterRangeBegin() const
{
    return clusterRangeBegin;
}

void Frames::setClusterRangeBegin(const quint16 &value)
{
    clusterRangeBegin = value;
}

quint16 Frames::getClusterRangeEnd() const
{
    return clusterRangeEnd;
}

void Frames::setClusterRangeEnd(const quint16 &value)
{
    clusterRangeEnd = value;
}

quint16 Frames::getMaxTot() const
{
    return maxTot;
}

quint16 Frames::getMaxCluster() const
{
    return maxCluster;
}

quint16 Frames::getTotRangeBegin() const
{
    return totRangeBegin;
}

void Frames::setTotRangeBegin(const quint16 &value)
{
    totRangeBegin = value;
}

quint16 Frames::getTotRangeEnd() const
{
    return totRangeEnd;
}

void Frames::setTotRangeEnd(const quint16 &value)
{
    totRangeEnd = value;
}
