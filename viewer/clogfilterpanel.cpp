#include "clogfilterpanel.h"
#include "ui_clogfilterpanel.h"

#include <QKeyEvent>
#include <QDebug>

ClogFilterPanel::ClogFilterPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClogFilterPanel)
{
    ui->setupUi(this);

    connect(ui->clusterRangeGroup,  SIGNAL(clicked(bool)), this, SLOT(slotEnableRange()));
    connect(ui->totRangeGroup,      SIGNAL(clicked(bool)), this, SLOT(slotEnableRange()));

    connect(ui->clusterRangeBegin,  SIGNAL(valueChanged(int)), this, SLOT(slotCheckIntersection(int)));
    connect(ui->clusterRangeEnd,    SIGNAL(valueChanged(int)), this, SLOT(slotCheckIntersection(int)));

    connect(ui->totRangeBegin,      SIGNAL(valueChanged(int)), this, SLOT(slotCheckIntersection(int)));
    connect(ui->totRangeEnd,        SIGNAL(valueChanged(int)), this, SLOT(slotCheckIntersection(int)));

    connect(ui->apply,              SIGNAL(clicked()),          this, SIGNAL(signalApplyFilter()) );
}

ClogFilterPanel::~ClogFilterPanel()
{
    delete ui;
}

void ClogFilterPanel::setClusterRangeMaximum(int value)
{
    ui->clusterRangeBegin->setMinimum(1);
    ui->clusterRangeBegin->setMaximum(value - 1);

    ui->clusterRangeEnd->setMinimum(ui->clusterRangeBegin->minimum() + 1);
    ui->clusterRangeEnd->setMaximum(value);
}

void ClogFilterPanel::setTotRangeMaximum(int value)
{
    ui->totRangeBegin->setMinimum(1);
    ui->totRangeBegin->setMaximum(value - 1);

    ui->totRangeEnd->setMinimum(ui->totRangeBegin->minimum() + 1);
    ui->totRangeEnd->setMaximum(value);
}

void ClogFilterPanel::setClusterEnd(int value)
{
    ui->clusterRangeEnd->setValue(value);
}

quint16 ClogFilterPanel::getClusterBegin() const
{
    return quint16(ui->clusterRangeBegin->value());
}

quint16 ClogFilterPanel::getClusterEnd() const
{
    return quint16(ui->clusterRangeEnd->value());
}

quint16 ClogFilterPanel::getTotBegin() const
{
    return quint16(ui->totRangeBegin->value());
}

quint16 ClogFilterPanel::getTotEnd() const
{
    return quint16(ui->totRangeEnd->value());
}

void ClogFilterPanel::setTotEnd(int value)
{
    ui->totRangeEnd->setValue(value);
}

void ClogFilterPanel::setLabelMax(quint16 value)
{
    ui->clusterLabel->setText("max: " + QString::number(value));
}

bool ClogFilterPanel::isClusterEnable()
{
    return ui->clusterRangeGroup->isChecked();
}

bool ClogFilterPanel::isTotEnable()
{
    return ui->totRangeGroup->isChecked();
}

void ClogFilterPanel::slotEnableRange()
{
    QGroupBox* groupBox = static_cast<QGroupBox*>(sender());

    if(groupBox->objectName() == "clusterRangeGroup")
    {
        if(groupBox->isChecked())
        {
            ui->clusterRangeBegin->setEnabled(true);
            ui->clusterRangeEnd->setEnabled(true);
            emit signalRangeEnabled(sender());
        }
        else
        {
            ui->clusterRangeBegin->setEnabled(false);
            ui->clusterRangeEnd->setEnabled(false);
            emit signalRangeDisabled(sender());
        }
    }
    else if(groupBox->objectName() == "totRangeGroup")
    {
        if(groupBox->isChecked())
        {
            ui->totRangeBegin->setEnabled(true);
            ui->totRangeEnd->setEnabled(true);
            emit signalRangeEnabled(sender());
        }
        else
        {
            ui->totRangeBegin->setEnabled(false);
            ui->totRangeEnd->setEnabled(false);
            emit signalRangeDisabled(sender());
        }
    }
}

void ClogFilterPanel::slotCheckIntersection(int value)
{
    QSpinBox* sp = static_cast<QSpinBox*>(sender());

    if(sp->objectName() == "clusterRangeBegin")
        if(value >= ui->clusterRangeEnd->value())
            sp->setValue(--value);

    if(sp->objectName() == "clusterRangeEnd")
        if(value <= ui->clusterRangeBegin->value())
            sp->setValue(++value);

    if(sp->objectName() == "totRangeBegin")
        if(value >= ui->totRangeEnd->value())
            sp->setValue(--value);

    if(sp->objectName() == "totRangeEnd")
        if(value <= ui->totRangeBegin->value())
            sp->setValue(++value);

    emit signalRangeChanged(sender(), quint16(value));
}

void ClogFilterPanel::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
        emit signalApplyFilter();
}

//bool ClogFilterPanel::event(QEvent *event)
//{
//    if(event->type() == QEvent::KeyRelease)
//    {
//        QKeyEvent* kEv = static_cast<QKeyEvent*>(event);
//        if(kEv->key() == Qt::Key_Enter)
//            emit signalApplyFilter();
//    }

//    return QWidget::event(event);
//}
