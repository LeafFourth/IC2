#include "register_p.h"

#include <assert.h>

#include <iostream>
#include <map>
#include <sstream>
#include <vector>

#include "json/JsonObject.h"

#include "utilities/string_util.h"

static std::map<std::string, JSMethodCall*> g_JSCalls;

/*
*TODO:
*  1、app 退出时的清除工作
*  2、线程安全
*/

static std::vector<MethodArg*> MakeCallArgs(const std::string &callTypes, const std::string &data)
{
    std::vector<MethodArg*> ret;

    JsonObject dataObj = JsonObject::Parse(data);
    std::vector<std::string> types = SplitStr(callTypes, ';', true);
    size_t i = 0;
    for (auto it : types) {
        std::stringstream format;
        format << "arg" << i;

        if (it == "string") {
            ret.push_back(new MethodArgImpl<std::string>(dataObj[format.str()].toString()));
        }
        else if (it == "double") {
            ret.push_back(new MethodArgImpl<double>(dataObj[format.str()].toDouble()));
        }
        else if (it == "int") {
            ret.push_back(new MethodArgImpl<int>(dataObj[format.str()].toInt()));
        }
        else if (it == "bool") {
            ret.push_back(new MethodArgImpl<bool>(dataObj[format.str()].toBool()));
        }
        else {
            assert(false);
        }
        ++i;
    }

    return std::move(ret);
}

static void ReleaseCallArgs(std::vector<MethodArg*> &args)
{
    for (auto it : args) {
        if (it) {
            delete it;
        }
    }

    args.clear();
}

bool JSParse(const std::string &data, JsCallMeta &meta)
{
    JsonObject jsObj = JsonObject::Parse(data);
    JsonObject call = jsObj["call"];
    if (!call.isString()) {
        std::cout << "call error" << std::endl;
        return false;
    }

    meta.call_ = call.toString();
    if (meta.call_.empty()) {
        std::cout << "call empty" << std::endl;
        return false;
    }

    JsonObject callData = jsObj["data"];
    if (!callData.isObject()) {
        std::cout << "call data error" << std::endl;
        return false;
    }

    JsonObject argNum = callData["argNum"];
    if (!argNum.isInt()) {
        std::cout << "arg num error" << std::endl;
        return false;
    }

    std::string types;
    int num = argNum.toInt();
    bool error = false;
    for (int i = 0; i < num; ++i) {
        std::stringstream format;
        format << "arg" << i;

        std::string key = format.str();

        JsonObject arg = callData[key];
        if (arg.isString()) {
            types.append("string").append(";");
        }
        else if (arg.isInt()) {
            types.append("number").append(";");
        }
        else if (arg.isBool()) {
            types.append("bool").append(";");
        }
        else {
            std::cout << "unsupport type" << std::endl;
            error = true;
            break;;
        }
    }

    if (error)
        return false;

    meta.data_ = callData.toJson();
    meta.argsType_ = types;
    return true;
}

static bool JSTypesIsCompatible(const std::string &jsonTypes, const std::string &funTypes)
{
    std::vector<std::string> types1 = SplitStr(jsonTypes, ';', true);
    std::vector<std::string> types2 = SplitStr(funTypes, ';', true);
    if (types1.size() != types2.size()) {
        return false;
    }

    size_t size = types1.size();
    bool result = true;
    for (size_t i = 0; i < size; ++i) {
        if (types1[i] == types2[i]) {
            continue;
        }

        if (types1[i] == "number") {
            if (types2[i] == "double" || types2[i] == "int") {
                continue;
            }
		} else if (types1[i] == types2[i]) {
			continue;
		}

        result = false;
        break;
    }

    return result;
}

bool JSMetaCall(JsCallMeta &meta)
{
    JSMethodCall *call = JSGetCall(meta.call_);
    if (!call) {
        return false;
    }

    std::string realTypes = call->argsType();

    if (!JSTypesIsCompatible(meta.argsType_, realTypes)) {
		std::cout << "uncompatible type:" << meta.argsType_ << "|" << realTypes << std::endl;
        return false;
    }

    std::vector<MethodArg*> args = MakeCallArgs(call->argsType(), meta.data_);
    call->call(args);
    ReleaseCallArgs(args);

    return true;
}

void JSStoreCall(const std::string &tag, JSMethodCall *call)
{
    JSMethodCall *old = JSGetCall(tag);
    if (old) {
        delete old;
        old = nullptr;
    }

    g_JSCalls.insert(std::make_pair(tag, call));
}

JSMethodCall *JSGetCall(const std::string &tag)
{
    auto it = g_JSCalls.find(tag);
    if (it == g_JSCalls.end()) {
        return nullptr;
    }

    return it->second;
}

void JSClearCall(const std::string &tag)
{
    g_JSCalls.erase(tag);
}


void JSUnregisterCall(const std::string &tag)
{
    JSClearCall(tag);
}

bool JSInvoke(const std::string &data)
{
    bool result = false;
    struct JsCallMeta meta;

    result = JSParse(data, meta);
    if (!result) {
        return false;
    }

    result = JSMetaCall(meta);
    if (!result) {
        return false;
    }

    return true;
}