#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include <QWidget>

namespace Ui {
class ProgressBar;
}

class ProgressBar : public QWidget
{
    Q_OBJECT

public:
    ProgressBar(uint max, QWidget *parent = nullptr);
    ~ProgressBar();

private:
    Ui::ProgressBar *ui;

public slots:
    void slotSetValue(int value);
};

#endif // PROGRESSBAR_H
