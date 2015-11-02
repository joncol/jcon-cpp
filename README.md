## JCON-CPP

If you're using **C++ 11** and **Qt**, and want to create a **JSON RPC 2.0**
client or server, using either **TCP** or **WebSockets** as underlying transport
layer, then **JCON-CPP** might prove useful.

In all of the following, replace "Tcp" with "WebSocket" to change the transport
method.


## Creating a Server

```c++
auto rpc_server = new jcon::JsonRpcTcpServer(parent);
```

Create a service (a collection of invokable methods):

1. Make your service class inherit `QObject`
2. Make sure your service method is accessible by the Qt meta object system
   (either by using the `Q_INVOKABLE` macro or by putting the method in a
   `public slots:` section). For instance:

```c++
class ExampleService : public QObject
{
    Q_OBJECT

public:
    ExampleService();
    virtual ~ExampleService();

    Q_INVOKABLE int getRandomInt(int limit);
};
```

Parameters and return types are automatically matched against the JSON RPC call,
using the Qt Meta object system, and you can use lists (`QVariantList`) and
dictionary type objects (`QVariantMap`) in addition to the standard primitive
types such as `QString`, `bool`, `int`, `float`, etc.

Register your service with:

```c++
auto service = new ExampleService;
rpc_server->registerService(service);
```

The server will store a collection of smart pointers so it will take care of
releasing the memory allocated above.

Finally, start listening for client connections by:

```c++
rpc_server->listen(6001);
```

Specify whatever port you want to use.


## Creating a Client

Simple:

```c++
auto rpc_client = std::make_shared<jcon::JsonRpcTcpClient>(parent);
rpc_client->connectToServer("127.0.0.1", 6001);
```

(No real need to use a smart pointer here, since the destructor will be called
as long as a parent `QObject` is provided.)


### Invoking a Remote Method Asynchronously

```c++
jcon::JsonRpcRequestPtr req = rpc_client->callAsync("getRandomInt", 10);
```

The returned `JsonRpcRequestPtr` can be used to set up a callback that is
invoked when the result of the JSON RPC call is ready:

```c++
req->connect(req.get(), &jcon::JsonRpcRequest::result,
             [](const QVariant& result) {
                 qDebug() << "result of RPC call:" << result;
                 qApp->exit();
             });
```

To handle errors:

```c++
req->connect(req.get(), &jcon::JsonRpcRequest::error,
             [](int code, const QString& message, const QVariant& data) {
                 qDebug() << "RPC error: " << message << " (" << code << ")";
                 qApp->exit();
             });
```


### Invoking a Remote Method Synchronously

```c++
jcon::JsonRpcResult result = rpc_client->call("getRandomInt", 10);

if (result.isSuccess()) {
    QVariant res = result.result();
} else {
    jcon::JsonRpcError error = rpc_client->lastError();
    QString err_str = error.toString();
}
```


### Expanding a List of Arguments

If you want to expand a list of arguments (instead of passing the list as a
single argument), use `callExpandArgs` and `callAsyncExpandArgs`.


## Known Issues

* Error handling needs to be improved
* Does not yet support batch requests/responses


## Contributing

Bug reports and pull requests are welcome on GitHub at
https://github.com/joncol/jcon-cpp.


## License

The library is available as open source under the terms of the [MIT
License](http://opensource.org/licenses/MIT).
