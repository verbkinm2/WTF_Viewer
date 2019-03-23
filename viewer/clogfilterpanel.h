#ifndef CLOGFILTERPANEL_H
#define CLOGFILTERPANEL_H

#include <QWidget>
#include <QEvent>

#include "frames/frames.h"

namespace Ui {
class ClogFilterPanel;
}

class ClogFilterPanel : public QWidget
{
    Q_OBJECT

public:
    explicit ClogFilterPanel(QWidget *parent = nullptr);
    ~ClogFilterPanel();

    void    setClusterRange(QVector<quint16> vector);
    void    setTotRange(QVector<quint16> vector);

//    void    setClusterBegin(QString value);
//    void    setClusterEnd(QString value);

    quint16 getClusterBegin() const;
    quint16 getClusterEnd() const;

    quint16 getTotBegin() const;
    quint16 getTotEnd() const;

//    void    setLabelClusterMaxMin(quint16 max, quint16 min);
//    void    setLabelTotMaxMin(quint16 max, quint16 min);

    bool    isClusterEnable();
    bool    isTotEnable();
    bool    isAllTotInCluster();

    bool    isMediPix();

private:
    Ui::ClogFilterPanel *ui;

    QStringList clusterListModel, clusterListBegin, clusterListEnd;
    QString clusterBeginLast, clusterEndLast;

    QStringList totListModel, totListBegin, totListEnd;
    QString totBeginLast, totEndLast;

    void    disconnectSignals       ();
    void    connectSignals          ();

//    void    clusterEndSetList       ();
//    Frames* frames = nullptr;

private slots:
    void    slotEnableRange         ();

//    void    slotPixGroupFilter      (bool checked);
    void    slotDates               (QString value);

public slots:
    void    slotApplyFilter         ();

signals:
//    void    signalRangeChanged      (QObject* obj, quint16 value);
    void    signalApplyFilter       ();

    void    signalPixGroupMidiPixSet(bool);
//    void    signalAllTotInClusterToggled(bool);

protected:

    virtual void keyReleaseEvent(QKeyEvent *event);
    bool event(QEvent *event);
};

#endif // CLOGFILTERPANEL_H
