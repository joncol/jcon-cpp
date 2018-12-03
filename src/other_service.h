#pragma once

#include <QObject>

class OtherService : public QObject
{
    Q_OBJECT

public:
    OtherService();
    virtual ~OtherService();

    Q_INVOKABLE int getRandomInt(int limit);
};
