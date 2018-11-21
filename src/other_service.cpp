#include "other_service.h"

#include <QtGlobal>

OtherService::OtherService() = default;

OtherService::~OtherService() = default;

int OtherService::getRandomInt(int limit)
{
    return -(qrand() % limit);
}
