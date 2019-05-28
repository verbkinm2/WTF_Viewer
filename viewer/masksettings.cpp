#include "masksettings.h"
#include "ui_masksettings.h"
#include "table.h"

#include <QFileDialog>
#include <QDebug>

MaskSettings::MaskSettings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MaskSettings)
{
    //выделение памяти
    arrayBin = new double* [BINNING_MAX_SIZE];
    for (int i = 0; i < BINNING_MAX_SIZE ; ++i)
        arrayBin[i] = new double[BINNING_MAX_SIZE ];

    for (int row = 0; row < BINNING_MAX_SIZE; ++row)
        for (int column = 0; column < BINNING_MAX_SIZE; ++column)
            arrayBin[row][column]= 1.00;

    ui->setupUi(this);

    ui->tabWidget->setTabEnabled(CONVOLUTION, false);
    ui->tabWidget->setTabEnabled(BACKPROJECTION, false);

    connect(ui->open_txt, SIGNAL(clicked(bool)), this, SLOT(slotOpenTXT()));
    connect(ui->save_txt, SIGNAL(clicked(bool)), this, SLOT(slotSaveTXT()));

    connect(ui->binning, SIGNAL(valueChanged(int)), this, SLOT(slotBinningChanged(int)));

    connect(ui->binning_button, SIGNAL(clicked()), this, SLOT(slotTableShow()));

    connect(ui->generated, SIGNAL(clicked()), this, SLOT(slotGenerate()));
}

MaskSettings::~MaskSettings()
{
    delete ui;

    for (int i = 0; i < BINNING_MAX_SIZE ; ++i)
        delete [] arrayBin[i];
    delete [] arrayBin;
}

int MaskSettings::getRank() const
{
    return ui->rank_of_mask->value();
}

int MaskSettings::getTransparency() const
{
    return ui->transparency_of_mask->value();
}

void MaskSettings::slotGenerate()
{
    ui->tabWidget->setTabEnabled(CONVOLUTION, true);
    ui->tabWidget->setTabEnabled(BACKPROJECTION, true);
}

void MaskSettings::slotOpenTXT()
{
    QString file = QFileDialog::getOpenFileName(this,
                                               "Open",
                                               QDir::homePath(),
                                               "TXT (*.txt)");
    if(!file.isNull()){
        //ловится в классе viewer_widget
        emit signalOpenTXT(file);
    }
}

void MaskSettings::slotSaveTXT()
{
    QString file = QFileDialog::getSaveFileName(this,
                                               "Save",
                                               QDir::homePath(),
                                               "TXT (*.txt)");

}

void MaskSettings::slotBinningChanged(int value)
{
    if(value > 1)
        ui->binning_button->setEnabled(true);
    else
        ui->binning_button->setEnabled(false);
}

void MaskSettings::slotTableShow()
{
    Table* table = new Table(ui->binning->value(), arrayBin);

    if(table->exec() == QDialog::Accepted)
        table->saveData();

    delete table;
}
