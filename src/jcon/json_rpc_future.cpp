#include "json_rpc_future.h"

namespace jcon {
    JsonRpcFuture::JsonRpcFuture(std::function<QVariant()> f, QThreadPool *threadPool) :
            threadPool(threadPool != nullptr ? threadPool : QThreadPool::globalInstance()),
            f(std::move(f)),
            watcher(new QFutureWatcher<QVariant>()),
            scheduled(new std::once_flag) {
    }

    void JsonRpcFuture::start(QObject *receiver, jcon::JsonRpcEndpoint *endpoint, std::function<void(QVariant)> success,
                              std::function<void(void)> failure) {
        std::call_once(*scheduled, &JsonRpcFuture::startPrivate, this, receiver, endpoint, success, failure);
    }

    void JsonRpcFuture::startPrivate(QObject *receiver, jcon::JsonRpcEndpoint *endpoint,
                                     std::function<void(QVariant)> success,
                                     std::function<void(void)> failure) {
        QObject::connect(watcher, &QFutureWatcher<QVariant>::finished, receiver,
                         [endpoint, success = std::move(success), watcher = this->watcher] {
                             watcher->deleteLater();
                             if (endpoint != nullptr) {
                                 QObject::disconnect(endpoint, &jcon::JsonRpcEndpoint::socketDisconnected, watcher,
                                                     nullptr);
                             }
                             success(watcher->result());
                         });

        if (endpoint != nullptr) {
            QObject::connect(endpoint, &jcon::JsonRpcEndpoint::socketDisconnected, watcher,
                             [failure = std::move(failure), watcher = this->watcher, receiver] {
                                 watcher->deleteLater();
                                 QObject::disconnect(watcher, &QFutureWatcher<QVariant>::finished, receiver,
                                                     nullptr);
                                 failure();
                             });
        }

        watcher->setFuture(QtConcurrent::run(threadPool, f));
    }

};