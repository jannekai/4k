#ifndef CONFIG_H
#define CONFIG_H

#define USE_SOUND_THREAD
#define WINDOWED true
#define DEBUG_DEVICE

#define X_RES		1280
#define Y_RES		720
#define GROUPS_X	2
#define GROUPS_Y	1
#define GROUPS_Z	1
#define THREAD_X	512
#define THREAD_Y	1
#define THREAD_Z	1
#define BUFFER_SIZE GROUPS_X * THREAD_X
#define M_PI 3.14159265358979323846

#define SMOOTHSTEP(x) ((x) * (x) * (3 - 2 * (x)))
#define RELEASE_AND_NULL(p) { p->Release(); p = NULL; }
#define RELEASE(p) { p->Release(); }

#ifdef _DEBUG
	#define CHK(e) { if ((e) < 0) { ExitProcess(0); } }	
#else
	#define CHK(e) { (e); }
#endif

#endif

