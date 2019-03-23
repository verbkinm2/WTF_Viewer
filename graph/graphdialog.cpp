#include "graphdialog.h"
#include "ui_graphdialog.h"

#include <QDebug>

GraphDialog::GraphDialog(Frames *frames, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GraphDialog)
{
    ui->setupUi(this);

    connect(ui->toEnergy, SIGNAL(toggled(bool)), this, SLOT(slotCoefficientDisable(bool)));

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    foreach(quint16 value, frames->getClustersLenghtList())
        ui->DataY->addItem(QString::number(value));
    QApplication::restoreOverrideCursor();


//    connect(ui->DataX,  SIGNAL(currentIndexChanged(QString)), this, SLOT(slotDates(QString)));
//    connect(ui->DataY,  SIGNAL(currentIndexChanged(QString)), this, SLOT(slotDates(QString)));

//    listModel << "Tot";

//    for (int i = frames; i != max + 1; ++i)
//        ui->DataY->addItem(QString::number(i));

//    listX = listModel;
//    listX.removeAt(1);
//    ui->DataX->addItems(listX);

    ui->DataX->addItem("Tot");
}

GraphDialog::~GraphDialog()
{
    delete ui;
}

void GraphDialog::slotCoefficientDisable(bool value)
{
    ui->coefficients->setDisabled(!value);
}

//void GraphDialog::slotDates(QString value)
//{
//    QComboBox* pCB = static_cast<QComboBox*>(sender());
//    QString currentText;
//    if(pCB->objectName() == "DataX")
//    {
//        currentText = ui->DataY->currentText();
//        disconnect(ui->DataY,  SIGNAL(currentIndexChanged(QString)), this, SLOT(slotDates(QString)));
//        listY.clear();
//        listY = listModel;
//        listY.removeOne(value);

//        ui->DataY->clear();
//        ui->DataY->addItems(listY);
//        ui->DataY->setCurrentText(currentText);
//        connect(ui->DataY,  SIGNAL(currentIndexChanged(QString)), this, SLOT(slotDates(QString)));
//    }
//    else if(pCB->objectName() == "DataY")
//    {
//        currentText = ui->DataX->currentText();
//        disconnect(ui->DataX,  SIGNAL(currentIndexChanged(QString)), this, SLOT(slotDates(QString)));
//        listX.clear();
//        listX = listModel;
//        listX.removeOne(value);

//        ui->DataX->clear();
//        ui->DataX->addItems(listX);
//        ui->DataX->setCurrentText(currentText);
//        connect(ui->DataX,  SIGNAL(currentIndexChanged(QString)), this, SLOT(slotDates(QString)));
//    }
//}
