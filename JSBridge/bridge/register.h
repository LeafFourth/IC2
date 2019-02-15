#ifndef REGISTER_H
#define REGISTER_H

#include <iostream>
#include <string>
#include <vector>

#include "impl/register_p.h"
//
//{
//    "call":"BindF4",
//        "data" : {
//        "argNum": 1,
//            "arg0" : "true"
//    }
//}

template <class FType, class ...T>
void JSRegisterCall(const std::string &tag, FType f, T&& ...args)
{
    JSMethodCall *call = JSMakeCall(f, std::forward<T>(args)...);

    JSStoreCall(tag, call);
}
#define REGISTER_JS_CALL(fun, ...) JSRegisterCall(std::string(#fun), fun, ##__VA_ARGS__)


void JSUnregisterCall(const std::string &tag);

bool JSInvoke(const std::string &data);


template <class ...Types>
std::string NativeInvokeStr(const std::string &method, Types&& ...args)
{
    JsonObject callJson;
    JsonObject dataJson;
    callJson.insert("call", method);

    NativeInvokehelper(dataJson, 0, std::forward<Types>(args)...);
    callJson.insert("data", dataJson);
    return std::move(callJson.toJson());
}


#endif // REGISTER_H
