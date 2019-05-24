#ifndef MASKSETTINGS_H
#define MASKSETTINGS_H

#include <QWidget>

namespace Ui {
class MaskSettings;
}

class MaskSettings : public QWidget
{
    Q_OBJECT

public:
    explicit MaskSettings(QWidget *parent = nullptr);
    ~MaskSettings();

    enum    TYPE_LIST {MASK, RESTORE_FUNCTION};

    int     getType() const;
    int     getRank() const;
    int     getTransparency() const;


private:
    Ui::MaskSettings *ui;

private slots:
    void    slotGenerate();
    void    slotOpenTXT();
    void    slotSaveTXT();

signals:
    void    signalGenerated();
};

#endif // MASKSETTINGS_H
