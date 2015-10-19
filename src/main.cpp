#include "example_service.h"

#include <jcon/json_rpc_file_logger.h>
#include <jcon/json_rpc_tcp_client.h>
#include <jcon/json_rpc_tcp_server.h>

#include <QApplication>
#include <QPushButton>
#include <QThread>

#include <memory>

void startServer(QObject* parent)
{
    auto rpc_server = new jcon::JsonRpcTcpServer(parent);
    auto service = new ExampleService;
    rpc_server->registerService(service);
    rpc_server->listen(6002);
}

jcon::JsonRpcClientPtr startClient(QObject* parent)
{
    auto rpc_client = std::make_shared<jcon::JsonRpcTcpClient>(parent);
    rpc_client->connectToServer("127.0.0.1", 6002);

    return rpc_client;
}

void invokeMethod(jcon::JsonRpcClient* rpc_client)
{
    qsrand(time(nullptr));

    jcon::JsonRpcClient::RequestPtr req = rpc_client->call("getRandomInt", 10);

    req->connect(req.get(), &jcon::JsonRpcRequest::handleResult,
                 [](const QVariant& result) {
                     qDebug() << "result of RPC call: " << result;
                     qApp->exit();
                 });

    req->connect(req.get(), &jcon::JsonRpcRequest::handleError,
                 [](int code, const QString& message, const QVariant& data) {
                     qDebug() << "RPC error: " << message
                              << " (" << code << ")";
                     qApp->exit();
                 });
}

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    startServer(&app);
    auto rpc_client = startClient(&app);

    invokeMethod(rpc_client.get());

    return app.exec();
}
