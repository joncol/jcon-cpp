#ifndef JSON_RPC_LOGGER_H
#define JSON_RPC_LOGGER_H

#include "jcon.h"

#include <memory>

class QString;

namespace jcon {

class JCON_API JsonRpcLogger
{
public:
    JsonRpcLogger();
    virtual ~JsonRpcLogger();

    virtual void logInfo(const QString& message) = 0;
    virtual void logWarning(const QString& message) = 0;
    virtual void logError(const QString& message) = 0;
};

typedef std::shared_ptr<JsonRpcLogger> JsonRpcLoggerPtr;

}

#endif
