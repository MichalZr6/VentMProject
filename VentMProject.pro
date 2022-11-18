QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

QXLSX_PARENTPATH=../QXlsx/QXlsx/         # current QXlsx path is . (. means curret directory)
QXLSX_HEADERPATH=../QXlsx/QXlsx/header/  # current QXlsx header path is ./header/
QXLSX_SOURCEPATH=../QXlsx/QXlsx/source/  # current QXlsx source path is ./source/
include(../QXlsx/QXlsx/QXlsx.pri)

INCLUDEPATH += $PWD/../myIncludes/

SOURCES += \
    ../myIncludes/errorsandwarningslogger.cpp \
    fixingsfasteners.cpp \
    main.cpp \
    mainwindow.cpp \
    mountingelements.cpp \
    qframedisplayengine.cpp \
    rowheaders.cpp \
    slings.cpp \
    summarycreator.cpp \
    ventduckt.cpp \
    ventelem.cpp \
    ventelemfactory.cpp \
    ventinsulation.cpp \
    ventpackelemfactory.cpp \
    ../myIncludes/Functions.cpp \
    ../myIncludes/Price.cpp \
    ../myIncludes/SimplExpr.cpp \
    vmoutputfilemngr.cpp \
    xlsxfilemanager.cpp

HEADERS += \
    ../myIncludes/errorsandwarningslogger.h \
    fixingsfasteners.h \
    mainwindow.h \
    mountingelements.h \
    qframedisplayengine.h \
    rowheaders.h \
    slings.h \
    summarycreator.h \
    ventduckt.h \
    ventelem.h \
    ventelemfactory.h \
    ventinsulation.h \
    ventpackelemfactory.h \
    ../myIncludes/conversion.hpp \
    ../myIncludes/Functions.h \
    ../myIncludes/Price.h \
    ../myIncludes/SimplExpr.h \
    vmoutputfilemngr.h \
    xlsxfilemanager.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
