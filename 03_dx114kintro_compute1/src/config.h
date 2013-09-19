#ifndef CONFIG_H
#define CONFIG_H

#define USE_SOUND_THREAD
#define WINDOWED true
#define DEBUG_DEVICE
#define RES_1280_720

#ifdef RES_1920_1200
#define X_RES    1920
#define Y_RES    1200
#endif

#ifdef RES_1920_1080
#define X_RES    1920
#define Y_RES    1080
#endif

#ifdef RES_1680_1050
#define X_RES    1680
#define Y_RES    1050
#endif

#ifdef RES_1280_720
#define X_RES   1280
#define Y_RES	720
#endif

#ifdef RES_1024_768
#define X_RES    1024
#define Y_RES    768
#endif

#ifdef RES_640_360
#define X_RES	640
#define Y_RES	360
#endif

#ifdef RES_255_255
#define X_RES    255
#define Y_RES    255
#endif

#define SMOOTHSTEP(x) ((x) * (x) * (3 - 2 * (x)))
#define RELEASE_AND_NULL(p) { p->Release(); p = NULL; }
#define RELEASE(p) { p->Release(); }

#ifdef _DEBUG
	#define CHK(e) { if ((e) < 0) { ExitProcess(0); } }	
#else
	#define CHK(e) { (e); }
#endif

#endif

