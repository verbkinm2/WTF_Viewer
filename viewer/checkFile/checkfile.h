#ifndef CHECKFILE_H
#define CHECKFILE_H

#include <QList>

struct ListData
{
    int row     = 0;
    int column  = 0;
    QList<int>  list;

    ListData(QString fileName);

    QList<int> checkFile(QString fileName);
};



#endif // CHECKFILE_H
