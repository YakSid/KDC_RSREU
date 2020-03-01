#-------------------------------------------------
#
# Project created by QtCreator 2019-02-08T16:31:33
#
#-------------------------------------------------

QT       += core gui sql axcontainer

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = KDC_RSREU
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
    cdatabasemanager.cpp \
    ckoldog.cpp \
    knowledgebase.cpp \
        main.cpp \
        mainwindow.cpp \
    startdialog.cpp \
    listkd.cpp \
    modellist.cpp \
    fragment.cpp \
    detailktr.cpp \
    kef.cpp

HEADERS += \
    cdatabasemanager.h \
    ckoldog.h \
    knowledgebase.h \
        mainwindow.h \
    startdialog.h \
    listkd.h \
    modellist.h \
    fragment.h \
    detailktr.h \
    kef.h

FORMS += \
    knowledgebase.ui \
        mainwindow.ui \
    startdialog.ui \
    listkd.ui \
    detailktr.ui \
    kef.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    KDA1.ico

RESOURCES += \
    res.qrc

win32:{
    RC_FILE += ic.rc
    VERSION = 1.0
    QMAKE_TARGET_COMPANY = Yakov Sidorenko
    QMAKE_TARGET_PRODUCT = Master KDA
    QMAKE_TARGET_COPYRIGHT = Yakov Sidorenko
}
