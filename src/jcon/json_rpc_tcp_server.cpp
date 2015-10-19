#include "json_rpc_tcp_server.h"
#include "json_rpc_tcp_socket.h"
#include "jcon_assert.h"

namespace jcon {

JsonRpcTcpServer::JsonRpcTcpServer(QObject* parent, JsonRpcLoggerPtr logger)
    : JsonRpcServer(parent, logger)
    , m_server(this)
{
    m_server.connect(&m_server, &QTcpServer::newConnection,
                     this, &JsonRpcTcpServer::newConnection);
}

JsonRpcTcpServer::~JsonRpcTcpServer()
{
    m_server.disconnect(this);
    close();
}

void JsonRpcTcpServer::listen(int port)
{
    logInfo(QString("listening on port %2").arg(port));
    m_server.listen(QHostAddress::Any, port);
}

void JsonRpcTcpServer::close()
{
    m_server.close();
}

JsonRpcEndpoint* JsonRpcTcpServer::findClient(QObject* socket)
{
    QTcpSocket* tcp_socket = qobject_cast<QTcpSocket*>(socket);
    auto it = m_client_endpoints.find(tcp_socket);
    return (it != m_client_endpoints.end()) ? it->second.get() : nullptr;
}

void JsonRpcTcpServer::newConnection()
{
    JCON_ASSERT(m_server.hasPendingConnections());
    if (m_server.hasPendingConnections()) {
        QTcpSocket* tcp_socket = m_server.nextPendingConnection();

        JCON_ASSERT(tcp_socket);
        if (!tcp_socket) {
            logError("pending socket was null");
            return;
        }

        logInfo("client connected: " + tcp_socket->peerAddress().toString());

        // TODO: maybe move this to base class?
        // {
        auto rpc_socket = std::make_shared<JsonRpcTcpSocket>(tcp_socket);

        auto endpoint =
            std::make_shared<JsonRpcEndpoint>(rpc_socket, log(), this);

        connect(endpoint.get(), &JsonRpcEndpoint::socketDisconnected,
                this, &JsonRpcTcpServer::clientDisconnected);

        connect(endpoint.get(), &JsonRpcEndpoint::jsonObjectReceived,
                this, &JsonRpcServer::jsonRequestReceived);

        m_client_endpoints[tcp_socket] = endpoint;
        // }
    }
}

void JsonRpcTcpServer::clientDisconnected(QObject* client_socket)
{
    QTcpSocket* tcp_socket = qobject_cast<QTcpSocket*>(client_socket);
    JCON_ASSERT(tcp_socket);
    if (!tcp_socket) {
        logError("client disconnected, but socket is null");
        return;
    }

    logInfo("client disconnected: " + tcp_socket->peerAddress().toString());
    auto it = m_client_endpoints.find(tcp_socket);
    JCON_ASSERT(it != m_client_endpoints.end());
    if (it == m_client_endpoints.end()) {
        logError("unknown client disconnected");
        return;
    }
    m_client_endpoints.erase(it);
}

}
