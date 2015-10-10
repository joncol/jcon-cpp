#ifndef JSONRPCCLIENT_H
#define JSONRPCCLIENT_H

#include "jcon.h"
#include "json_rpc_endpoint.h"
#include "json_rpc_logger.h"
#include "json_rpc_request.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QUuid>

#include <map>
#include <memory>

namespace jcon {

class JsonRpcSocket;

class JCON_API JsonRpcClient : public QObject
{
    Q_OBJECT

public:
    typedef QString RequestId;
    typedef std::shared_ptr<JsonRpcRequest> RequestPtr;

    JsonRpcClient(JsonRpcSocketPtr socket,
                  QObject* parent = nullptr,
                  JsonRpcLoggerPtr logger = nullptr);
    virtual ~JsonRpcClient();

    void connectToServer(const QString& host, int port);
    void disconnectFromServer();

    template<typename... T>
    RequestPtr call(const QString& method, T&&... params);

protected:
    void logError(const QString& msg);

private slots:
    void jsonResponseReceived(const QJsonObject& obj);

private:
    static const QString InvalidRequestId;

    static void getJsonErrorInfo(const QJsonObject& response,
                                 int& code,
                                 QString& message,
                                 QVariant& data);

    typedef std::map<RequestId, RequestPtr> RequestMap;

    JsonRpcLoggerPtr m_logger;
    JsonRpcEndpointPtr m_endpoint;
    RequestMap m_outstanding_requests;
};

typedef std::shared_ptr<JsonRpcClient> JsonRpcClientPtr;

template<typename... T>
JsonRpcClient::RequestPtr JsonRpcClient::call(const QString& method,
                                              T&&... params)
{
    RequestId id = QUuid::createUuid().toString();
    auto request = std::make_shared<JsonRpcRequest>(this, id);
    m_outstanding_requests[id] = request;

    QJsonObject req_json_obj {
        { "jsonrpc", "2.0" },
        { "method", method },
        { "id", id }
    };

    if (sizeof...(params) > 0) {
        QJsonArray json_params{std::forward<T>(params)...};
        req_json_obj["params"] = json_params;
    }

    m_endpoint->send(QJsonDocument(req_json_obj));

    return request;
}

}

#endif
