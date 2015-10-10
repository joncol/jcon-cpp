#ifndef JSONRPCTCPCLIENT_H
#define JSONRPCTCPCLIENT_H

#include "json_rpc_client.h"
#include "json_rpc_logger.h"

#include <QObject>

namespace jcon {

class JsonRpcTcpClient : public JsonRpcClient
{
    Q_OBJECT

public:
    JsonRpcTcpClient(QObject* parent = nullptr,
                     JsonRpcLoggerPtr logger = nullptr);
    virtual ~JsonRpcTcpClient();
};

}

#endif
