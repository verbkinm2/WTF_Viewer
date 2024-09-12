#include <QtCore/QVector>
#include <QtCore/QRandomGenerator>
#include <QtCore/QRect>
#include <QtGui/QColor>

#include "customtablemodel.h"

CustomTableModel::CustomTableModel(QObject *parent) :
    QAbstractTableModel(parent)
{

}
CustomTableModel::~CustomTableModel()
{
    foreach (QVector<qreal> * vector, m_data)
        delete vector;
}

void CustomTableModel::setVector(QVector<QPointF> vector)
{
    m_columnCount = 2;
    m_rowCount = vector.count();

    for (int i = 0; i < m_rowCount; i++)
    {
        QVector<qreal>* dataVec = new QVector<qreal>(m_columnCount);
        for (int k = 0; k < dataVec->size(); k++) {
            if (k % 2 == 0)
                dataVec->replace(k, vector.at(i).x());
            else
                dataVec->replace(k, vector.at(i).y());
        }
        m_data.append(dataVec);
    }
}

void CustomTableModel::addPoint(QPointF point)
{
    m_rowCount++;
    QVector<qreal>* dataVec = new QVector<qreal>(m_columnCount);
    for (int k = 0; k < dataVec->size(); k++) {
        if (k % 2 == 0)
            dataVec->replace(k, point.x());
        else
            dataVec->replace(k, point.y());
    }
    m_data.append(dataVec);



}
int CustomTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_data.count();
}

int CustomTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_columnCount;
}

QVariant CustomTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        if (section % 2 == 0)
            return "x";
        else
            return "y";
    } else {
        return QString("%1").arg(section + 1);
    }
}

QVariant CustomTableModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole) {
        return m_data[index.row()]->at(index.column());
    } else if (role == Qt::EditRole) {
        return m_data[index.row()]->at(index.column());
    } else if (role == Qt::BackgroundRole) {
        for (const QRect &rect : m_mapping) {
            if (rect.contains(index.column(), index.row()))
                return QColor(m_mapping.key(rect));
        }
        // cell not mapped return white color
        return QColor(Qt::white);
    }
    return QVariant();
}

bool CustomTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole)
    {
        m_data[index.row()]->replace(index.column(), value.toDouble());
        emit dataChanged(index, index);
        return true;
    }
    return false;
}

Qt::ItemFlags CustomTableModel::flags(const QModelIndex &index) const
{
    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

void CustomTableModel::addMapping(QString color, QRect area)
{
    m_mapping.insertMulti(color, area);
}

void CustomTableModel::updateColor(QColor color)
{
    QRect rect = m_mapping.values().first();
    m_mapping.clear();
    m_mapping.insert(color.name(), rect);
}
