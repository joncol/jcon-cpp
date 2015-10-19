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
#include <utility>

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

    bool connectToServer(const QString& host, int port);
    void disconnectFromServer();

    bool isConnected() const;

    QHostAddress clientAddress() const;
    int clientPort() const;

    QHostAddress serverAddress() const;
    int serverPort() const;

    template<typename... T>
    RequestPtr call(const QString& method, T&&... params);

signals:
    /// Emitted when a connection has been made to the server.
    void socketConnected(QObject* socket);

    /// Emitted when connection to server is lost.
    void socketDisconnected(QObject* socket);

protected:
    void logError(const QString& msg);

private slots:
    void jsonResponseReceived(const QJsonObject& obj);

private:
    static const QString InvalidRequestId;

    // helper to minimize template size
    std::pair<RequestPtr, QJsonObject> prepareCall(const QString& method);
    std::pair<RequestPtr, RequestId> createRequest();
    static RequestId createUuid();
    QJsonObject createRequestJsonObject(const QString& method,
                                        const QString& id);

    template<typename T>
    void convertToQVariantList(QVariantList& result, T&& x);

    template<typename T, typename... Ts>
    void convertToQVariantList(QVariantList& result, T&& head, Ts&&... tail);

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
    RequestPtr request;
    QJsonObject req_json_obj;
    std::tie(request, req_json_obj) = prepareCall(method);

    if (sizeof...(T) > 0) {
        QVariantList param_list;
        convertToQVariantList(param_list, std::forward<T>(params)...);
        req_json_obj["params"] = QJsonArray::fromVariantList(param_list);
    }

    m_endpoint->send(QJsonDocument(req_json_obj));

    return request;
}

template<typename T>
void JsonRpcClient::convertToQVariantList(QVariantList& result, T&& x)
{
    result.push_front(x);
}

template<typename T, typename... Ts>
void JsonRpcClient::convertToQVariantList(QVariantList& result,
                                          T&& head, Ts&&... tail)
{
    convertToQVariantList(result, std::forward<Ts>(tail)...);
    result.push_front(head);
}

}

#endif
