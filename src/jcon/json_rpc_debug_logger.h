#ifndef JSON_RPC_DEBUG_LOGGER_H
#define JSON_RPC_DEBUG_LOGGER_H

#include "jcon.h"
#include "json_rpc_logger.h"

namespace jcon {

class JCON_API JsonRpcDebugLogger : public JsonRpcLogger
{
public:
    void logInfo(const QString& message) override;
    void logWarning(const QString& message) override;
    void logError(const QString& message) override;
};

}

#endif
