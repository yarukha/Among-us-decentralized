#include "Client.h"
#include "Server.h"
#include "main.h"

Client::Client(QString peerAddress)
{
    socket = new QTcpSocket(this);
    connect(socket, SIGNAL(readyRead()), this, SLOT(dataReceived()));
    connect(socket, SIGNAL(connected()), this, SLOT(connecte()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(deconnecte()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)));

    messageSize = 0;
    isConnected = false;

    socket->abort(); // On désactive les connexions précédentes s'il y en a
    quint16 serverPort = DEFAULT_SERVER_PORT;
    if(peerAddress.contains(':'))
    {
        QStringList peerAddressParts = peerAddress.split(':');
        serverPort = peerAddressParts[1].toInt();
        peerAddress = peerAddressParts[0];
    }
    // Orange is so bad port opening doesn't work anymore but DMZ does :'(
    qInfo(("client connecting to " + peerAddress + " on port " + QString::number(serverPort) + "...").toStdString().c_str());
    socket->connectToHost(peerAddress/*"2a01:cb00:774:4300:a4ba:9926:7e3a:b6c1"*//*"192.168.1.45"*//*"localhost"*//*"90.127.197.24"*//*"2a01:cb00:774:4300:531:8a76:deda:2b53"*//*a secret domain name*/, serverPort); // On se connecte au serveur demandé
    //qInfo("client connected ?");
    /*while(!isConnected) // looping seems important instead of single call to processEvents, doing this bloquant way assume that all remote peers trying to be connected to will succeed quickly or will have to wait for timeout (if there is any)
    {
        QCoreApplication::processEvents();
        QThread::msleep(1);
    }*/
}

void Client::connecte()
{
    isConnected = true;
    qInfo("connecte");
}

void Client::deconnecte()
{
    qInfo("deconnecte");
}

void Client::sendToServer(QString messageToSend) // not very useful in theory...
{
    sendToSocket(socket, messageToSend);
}

// On a reçu un paquet (ou un sous-paquet)
void Client::dataReceived()
{
    /* Même principe que lorsque le serveur reçoit un paquet :
    On essaie de récupérer la taille du message
    Une fois qu'on l'a, on attend d'avoir reçu le message entier (en se basant sur la taille annoncée messageSize)*/
    QDataStream in(socket);

    if(messageSize == 0)
    {
        if(socket->bytesAvailable() < (int)sizeof(quint16))
             return;
        in >> messageSize;
    }

    if(socket->bytesAvailable() < messageSize)
        return;

    // Si on arrive jusqu'à cette ligne, on peut récupérer le message entier
    QString receivedMessage;
    in >> receivedMessage;

    qInfo(("client received: " + receivedMessage).toStdString().c_str());
    if(askingAll)
    {
        QString peerString = socketToString(socket);
        if(askingAllMessages.find(peerString) != askingAllMessages.end() && askingAllMessages[peerString] == "") // second condition in order not to someone to revoke what he claimed
        {
            askingAllMessages[peerString] = receivedMessage;
            askingAllMessagesCounter--;
            qInfo(("askingAllMessagesCounter: " + QString::number(askingAllMessagesCounter)).toStdString().c_str());
        }
        else
            processMessageClient(receivedMessage);
    }
    else
        processMessageClient(receivedMessage);

    // On remet la taille du message à 0 pour pouvoir recevoir de futurs messages
    messageSize = 0;
}

void Client::processMessageClient(QString message)
{
    QStringList messageParts = message.split(NETWORK_SEPARATOR);
    quint32 messagePartsSize = messageParts.size();
    for(quint32 messagePartsIndex = 0; messagePartsIndex < messagePartsSize; messagePartsIndex++)
    {
        QString messagePart = messageParts[messagePartsIndex];
        if(messagePart.startsWith("peers "))
        {
            QString connected = messagePart.replace("peers ", "");
            QStringList connectedParts = connected.split(" ");
            quint32 connectedPartsSize = connectedParts.size();
            for(quint32 connectedPartsIndex = 0; connectedPartsIndex < connectedPartsSize; connectedPartsIndex++)
            {
                QString connectedPart = connectedParts[connectedPartsIndex];
                discoverClient(connectedPart);
                //sendToSocket();
            }
        }
        /*else if(messagePart.startsWith("nickname "))
        {
            QString otherPlayeNickname = messagePart.replace("nickname ", "");
            inGameUI->spawnOtherPlayer(otherPlayeNickname);
        }*/
        else
        {
            processMessageCommon(socket, messagePart);
        }
    }
}

void Client::socketError(QAbstractSocket::SocketError error) // not used
{
    switch(error)
    {
        case QAbstractSocket::HostNotFoundError:
            qWarning("Error: the server couldn't be found. Check IP and port.");
            break;
        case QAbstractSocket::ConnectionRefusedError:
            qWarning("Error: the server refused the connection.");
            break;
        case QAbstractSocket::RemoteHostClosedError:
            qWarning("Error: the server shutdown the connection.");
            break;
        default:
            qWarning(("Error: " + socket->errorString()).toStdString().c_str());
    }
}

void discoverClient(QString peerAddress)
{
    Client* client = new Client(peerAddress);
    clients.push_back(client);
    //client->sendToServer("nickname " + nickname);
    //client->sendToServer("discovering " + inGameUI->currPlayer.nickname);
    client->sendToServer("discovering" /*+ inGameUI->currPlayer.nickname*/);
}
