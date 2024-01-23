#include "example_service.h"
#include "other_service.h"
#include "notification_service.h"

#include <jcon/json_rpc_tcp_client.h>
#include <jcon/json_rpc_tcp_server.h>
#include <jcon/json_rpc_websocket_client.h>
#include <jcon/json_rpc_websocket_server.h>

#include <QCoreApplication>
#include <QUrl>

#include <ctime>
#include <iostream>
#include <memory>

enum class SocketType {tcp, websocket};

jcon::JsonRpcServer* startServer(QObject* parent,
                                 SocketType socket_type = SocketType::tcp,
                                 bool allow_notifications = false)
{
    jcon::JsonRpcServer* rpc_server;
    if (socket_type == SocketType::tcp) {
        qDebug() << "Starting TCP server";
        rpc_server = new jcon::JsonRpcTcpServer(parent);
    } else {
        qDebug() << "Starting WebSocket server";
        rpc_server = new jcon::JsonRpcWebSocketServer(parent);
    }

    if (allow_notifications)
        rpc_server->enableSendNotification(true);

    auto service1 = new ExampleService;
    auto service2 = new NotificationService;

    rpc_server->registerServices({ service1, service2 });
    rpc_server->listen(6002);
    return rpc_server;
}

jcon::JsonRpcServer* startNamespacedServer(
    QObject* parent,
    SocketType socket_type = SocketType::tcp,
    bool allow_notifications = false)
{
    jcon::JsonRpcServer* rpc_server;
    if (socket_type == SocketType::tcp) {
        qDebug() << "Starting TCP server";
        rpc_server = new jcon::JsonRpcTcpServer(parent);
    } else {
        qDebug() << "Starting WebSocket server";
        rpc_server = new jcon::JsonRpcWebSocketServer(parent);
    }

    if (allow_notifications)
        rpc_server->enableSendNotification(true);

    auto service1 = new ExampleService;
    auto service2 = new OtherService;
    auto service3 = new NotificationService;

    rpc_server->registerServices(
        {
            { service1, "ex/myFirstNamespace" },
            { service2, "ex/myOtherNamespace" },
            { service3, "ex/myNotificationNamespace" }
        }, "/");
    rpc_server->listen(6002);
    return rpc_server;
}

jcon::JsonRpcClient* startClient(QObject* parent,
                                 SocketType socket_type = SocketType::tcp,
                                 bool allow_notifications = false)
{
    jcon::JsonRpcClient* rpc_client;
    if (socket_type == SocketType::tcp) {
        rpc_client = new jcon::JsonRpcTcpClient(parent);
        rpc_client->connectToServer("127.0.0.1", 6002);
    } else {
        rpc_client = new jcon::JsonRpcWebSocketClient(parent);
        // This is just to illustrate the fact that connectToServer also accepts
        // a QUrl argument.
        rpc_client->connectToServer(QUrl("ws://127.0.0.1:6002"));
    }

    if (allow_notifications)
        rpc_client->enableReceiveNotification(true);

    return rpc_client;
}

void invokeMethodAsync(jcon::JsonRpcClient* rpc_client)
{
    auto req = rpc_client->callAsync("getRandomInt", 10);

    req->connect(req.get(), &jcon::JsonRpcRequest::result,
                 [](const QVariant& result) {
                     qDebug() << "result of asynchronous RPC call:" << result;
                 });

    req->connect(req.get(), &jcon::JsonRpcRequest::error,
                 [](int code, const QString& message) {
                     qDebug() << "RPC error:" << message
                              << " (" << code << ")";
                 });
}

void invokeMethodSync(jcon::JsonRpcClient* rpc_client)
{
    auto result = rpc_client->call("getRandomInt", 100);

    if (result->isSuccess()) {
        qDebug() << "result of synchronous RPC call:" << result->result();
    } else {
        qDebug() << "RPC error:" << result->toString();
    }
}

void invokeStringMethodAsync(jcon::JsonRpcClient* rpc_client)
{
    auto req = rpc_client->callAsync("printMessage", "hello, world");

    req->connect(req.get(), &jcon::JsonRpcRequest::result,
                 [](const QVariant& result) {
                     qDebug() << "result of asynchronous RPC call:" << result;
                 });

    req->connect(req.get(), &jcon::JsonRpcRequest::error,
                 [](int code, const QString& message) {
                     qDebug() << "RPC error:" << message
                              << " (" << code << ")";
                 });
}

void invokeNamedParamsMethodAsync(jcon::JsonRpcClient* rpc_client)
{
    auto req = rpc_client->callAsyncNamedParams("namedParams",
                                                QVariantMap{
                                                    {"msg", "hello, world"},
                                                    {"answer", 42}
                                                });

    req->connect(req.get(), &jcon::JsonRpcRequest::result,
                 [](const QVariant& result) {
                     qDebug() << "result of asynchronous RPC call:" << result;
                 });

    req->connect(req.get(), &jcon::JsonRpcRequest::error,
                 [](int code, const QString& message) {
                     qDebug() << "RPC error:" << message
                              << " (" << code << ")";
                 });
}

void invokeStringMethodSync(jcon::JsonRpcClient* rpc_client)
{
    auto result = rpc_client->call("printMessage", "hello, world");

    if (result->isSuccess()) {
        qDebug() << "result of synchronous RPC call:" << result->result();
    } else {
        qDebug() << "RPC error:" << result->toString();
    }
}

void invokeNotification(jcon::JsonRpcClient* rpc_client)
{
    rpc_client->notification("printNotification", "hello, notification");
}

void invokeNamespacedMethods(jcon::JsonRpcClient* rpc_client)
{
    auto result = rpc_client->call("ex/myFirstNamespace/getRandomInt", 100);

    if (result->isSuccess()) {
        qDebug() << "result of first namespaced synchronous RPC call:"
                 << result->result();
    } else {
        qDebug() << "RPC error:" << result->toString();
    }

    result = rpc_client->call("ex/myOtherNamespace/getRandomInt", 100);

    if (result->isSuccess()) {
        qDebug() << "result of second namespaced synchronous RPC call:"
                 << result->result();
    } else {
        qDebug() << "RPC error:" << result->toString();
    }
}

void waitForOutstandingRequests(jcon::JsonRpcClient* rpc_client)
{
    if (rpc_client->outstandingRequestCount() > 0) {
        qDebug().noquote() << QString("Waiting for %1 outstanding requests")
            .arg(rpc_client->outstandingRequestCount());

        while (rpc_client->outstandingRequestCount() > 0) {
            qDebug() << "Calling QCoreApplication::processEvents()";
            QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
        }
    } else {
        qDebug() << "No outstanding requests, quitting";
    }
}

/**
 * Example code of running both a server and a client and making some requests
 * from the client to the server.
 */
void runServerAndClient(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    {
        auto server = startServer(nullptr, SocketType::tcp);
        auto rpc_client = startClient(&app, SocketType::tcp);

        invokeNotification(rpc_client);
        invokeMethodAsync(rpc_client);
        invokeMethodSync(rpc_client);
        invokeStringMethodSync(rpc_client);
        invokeStringMethodAsync(rpc_client);
        invokeNamedParamsMethodAsync(rpc_client);

        waitForOutstandingRequests(rpc_client);
        delete server;
    }

    {
        auto server = startNamespacedServer(nullptr);
        auto rpc_client = startClient(&app, SocketType::tcp);
        invokeNamespacedMethods(rpc_client);
        waitForOutstandingRequests(rpc_client);
        delete server;
    }

    {
        auto server = startServer(nullptr, SocketType::tcp, true);
        auto rpc_client = startClient(&app, SocketType::tcp, true);
        QObject::connect(rpc_client, &jcon::JsonRpcClient::notificationReceived,
            &app, [](const QString& key, const QVariant& value){
                qDebug() << "Received notification:"
                         << "Key:" << key
                         << "Value:" << value;
            });
        app.exec();
        delete server;
    }
}

/**
 * Example of starting a server and keeping it running indefinitely.
 */
void runServer(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);
    auto server = startServer(nullptr, SocketType::websocket);
    app.exec();
}

int main(int argc, char* argv[])
{
    runServerAndClient(argc, argv);
    // runServer(argc, argv);
    return 0;
}
