#include "json_rpc_websocket_client.h"
#include "json_rpc_websocket.h"

#include <memory>

namespace jcon {

JsonRpcWebSocketClient::JsonRpcWebSocketClient(QObject* parent,
                                               JsonRpcLoggerPtr logger)
    : JsonRpcClient(std::make_shared<JsonRpcWebSocket>(), parent, logger)
{
}

JsonRpcWebSocketClient::~JsonRpcWebSocketClient()
{
}

}
