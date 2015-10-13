#ifndef JSON_RPC_WEBSOCKET_SERVER_H
#define JSON_RPC_WEBSOCKET_SERVER_H

#include "jcon.h"
#include "json_rpc_server.h"
#include "json_rpc_endpoint.h"
#include "json_rpc_socket.h"

#include <map>

class QWebSocket;
class QWebSocketServer;

namespace jcon {

class JCON_API JsonRpcWebSocketServer : public JsonRpcServer
{
    Q_OBJECT

public:
    JsonRpcWebSocketServer(QObject* parent = nullptr,
                           JsonRpcLoggerPtr logger = nullptr);
    virtual ~JsonRpcWebSocketServer();

    void listen(int port) override;
    void close() override;

protected:
    JsonRpcEndpoint* findClient(QObject* socket) override;

private slots:
    /// Called when the underlying QWebSocketServer gets a new client
    /// connection.
    void newConnection() override;

    /// Called when the underlying QWebSocketServer loses a client connection.
    void clientDisconnected(QObject* client_socket) override;

private:
    QWebSocketServer* m_server;

    /// Clients are uniquely identified by their QWebSocket*.
    std::map<QWebSocket*, JsonRpcEndpointPtr> m_client_endpoints;
};

}

#endif
