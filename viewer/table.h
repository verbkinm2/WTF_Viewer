#ifndef TABLE_H
#define TABLE_H

#include <QDialog>

namespace Ui {
class Table;
}

class Table : public QDialog
{
    Q_OBJECT

public:
    explicit Table(int size, int** array, QWidget *parent = nullptr);
    ~Table();

    void saveData();

private:
    Ui::Table *ui;

    int** arrayBin = nullptr;

    // QWidget interface
protected:
    virtual void resizeEvent(QResizeEvent *event);
};

#endif // TABLE_H
