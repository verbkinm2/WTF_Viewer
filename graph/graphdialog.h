#ifndef GRAPHDIALOG_H
#define GRAPHDIALOG_H

#include <QDialog>
#include "../viewer/frames/frames.h"

namespace Ui {
class GraphDialog;
}

class GraphDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GraphDialog(Frames* frames, QWidget *parent = nullptr);
    ~GraphDialog();

private:
    Ui::GraphDialog *ui;

//    QStringList listModel, listX, listY;

private slots:
    void slotCoefficientDisable(bool value);

//    void slotDates(QString value);
};

#endif // GRAPHDIALOG_H
