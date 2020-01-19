#-------------------------------------------------
#
# Project created by QtCreator 2020-01-14T16:25:44
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SymposiumGui
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

CONFIG += c++17

SOURCES += \
        about.cpp \
        changeuserinfo.cpp \
        deleteaccount.cpp \
        directory.cpp \
        exit.cpp \
        home.cpp \
        icon.cpp \
        inserturi.cpp \
        main.cpp \
        mainwindow.cpp\
        ../../uri.cpp\
        ../../privilege.cpp\
        ../../SymposiumException.cpp\
        ../../AccessStrategy.cpp\
        ../../document.cpp \
        ../../filesystem.cpp \
        ../../message.cpp \
        ../../resourceType.cpp \
        ../../symbol.cpp \
        ../../SymClient.cpp \
        ../../SymServer.cpp \
        ../../user.cpp \
        ../../lib/hash/sha256.cpp \
        sigin.cpp \
        signup.cpp

HEADERS += \
        about.h \
        changeuserinfo.h \
        deleteaccount.h \
        directory.h \
        exit.h \
        home.h \
        icon.h \
        inserturi.h \
        mainwindow.h\
        ../../uri.h \
        ../../privilege.h\
        ../../SymposiumException.h \
        ../../Symposium.h\
        ../../filesystem.h \
        ../../AccessStrategy.h \
        ../../counter.h \
        ../../document.h \
        ../../message.h \
        ../../messageData.h \
        ../../resourceType.h \
        ../../symbol.h \
        ../../SymClient.h \
        ../../SymServer.h \
        ../../user.h \
        ../../lib/hash/sha256.h \
        sigin.h \
        signup.h

FORMS += \
        about.ui \
        changeuserinfo.ui \
        deleteaccount.ui \
        directory.ui \
        exit.ui \
        home.ui \
        icon.ui \
        inserturi.ui \
        mainwindow.ui \
        sigin.ui \
        signup.ui


DEFINES = BOOST_ALL_NO_LIB
Boost_USE_STATIC_LIBS = ON
win32 {
    DEFINES = BOOST_ALL_NO_LIB
    BOOSTHOME = C:/MinGW
    INCLUDEPATH += C:/MinGW/include
    LIBS += -LC:/MinGW/lib
}

 LIBS += -lboost_serialization \
         -lboost_wserialization \
         -lboost_random

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
