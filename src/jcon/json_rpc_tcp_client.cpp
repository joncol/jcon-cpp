#include "json_rpc_tcp_client.h"
#include "json_rpc_tcp_socket.h"

#include <memory>

namespace jcon {

JsonRpcTcpClient::JsonRpcTcpClient(QObject* parent, JsonRpcLoggerPtr logger)
    : JsonRpcClient(std::make_shared<JsonRpcTcpSocket>(), parent, logger)
{
}

JsonRpcTcpClient::~JsonRpcTcpClient()
{
}

}
