#include "example_service.h"

#include <QDebug>
#include <QtGlobal>

ExampleService::ExampleService() = default;

ExampleService::~ExampleService() = default;

int ExampleService::getRandomInt(int limit)
{
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

void ExampleService::namedParams(const QVariantMap& m)
{
    qDebug().noquote() << QString("-> namedParams");
    qDebug().noquote() << QString("  Argument count: %1").arg(m.keys().size());
    qDebug().noquote() << m["msg"].toString();
    qDebug().noquote() << m["answer"].toInt();
}
