#include "example_service.h"

#include <jcon/json_rpc_tcp_server.h>

#include <QDebug>
#include <QtGlobal>

ExampleService::ExampleService() = default;

ExampleService::~ExampleService() = default;

int ExampleService::getRandomInt(int limit)
{
    qDebug().noquote() << QString("-> getRandomInt: '%1' (client IP: %2)")
        .arg(limit)
        .arg(jcon::JsonRpcServer::clientEndpoint()->peerAddress().toString());
    return qrand() % limit;
}

QString ExampleService::printMessage(const QString& msg)
{
    qDebug().noquote() << QString("-> printMessage: '%1'").arg(msg);
    return QString("Return: '%1'").arg(msg);
}

void ExampleService::printNotification(const QString &msg) {
    qDebug().noquote() << QString("-> printNotification: '%1'").arg(msg);
}

void ExampleService::namedParams(QString& msg, int answer)
{
    qDebug().noquote() << QString("-> namedParams");
    qDebug().noquote() << "  msg: " << msg;
    qDebug().noquote() << "  answer: " << answer;
}

jcon::JsonRpcFuture ExampleService::futureGetRandomInt(int limit)
{
    qDebug().noquote() << QString("-> getRandomInt: '%1' (client IP: %2)")
    .arg(limit)
    .arg(jcon::JsonRpcServer::clientEndpoint()->peerAddress().toString());

    return {[limit]{
        QThread::sleep(5);
        return qrand() % limit;
    }};
}
