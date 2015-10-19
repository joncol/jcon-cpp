#include "json_rpc_endpoint.h"
#include "json_rpc_socket.h"
#include "jcon_assert.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QTcpSocket>

namespace jcon {

/// Remove \p n bytes from \p bytes.
static QByteArray chopLeft(const QByteArray& bytes, int n);

JsonRpcEndpoint::JsonRpcEndpoint(JsonRpcSocketPtr socket,
                                 JsonRpcLoggerPtr logger,
                                 QObject* parent)
    : QObject(parent)
    , m_logger(logger)
    , m_socket(socket)
{
    connect(m_socket.get(), &JsonRpcSocket::socketConnected,
            this, &JsonRpcEndpoint::socketConnected);

    connect(m_socket.get(), &JsonRpcSocket::socketDisconnected,
            this, &JsonRpcEndpoint::socketDisconnected);

    connect(m_socket.get(), &JsonRpcSocket::dataReceived,
            this, &JsonRpcEndpoint::dataReceived);
}

JsonRpcEndpoint::~JsonRpcEndpoint()
{
    m_socket->disconnect(this);
}

bool JsonRpcEndpoint::connectToHost(const QString& host, int port, int msecs)
{
    m_socket->connectToHost(host, port);

    if (!m_socket->waitForConnected(msecs)) {
        m_logger->logError("could not connect to JSON RPC server: " +
                           m_socket->errorString());
        return false;
    }

    m_logger->logInfo(QString("connected to server %1:%2").arg(host).arg(port));
    return true;
}

void JsonRpcEndpoint::connectToHostAsync(const QString& host, int port)
{
    m_socket->connectToHost(host, port);
}

void JsonRpcEndpoint::disconnectFromHost()
{
    m_socket->disconnectFromHost();
}

bool JsonRpcEndpoint::isConnected() const
{
    return m_socket->isConnected();
}

QHostAddress JsonRpcEndpoint::localAddress() const
{
    return m_socket->localAddress();
}

int JsonRpcEndpoint::localPort() const
{
    return m_socket->localPort();
}

QHostAddress JsonRpcEndpoint::peerAddress() const
{
    return m_socket->peerAddress();
}

int JsonRpcEndpoint::peerPort() const
{
    return m_socket->peerPort();
}


void JsonRpcEndpoint::send(const QJsonDocument& doc)
{
    QByteArray bytes = doc.toJson();
    m_socket->send(bytes);
}

void JsonRpcEndpoint::dataReceived(const QByteArray& bytes, QObject* socket)
{
    JCON_ASSERT(bytes.length() > 0);
    m_recv_buffer += bytes;
    m_recv_buffer = processBuffer(m_recv_buffer.trimmed(), socket);
}

QByteArray JsonRpcEndpoint::processBuffer(const QByteArray& buffer,
                                          QObject* socket)
{
    QByteArray buf(buffer);

    JCON_ASSERT(buf[0] == '{');

    bool in_string = false;
    int brace_nesting_level = 0;
    QByteArray json_obj;

    int i = 0;
    while (i < buf.length() ) {
        const char curr_ch = buf[i++];

        if (curr_ch == '"')
            in_string = !in_string;

        if (!in_string) {
            if (curr_ch == '{')
                ++brace_nesting_level;

            if (curr_ch == '}') {
                --brace_nesting_level;
                JCON_ASSERT(brace_nesting_level >= 0);

                if (brace_nesting_level == 0) {
                    auto doc = QJsonDocument::fromJson(buf.left(i));
                    JCON_ASSERT(!doc.isNull());
                    JCON_ASSERT(doc.isObject());
                    if (doc.isObject())
                        emit jsonObjectReceived(doc.object(), socket);
                    buf = chopLeft(buf, i);
                    i = 0;
                    continue;
                }
            }
        }
    }
    return buf;
}

QByteArray chopLeft(const QByteArray& bytes, int n)
{
    return bytes.right(bytes.length() - n);
}

}
