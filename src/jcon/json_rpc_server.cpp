#include "json_rpc_server.h"
#include "json_rpc_endpoint.h"
#include "json_rpc_file_logger.h"
#include "jcon_assert.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVariant>
#include <QMetaMethod>

namespace jcon {

const QString JsonRpcServer::InvalidRequestId = "";

JsonRpcServer::JsonRpcServer(QObject* parent, JsonRpcLoggerPtr logger)
    : QObject(parent)
    , m_logger(logger)
{
    if (!m_logger) {
        m_logger = std::make_shared<JsonRpcFileLogger>("server_log.txt");
    }
}

JsonRpcServer::~JsonRpcServer()
{
}

void JsonRpcServer::registerService(QObject* service)
{
    m_services.push_back(std::shared_ptr<QObject>(service));
}

void JsonRpcServer::jsonRequestReceived(const QJsonObject& request,
                                        QObject* socket)
{
    JCON_ASSERT(request.value("jsonrpc").toString() == "2.0");

    if (request.value("jsonrpc").toString() != "2.0") {
        logError("invalid protocol tag");
        return;
    }

    QString method_name = request.value("method").toString();
    if (method_name.isEmpty()) {
        logError("no method present in request");
    }

    QVariant params = request.value("params").toVariant();

    QString request_id = request.value("id").toString(InvalidRequestId);

    QVariant return_value;
    if (!dispatch(method_name, params, request_id, return_value)) {
        auto msg = QString("method '%1' not found, check name and "
                           "parameter types ").arg(method_name);
        logError(msg);

        // send error response if request had valid ID
        if (request_id != InvalidRequestId) {
            QJsonDocument error =
                createErrorResponse(request_id, EC_MethodNotFound, msg);

            JsonRpcEndpoint* endpoint = findClient(socket);
            if (!endpoint) {
                logError("invalid client socket, cannot send response");
                return;
            }

            endpoint->send(error);
        }
    }

    // send response if request had valid ID
    if (request_id != InvalidRequestId) {
        QJsonDocument response = createResponse(request_id,
                                                return_value,
                                                method_name);

        JsonRpcEndpoint* endpoint = findClient(socket);
        if (!endpoint) {
            logError("invalid client socket, cannot send response");
            return;
        }

        endpoint->send(response);
    }
}

bool JsonRpcServer::dispatch(const QString& method_name,
                             const QVariant& params,
                             const QString& request_id,
                             QVariant& return_value)
{
    for (auto s : m_services) {
        const QMetaObject* meta_obj = s->metaObject();
        for (int i = 0; i < meta_obj->methodCount(); ++i) {
            auto meta_method = meta_obj->method(i);
            if (meta_method.name() == method_name) {
                if (call(s.get(), meta_method, params.toList(), return_value)) {
                    return true;
                }
            }
        }
    }
    return false;
}

// based on https://gist.github.com/andref/2838534.
// TODO: support named arguments
bool JsonRpcServer::call(QObject* object,
                         QMetaMethod meta_method,
                         QVariantList args,
                         QVariant& return_value)
{
    return_value = QVariant();

    QList<QByteArray> method_types = meta_method.parameterTypes();
    if (args.size() != method_types.size()) {
        // logError(QString("wrong number of arguments to method %1 -- "
        //                  "expected %2 arguments, but got %3")
        //          .arg(meta_method.methodSignature())
        //          .arg(meta_method.parameterCount())
        //          .arg(args.size()));
        return false;
    }

    QVariantList converted;

    for (int i = 0; i < method_types.size(); i++) {
        const QVariant& arg = args.at(i);

        QByteArray arg_type_name = arg.typeName();
        QByteArray param_type_name = method_types.at(i);

        QVariant::Type param_type = QVariant::nameToType(param_type_name);

        QVariant copy = QVariant(arg);

        if (copy.type() != param_type) {
            if (copy.canConvert(param_type)) {
                if (!copy.convert(param_type)) {
                    // qDebug() << "cannot convert" << arg_type_name
                    //          << "to" << param_type_name;
                    return false;
                }
            }
        }

        converted << copy;
    }

    QList<QGenericArgument> arguments;

    for (int i = 0; i < converted.size(); i++) {

        // Notice that we have to take a reference to the argument, else we'd be
        // pointing to a copy that will be destroyed when this loop exits.
        QVariant& argument = converted[i];

        // A const_cast is needed because calling data() would detach the
        // QVariant.
        QGenericArgument generic_argument(
            QMetaType::typeName(argument.userType()),
            const_cast<void*>(argument.constData())
        );

        arguments << generic_argument;
    }

    const char* return_type_name = meta_method.typeName();
    int return_type = QMetaType::type(return_type_name);
    if (return_type != QMetaType::Void) {
        return_value = QVariant(return_type, nullptr);
    }

    QGenericReturnArgument return_argument(
        return_type_name,
        const_cast<void*>(return_value.constData())
    );

    // perform the call
    bool ok = meta_method.invoke(
        object,
        Qt::DirectConnection,
        return_argument,
        arguments.value(0),
        arguments.value(1),
        arguments.value(2),
        arguments.value(3),
        arguments.value(4),
        arguments.value(5),
        arguments.value(6),
        arguments.value(7),
        arguments.value(8),
        arguments.value(9)
    );

    if (!ok) {
        // qDebug() << "calling" << meta_method.methodSignature() << "failed.";
        return false;
    }

    return true;
}

QJsonDocument JsonRpcServer::createResponse(const QString& request_id,
                                            const QVariant& return_value,
                                            const QString& method_name)
{
    QJsonObject res_json_obj {
        { "jsonrpc", "2.0" },
        { "id", request_id }
    };

    // TODO: is there a cleaner way of doing this?

    if (return_value.type() == QVariant::List) {
       auto ret_doc = QJsonDocument::fromVariant(return_value);
       res_json_obj["result"] = ret_doc.array();
    } else if (return_value.type() == QVariant::Map) {
       auto ret_doc = QJsonDocument::fromVariant(return_value);
       res_json_obj["result"] = ret_doc.object();
    } else if (return_value.type() == QVariant::Int) {
        res_json_obj["result"] = return_value.toInt();
    } else if (return_value.type() == QVariant::Double) {
        res_json_obj["result"] = return_value.toDouble();
    } else if (return_value.type() == QVariant::Bool) {
        res_json_obj["result"] = return_value.toBool();
    } else {
        auto msg =
            QString("method '%1' has unknown return type: %2")
            .arg(method_name)
            .arg(return_value.type());
        logError(msg);
        return createErrorResponse(request_id, EC_InvalidRequest, msg);
    }

    return QJsonDocument(res_json_obj);
}

QJsonDocument JsonRpcServer::createErrorResponse(const QString& request_id,
                                                 int code,
                                                 const QString& message)
{
    QJsonObject error_object {
        { "code", code },
        { "message", message }
    };

    QJsonObject res_json_obj {
        { "jsonrpc", "2.0" },
        { "error", error_object },
        { "id", request_id }
    };
    return QJsonDocument(res_json_obj);
}

void JsonRpcServer::logInfo(const QString& msg)
{
    m_logger->logInfo("JSON RPC server: " + msg);
}

void JsonRpcServer::logError(const QString& msg)
{
    m_logger->logError("JSON RPC server error: " + msg);
}

}
