QT += core gui

QT += network widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
requires(qtConfig(filedialog))
qtHaveModule(printsupport): QT += printsupport

TARGET = SymposiumGui
TEMPLATE = app

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
        ../../../../AccessStrategy.cpp \
        ../../../../SymClient.cpp \
        ../../../../SymServer.cpp \
        ../../../../SymposiumException.cpp \
        ../../../../document.cpp \
        ../../../../filesystem.cpp \
        ../../../../lib/hash/sha256.cpp \
        mainServer.cpp \
        ../serverdispatcher.cpp \
        ../clientdispatcher.cpp \
        ../../../../message.cpp \
        ../../../../resourceType.cpp \
        ../../../../symbol.cpp \
        ../../../../user.cpp \
        ../../../../uri.cpp \
        ../../../../privilege.cpp \
        ../../about.cpp \
        ../../activealwayslink.cpp \
        ../../activecounterlink.cpp \
        ../../activenonlink.cpp \
        ../../activetimerlink.cpp \
        ../../alluser.cpp \
        ../../changeuserinfo.cpp \
        ../../choosedir.cpp \
        ../../choosepriv.cpp \
        ../../deleteaccount.cpp \
        ../../directory.cpp \
        ../../errorconnection.cpp \
        ../../errorlogout.cpp \
        ../../exit.cpp \
        ../../home.cpp \
        ../../icon.cpp \
        ../../inserturi.cpp \
        ../../mainwindow.cpp \
        ../../notepad.cpp \
        ../../onlineusers.cpp \
        ../../qtexteditlabels.cpp \
        ../../sigin.cpp \
        ../../signup.cpp \
        ../../textedit.cpp \
        ../../unsuccessdeleteaccount.cpp

HEADERS += \
        ../../../../AccessStrategy.h \
        ../../../../SymClient.h \
        ../../../../SymServer.h \
        ../../../../SymposiumException.h \
        ../../../../document.h \
        ../../../../filesystem.h \
        ../../../../lib/hash/sha256.h \
        ../../../../message.h \
        ../../../../messageData.h \
        ../../../../resourceType.h \
        ../../../../symbol.h \
        ../../../../uri.h \
        ../../../../Symposium.h \
        ../../../../privilege.h \
        ../../../../user.h \
        ../../../../counter.h \
        ../../../../messageData.h \
        ../serverdispatcher.h \
        ../clientdispatcher.h \
        ../../about.h \
        ../../activealwayslink.h \
        ../../activecounterlink.h \
        ../../activenonlink.h \
        ../../activetimerlink.h \
        ../../alluser.h \
        ../../changeuserinfo.h \
        ../../choosedir.h \
        ../../choosepriv.h \
        ../../deleteaccount.h \
        ../../directory.h \
        ../../errorconnection.h \
        ../../errorlogout.h \
        ../../exit.h \
        ../../home.h \
        ../../icon.h \
        ../../inserturi.h \
        ../../mainwindow.h \
        ../../notepad.h \
        ../../onlineusers.h \
        ../../qtexteditlabels.h \
        ../../sigin.h \
        ../../signup.h \
        ../../textedit.h \
        ../../unsuccessdeleteaccount.h

FORMS += \
        ../../about.ui \
        ../../activealwayslink.ui \
        ../../activecounterlink.ui \
        ../../activenonlink.ui \
        ../../activetimerlink.ui \
        ../../alluser.ui \
        ../../changeuserinfo.ui \
        ../../choosedir.ui \
        ../../choosepriv.ui \
        ../../deleteaccount.ui \
        ../../directory.ui \
        ../../errorconnection.ui \
        ../../errorlogout.ui \
        ../../exit.ui \
        ../../folder1.ui \
        ../../home.ui \
        ../../icon.ui \
        ../../inserturi.ui \
        ../../mainwindow.ui \
        ../../newfolderwindow.ui \
        ../../notepad.ui \
        ../../onlineusers.ui \
        ../../sigin.ui \
        ../../signup.ui \
        ../../unsuccessdeleteaccount.ui

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
    ../../resources.qrc
