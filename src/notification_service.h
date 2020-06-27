#pragma once
#include <QObject>

class QTimer;

class NotificationService : public QObject
{
    Q_OBJECT

public:
    NotificationService(QObject *parent = nullptr);
    virtual ~NotificationService();

signals:
    void sendUnsolicitedNotification(const QString&, const QVariant&);

private:
    QTimer *m_timer;
};
