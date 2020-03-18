#ifndef SERVERDISPATCHER_H
#define SERVERDISPATCHER_H


#include <QtNetwork>
#include <boost/archive/text_iarchive.hpp>

 namespace Symposium {

    class ServerDispatcher: public QTcpServer
    {
        Q_OBJECT

    private:
        QMap<int, QTcpSocket*> Connected_Clients;   //QMap<SocketDescriptor, QTcpSocket>
        QMap<int, int> Connected_SymUser;           //QMap<SiteID, SocketDescriptor>
        QTcpSocket* tmp;

    public:
        ServerDispatcher();
        void startServer();
        void sendMessage(QString s);

    protected:
        void incomingConnection(qintptr socketDescriptor) override;

    public slots:
        void readyRead();
    };

 }

#endif // SERVERDISPATCHER_H
