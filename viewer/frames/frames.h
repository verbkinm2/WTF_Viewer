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

    quint16 getFrameCount();
    quint16 getClusterCount(quint16 frameNumber);
    quint16 getClusterLenght(quint16 frameNumber, quint16 clusterNumber);
    uint    getEventCountInCluster(uint frameNumber, uint clusterNumber);
//    const quint16    &getMax();
//    const quint16    &getMin();
     ePoint getEPoint(uint frameNumber, uint clusterNumber, uint eventNumber) const;

    void    setFile         (QString path);

    void    setClusterRange (quint16 begin, quint16 end);
    void    setTotRange     (quint16 begin, quint16 end);

    void    resetClusterRange();
    void    resetTotRange   ();

    void    setMediPix      (bool enable = true);
    void    setTimePix      (bool enable = true);

    bool    isMediPix       ();
    bool    isTimePix       ();

    void    clear           ();
    const QList<OneFrame>&  getList();

    bool    clusterInRange  (quint16 clusterLength);
    bool    totInRange      (uint frameNumber, quint16 clusterNumber);

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

    quint16 getMinTot() const;

    quint16 getMinCluster() const;

private:
    QList<OneFrame> list;

    quint16  clusterRangeBegin   = 0;
    quint16  clusterRangeEnd     = std::numeric_limits<quint16>::max();

    quint16 totRangeBegin       = 0;
    quint16 totRangeEnd         = std::numeric_limits<quint16>::max();

    bool    mediPix             = true;
    bool    timePix             = false;

    //максимальное и минимальное значение ToT во всех фреймах
    quint16    maxTot           = 0;
    quint16    maxCluster       = 0;
    quint16    minTot           = std::numeric_limits<quint16>::max();
    quint16    minCluster       = std::numeric_limits<quint16>::max();

private slots:
    void    slotSetMediPix(bool checked);

signals:
    void signalFrameCreated(int);
    void signalFramesCreated();
};

#endif // FRAMES_H
