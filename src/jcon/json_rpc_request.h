#ifndef JSONRPCREQUEST_H
#define JSONRPCREQUEST_H

#include "jcon.h"

#include <QDateTime>
#include <QObject>

namespace jcon {

class JCON_API JsonRpcRequest : public QObject
{
    Q_OBJECT

public:
    JsonRpcRequest(QObject* parent,
                   QString id,
                   QDateTime timestamp = QDateTime::currentDateTime());
    virtual ~JsonRpcRequest();

signals:
    void result(const QVariant& result);
    void error(int code, const QString& message, const QVariant& data);

private:
    QString m_id;
    QDateTime m_timestamp;
};

}

#endif
