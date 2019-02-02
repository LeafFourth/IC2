#ifndef REGISTER_H
#define REGISTER_H

#include <iostream>
#include <string>
#include <vector>

#include "impl/register_p.h"

template <class FType, class ...T>
void JSRegisterCall(const std::string &tag, FType f, T&& ...args)
{
    JSMethodCall *call = JSMakeCall(f, std::forward<T>(args)...);

    JSStoreCall(tag, call);
}
#define REGISTER_JS_CALL(fun, ...) JSRegisterCall(std::string(#fun), fun, ##__VA_ARGS__)


void JSUnregisterCall(const std::string &tag);

bool JSInvoke(const std::string &data);


#endif // REGISTER_H
