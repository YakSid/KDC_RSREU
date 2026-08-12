#-------------------------------------------------
#
# Project created by QtCreator 2019-02-08T16:31:33
#
#-------------------------------------------------

QT       += core gui sql axcontainer

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = KDC_RSREU
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

CONFIG += c++11

SOURCES += \
    cconstants.cpp \
    cdatabasemanager.cpp \
    cjsonmanager.cpp \
    ckoldog.cpp \
    knowledgebase.cpp \
        main.cpp \
        mainwindow.cpp \
    modelsecond.cpp \
    startdialog.cpp \
    listkd.cpp \
    modellist.cpp \
    fragment.cpp \
    detailktr.cpp \
    kef.cpp

HEADERS += \
    cconstants.h \
    cdatabasemanager.h \
    cjsonmanager.h \
    ckoldog.h \
    knowledgebase.h \
        mainwindow.h \
    modelsecond.h \
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
