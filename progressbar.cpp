#include "progressbar.h"
#include "ui_progressbar.h"

ProgressBar::ProgressBar(int max, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProgressBar)
{
    ui->setupUi(this);

    ui->progressBar->setMaximum(max);
    this->setWindowFlag(Qt::SplashScreen);

    ui->progressBar->hide();
}

ProgressBar::~ProgressBar()
{
    delete ui;
}

void ProgressBar::slotSetValue(int value)
{
    ui->progressBar->setValue(value);
}
