#ifndef FRAMES_H
#define FRAMES_H

#include "oneframe.h"

#include <limits>
#include <QObject>

class Frames : public QObject
{
    Q_OBJECT

public:
    Frames(QObject *parent = nullptr);

    void    addFrame        (int number);
    void    appendCluster   ();

    void    appendEPoint    (ePoint point);
    void    appendEPoint    (quint8 x, quint8 y, int tot);

    int getFrameCount();
    int getClusterCount(int frameNumber);
    int getClusterLenght(int frameNumber, int clusterNumber);
    int getEventCountInCluster(int frameNumber, int clusterNumber);

    ePoint getEPoint(int frameNumber, int clusterNumber, int eventNumber) const;

    void    setFile         (QString path);

//    void    resetClusterRange();
//    void    resetTotRange   ();

//    void    setMediPix      (bool enable = true);
//    void    setTimePix      (bool enable = true);

//    bool    isMediPix       ();
//    bool    isTimePix       ();

    void    clear           ();
    const QList<OneFrame>&  getList();

    bool    clusterInRange  (int clusterLength,
                             int clusterRangeBegin, int clusterRangeEnd);
    bool    totInRange      (int frameNumber, int clusterNumber,
                             int totRangeBegin, int totRangeEnd);

    QList<ePoint> getListTotInRange(int frameNumber, int clusterNumber,
                                    int totRangeBegin, int totRangeEnd) const;

//    int  getClusterRangeBegin() const;
//    void    setClusterRangeBegin(const int &value);

//    int getClusterRangeEnd() const;
//    void    setClusterRangeEnd(const int &value);

//    int getTotRangeBegin() const;
//    void setTotRangeBegin(const int &value);

//    int getTotRangeEnd() const;
//    void setTotRangeEnd(const int &value);

    QVector<int> getClustersLenghtList();
    //получение вектора кол-ва тотов с кластера заданного размера
    QVector<QPointF> getClusterVectorTot(int clusterLenght);
    //получение вектора кол-ва кластеров
    QVector<QPointF> getClusterVector();
    QVector<int> getTotLenghtList();

//    bool getAllTotInCluster() const;
//    bool isAllTotInCluster() const;
//    void setAllTotInCluster(bool value);

private:
    QList<OneFrame> list;

//    int  clusterRangeBegin   = 0;
//    int  clusterRangeEnd     = std::numeric_limits<int>::max();

//    int totRangeBegin       = 0;
//    int totRangeEnd         = std::numeric_limits<int>::max();

//    bool    mediPix             = true;
//    bool    timePix             = false;
//    bool    allTotInCluster     = true;

    //максимальное и минимальное значение ToT во всех фреймах
//    int    maxTot           = 0;
//    int    maxCluster       = 0;
//    int    minTot           = std::numeric_limits<int>::max();
//    int    minCluster       = std::numeric_limits<int>::max();

private slots:
//    void    slotSetMediPix(bool checked);
//    void    slotSetAllTotInCluster(bool checked);

signals:
    void signalFrameCreated(int);
    void signalFramesCreated();
};

#endif // FRAMES_H
