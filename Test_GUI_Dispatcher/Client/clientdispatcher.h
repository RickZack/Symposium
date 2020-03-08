#ifndef CLIENTDISPATCHER_H
#define CLIENTDISPATCHER_H

#include <QObject>
#include <QWidget>
#include <QtNetwork>


namespace Symposium {

    class Clientdispatcher: public QObject
    {
        Q_OBJECT

    private:
        QTcpSocket socket;
        QHostAddress svAddress = QHostAddress("127.0.0.1");
        quint16 svPort = 1234;

    public:
        Clientdispatcher(QObject *parent = nullptr);
        void sendMessage();

    public slots:
        void readyRead();
    };

}

#endif // CLIENTDISPATCHER_H
