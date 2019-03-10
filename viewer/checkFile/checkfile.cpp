#include "checkfile.h"
#include <QFile>

ListData::ListData(QString fileName)
{
     this->checkFile(fileName);
}
QList<quint16> ListData::checkFile(QString fileName)
{
    QFile file(fileName);

    if(file.open(QIODevice::ReadOnly) )
    {
        //переменная для наибольшего кол-ва элементов в строке
        int maxCountElement = 0;
        int countString  = 0;

        //определяем наибольшее кол-во элементов в строке
        while (!file.atEnd()) {
            int counInLine = QString(file.readLine()).remove("\r\n").split(" ").length();
            maxCountElement < counInLine ? maxCountElement = counInLine: NULL ;
            countString++;
        }
        file.seek(0);

        while (!file.atEnd()) {
            QStringList splitString = QString(file.readLine()).remove("\r\n").split(" ");
            int countInLine = splitString.length();

            //максимальное число элемента - 65535, все что больше будет = 0, отрицательные числа будут равны: 65536 плюс отрицательное число
            foreach (QString element, splitString)
                list.append(element.toUShort());

            //если в текущей строке элементов меньше чем в максимальной строке - добавляем нули
            while (countInLine != maxCountElement)
            {
                list.append(0);
                countInLine++;
            }

        }
        column = maxCountElement;
        row    = countString;
        file.close();
    }

    return list;
}
