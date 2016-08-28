#pragma once
#include "json_rpc_client.h"

namespace jcon {

class JCON_API JsonRpcWebSocketClient : public JsonRpcClient
{
    Q_OBJECT

public:
    JsonRpcWebSocketClient(QObject* parent = nullptr,
                           std::shared_ptr<JsonRpcLogger> logger = nullptr);
    virtual ~JsonRpcWebSocketClient();
};

}
