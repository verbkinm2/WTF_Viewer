#include "graphdialog.h"
#include "ui_graphdialog.h"

#include <QDebug>

GraphDialog::GraphDialog(quint16 min, quint16 max, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GraphDialog)
{
    ui->setupUi(this);

    connect(ui->toEnergy, SIGNAL(toggled(bool)), this, SLOT(slotCoefficientDisable(bool)));

    connect(ui->DataX,  SIGNAL(currentIndexChanged(QString)), this, SLOT(slotDates(QString)));
    connect(ui->DataY,  SIGNAL(currentIndexChanged(QString)), this, SLOT(slotDates(QString)));

    listModel << "Tot";

    for (int i = min; i != max + 1; ++i)
        listModel << QString::number(i);

    listX = listModel;
    listX.removeAt(1);
    ui->DataX->addItems(listX);
}

GraphDialog::~GraphDialog()
{
    delete ui;
}

void GraphDialog::slotCoefficientDisable(bool value)
{
    ui->coefficients->setDisabled(!value);
}

void GraphDialog::slotDates(QString value)
{
    QComboBox* pCB = static_cast<QComboBox*>(sender());
    QString currentText;
    if(pCB->objectName() == "DataX")
    {
        currentText = ui->DataY->currentText();
        disconnect(ui->DataY,  SIGNAL(currentIndexChanged(QString)), this, SLOT(slotDates(QString)));
        listY.clear();
        listY = listModel;
        listY.removeOne(value);

        ui->DataY->clear();
        ui->DataY->addItems(listY);
        ui->DataY->setCurrentText(currentText);
        connect(ui->DataY,  SIGNAL(currentIndexChanged(QString)), this, SLOT(slotDates(QString)));
    }
    else if(pCB->objectName() == "DataY")
    {
        currentText = ui->DataX->currentText();
        disconnect(ui->DataX,  SIGNAL(currentIndexChanged(QString)), this, SLOT(slotDates(QString)));
        listX.clear();
        listX = listModel;
        listX.removeOne(value);

        ui->DataX->clear();
        ui->DataX->addItems(listX);
        ui->DataX->setCurrentText(currentText);
        connect(ui->DataX,  SIGNAL(currentIndexChanged(QString)), this, SLOT(slotDates(QString)));
    }
}
