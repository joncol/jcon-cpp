#ifndef JSON_RPC_TCP_CLIENT_H
#define JSON_RPC_TCP_CLIENT_H

#include "json_rpc_client.h"

namespace jcon {

class JCON_API JsonRpcTcpClient : public JsonRpcClient
{
    Q_OBJECT

public:
    JsonRpcTcpClient(QObject* parent = nullptr,
                     JsonRpcLoggerPtr logger = nullptr);
    virtual ~JsonRpcTcpClient();
};

}

#endif
