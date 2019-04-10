#-------------------------------------------------
#
# Project created by QtCreator 2018-10-28T17:44:35
#
#-------------------------------------------------

QT       += core gui charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = WTF_Viewer
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    eventfilter/fingerslide.cpp \
    eventfilter/eventfilter.cpp \
    export/export.cpp \
    viewer/checkFile/checkfile.cpp \
    viewer/frames/frames.cpp \
    viewer/frames/oneframe.cpp \
    viewer/viewer_widget.cpp \
    progressbar.cpp \
    viewer/clogfilterpanel.cpp \
    graph/graphdialog.cpp \
    graph/centralwidget.cpp \
    graph/chart.cpp \
    graph/chartview.cpp \
    graph/customtablemodel.cpp \
    graph/panelwidget.cpp \
    viewer/toolspanel.cpp

HEADERS += \
    eventfilter/eventfilter.h \
    eventfilter/fingerslide.h \
    export/export.h \
    viewer/checkFile/checkfile.h \
    viewer/frames/frames.h \
    viewer/frames/oneframe.h \
    viewer/viewer_widget.h \
    mainwindow.h \
    progressbar.h \
    viewer/clogfilterpanel.h \
    graph/graphdialog.h \
    graph/centralwidget.h \
    graph/chart.h \
    graph/chartview.h \
    graph/customtablemodel.h \
    graph/panelwidget.h \
    viewer/toolspanel.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

FORMS += \
    viewer\viewer_widget.ui \
    export/export.ui \
    progressbar.ui \
    viewer/clogfilterpanel.ui \
    graph/graphdialog.ui \
    graph/panelwidget.ui \
    viewer/toolspanel.ui

RESOURCES += \
    img.qrc

SUBDIRS += \
    WTF_Viewer.pro

DISTFILES += \
    img/atom.png \
    img/author.png \
    img/cut.png \
    img/exit.png \
    img/minus.png \
    img/pen.png \
    img/plus.png \
    img/qt_logo.png \
    img/reset.png \
    img/rotate_minus.png \
    img/rotate_plus.png \
    img/sekection_border.png
