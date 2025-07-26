QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    additemdialog.cpp \
    categorymanager.cpp \
    main.cpp \
    mainwindow.cpp \
    menumanager.cpp \
    saleshistory.cpp

HEADERS += \
    additemdialog.h \
    categorymanager.h \
    mainwindow.h \
    menumanager.h \
    saleshistory.h

FORMS += \
    additemdialog.ui \
    categorymanager.ui \
    mainwindow.ui \
    menumanager.ui \
    saleshistory.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
QT += sql

QT += printsupport

RESOURCES += \
    resources.qrc
