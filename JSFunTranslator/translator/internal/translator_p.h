#ifndef REGISTER_P_H
#define REGISTER_P_H

#include <assert.h>
#include <functional>
#include <string>
#include <type_traits>
#include <vector>

#include "json/JsonObject.h"

struct MethodArg {
    MethodArg() : data(nullptr) {}

    virtual std::string type() = 0;

    virtual ~MethodArg() {}

    void *data;
};

template <class T>
struct MethodArgImpl : public MethodArg {
    MethodArgImpl(const T& m) {
        (T*&)data = new T(m);
    }
    MethodArgImpl(T&& m) {
        (T*&)data = new T(std::move(m));
    }

    std::string type() override
    {
        return TypeCast<T>::type();
    }

    ~MethodArgImpl()
    {
        if (data) {
            delete (T*)data;
        }
    }
};

struct JsCallMeta {
    std::string call_;
    std::string data_;
    std::string argsType_;
};

struct JSMethodCall {
    virtual std::string argsType() = 0;
    virtual void call(std::vector<MethodArg*> &list) = 0;
};

template <class T>
struct TypeCast;

template <class T>
struct TypeCast<const T&> {
    static std::string type()
    {
        return TypeCast<T>::type();
    }
};

template <class T>
struct TypeCast<T&> {
    static std::string type()
    {
        return TypeCast<T>::type();
    }
};

#define TYPE_CAST(tt, id)   \
template <> \
struct TypeCast<tt> { \
	static std::string type() \
	{ \
		return id; \
	} \
};

TYPE_CAST(std::string, "string")
TYPE_CAST(int, "int")
TYPE_CAST(bool, "bool")
TYPE_CAST(double, "double")
TYPE_CAST(JsonObject, "object")




template <size_t...indices>
struct IndicesPack {
};

template <class Pack, class ...Types>
struct IndexPackGetter {
    typedef Pack type;
};

template <size_t...ind, class T, class ...Types>
struct IndexPackGetter<IndicesPack<ind...>, T, Types...> : public IndexPackGetter<IndicesPack<ind..., sizeof...(ind)>, Types...>{
};

template <class ...T>
struct MethodArgsType {
    static void type(std::string &argsList) {
        return;
    }
};

template <class T1, class ...T2>
struct MethodArgsType<T1, T2...> {
    static void type(std::string &argsList) {
        argsList.append(TypeCast<T1>::type()).append(";");
        MethodArgsType<T2...>::type(argsList);
    }
};


template <size_t n, class ...T2>
struct CheckSameType {
    static bool same(std::vector<MethodArg*> &list)
    {
        return true;
    }
};

template <size_t n, class T, class ...T2>
struct CheckSameType<n, T, T2...> {
    static bool same(std::vector<MethodArg*> &list)
    {
        if (n >= list.size()) {
            return false;
        }
        TypeCast<const std::string&>::type();
        if (TypeCast<T>::type() != list[n]->type()) {
            return false;
        }

        return CheckSameType<(size_t)n + 1, T2...>::same(list);
    }
};

template <class ...T>
struct CheckArgs {
    static bool Check(std::vector<MethodArg*> &list)
    {
        if (sizeof...(T) != list.size()) {
            assert(false);
            return false;
        }

        return CheckSameType<(size_t)0, T...>::same(list);
    }
};


bool JSParse(const std::string &data, JsCallMeta &meta);
bool JSMetaCall(JsCallMeta &meta);

void JSStoreCall(const std::string &tag, JSMethodCall *call);
JSMethodCall *JSGetCall(const std::string &tag);
void JSClearCall(const std::string &tag);

template <class Ftype>
struct JSMethodCallImpl;

template <class R, class ...T>
struct JSMethodCallImpl<R(T...)> : public JSMethodCall {
    typedef R(*FunType)(T...);

    JSMethodCallImpl(FunType f)
    {
        f_ = f;
    }
    std::string argsType() override
    {
        std::string ret;
        MethodArgsType<T...>::type(ret);
        return ret;
    }

    void call(std::vector<MethodArg*> &list) override
    {
        expandCall(typename IndexPackGetter<IndicesPack<>, T...>::type(), list);
    }

private:
    template <size_t ...index>
    void expandCall(const IndicesPack<index...> &, std::vector<MethodArg*> &list)
    {
        if (!CheckArgs<T...>::Check(list)) {
			std::cout << "args is not correct" << std::endl;
            assert(false);
            return;
        }

        f_(
            ((typename std::remove_reference<T>::type&)*(typename std::remove_reference<T>::type*)(list[index]->data)
            )...);
    }

private:
    FunType f_;
};



template <class ...T>
struct CheckContainsPack {

};

template <class Pack1, class Pack2>
struct CheckContainsHelper: public std::false_type {
};

template <bool check, class B, class C>
struct BaseIf {
    typedef CheckContainsHelper<int, int> Base;
};

template <class B, class C>
struct BaseIf<true, B, C> {
    typedef CheckContainsHelper<B, C> Base;
};

template <class T, class T2>
struct CoreTypeSame : 
        public std::is_same<typename std::remove_const<typename std::remove_reference<T>::type>::type,
							typename std::remove_const<typename std::remove_reference<T2>::type>::type> {
};

//template <class T, class T2>
//struct Compatible : std::false_type {
//};


//缺陷，不能实现指针的包含判断
template <class T11, class ...T1, class T22, class ...T2>
struct CheckContainsHelper<CheckContainsPack<T11, T1...>, CheckContainsPack<T22, T2...> > : public 
        BaseIf<CoreTypeSame<T11, T22>::value, CheckContainsPack<T1...>, CheckContainsPack<T2...>>::Base
{
    //static bool same() 
    //{
    //    typedef std::remove_const<typename std::remove_reference<T11>::type>::type A1;
    //    typedef std::remove_const<typename std::remove_reference<T22>::type>::type A2;

    //    if (!std::is_same<A1, A2>::value) {
    //        return false;
    //    }

    //    return CheckContainsHelper<CheckContainsPack<T1...>, CheckContainsPack<T2...>::same();
    //}
};

template <class ...T2>
struct CheckContainsHelper<CheckContainsPack<>, CheckContainsPack<T2...> >: public std::true_type {
};

template <class ...T>
struct BindTypePack {
};

template <class P, class T1, class T2>
struct OtherArgsType;

template <class ...PT>
struct OtherArgsType<BindTypePack<PT...>, BindTypePack<>, BindTypePack<>> {
	typedef BindTypePack<PT...> pack;
};

template <class ...PT, class T11, class ...T12, class T21, class ...T22>
struct OtherArgsType<BindTypePack<PT...>, BindTypePack<T11, T12...>, BindTypePack<T21, T22...>> :
		public OtherArgsType<BindTypePack<PT...>, BindTypePack<T12...>, BindTypePack<T22...>> {
};

template <class ...PT, int T11, class ...T12, class T21, class ...T22>
struct OtherArgsType<BindTypePack<PT...>, BindTypePack<const std::_Ph<T11>&, T12...>, BindTypePack<T21, T22...>> :
	public OtherArgsType<BindTypePack<PT..., T21>, BindTypePack<T12...>, BindTypePack<T22...>> {
};

template <class ...PT, int T11, class ...T12, class T21, class ...T22>
struct OtherArgsType<BindTypePack<PT...>, BindTypePack<std::_Ph<T11>, T12...>, BindTypePack<T21, T22...>> :
	public OtherArgsType<BindTypePack<PT..., T21>, BindTypePack<T12...>, BindTypePack<T22...>> {
};

template <class ...PT, int T11, class ...T12, class T21, class ...T22>
struct OtherArgsType<BindTypePack<PT...>, BindTypePack<std::_Ph<T11>&&, T12...>, BindTypePack<T21, T22...>> :
    public OtherArgsType<BindTypePack<PT..., T21>, BindTypePack<T12...>, BindTypePack<T22...>>{
};

template <class ...PT, int T11, class ...T12, class T21, class ...T22>
struct OtherArgsType<BindTypePack<PT...>, BindTypePack<std::_Ph<T11>&, T12...>, BindTypePack<T21, T22...>> :
    public OtherArgsType<BindTypePack<PT..., T21>, BindTypePack<T12...>, BindTypePack<T22...>>{
};

//template <class ...T22>
//struct OtherArgsType<BindTypePack<>, BindTypePack<T22...>> {
//	typedef BindTypePack<T22...> pack;
//};

//template <class C, class P>
//struct JSMethodCallImpl2;


template <class ...T>
void TestTemplate(T&& ...args)
{

}


template <class C, class ...T>
struct JSMethodCallImpl2 : public JSMethodCall  {

	JSMethodCallImpl2(C &f) : f_(f)
    {
    }

	JSMethodCallImpl2(C &&f) : f_(std::move(f))
	{
	}

    std::string argsType() override
    {
        std::string ret;
        MethodArgsType<T...>::type(ret);
        return ret;
    }

    void call(std::vector<MethodArg*> &list) override
    {
        expandCall(typename IndexPackGetter<IndicesPack<>, T...>::type(), list);
    }

private:
    template <size_t ...index>
    void expandCall(const IndicesPack<index...> &, std::vector<MethodArg*> &list)
    {
        if (!CheckArgs<T...>::Check(list)) {
            std::cout << "args is not correct" << std::endl;
            assert(false);
            return;
        }
		//TestTemplate(((typename std::remove_reference<T>::type&)*(typename std::remove_reference<T>::type*)(list[index]->data)
		//	    )...);
		//f_(std::string(""));
        f_(
            ((typename std::remove_reference<T>::type&)*(typename std::remove_reference<T>::type*)(list[index]->data)
            )...);
    }

private:
    C f_;
};



template <class R, class ...T>
JSMethodCall *JSMakeCall(R(*f)(T...))
{
    return new JSMethodCallImpl<R(T...)>(f);
}

template <class Fun, class ...T>
JSMethodCall *JSMakeCallHelper(Fun &&f, BindTypePack<T...> &&)
{
	return new JSMethodCallImpl2<typename std::remove_reference<Fun>::type, T...>(std::forward<Fun>(f));
}

//暂时需要依赖绑定的时候std::placeholders::_1等限制
//可以借鉴chromium的代码移除此限制，暂时先不考虑了
//暂时不做成员函数的兼容，通过此绑定稍微麻烦一点也能实现
template <class R, class ...T1, class ...T2>
JSMethodCall *JSMakeCall(R(*f)(T1...), T2&& ...args)
{
	return JSMakeCallHelper(std::bind(f, std::forward<T2>(args)...), typename OtherArgsType<BindTypePack<>, BindTypePack<T2...>, BindTypePack<T1...>>::pack());

	//auto ff = std::bind(f, std::forward<T2>(args)...);
	//return JSMakeCallHelper(ff, typename OtherArgsType<BindTypePack<>, BindTypePack<T2...>, BindTypePack<T1...>>::pack());

}


template <class ...Types>
void NativeInvokehelper(JsonObject &json, int index, Types&& ...args)
{
    json.insert("argNum", index);
}

template <class T, class ...Types>
void NativeInvokehelper(JsonObject &json, int index, T &&arg, Types&& ...args)
{
    std::stringstream ss;
    ss << "arg" << index;
    std::string key = ss.str();

    json.insert(key, arg);
    NativeInvokehelper(json, index + 1, args...);
}



#endif //REGISTER_P_H