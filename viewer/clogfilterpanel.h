#ifndef CLOGFILTERPANEL_H
#define CLOGFILTERPANEL_H

#include <QWidget>
#include <QEvent>

namespace Ui {
class ClogFilterPanel;
}

class ClogFilterPanel : public QWidget
{
    Q_OBJECT

public:
    explicit ClogFilterPanel(QWidget *parent = nullptr);
    ~ClogFilterPanel();

    void    setClusterRange(int max, int min);
    void    setTotRange(int max, int min);

    void    setClusterBegin(int value);
    void    setClusterEnd(int value);

    quint16 getClusterBegin() const;
    quint16 getClusterEnd() const;

    quint16 getTotBegin() const;
    quint16 getTotEnd() const;

    void    setTotBegin(int value);
    void    setTotEnd(int value);

    void    setLabelClusterMaxMin(quint16 max, quint16 min);
    void    setLabelTotMaxMin(quint16 max, quint16 min);

    bool    isClusterEnable();
    bool    isTotEnable();

//    void    setCluster

private:
    Ui::ClogFilterPanel *ui;

private slots:
    void    slotEnableRange();
    void    slotCheckIntersection   (int value);

    void    slotPixGroupFilter(bool checked);

signals:
    void    signalRangeChanged      (QObject* obj, quint16 value);
    void    signalApplyFilter       ();
    void    signalRangeEnabled      (QObject*);
    void    signalRangeDisabled     (QObject*);

    void    signalPixGroupMidiPixSet(bool);

protected:

    virtual void keyReleaseEvent(QKeyEvent *event);


};

#endif // CLOGFILTERPANEL_H
