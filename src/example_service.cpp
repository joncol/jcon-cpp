#include "example_service.h"

#include <QDebug>
#include <QtGlobal>

ExampleService::ExampleService() = default;

ExampleService::~ExampleService() = default;

int ExampleService::getRandomInt(int limit)
{
    return qrand() % limit;
}

void ExampleService::printMessage(const QString& msg)
{
    qDebug().noquote() << QString("-> printMessage: '%1'").arg(msg);
}
