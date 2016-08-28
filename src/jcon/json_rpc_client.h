#pragma once
#include "jcon.h"
#include "json_rpc_endpoint.h"
#include "json_rpc_error.h"
#include "json_rpc_logger.h"
#include "json_rpc_request.h"
#include "json_rpc_result.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

#include <map>
#include <memory>
#include <utility>

namespace jcon {

class JsonRpcSocket;

class JCON_API JsonRpcClient : public QObject
{
    Q_OBJECT

public:
    using RequestId = QString ;

    JsonRpcClient(std::shared_ptr<JsonRpcSocket> socket,
                  QObject* parent = nullptr,
                  std::shared_ptr<JsonRpcLogger> logger = nullptr);
    virtual ~JsonRpcClient();

    /// @return true if connection was successful
    bool connectToServer(const QString& host, int port);
    void disconnectFromServer();

    bool isConnected() const;

    QHostAddress clientAddress() const;
    int clientPort() const;

    QHostAddress serverAddress() const;
    int serverPort() const;

    template<typename... T>
    std::shared_ptr<JsonRpcResult> call(const QString& method, T&&... params);

    template<typename... T>
    std::shared_ptr<JsonRpcRequest> callAsync(const QString& method,
                                              T&&... params);

    /// Expand arguments in list before making the RPC call
    std::shared_ptr<JsonRpcResult> callExpandArgs(const QString& method,
                                                  const QVariantList& params);

    /// Expand arguments in list before making the RPC call
    std::shared_ptr<JsonRpcRequest>
        callAsyncExpandArgs(const QString& method, const QVariantList& params);

    JsonRpcError lastError() const { return m_last_error; }

signals:
    /// Emitted when a connection has been made to the server.
    void socketConnected(QObject* socket);

    /// Emitted when connection to server is lost.
    void socketDisconnected(QObject* socket);

    /// Emitted when the RPC socket has an error.
    void socketError(QObject* socket, QAbstractSocket::SocketError error);

    void syncCallSucceeded();
    void syncCallFailed();

protected:
    void logError(const QString& msg);

private slots:
    void syncCallResult(const QVariant& result);
    void syncCallError(int code, const QString& message, const QVariant& data);
    void jsonResponseReceived(const QJsonObject& obj);

private:
    static const int CallTimeout = 5000;
    static const QString InvalidRequestId;

    static QString getCallLogMessage(const QString& method,
                                     const QVariantList& params);

    std::shared_ptr<JsonRpcResult>
        waitForSyncCallbacks(const JsonRpcRequest* request);

    std::pair<std::shared_ptr<JsonRpcRequest>, QJsonObject>
        prepareCall(const QString& method);

    std::pair<std::shared_ptr<JsonRpcRequest>, RequestId> createRequest();
    static RequestId createUuid();
    QJsonObject createRequestJsonObject(const QString& method,
                                        const QString& id);

    void convertToQVariantList(QVariantList& result) {}

    template<typename T>
    void convertToQVariantList(QVariantList& result, T&& x);

    template<typename T, typename... Ts>
    void convertToQVariantList(QVariantList& result, T&& head, Ts&&... tail);

    static void getJsonErrorInfo(const QJsonObject& response,
                                 int& code,
                                 QString& message,
                                 QVariant& data);

    using RequestMap = std::map<RequestId, std::shared_ptr<JsonRpcRequest>>;

    std::shared_ptr<JsonRpcLogger> m_logger;
    std::shared_ptr<JsonRpcEndpoint> m_endpoint;
    RequestMap m_outstanding_requests;
    QVariant m_last_result;
    JsonRpcError m_last_error;
};

template<typename... T>
std::shared_ptr<JsonRpcResult>
JsonRpcClient::call(const QString& method, T&&... params)
{
    auto req = callAsync(method, std::forward<T>(params)...);
    return waitForSyncCallbacks(req.get());
}

template<typename... T>
std::shared_ptr<JsonRpcRequest>
JsonRpcClient::callAsync(const QString& method, T&&... params)
{
    if (!isConnected()) {
        m_logger->logError("cannot call RPC method when not connected");
        throw std::runtime_error("cannot call RPC method when not connected");
    }
    std::shared_ptr<JsonRpcRequest> request;
    QJsonObject req_json_obj;
    std::tie(request, req_json_obj) = prepareCall(method);

    QVariantList param_list;
    convertToQVariantList(param_list, std::forward<T>(params)...);
    req_json_obj["params"] = QJsonArray::fromVariantList(param_list);

    m_logger->logInfo(getCallLogMessage(method, param_list));
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
