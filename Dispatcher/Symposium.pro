QT -= gui

QT += core gui

QT += network widgets

CONFIG += c++11 console
CONFIG -= app_bundle

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
requires(qtConfig(filedialog))
qtHaveModule(printsupport): QT += printsupport

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++17     

SOURCES += \
        ../AccessStrategy.cpp \
        ../SymClient.cpp \
        ../SymServer.cpp \
        ../SymposiumException.cpp \
        ../document.cpp \
        ../filesystem.cpp \
        ../lib/hash/sha256.cpp \
        clientdispatcher.cpp \
        mainClient.cpp \
        mainServer.cpp \
        serverdispatcher.cpp \
        ../message.cpp \
        ../resourceType.cpp \
        ../symbol.cpp \
        ../user.cpp \
        ../uri.cpp \
        ../privilege.cpp

HEADERS += \
        ../AccessStrategy.h \
        ../SymClient.h \
        ../SymServer.h \
        ../SymposiumException.h \
        ../document.h \
        ../filesystem.h \
        ../lib/hash/sha256.h \
        ../message.h \
        ../messageData.h \
        ../resourceType.h \
        ../symbol.h \
        ../uri.h \
        ../Symposium.h \
        ../privilege.h \
        ../user.h \
        clientdispatcher.h \
        serverdispatcher.h

DEFINES = BOOST_ALL_NO_LIB
Boost_USE_STATIC_LIBS = ON


BOOSTHOME = C:/MinGW
INCLUDEPATH += C:/MinGW/include
LIBS += -LC:/MinGW/lib



 LIBS += -lboost_serialization \
         -lboost_wserialization \
         -lboost_random

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
