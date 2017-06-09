#pragma once
#include <QObject>

class ExampleService : public QObject
{
    Q_OBJECT

public:
    ExampleService();
    virtual ~ExampleService();

    Q_INVOKABLE int getRandomInt(int limit);
    Q_INVOKABLE void printMessage(const QString& msg);
};
