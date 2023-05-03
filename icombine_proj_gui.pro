QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++20 \
          warn_off

TARGET = "L3 Report Generator"
QMAKE_TARGET_DESCRIPTION += "L3 Report Generator"

VERSION_MAJOR = 1
VERSION_MINOR = 0

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    aboutdialog.cpp \
    archiveinoutdialog.cpp \
    archiveparser.cpp \
    customtreewidget.cpp \
    customtreewidgetitem.cpp \
    exportcompletebox.cpp \
    filenotpopup.cpp \
    grouptreewidgetitem.cpp \
    loadingpopup.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    aboutdialog.h \
    archiveinoutdialog.h \
    archiveparser.h \
    customtreewidget.h \
    customtreewidgetitem.h \
    exportcompletebox.h \
    filenotpopup.h \
    grouptreewidgetitem.h \
    loadingpopup.h \
    mainwindow.h \
    test_case_types.h \
    version.h \
    zip_file.hpp

FORMS += \
    aboutdialog.ui \
    archiveinoutdialog.ui \
    exportcompletebox.ui \
    filenotpopup.ui \
    loadingpopup.ui \
    mainwindow.ui

RC_ICONS += \
    ./img/icon.ico

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource.qrc
