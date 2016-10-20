#pragma once

#include "jcon.h"

#include <QString>
#include <QVariant>

namespace jcon {

class JCON_API JsonRpcResult
{
public:
    virtual ~JsonRpcResult() {}

    virtual bool isSuccess() const = 0;
    virtual QVariant result() const = 0;
    virtual QString toString() const = 0;
};

}
