这里提供了一个C++部分的JSBridge，主要完成：
    1、注册C++函数（JSRegisterCall），并且内部通过std::bind实现了普通方法（非成员方法，暂不提供成员函数的绑定）绑定部分参数的功能；
    2、从json内容解析并构造参数然后调用已注册的函数，参数支持四种类型：字符串、int、double、bool。
    3、提供一个简单的json封装，由cJson提供实现。
不能：
    1、不提供C++和JS的数据交换的通道，这个也是一个完整的JSBridge缺失的部分。由于JS和C++之间的通信途径很多，比如：C++的JS解析部分可以是一个浏览器内核如libcef， QtWebEngine，IE，这些内核都是通过二进制编程库（so或者dll）的形式存在，具备和C++通信的API，更有甚于完全可以通过本地socket提供信息交换的通道（这时，甚至可以提供运行于浏览器的页面和本地的应用的交互）




bridge解析参数并调用函数示例：
注意由JS提供的参数对应的方法参数必须是std::string、int、double、bool四种，可以是对应的引用类型
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

void TestBind()
{
	int a = 0;

	REGISTER_JS_CALL(BindF, (int*)&a);
    //REGISTER_JS_CALL(BindF, (const int*)&a);  //测试，编译不通过,被绑定的参数类型不兼容
	REGISTER_JS_CALL(BindF2, a, std::placeholders::_1);


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
}

int main()
{
    TestJSCall();
    TestBind(); 
}
