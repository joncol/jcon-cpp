#ifndef JSON_RPC_FILE_LOGGER_H
#define JSON_RPC_FILE_LOGGER_H

#include "json_rpc_logger.h"

#include <QFile>
#include <QTextStream>

namespace jcon {

class JsonRpcFileLogger : public JsonRpcLogger
{
public:
    JsonRpcFileLogger(const QString& filename);
    ~JsonRpcFileLogger() override;

    void logInfo(const QString& message) override;
    void logWarning(const QString& message) override;
    void logError(const QString& message) override;

private:
    QFile m_file;
    QTextStream m_stream;
};

typedef std::shared_ptr<JsonRpcLogger> JsonRpcLoggerPtr;

}

#endif
