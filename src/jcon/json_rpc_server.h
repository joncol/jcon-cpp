#ifndef JSONRPCSERVER_H
#define JSONRPCSERVER_H

#include "jcon.h"
#include "json_rpc_logger.h"

#include <QObject>

#include <memory>

namespace jcon {

class JsonRpcEndpoint;
class JsonRpcSocket;

class JCON_API JsonRpcServer : public QObject
{
    Q_OBJECT

public:
    JsonRpcServer(QObject* parent = nullptr, JsonRpcLoggerPtr logger = nullptr);
    virtual ~JsonRpcServer();

    virtual void registerService(QObject* service);

    virtual void listen(int port) = 0;
    virtual void close() = 0;

protected:
    virtual JsonRpcEndpoint* findClient(QObject* socket) = 0;

public slots:
    void jsonRequestReceived(const QJsonObject& request, QObject* socket);

protected slots:
    virtual void newConnection() = 0;
    virtual void clientDisconnected(QObject* client_socket) = 0;

protected:
    void logInfo(const QString& msg);
    void logError(const QString& msg);
    JsonRpcLoggerPtr log() { return m_logger; }

private:
    static const QString InvalidRequestId;

    enum ErrorCodes {
        EC_ParseError = -32700,
        EC_InvalidRequest = -32600,
        EC_MethodNotFound = -32601,
        EC_InvalidParams = -32602,
        EC_InternalError = -32603
    };

    bool dispatch(const QString& method_name,
                  const QVariant& params,
                  const QString& request_id,
                  QVariant& return_value);

    bool call(QObject* object,
              const QMetaMethod& meta_method,
              const QVariantList& args,
              QVariant& return_value);

    bool call(QObject* object,
              const QMetaMethod& meta_method,
              const QVariantMap& args,
              QVariant& return_value);


    bool convertArgs(const QMetaMethod& meta_method,
                     const QVariantList& args,
                     QVariantList& converted);

    bool convertArgs(const QMetaMethod& meta_method,
                     const QVariantMap& args,
                     QVariantList& converted);

    bool doCall(QObject* object,
                const QMetaMethod& meta_method,
                QVariantList& converted_args,
                QVariant& return_value);

    QJsonDocument createResponse(const QString& request_id,
                                 const QVariant& return_value,
                                 const QString& method_name);
    QJsonDocument createErrorResponse(const QString& request_id,
                                      int code,
                                      const QString& message);

    JsonRpcLoggerPtr m_logger;
    std::vector<std::shared_ptr<QObject>> m_services;
};

}

#endif
