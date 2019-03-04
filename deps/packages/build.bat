::::::::::
::从源码构建zlib和openssl
::::::::::


set ZLIB_SRC_ROOT=%~dp0zlib-1.2.11\
set OPENSSL_SRC_ROOT=%~dp0openssl-0.9.8k\

set DEPS_INCLUDE_ROOT=%~dp0..\include\
set DEPS_LIB_ROOT=%~dp0..\lib\
set DESP_DLL_ROOT=%~dp0..\dll\

::::::::::
::步骤一：初始化VC编译环境，使用vs2017构建
::::::::::

::初始化vc环境
call "D:\vs_2017\VC\Auxiliary\Build\vcvars32.bat"

::::::::::
::步骤二：解压源码到当前目录
::::::::::


::::::::::
:: 步骤三：编译zlib，命令见源代码zlib-1.2.11\Win32\Makefile.msc 
::::::::::

cd /d %ZLIB_SRC_ROOT%
nmake -f win32/Makefile.msc LOC="-DASMV -DASMINF"   OBJA="inffas32.obj match686.obj"


::::::::::
::步骤四：编译 openssl,openssl每个版本接口都有变动，只有使用0.9.8k能够正常编译
::::::::::

cd /d %OPENSSL_SRC_ROOT%

::执行perl的时候找不到desboth.pl，需要先复制一下
cp %OPENSSL_SRC_ROOT%crypto\des\asm\desboth.pl %OPENSSL_SRC_ROOT%crypto\perlasm


::详细步骤见openssl-0.9.8k\INSTALL.W32
::需安装perl 并把perl 加到当前的path环境变量中, http://www.activestate.com/ActivePerl
perl Configure VC-WIN32

::编译，不使用汇编优化，因为找不到nasm 0.98 或者可用的masm版本， nasm安装后没有编译时使用的nasmw.exe
call ms\do_masm

set _CL_=/WX-
nmake -f ms\ntdll.mak
nmake -f ms\ntdll.mak test

::::::::::
:: 步骤五：复制文件到deps跟目录
::::::::::

xcopy /Y %ZLIB_SRC_ROOT%zlib.h %DEPS_INCLUDE_ROOT%
xcopy /Y %ZLIB_SRC_ROOT%zconf.h %DEPS_INCLUDE_ROOT%
xcopy /Y %ZLIB_SRC_ROOT%zdll.lib %DEPS_LIB_ROOT%
xcopy /Y %ZLIB_SRC_ROOT%zlib1.dll %DESP_DLL_ROOT%

xcopy /E /Y %OPENSSL_SRC_ROOT%include %DEPS_INCLUDE_ROOT%
xcopy /Y %OPENSSL_SRC_ROOT%out32dll\libeay32.lib %DEPS_LIB_ROOT%
xcopy /Y %OPENSSL_SRC_ROOT%out32dll\ssleay32.lib %DEPS_LIB_ROOT%
xcopy /Y %OPENSSL_SRC_ROOT%out32dll\libeay32.dll %DESP_DLL_ROOT%
xcopy /Y %OPENSSL_SRC_ROOT%out32dll\ssleay32.dll %DESP_DLL_ROOT%


pause