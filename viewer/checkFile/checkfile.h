#ifndef CHECKFILE_H
#define CHECKFILE_H

#include <QList>

struct ListData
{
    int row     = 0;
    int column  = 0;
    QList<quint16>  list;

    ListData(QString fileName);

    QList<quint16> checkFile(QString fileName);
};



#endif // CHECKFILE_H
