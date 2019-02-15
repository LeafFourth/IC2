// JSBridge.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"

#include <assert.h>

#include <functional>
#include <iostream>
#include <string>
#include <sstream>
#include <type_traits>
#include <vector>

#include "json/JsonObject.h"

#include "bridge/register.h"

#include "utilities/string_util.h"

void f(int i)
{
    printf("test:%d\n", i);
}

void TestMethodCall()
{
    JSMethodCall *call = JSMakeCall(f);
    std::vector<MethodArg*> args;
    args.push_back(new MethodArgImpl<int>(1));
    call->call(args);
    std::cout << call->argsType();
}

void TestJson()
{
    const std::string jsonStr(
R"({
    "ArgNum": 2,
    "Arg0": "hello",
    "Arg1": true,
    "obj": {"a": "fff", "arr":[1, 2, 3]}
})");


    JsonObject jsonObj = JsonObject::Parse(jsonStr);
    if (!jsonObj.isObject()) {
        std::cout << "str is not obj" << std::endl;

        return;
    }

    JsonObject ArgNum = jsonObj["ArgNum"];
    if (ArgNum.isInt()) {
        std::cout << "ArgNum:" << ArgNum.toInt() << std::endl;
    }
    else {
        std::cout << "get ArgNum error" << std::endl;
    }

    JsonObject a = jsonObj["obj.a"];
    if (a.isString()) {
        std::cout << "a:" << a.toString() << std::endl;
    } else {
        std::cout  << "get a error" << std::endl;
    }

    JsonObject arr = jsonObj["obj.arr"];
    if (arr.isArray()) {
        JsonArray jsonArr = arr.toArray();
        std::cout << "arr[2]:" << jsonArr[2].toInt() << std::endl;
    } else {
        std::cout << "get arr[2] error" << std::endl;
    }
}

void LocalCall(const std::string &a, double b)
{
	std::cout << "LocalCall(const std::string &a, double b):" << a << "|" << b << std::endl;
}

void LocalCall1(int a, std::string &b)
{
	std::cout << "LocalCall(int a, std::string &b):" << a << "|" << b << std::endl;
}

void LocalCall2(int a, const std::string &b)
{
	std::cout << "LocalCall2(int a, const std::string &b):" << a << "|" << b << std::endl;
}

void LocalCall3(int a, std::string b)
{
	std::cout << "LocalCall3(int a, std::string &b):" << a << "|" << b << std::endl;
}


void TestJSCall()
{
	REGISTER_JS_CALL(LocalCall);
	REGISTER_JS_CALL(LocalCall1);
	REGISTER_JS_CALL(LocalCall2);
	REGISTER_JS_CALL(LocalCall3);

	{
		const std::string jsonStr(
			R"({
				"call":"LocalCall",
				"data": {
				"argNum": 2,
				"arg0": 1,
				"arg1": "true"
				}
			})");

		
		JSInvoke(jsonStr);
	}

	{
		const std::string jsonStr(
			R"({
				"call":"LocalCall1",
				"data": {
				"argNum": 2,
				"arg0": 1,
				"arg1": "true"
				}
			})");


		JSInvoke(jsonStr);
	}

	{
		const std::string jsonStr(
			R"({
				"call":"LocalCall2",
				"data": {
				"argNum": 2,
				"arg0": 1,
				"arg1": "true"
				}
			})");


		JSInvoke(jsonStr);
	}

	{
		const std::string jsonStr(
			R"({
				"call":"LocalCall3",
				"data": {
				"argNum": 2,
				"arg0": 1,
				"arg1": "true"
				}
			})");


		JSInvoke(jsonStr);
	}
}

static void BindF(int*)
{

}

static void BindF2(int a, const std::string &c)
{

}

static void BindF3(int a, int b)
{

}

static void BindF4(int a, const JsonObject &obj)
{

}

void TestBind()
{

	int a = 0;
	//JSMakeCall(BindF, (int*)&a);
	//JSMakeCall(BindF, (const int*)&a);

	REGISTER_JS_CALL(BindF, (int*)&a);
	//REGISTER_JS_CALL(BindF2, (int*)&a);
	REGISTER_JS_CALL(BindF2, a, std::placeholders::_1);
  REGISTER_JS_CALL(BindF4, a, std::placeholders::_1);

	//auto ff22 = std::bind(BindF2, a, std::placeholders::_1);
	//ff22(std::string("123"));
	//JSMakeCall(BindF2, a, std::placeholders::_2);

	//JSRegisterCall("BindF2", BindF2, a, std::placeholders::_2);

	{
		const std::string jsonStr(
			R"({
				"call":"BindF2",
				"data": {
				"argNum": 1,
				"arg0": "true"
				}
			})");


		JSInvoke(jsonStr);
	}

  {
    const std::string jsonStr(
      R"({
				  "call":"BindF4",
				  "data": {
				  "argNum": 1,
				  "arg0": "true"
				  }
			  })");


    JSInvoke(jsonStr);
  }

  {
    const std::string jsonStr(
      R"({
				  "call":"BindF4",
				  "data": {
				  "argNum": 1,
          "arg0": {}
				  }
			  })");


    JSInvoke(jsonStr);
  }
}





/*
 * test native calling js
 */
void TestCallJS()
{
    std::string json1 = NativeInvokeStr("fun");
    std::cout << json1 << std::endl;

    std::string json2 = NativeInvokeStr("fun", 1);
    std::cout << json1 << std::endl;

    std::string json3 = NativeInvokeStr("fun", JsonObject::Parse("{\"a\": 1}"));
    std::cout << json3 << std::endl;

    std::string json4 = NativeInvokeStr("fun", 1, JsonObject::Parse("{\"a\": 1}"));
    std::cout << json4 << std::endl;

}



int _tmain(int argc, _TCHAR* argv[])
{
	//TestJSCall();
	//TestBind();

    //TestCallJS();
    getchar();

    return 0;
}



