#include "masksettings.h"
#include "ui_masksettings.h"

#include <QFileDialog>
//#include <QDebug>

MaskSettings::MaskSettings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MaskSettings)
{
    ui->setupUi(this);

    connect(ui->open_txt, SIGNAL(clicked(bool)), this, SLOT(slotOpenTXT()));
    connect(ui->save_txt, SIGNAL(clicked(bool)), this, SLOT(slotSaveTXT()));
}

MaskSettings::~MaskSettings()
{
    delete ui;
}

int MaskSettings::getType() const
{
    return ui->mask_or_function->currentIndex();
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

}

void MaskSettings::slotOpenTXT()
{
    QString file = QFileDialog::getOpenFileName(this,
                                               "Open",
                                               QDir::homePath(),
                                               "TXT (*.txt)");
}

void MaskSettings::slotSaveTXT()
{
    QString file = QFileDialog::getSaveFileName(this,
                                               "Save",
                                               QDir::homePath(),
                                               "TXT (*.txt)");

}
