#include "other_service.h"

#include <QRandomGenerator>

OtherService::OtherService() = default;

OtherService::~OtherService() = default;

int OtherService::getRandomInt(int limit)
{
    return -(QRandomGenerator::global()->generate() % limit);
}
