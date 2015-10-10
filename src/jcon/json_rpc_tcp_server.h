#ifndef JSONRPCTCPSERVER_H
#define JSONRPCTCPSERVER_H

#include "jcon.h"
#include "json_rpc_server.h"
#include "json_rpc_endpoint.h"
#include "json_rpc_socket.h"

#include <QTcpServer>

#include <map>

namespace jcon {

class JCON_API JsonRpcTcpServer : public JsonRpcServer
{
    Q_OBJECT

public:
    JsonRpcTcpServer(QObject* parent = nullptr,
                     JsonRpcLoggerPtr logger = nullptr);
    virtual ~JsonRpcTcpServer();

    void listen(const QString& address, int port) override;
    void close() override;

protected:
    JsonRpcEndpoint* findClient(QObject* socket) override;

private slots:
    /// Called when the underlying QTcpServer gets a new client connection.
    void newConnection() override;

    /// Called when the underlying QTcpServer loses a client connection.
    void clientDisconnected(QObject* client_socket) override;

private:
    QTcpServer m_server;

    /// Clients are uniquely identified by their QTcpSocket*.
    std::map<QTcpSocket*, JsonRpcEndpointPtr> m_client_endpoints;
};

}

#endif
