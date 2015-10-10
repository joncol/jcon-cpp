#include "json_rpc_endpoint.h"
#include "json_rpc_socket.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QTcpSocket>

#include <cassert>

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
    connect(m_socket.get(), &JsonRpcSocket::dataReceived,
            this, &JsonRpcEndpoint::dataReceived);
    connect(m_socket.get(), &JsonRpcSocket::socketDisconnected,
            this, &JsonRpcEndpoint::socketDisconnected);
}

JsonRpcEndpoint::~JsonRpcEndpoint()
{
    m_socket->disconnect(this);
}

void JsonRpcEndpoint::connectToHost(const QString& host, int port)
{
    m_socket->connectToHost(host, port);
    if (!m_socket->waitForConnected()) {
        m_logger->logError("could not connect to JSON RPC server: " +
                           m_socket->errorString());
        return;
    }
    m_logger->logInfo(QString("connected to server %1:%2").arg(host).arg(port));
}

void JsonRpcEndpoint::disconnectFromHost()
{
    m_socket->disconnectFromHost();
}

void JsonRpcEndpoint::send(const QJsonDocument& doc)
{
    QByteArray bytes = doc.toJson();
    m_socket->send(bytes);
}

void JsonRpcEndpoint::dataReceived(const QByteArray& bytes, QObject* socket)
{
    assert(bytes.length() > 0);
    m_recv_buffer += bytes;
    m_recv_buffer = processBuffer(m_recv_buffer.trimmed(), socket);
}

QByteArray JsonRpcEndpoint::processBuffer(const QByteArray& buffer,
                                          QObject* socket)
{
    QByteArray buf(buffer);

    assert(buf[0] == '{');

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
                assert(brace_nesting_level >= 0);

                if (brace_nesting_level == 0) {
                    auto doc = QJsonDocument::fromJson(buf.left(i));
                    assert(!doc.isNull());
                    assert(doc.isObject());
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
