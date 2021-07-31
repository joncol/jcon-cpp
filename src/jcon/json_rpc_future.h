#pragma once

#include <functional>
#include <QMetaType>
#include <QThreadPool>
#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrentRun>
#include "json_rpc_endpoint.h"

namespace jcon {

    class JsonRpcFuture {
    public:
        JsonRpcFuture() = default;

        ~JsonRpcFuture() = default;

        JsonRpcFuture(const JsonRpcFuture &) = default;

        JsonRpcFuture &operator=(const JsonRpcFuture &) = default;

        JsonRpcFuture(std::function<QVariant()> f, QThreadPool *threadPool = QThreadPool::globalInstance());

        void start(QObject *receiver, jcon::JsonRpcEndpoint *endpoint, std::function<void(QVariant)> success,
                   std::function<void(void)> failure = [] {});

    private:
        void startPrivate(QObject *receiver, jcon::JsonRpcEndpoint *endpoint, std::function<void(QVariant)> success,
                          std::function<void(void)> failure = [] {});

        QThreadPool *threadPool;
        std::function<QVariant()> f;
        QFutureWatcher<QVariant> *watcher;
        std::shared_ptr<std::once_flag> scheduled;
    };

};

Q_DECLARE_METATYPE(jcon::JsonRpcFuture);