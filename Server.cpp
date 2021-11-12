#include "Server.h"
#include <QTcpServer>
#include "main.h"
#include <unistd.h> // only linux...

// should use IPv6 by default

/*QTcpServer* server;
QList<QTcpSocket*> clients;
quint16 messageSize;

void newConnection(),
     dataReceived(),
     clientDisconnected();*/

Server::Server(quint16 serverPort)
{
    server = new QTcpServer();
    if(!server->listen(QHostAddress::Any, serverPort))
    {
        qWarning(("Server couldn't start for reason: " + server->errorString()).toStdString().c_str());
    }
    else
    {
        qWarning("Server started !");
        //QObject::connect(server, &QTcpServer::newConnection, [](){ newConnection(); });
        connect(server, SIGNAL(newConnection()), this, SLOT(newConnection()));
    }

    messageSize = 0;
}

void Server::newConnection()
{
    QTcpSocket* newClient = server->nextPendingConnection();
    clients << newClient;

    //QObject::connect(newClient, &QIODevice::readyRead, [](){ dataReceived(); });
    //QObject::connect(newClient, &QAbstractSocket::disconnected, [](){ clientDisconnected(); });
    connect(newClient, SIGNAL(readyRead()), this, SLOT(dataReceived()));
    ///connect(newClient, SIGNAL(disconnected()), this, SLOT(clientDisconnected()));
}

void Server::dataReceived()
{
    // 1 : on reçoit un paquet (ou un sous-paquet) d'un des clients

    // On détermine quel client envoie le message (recherche du QTcpSocket du client)
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if(socket == 0) // Si par hasard on n'a pas trouvé le client à l'origine du signal, on arrête la méthode
        return;

    // Si tout va bien, on continue : on récupère le message
    QDataStream in(socket);

    if(messageSize == 0) // Si on ne connaît pas encore la taille du message, on essaie de la récupérer
    {
        if(socket->bytesAvailable() < (int)sizeof(quint16)) // On n'a pas reçu la taille du message en entier
             return;

        in >> messageSize; // Si on a reçu la taille du message en entier, on la récupère
    }

    // Si on connaît la taille du message, on vérifie si on a reçu le message en entier
    if (socket->bytesAvailable() < messageSize) // Si on n'a pas encore tout reçu, on arrête la méthode
        return;

    // Si ces lignes s'exécutent, c'est qu'on a reçu tout le message : on peut le récupérer !
    QString message;
    in >> message;
    qInfo(("server received: " + message).toStdString().c_str());
    message = processMessageServer(socket, message);

    //message = processMessage(message);
    sendToSocket(socket, message);

    // 2 : remise de la taille du message à 0 pour permettre la réception des futurs messages
    messageSize = 0;
}

// assume not same message a second time until others validated
QMap<QPair<QString, QString>, QMap<QString, QString>> waitingMessages; // QMap<QPair<peerAddress, message>, verificatorsAddresses>

QString Server::processMessageServer(QTcpSocket* socket, QString message)
{
    QStringList messageParts = message.split(NETWORK_SEPARATOR);
    quint32 messagePartsSize = messageParts.size();
    QString res = "";
    for(quint32 messagePartsIndex = 0; messagePartsIndex < messagePartsSize; messagePartsIndex++)
    {
        QString messagePart = messageParts[messagePartsIndex];
        /*if(messagePart.startsWith("nickname "))
        {
            QString otherPlayeNickname = messagePart.replace("nickname ", "");
            inGameUI->spawnOtherPlayer(otherPlayeNickname);
            res += "nickname " + inGameUI->currPlayer.nickname;
        }*/
        if(messagePart == "discovering"/*messagePart.startsWith("discovering ")*/)
        {
            //QString otherPlayeNickname = messagePart.replace("discovering ", "");
            //inGameUI->spawnOtherPlayer(otherPlayeNickname);
            quint16 clientsSize = clients.size();//,
                    //clientsTreated = 0;
            if(clientsSize > 1)
            {
                res += "peers ";
                QStringList fullAddresses;
                for(quint16 clientsIndex = 0; clientsIndex < clientsSize; clientsIndex++)
                {
                    QTcpSocket* currentSocket = clients[clientsIndex]; // should also work with clients in InGameUI
                    if(currentSocket != socket)
                    {
                        QString peerFullAddress = socketToString(currentSocket);
                        fullAddresses.push_back(peerFullAddress);
                        /*res += peerFullAddress;
                        clientsTreated++;
                        if(clientsTreated < clientsSize - 2) // warning unsigned etc
                        {
                            res += " ";
                        }*/
                    }
                }
                res += fullAddresses.join(" ");
            }
            //res += "nickname " + inGameUI->currPlayer.nickname; // no not trustable data until
        }
        if(messagePartsIndex < messagePartsSize - 1)
            res += NETWORK_SEPARATOR;
    }
    return res;
}

void Server::clientDisconnected()
{
    // On détermine quel client se déconnecte
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if(socket == 0) // Si par hasard on n'a pas trouvé le client à l'origine du signal, on arrête la méthode
        return;

    clients.removeOne(socket);

    socket->deleteLater();
}

void sendToSocket(QTcpSocket* socket, QString messageToSend)
{
    if(messageToSend == "") return;
    QByteArray paquet;
    QDataStream out(&paquet, QIODevice::WriteOnly);

    // On prépare le paquet à envoyer

    out << (quint16)0;
    out << messageToSend;
    out.device()->seek(0);
    out << (quint16)(paquet.size() - sizeof(quint16));

    socket->write(paquet); // On envoie le paquet
}

QString askAll(QString message)
{
    QList<QTcpSocket*> peers = getPeers();
    QStringList verificatorsAddresses;
    quint16 peersSize = peers.size();
    for(quint16 peersIndex = 0; peersIndex < peersSize; peersIndex++)
    {
        QTcpSocket* peer = peers[peersIndex];
        verificatorsAddresses.push_back(socketToString(peer));
    }
    //QPair<QString, QString> key = qMakePair("localhost", "askAll");
    ///waitingMessages[key] = verificatorsAddresses;
    while(true)
    {
        usleep(1000);
        //if(waitingMessages[key].empty())
            break;
    }

}

QList<QTcpSocket*> getPeers()
{
    QList<QTcpSocket*> res = server->clients;
    quint16 clientsSize = clients.size();
    for(quint16 clientsIndex = 0; clientsIndex < clientsSize; clientsIndex++)
    {
        QTcpSocket* client = clients[clientsIndex]->socket;
        res.append(client);
    }
    return res;
}

QString socketToString(QTcpSocket* socket)
{
    return socket->peerAddress().toString()/*not sure about this*/ + ":" + QString::number(socket->peerPort());
}
