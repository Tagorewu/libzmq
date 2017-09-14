// data_deamon.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
//#include <cstdio>
#include <cstdlib>
#include <stdint.h>
//#include <unistd.h>
//#include "epc660.h"
#include "zmq.h"
#include "epc660_client.h"

#define FMT_TYPE_MONOFREQ  1
#define FMT_TYPE_MULTIFREQ 0

#define IMG_WIDTH 320
#define IMG_HEIGHT 240

static int expo_time = 250;

static uint8_t *g_frame_dist;
static uint8_t *g_frame_gray;

#define PIXEL_1MM_FMT 1

typedef struct {
	uint32_t width;
	uint32_t height;
	uint32_t depth;
}frame_hdr_t;



int main(int argc, char **argv)
{
	int i;
	int *pGray = new int[IMG_WIDTH * IMG_HEIGHT];
	int *pDepth = new int[IMG_WIDTH * IMG_HEIGHT];
	int frame_cnt = 0x7fffffff;
/*	frame_hdr_t hdr = {
		.width = IMG_WIDTH,
		.height = IMG_HEIGHT,
#if PIXEL_1MM_FMT == 1
		.depth = 2,
#else
		.depth = 1
#endif
	};
*/
	frame_hdr_t hdr = {
		IMG_WIDTH,
		IMG_HEIGHT,
#if PIXEL_1MM_FMT == 1
		2,
#else
		1
#endif
	};
	if (argc > 1) {
		frame_cnt = atoi(argv[1]);
	}

	g_frame_dist = (uint8_t *)malloc(IMG_WIDTH * IMG_HEIGHT * 2 + sizeof(frame_hdr_t));
	if (!g_frame_dist) {
		return -1;
	}

	g_frame_gray = (uint8_t *)malloc(IMG_WIDTH * IMG_HEIGHT * 2 + sizeof(frame_hdr_t));
	if (!g_frame_gray) {
		return -1;
	}

	memcpy(g_frame_dist, &hdr, sizeof(hdr));
	memcpy(g_frame_gray, &hdr, sizeof(hdr));

	/* dump depth frame to bmp */
	//dump_img(frame_idx, (char *)img_dist, img_width, img_height, sizeof(int));

	//printf("get %d frames. ..\n", frame_cnt);
	int linger_to = 100; // 100ms;
	void *zmq_ctx = zmq_ctx_new();
	void *publisher = zmq_socket(zmq_ctx, ZMQ_PUB);

	zmq_setsockopt(publisher, ZMQ_LINGER, &linger_to, sizeof(int)); // 100ms linger
	zmq_bind(publisher, "tcp://*:5678");

	//test mono freq
	//epc660_open(FMT_TYPE_MONOFREQ);

	//epc660_setExposure(expo_time);
	//printf(" expo = %d\n", epc660_getExposure());

	//epc660_setFrameRate(15);
	//printf(" fps = %d\n", epc660_getFrameRate());

	epc660_set3DMode();


	for (i = 0; i < frame_cnt || 1; i++) {
		int n;
		double fvector[32];
		int fvector_cnt;
		int pred_result;

		if (i % 100 == 0) {
			printf(" frames : %d\n", i + 1);
		}
		epc660_fetchdepthdata(pGray, pDepth, expo_time, 0);

#if PIXEL_1MM_FMT != 1
		/* convert to uint8 data */
		for (n = 0; n < IMG_WIDTH * IMG_HEIGHT; n++) {
			g_frame_dist[sizeof(hdr) + n] = (pDepth[n] & 0xff) * 255 / 100;
			g_frame_gray[sizeof(hdr) + n] = (pGray[n] & 0xff) * 255 / 100;
		}

		zmq_send(publisher, "DIST", 4, ZMQ_SNDMORE);
		zmq_send(publisher, g_frame_dist, sizeof(hdr) + sizeof(uint8_t) * IMG_WIDTH * IMG_HEIGHT, 0);
#else
		for (n = 0; n < IMG_WIDTH * IMG_HEIGHT; n++) {
			uint16_t *p16 = (uint16_t *)(g_frame_dist + sizeof(hdr));
			uint16_t dist = pDepth[n] & 0xffff;
/*
			if (dist <= 100) {
				dist = 65535;
			}
			else {
				dist *= 10;
			}
*/			p16[n] = dist;

			//cp gray 
			p16 = (uint16_t *)(g_frame_gray + sizeof(hdr));
			p16[n] = (pGray[n] & 0xffff) * 10;
		}

		zmq_send(publisher, "DIST", 4, ZMQ_SNDMORE);
		zmq_send(publisher, g_frame_dist, sizeof(hdr) + sizeof(uint16_t) * IMG_WIDTH * IMG_HEIGHT, 0);
#endif
	}

	//epc660_close();

#if 0
	//test muti freq
	epc660_open(FMT_TYPE_MULTIFREQ);

	epc660_setFrameRate(30);
	epc660_getFrameRate();
	epc660_setFrameRate(15);
	epc660_getFrameRate();

	epc660_getExposure();
	epc660_setExposure(400);
	epc660_getExposure();
	epc660_fetchdepthdata(pGray, pDepth, 400, 1);
	epc660_setExposure(200);
	epc660_getExposure();
	epc660_fetchdepthdata(pGray, pDepth, 200, 1);

	epc660_close();
#endif
	//epc660_destroy();

	zmq_close(publisher);
	zmq_ctx_destroy(zmq_ctx);
	delete pGray;
	delete pDepth;
	getchar();
	return 0;
}


