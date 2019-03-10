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

    void    addFrame        (uint number);
    void    appendCluster   ();

    void    appendEPoint    (ePoint point);
    void    appendEPoint    (quint8 x, quint8 y, quint16 tot);

    uint    getFrameCount   ();
    uint    getClusterCount (uint frameNumber);
    quint8  getClusterLenght(uint frameNumber, quint8 clusterNumber);
    uint    getEventCountInCluster(uint frameNumber, uint clusterNumber);
//    const quint16    &getMax();
//    const quint16    &getMin();
     ePoint getEPoint(uint frameNumber, uint clusterNumber, uint eventNumber) const;

    void    setFile         (QString path);

    void    setClusterRange (quint8 begin, quint8 end);
    void    setTotRange     (quint16 begin, quint16 end);

    void    resetClusterRange();
    void    resetTotRange   ();

    void    setMediPix      (bool enable = true);
    void    setTimePix      (bool enable = true);

    bool    isMediPix       ();
    bool    isTimePix       ();

    void    clear           ();
    const QList<OneFrame>&  getList();

    bool    clusterInRange  (quint8 clusterLength);
    bool    totInRange      (uint frameNumber, quint8 clusterNumber);

    quint16  getClusterRangeBegin() const;
    void    setClusterRangeBegin(const quint16 &value);

    quint16 getClusterRangeEnd() const;
    void    setClusterRangeEnd(const quint16 &value);

    quint16 getMaxTot() const;
    quint16 getMaxCluster() const;

    quint16 getTotRangeBegin() const;
    void setTotRangeBegin(const quint16 &value);

    quint16 getTotRangeEnd() const;
    void setTotRangeEnd(const quint16 &value);

private:
    QList<OneFrame> list;

    quint16  clusterRangeBegin   = 0;
    quint16  clusterRangeEnd     = std::numeric_limits<quint8>::max();

    quint16 totRangeBegin       = 0;
    quint16 totRangeEnd         = std::numeric_limits<quint16>::max();

    bool    mediPix             = true;
    bool    timePix             = false;

    //максимальное и минимальное значение ToT во всех фреймах
    quint16    maxTot           = 0;
    quint16    maxCluster       = 0;

signals:
    void signalFrameCreated(int);
    void signalFramesCreated();
};

#endif // FRAMES_H
