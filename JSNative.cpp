namespace {
	struct MethodMeta {
		void *fun;
		void funName;
		QString argsInfo;
	};
  
  template <class ...Args>
  struct MethodArg;
  
  struct Arg {
    void *solid;
  };
  
  template <size_t size>
  struct MethodArg : public struct <size - 1> {
    Arg &get()
    {
      return mem;
    }
    
  private:
    Arg  mem;
  };
  
  template <>
  struct MethodArg<0> {
    Arg &get()
    {
      return mem;
    }
    
  private:
    Arg  mem;
      
  };
  
  template <size_t ...index>
  
	static std::map<QString, MethodMeta> g_registerFuns;
  
  template <class ...Args>
}

REGISTERMETHOD(fun) \
{ \
	QString name = #fun; \
	RegisterMethod(fun, funName); \
}

template <class T>
struct ArgMap;

template <class T>
struct ArgMap<const T&> : public ArgMap<T> {
};


template <>
struct ArgMap<QString> {
  typedef type QString;
  
  static const QString ArgInfo()
  {
    return "string";
  }
}

template <>
struct ArgMap<double> {
  typedef type double;
  
  static const QString ArgInfo()
  {
    return "double";
  }
}

template <>
struct ArgMap<int> {
  typedef type double;
  
  static const QString ArgInfo()
  {
    return "int";
  }
}

void FunArgsInfoHelper(void (*f)(void), QString &info)
{
  return;
}

template <class T>
void FunArgsInfoHelper(void (*f)(T), QString &info)
{
  info.append(ArgMap<T>);
}

template <class T, class ...ArgsType>
void FunArgsInfoHelper(void (*f)(T, ArgsType ...), QString &info)
{
  info.append(ArgMap<T>).append(",");
  
  typddef void (*type)(ArgsType ...);
  FunArgsInfoHelper((type)nullptr, info);
}

template <class ...ArgsType>
const QString FunArgsInfo(void (*f)(ArgsType ...))
{
  QString info;
  FunArgsInfoHelper(f, info);
  
  return std::move(info);
}





template <class Fun>
void RegisterMethod(Fun f, const QString &funName)
{
	if (!g_registerFuns.contains(funName)) {
     return;
	}
	MethodMeta meta;
	meta.fun = f;
  meta.funName = funName;
  meta.argsInfo = FunArgsInfo(f);
  
  g_registerFuns.insert(meta);
}

void InvokeFun(const QString &json)
{
  QString function = json.getVaule("functionName");
  int agrsNum = json.getVaule("argsNum");
  
  switch(1)
  
}
