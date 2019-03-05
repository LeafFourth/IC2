// example.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include <stdio.h>
#include <iostream>

#include <winsock.h>

#include "librtmp/rtmp.h"

void InitWindowsSocks()
{
	WORD version;
	WSADATA wsaData;

	version = MAKEWORD(1, 1);
	int ret = WSAStartup(version, &wsaData);
}

static void ErrorGrab()
{
	printf("error\n");
}

static void PrintSpace(int number)
{
	if (number < 0) {
		return;
	}
	for (int i = 0; i < number; ++i) {
		printf(" ");
	}
	
}

static void PrintObjectHelper(AMFObject *obj, int indent)
{
	for (int i = 0; i < obj->o_num; ++i) {
		PrintSpace(indent);
		AMFObjectProperty &p = obj->o_props[i];
		switch (p.p_type) {
		case AMF_NUMBER:
			printf("%s:%lf", p.p_name.av_val, p.p_vu.p_number);
			break;
		case AMF_BOOLEAN:
			printf("%s:%d", p.p_name.av_val, (int)p.p_vu.p_number);
			break;
		case AMF_STRING:
			printf("%s:%s", p.p_name.av_val, (int)p.p_vu.p_aval.av_val);
			break;
		case AMF_OBJECT:
			printf("%s:\n", p.p_name.av_val);
			PrintObjectHelper(&p.p_vu.p_object, indent + 2);
			break;
		}
		printf("\n");
	}
}

static void PrintObject(AMFObject *obj)
{
	printf("\n");
	PrintObjectHelper(obj, 0);
}



int main()
{
	RTMP_PROTOCOL_RTMP;
	InitWindowsSocks();


	RTMP rtmp = { 0 };

	AVal hostname = { 0, 0 };
	unsigned int port = 1935;
	AVal sockshost = { 0, 0 };
	AVal playpath = { 0, 0 };
	AVal tcUrl = { 0, 0 };
	AVal swfUrl = { 0, 0 };
	AVal pageUrl = { 0, 0 };
	AVal app = { 0, 0 };
	AVal auth = { 0, 0 };
	uint32_t swfSize = 0;
	AVal swfHash = { 0, 0 };
	AVal flashVer = { 0, 0 };
	AVal subscribepath = { 0, 0 };
	uint32_t dSeek = 0;
	int dStopOffset = 10000;  //10s
	int bLiveStream;
	long timeout;
	int rtmpType = 0;

	int ret = FALSE;

	//RTMP_ParseURL("rtmp://live.hkstv.hk.lxdns.com/live/hks", &rtmpType, &hostname, &port, &playpath, &app);

	rtmpType = RTMP_PROTOCOL_RTMP;
	bLiveStream = TRUE;
	timeout = 120; //120s

#define STR2AVAL(av,str)	av.av_val = (char*)str; av.av_len = strlen(av.av_val)
	STR2AVAL(hostname, "live.hkstv.hk.lxdns.com");
	STR2AVAL(app, "live");
	STR2AVAL(playpath, "hks");

#undef STR2AVAL

#define DO_CHECK_ERROR(fun) \
	ret = fun; \
	if (!ret) { \
		ErrorGrab(); \
	}

	//RTMP_SetupStream(&rtmp, RTMP_PROTOCOL_RTMP, &hostname, port, &sockshost, &playpath,
	//	&tcUrl, &swfUrl, &pageUrl, &app, &auth, &swfHash, swfSize,
	//	&flashVer, &subscribepath, dSeek, dStopOffset, bLiveStream, timeout);

	RTMP_Init(&rtmp);


	DO_CHECK_ERROR(RTMP_SetupURL(&rtmp, (char*)"rtmp://live.hkstv.hk.lxdns.com/live/hks1"));
	DO_CHECK_ERROR(RTMP_Connect(&rtmp, nullptr));
	DO_CHECK_ERROR(RTMP_ConnectStream(&rtmp, 0));
	char buf[256];
	int retSize = 0;

	RTMPPacket pak;
	memset((void*)&pak, 0, sizeof(pak));
	while (RTMP_ReadPacket(&rtmp, &pak)) {
		switch (pak.m_packetType) {
		case 18: {
			AMFObject obj;
			memset((void*)&obj, 0, sizeof(obj));
			AMF_Decode(&obj, pak.m_body, pak.m_nBodySize, FALSE);
			PrintObject(&obj);
			int debug = 0;
		}
			
		}
		RTMPPacket_Free(&pak);
	}

    std::cout << "Hello World!\n"; 

	getchar();
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
