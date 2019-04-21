#ifndef SETTINGSIMAGE_H
#define SETTINGSIMAGE_H

#include <QDialog>
#include <QSettings>

namespace Ui {
class SettingsImage;
}

class SettingsImage : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsImage(QSettings& settings, QWidget *parent = nullptr);
    ~SettingsImage();

    void    writeSettings();


private slots:
    void    on_actionAccepted_triggered();

    void    on_actionReject_triggered();

    void on_actionsetColor_triggered();

private:
    Ui::SettingsImage *ui;
    QSettings* settings;

    void    readSettings();
};

#endif // SETTINGSIMAGE_H
