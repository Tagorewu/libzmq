#include "stdafx.h"
#include "stdio.h"
#include <winsock2.h>
#include <cstdio>
#include <cstdlib>
#include <stdint.h>
//#include <unistd.h>
#include "zmq.h"
//#include "sys/socket.h"

void epc660_open();

//epc660_setExposure(expo_time);

#define SERVER_IP "192.168.1.10"
#define SERVER_PORT 50660

//epc660_setFrameRate(15);
const char CMD_getGray[] = "getBWSorted\n";
const char CMD_get3D[] = "getDistanceSorted\n";
const char CMD_loadGrayMode[] = "loadConfig 0\n";
const char CMD_load3DMode[] = "loadConfig 1\n";
const char CMD_setInteTime3D[] = "setIntegrationTime3D 0\n";

void epc660_fetchdepthdata(int *pGray, int *pDepth, int expo_time, int flag){

#ifdef USE_ZMQSOCKET
		void *ctx;
		void *s;
	    ctx = zmq_init (1);	 //@param: specify thread pool
		s = zmq_socket (ctx, ZMQ_REQ); //@alt: ZMQ_REQ
		
		zmq_connect(s, "tcp://192.168.1.10:50660");
		
		zmq_send(s, CMD_getGray, strlen(CMD_getGray), 0);
		zmq_recv(s, pGray, 320 * 240 * 2, 1000);
		memcpy(pDepth, pGray, 320 * 240 * 2);
		zmq_close(s);
		zmq_ctx_destroy(ctx);
#else
		WORD sockVersion = MAKEWORD(2, 2);
		WSADATA data;
		if (WSAStartup(sockVersion, &data) != 0)
		{
			return;
		}
		//int sock;
		SOCKET sock;
		struct sockaddr_in serv_addr;

		int str_len;

		sock = socket(PF_INET, SOCK_STREAM, 0);
		if (sock == -1) {
			printf("socket() error");
		}

		serv_addr.sin_family = AF_INET;
		serv_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
		serv_addr.sin_port = htons(SERVER_PORT);

		//getGray
		if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
			printf("connect() error");
		}
		//str_len = recv(sock, (char *) pGray, 320*240*2, 0);	
		send(sock, CMD_get3D, strlen(CMD_get3D), 0);

		int recved_len = 0;
		while (recved_len < 320*240*2-10 ) {
			 str_len = recv(sock, (char *) pDepth + recved_len, 320*240*2-recved_len, 0);			
			if (str_len == -1) {
				printf("read() error");
			}
			else
				recved_len += str_len;
		}			

		//memcpy(pDepth, pGray, 320 * 240);
		//printf("Message from server : %s \n", message);
		closesocket(sock);
#endif

}

void epc660_setGrayMode() {
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA data;
	char rsp[16];

	if (WSAStartup(sockVersion, &data) != 0)
	{
		return;
	}
	//int sock;
	SOCKET sock;
	struct sockaddr_in serv_addr;

	int str_len;

	sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock == -1) {
		printf("socket() error");
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
	serv_addr.sin_port = htons(SERVER_PORT);

	//getGray
	if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
		printf("connect() error");
	}

	send(sock, CMD_loadGrayMode, strlen(CMD_loadGrayMode), 0);

	int recved_len = 0;

	str_len = recv(sock, (char *)rsp, 12, 0);
	if (str_len == -1) {
		printf("read() error");
	}
	else{
		//success or failed	
		printf("rsp: len = %d\n", str_len);
		for (int i = 0; i < str_len; i++)
			printf("0x%x ", (rsp[i]));
		printf("\n");
	}
	//printf("Message from server : %s \n", message);
	closesocket(sock);

}

void epc660_set3DMode() {
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA data;
	char rsp[16];

	if (WSAStartup(sockVersion, &data) != 0)
	{
		return;
	}
	//int sock;
	SOCKET sock;
	struct sockaddr_in serv_addr;

	int str_len;

	sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock == -1) {
		printf("socket() error");
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
	serv_addr.sin_port = htons(SERVER_PORT);

	//getGray
	if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
		printf("connect() error");
	}

	send(sock, CMD_load3DMode, strlen(CMD_load3DMode), 0);

	int recved_len = 0;

	str_len = recv(sock, (char *)rsp, 12, 0);
	if (str_len == -1) {
		printf("read() error");
	}
	else {
		//success or failed	
		printf("rsp: len = %d\n", str_len);
		for(int i = 0; i < str_len; i++)
			printf("0x%x ", (rsp[i]) );
		printf("\n");
	}
	//printf("Message from server : %s \n", message);
	closesocket(sock);

}

void epc660_setInteTime3D(int time) {
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA data;
	char rsp[16];

	if (WSAStartup(sockVersion, &data) != 0)
	{
		return;
	}
	//int sock;
	SOCKET sock;
	struct sockaddr_in serv_addr;

	int str_len;

	sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock == -1) {
		printf("socket() error");
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
	serv_addr.sin_port = htons(SERVER_PORT);

	//getGray
	if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
		printf("connect() error");
	}

	send(sock, CMD_setInteTime3D, strlen(CMD_setInteTime3D), 0);

	int recved_len = 0;

	str_len = recv(sock, (char *)rsp, 12, 0);
	if (str_len == -1) {
		printf("read() error");
	}
	else {
		//success or failed	
		printf("rsp: len = %d\n", str_len);
		for (int i = 0; i < str_len; i++)
			printf("0x%x ", (rsp[i]));
		printf("\n");
	}
	//printf("Message from server : %s \n", message);
	closesocket(sock);

}