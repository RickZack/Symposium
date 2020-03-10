#ifndef CLIENTDISPATCHER_H
#define CLIENTDISPATCHER_H

#include <QObject>
#include <QWidget>
#include <QtNetwork>
#include "login.h"

class login;

    class Clientdispatcher: public QObject
    {
        Q_OBJECT

    private:
        QTcpSocket socket;
        QHostAddress svAddress = QHostAddress("127.0.0.1");
        quint16 svPort = 1234;
        login *lg;

    public:
        Clientdispatcher(QObject *parent = nullptr);
        void logIn(const QString &username, const QString &pwd);
        void setlg(login* lg);
        void sendMessage();
        void sendMessage(QString s);

    public slots:
        void readyRead();
    };

#endif // CLIENTDISPATCHER_H
