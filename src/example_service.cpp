#include "example_service.h"

#include <QDebug>
#include <QtGlobal>

ExampleService::ExampleService()
{
}

ExampleService::~ExampleService()
{
}

int ExampleService::getRandomInt(int limit)
{
    return qrand() % limit;
}
