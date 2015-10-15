#ifndef JSONRPCENDPOINT_H
#define JSONRPCENDPOINT_H

#include "jcon.h"
#include "json_rpc_logger.h"
#include "json_rpc_socket.h"

#include <QByteArray>

#include <memory>

class QJsonObject;
class QTcpSocket;

namespace jcon {

/**
 * Abstraction layer around JsonRpcSocket. Takes care of deserializing complete
 * JSON objects from byte stream.
 */
class JCON_API JsonRpcEndpoint : public QObject
{
    Q_OBJECT

public:
    JsonRpcEndpoint(JsonRpcSocketPtr socket,
                    JsonRpcLoggerPtr logger,
                    QObject* parent = nullptr);
    virtual ~JsonRpcEndpoint();

    bool connectToHost(const QString& host, int port, int msecs = 5000);
    void connectToHostAsync(const QString& host, int port);
    void disconnectFromHost();
    bool isConnected() const;

    QHostAddress localAddress() const;
    int localPort() const;
    QHostAddress peerAddress() const;
    int peerPort() const;

    void send(const QJsonDocument& doc);

signals:
    /**
     * Emitted for every JSON object received.
     *
     * @param[in] obj The JSON object received.
     * @param[in] sender The socket identifier (e.g. a QTcpSocket*).
     */
    void jsonObjectReceived(const QJsonObject& obj, QObject* sender);

    /// Emitted when the underlying socket is connected.
    void socketConnected(QObject* socket);

    /// Emitted when the underlying socket is disconnected.
    void socketDisconnected(QObject* socket);

private slots:
    void dataReceived(const QByteArray& bytes, QObject* socket);

private:
    /** Check buffer for complete JSON objects, and emit jsonObjectReceived for
        each one. */
    QByteArray processBuffer(const QByteArray& buf, QObject* socket);

    JsonRpcLoggerPtr m_logger;
    JsonRpcSocketPtr m_socket;
    QByteArray m_recv_buffer;
};

typedef std::shared_ptr<JsonRpcEndpoint> JsonRpcEndpointPtr;

}

#endif
