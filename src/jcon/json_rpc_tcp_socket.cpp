#include "json_rpc_tcp_socket.h"
#include "jcon_assert.h"

namespace jcon {

JsonRpcTcpSocket::JsonRpcTcpSocket()
    : m_socket(new QTcpSocket(this))
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
	this->disconnect(m_socket);
}

void JsonRpcTcpSocket::setupSocket()
{
    m_socket->setSocketOption(QAbstractSocket::LowDelayOption, "1");
    m_socket->setSocketOption(QAbstractSocket::KeepAliveOption, "1");

    connect(m_socket, &QTcpSocket::connected, this, [this]() {
        emit socketConnected(m_socket);
    });

    connect(m_socket, &QTcpSocket::disconnected, this, [this]() {
		QTcpSocket* this_socket = m_socket;
        emit socketDisconnected(this_socket);
    });

    connect(m_socket, &QTcpSocket::readyRead,
            this, &JsonRpcTcpSocket::dataReady);

    void (QAbstractSocket::*errorFun)(QAbstractSocket::SocketError) =
        &QAbstractSocket::error;
    connect(m_socket, errorFun, this,
            [this](QAbstractSocket::SocketError error) {
				QTcpSocket* this_socket = m_socket;
                emit socketError(this_socket, error);
				this_socket->close();
            });
}

void JsonRpcTcpSocket::connectToHost(QString host, int port)
{
    m_socket->connectToHost(host, port,
                            QIODevice::ReadWrite,
                            QAbstractSocket::IPv4Protocol);
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

bool JsonRpcTcpSocket::isConnected() const
{
    return m_socket->state() == QAbstractSocket::ConnectedState;
}

size_t JsonRpcTcpSocket::send(const QByteArray& data)
{
    auto sz = m_socket->write(data);
    m_socket->flush();
    return sz;
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
