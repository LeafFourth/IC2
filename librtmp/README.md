此项目（以下称"现项目"）是由RTMPDump项目部分源码构建的Win32工程，RTMPDump项目地址：http://rtmpdump.mplayerhq.hu

原项目在windows上未提供MSVC的构建方式，现项目通过原始的Makefile文件构建了使用VS2017编译的工程，同时依赖的openssl和zlib都使用VS2017编译。


This project built from the part of source code from the project "RTMPDump". The URL of RTMPDump is http://rtmpdump.mplayerhq.hu .

The project "RTMPDump" provided with some ways to build code without MSVC, but this project found a way to build code with VS2017 according to the original makefile and at the same time, the dependent libraries openssl and zlib were all built by VS2017.
