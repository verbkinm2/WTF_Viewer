#include "eventfilter.h"

#include <QEvent>
#include <QKeyEvent>
#include <QTreeView>
#include <QModelIndex>
#include <QFileSystemModel>
#include <QDebug>

EventFilter::EventFilter(QObject *parent) : QObject(parent)
{

}
/*virtual*/ bool EventFilter::eventFilter(QObject* obj, QEvent* event)
{
    if(event->type() == QEvent::KeyRelease){
        QKeyEvent* ke = static_cast<QKeyEvent*>(event);
        if( (ke->key() == Qt::Key_Down) || (ke->key() == Qt::Key_Up))
        {
            QTreeView* tree = static_cast<QTreeView*>(obj);
            QModelIndex qmi = tree->currentIndex();
            QFileSystemModel fsm;
            if( !(fsm.fileInfo(qmi).isDir()) )
                emit tree->clicked(qmi);
        }
    }

    return false;
}
