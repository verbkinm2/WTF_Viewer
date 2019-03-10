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

    void    setClusterRangeMaximum(int value);
    void    setTotRangeMaximum(int value);

    void    setClusterEnd(int value);

    quint16 getClusterBegin() const;
    quint16 getClusterEnd() const;

    quint16 getTotBegin() const;
    quint16 getTotEnd() const;
    void    setTotEnd(int value);

    void    setLabelMax(quint16 value);

    bool    isClusterEnable();
    bool    isTotEnable();

//    void    setCluster

private:
    Ui::ClogFilterPanel *ui;

private slots:
    void    slotEnableRange();
    void    slotCheckIntersection   (int value);

signals:
    void    signalRangeChanged      (QObject* obj, quint16 value);
    void    signalApplyFilter       ();
    void    signalRangeEnabled      (QObject*);
    void    signalRangeDisabled     (QObject*);

protected:

//    virtual bool event(QEvent *event);

    virtual void keyReleaseEvent(QKeyEvent *event);


};

#endif // CLOGFILTERPANEL_H
