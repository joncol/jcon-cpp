#ifndef EXAMPLESERVICE_H
#define EXAMPLESERVICE_H

#include <QObject>

class ExampleService : public QObject
{
    Q_OBJECT

public:
    ExampleService();
    virtual ~ExampleService();

    Q_INVOKABLE int getRandomInt(int limit);
};


#endif
