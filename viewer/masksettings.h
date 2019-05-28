#ifndef MASKSETTINGS_H
#define MASKSETTINGS_H

#include <QWidget>

#define BINNING_MAX_SIZE 15

namespace Ui {
class MaskSettings;
}

class MaskSettings : public QWidget
{
    Q_OBJECT

public:
    explicit MaskSettings(QWidget *parent = nullptr);
    ~MaskSettings();

    int     getRank() const;
    int     getTransparency() const;


private:
    Ui::MaskSettings *ui;

    double** arrayBin = nullptr;

    enum TAB_NAME{MASK, CONVOLUTION, BACKPROJECTION};

private slots:
    void    slotGenerate();
    void    slotOpenTXT();
    void    slotSaveTXT();
    void    slotBinningChanged(int);

    void    slotTableShow();

signals:
    void    signalGenerated();
    void    signalOpenTXT(QString);
};

#endif // MASKSETTINGS_H
