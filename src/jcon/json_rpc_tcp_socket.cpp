#include "json_rpc_tcp_socket.h"
#include "jcon_assert.h"

namespace jcon {

JsonRpcTcpSocket::JsonRpcTcpSocket()
    : m_socket(new QTcpSocket)
{
    setupSocket();
}

JsonRpcTcpSocket::JsonRpcTcpSocket(QTcpSocket* socket)
    : m_socket(socket)
{
    setupSocket();
}

JsonRpcTcpSocket::~JsonRpcTcpSocket()
{
    m_socket->disconnect(this);
    m_socket->deleteLater();
}

void JsonRpcTcpSocket::setupSocket()
{
    m_socket->setSocketOption(QAbstractSocket::LowDelayOption, "1");
    m_socket->setSocketOption(QAbstractSocket::KeepAliveOption, "1");

    connect(m_socket, &QTcpSocket::readyRead,
            this, &JsonRpcTcpSocket::dataReady);

    connect(m_socket, &QTcpSocket::disconnected, [this]() {
        emit socketDisconnected(m_socket);
    });
}

void JsonRpcTcpSocket::connectToHost(QString host, int port)
{
    m_socket->connectToHost(host, port);
}

bool JsonRpcTcpSocket::waitForConnected(int msecs)
{
    return m_socket->waitForConnected(msecs);
}

void JsonRpcTcpSocket::disconnectFromHost()
{
    m_socket->disconnectFromHost();
    m_socket->close();
}

void JsonRpcTcpSocket::send(const QByteArray& data)
{
    m_socket->write(data);
}

QString JsonRpcTcpSocket::errorString() const
{
    return m_socket->errorString();
}

QHostAddress JsonRpcTcpSocket::localAddress() const
{
    return m_socket->localAddress();
}

int JsonRpcTcpSocket::localPort() const
{
    return m_socket->localPort();
}

QHostAddress JsonRpcTcpSocket::peerAddress() const
{
    return m_socket->peerAddress();
}

int JsonRpcTcpSocket::peerPort() const
{
    return m_socket->peerPort();
}

void JsonRpcTcpSocket::dataReady()
{
    JCON_ASSERT(m_socket->bytesAvailable() > 0);
    QByteArray bytes = m_socket->read(m_socket->bytesAvailable());
    emit dataReceived(bytes, m_socket);
}

}
