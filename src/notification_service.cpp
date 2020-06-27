#include "notification_service.h"

#include <QMap>
#include <QTimer>
#include <QVariant>

NotificationService::NotificationService(QObject *parent) : QObject(parent),
    m_timer(new QTimer(this))
{
    m_timer->setInterval(2000);
    connect(m_timer, &QTimer::timeout, this, [this](){
        emit sendUnsolicitedNotification("myString", "myValue");
        emit sendUnsolicitedNotification("myInteger", 1);

        QMap<QString, QVariant> map;
        map["one"] = 1;
        map["three"] = 3;
        map["seven"] = 7;
        emit sendUnsolicitedNotification("myMap", map);

        emit sendUnsolicitedNotification("myDouble", 54.53);
    });
    m_timer->start();
}

NotificationService::~NotificationService() = default;
