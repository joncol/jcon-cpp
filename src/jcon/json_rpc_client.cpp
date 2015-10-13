#include "json_rpc_client.h"
#include "json_rpc_file_logger.h"
#include "jcon_assert.h"

#include <memory>

namespace jcon {

const QString JsonRpcClient::InvalidRequestId = "";

JsonRpcClient::JsonRpcClient(JsonRpcSocketPtr socket,
                             QObject* parent,
                             JsonRpcLoggerPtr logger)
    : QObject(parent)
    , m_logger(logger)
{
    if (!m_logger) {
        m_logger = std::make_shared<JsonRpcFileLogger>("client_log.txt");
    }

    m_endpoint = std::make_shared<JsonRpcEndpoint>(socket, m_logger, this);
}

JsonRpcClient::~JsonRpcClient()
{
    disconnectFromServer();
}

bool JsonRpcClient::connectToServer(const QString& host, int port)
{
    if (!m_endpoint->connectToHost(host, port)) {
        return false;
    }

    connect(m_endpoint.get(), &JsonRpcEndpoint::jsonObjectReceived,
            this, &JsonRpcClient::jsonResponseReceived);

    return true;
}

void JsonRpcClient::disconnectFromServer()
{
    m_endpoint->disconnectFromHost();
    m_endpoint->disconnect(this);
}

void JsonRpcClient::jsonResponseReceived(const QJsonObject& response)
{
    JCON_ASSERT(response["jsonrpc"].toString() == "2.0");

    if (response.value("jsonrpc").toString() != "2.0") {
        logError("invalid protocol tag");
        return;
    }

    if (response.value("error").isObject()) {
        int code;
        QString msg;
        QVariant data;
        getJsonErrorInfo(response, code, msg, data);
        logError(QString("(%1) - %2").arg(code).arg(msg));

        RequestId id = response.value("id").toString(InvalidRequestId);
        if (id != InvalidRequestId) {
            auto it = m_outstanding_requests.find(id);
            if (it == m_outstanding_requests.end()) {
                logError(QString("got error response for non-existing "
                                 "request: %1").arg(id));
                return;
            }
            emit it->second->handleError(code, msg, data);
            m_outstanding_requests.erase(it);
        }

        return;
    }

    if (response["result"].isUndefined()) {
        logError("result is undefined");
        return;
    }

    RequestId id = response.value("id").toString(InvalidRequestId);
    if (id == InvalidRequestId) {
        logError("response ID is undefined");
        return;
    }

    QVariant result = response.value("result").toVariant();

    auto it = m_outstanding_requests.find(id);
    if (it == m_outstanding_requests.end()) {
        logError(QString("got response to non-existing request: %1").arg(id));
        return;
    }

    emit it->second->handleResult(result);
    m_outstanding_requests.erase(it);
}

void JsonRpcClient::getJsonErrorInfo(const QJsonObject& response,
                                     int& code,
                                     QString& message,
                                     QVariant& data)
{
    QJsonObject error = response["error"].toObject();
    code = error["code"].toInt();
    message = error["message"].toString("unknown error");
    data = error.value("data").toVariant();
}

void JsonRpcClient::logError(const QString& msg)
{
    m_logger->logError("JSON RPC client error: " + msg);
}

}
