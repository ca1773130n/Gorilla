/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   api.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - api.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#ifndef GIRL_API_H
#define GIRL_API_H 1

#include "girl.h"

typedef char *GirlToken;
typedef char *GirlPointer;
#define GIRL_NULL NULL

/* Basic control flow, scoping, stacks */
extern "C" GIRLCOREDLL void girlIdentity();
extern "C" GIRLCOREDLL void girlTranslate(float dx, float dy, float dz);
extern "C" GIRLCOREDLL void girlRotate(float angle, float ax, float ay, float az);
extern "C" GIRLCOREDLL void girlScale(float sx, float sy, float sz);
extern "C" GIRLCOREDLL void girlLookAt(float ex, float ey, float ez, float lx, float ly, float lz, float ux, float uy, float uz);
extern "C" GIRLCOREDLL void girlConcatTransform(float transform[16]);
extern "C" GIRLCOREDLL void girlTransform(float transform[16]);
extern "C" GIRLCOREDLL void girlCoordinateSystem(const char *);
extern "C" GIRLCOREDLL void girlCoordSysTransform(const char *);
extern "C" GIRLCOREDLL void girlPixelFilter(const char *name, ...);
extern "C" GIRLCOREDLL void girlPixelFilterV(const char *name, int n, GirlToken tokens[], GirlPointer params[]);
extern "C" GIRLCOREDLL void girlFilm(const char *name, ...);
extern "C" GIRLCOREDLL void girlFilmV(const char *name, int n, GirlToken tokens[], GirlPointer params[]);
extern "C" GIRLCOREDLL void girlSampler(const char *name, ...);
extern "C" GIRLCOREDLL void girlSamplerV(const char *name, int n, GirlToken tokens[], GirlPointer params[]);
extern "C" GIRLCOREDLL void girlAccelerator(const char *name, ...);
extern "C" GIRLCOREDLL void girlAcceleratorV(const char *name, int n, GirlToken tokens[], GirlPointer params[]);
extern "C" GIRLCOREDLL void girlSurfaceIntegrator(const char *name, ...);
extern "C" GIRLCOREDLL void girlSurfaceIntegratorV(const char *name, int n, GirlToken tokens[], GirlPointer params[]);
extern "C" GIRLCOREDLL void girlVolumeIntegrator(const char *name, ...);
extern "C" GIRLCOREDLL void girlVolumeIntegratorV(const char *name, int n, GirlToken tokens[], GirlPointer params[]);
extern "C" GIRLCOREDLL void girlCamera(const char *name, ...);
extern "C" GIRLCOREDLL void girlCameraV(const char *name, int n, GirlToken tokens[], GirlPointer params[]);
extern "C" GIRLCOREDLL void girlWorldBegin();
extern "C" GIRLCOREDLL void girlAttributeBegin();
extern "C" GIRLCOREDLL void girlAttributeEnd();
extern "C" GIRLCOREDLL void girlTransformBegin();
extern "C" GIRLCOREDLL void girlTransformEnd();
extern "C" GIRLCOREDLL void girlTexture(const char *name, const char *type, const char *texname, ...);
extern "C" GIRLCOREDLL void girlTextureV(const char *name, const char *type, const char *texname, int n, GirlToken tokens[], GirlPointer params[]);
extern "C" GIRLCOREDLL void girlMaterial(const char *name, ...);
extern "C" GIRLCOREDLL void girlMaterialV(const char *name, int n, GirlToken tokens[], GirlPointer params[]);
extern "C" GIRLCOREDLL void girlMakeNamedMaterial(const char *name, ...);
extern "C" GIRLCOREDLL void girlMakeNamedMaterialV(const char *name, int n, GirlToken tokens[], GirlPointer params[]);
extern "C" GIRLCOREDLL void girlNamedMaterial(const char *name, ...);
extern "C" GIRLCOREDLL void girlNamedMaterialV(const char *name, int n, GirlToken tokens[], GirlPointer params[]);
extern "C" GIRLCOREDLL void girlLightSource(const char *name, ...);
extern "C" GIRLCOREDLL void girlLightSourceV(const char *name, int n, GirlToken tokens[], GirlPointer params[]);
extern "C" GIRLCOREDLL void girlAreaLightSource(const char *name, ...);
extern "C" GIRLCOREDLL void girlAreaLightSourceV(const char *name, int n, GirlToken tokens[], GirlPointer params[]);
extern "C" GIRLCOREDLL void girlPortalShape(const char *name, ...);
extern "C" GIRLCOREDLL void girlPortalShapeV(const char *name, int n, GirlToken tokens[], GirlPointer params[]);
extern "C" GIRLCOREDLL void girlShape(const char *name, ...);
extern "C" GIRLCOREDLL void girlShapeV(const char *name, int n, GirlToken tokens[], GirlPointer params[]);
extern "C" GIRLCOREDLL void girlReverseOrientation();
extern "C" GIRLCOREDLL void girlVolume(const char *name, ...);
extern "C" GIRLCOREDLL void girlVolumeV(const char *name, int n, GirlToken tokens[], GirlPointer params[]);
extern "C" GIRLCOREDLL void girlObjectBegin(const char *name);
extern "C" GIRLCOREDLL void girlObjectEnd();
extern "C" GIRLCOREDLL void girlObjectInstance(const char *name);
extern "C" GIRLCOREDLL void girlMotionInstance(const char *name, float startTime, float endTime, const char *toTransform);
extern "C" GIRLCOREDLL void girlWorldEnd();
extern "C" GIRLCOREDLL void girlSetNotToRender();

/* User interactive thread functions */
extern "C" GIRLCOREDLL void girlStart();
extern "C" GIRLCOREDLL void girlPause();
extern "C" GIRLCOREDLL void girlExit();
extern "C" GIRLCOREDLL void girlWait();

extern "C" GIRLCOREDLL void girlSetHaltSamplePerPixel(int haltspp, bool haveEnoughSamplePerPixel, bool suspendThreadsWhenDone);

/* Controlling number of threads */
extern "C" GIRLCOREDLL int girlAddThread();
extern "C" GIRLCOREDLL void girlRemoveThread();

enum ThreadSignals { RUN, PAUSE, EXIT};
struct RenderingThreadInfo {
	int threadIndex;
	ThreadSignals status;
};
// Dade - return the number of rendering threads and fill the info buffer with
// information about the threads
extern "C" GIRLCOREDLL int girlGetRenderingThreadsStatus(RenderingThreadInfo *info, int maxInfoCount);

/* Framebuffer access */
extern "C" GIRLCOREDLL void girlUpdateFramebuffer();
extern "C" GIRLCOREDLL unsigned char* girlFramebuffer();

/* Networking */
extern "C" GIRLCOREDLL void girlAddServer(const char * name);
extern "C" GIRLCOREDLL void girlRemoveServer(const char * name);
extern "C" GIRLCOREDLL int girlGetServerCount();
extern "C" GIRLCOREDLL void girlUpdateFilmFromNetwork();
extern "C" GIRLCOREDLL void girlSetNetworkServerUpdateInterval(int updateInterval);
extern "C" GIRLCOREDLL int girlGetNetworkServerUpdateInterval();

struct RenderingServerInfo {
	int serverIndex;

	// Dade - connection information
	const char *name; // Dade - name/ip address of the server
	const char *port; // Dade - tcp port of the server
	const char *sid; // Dade - session id for the server

	unsigned int secsSinceLastContact;
	double numberOfSamplesReceived;
};
// Dade - return the number of rendering servers and fill the info buffer with
// information about the servers
extern "C" GIRLCOREDLL int girlGetRenderingServersStatus(RenderingServerInfo *info, int maxInfoCount);

/* Informations and statistics */
extern "C" GIRLCOREDLL double girlStatistics(const char *statName);

// Dade - enable debug mode
extern "C" GIRLCOREDLL void girlEnableDebugMode();

/* Error Handlers */
extern "C" int girlLastError; /*  Keeps track of the last error code */
typedef GIRLCOREDLL void (*GirlErrorHandler)(int code, int severity, const char *msg);
extern "C" GIRLCOREDLL void girlErrorHandler(GirlErrorHandler handler);
extern "C" GIRLCOREDLL void girlErrorAbort(int code, int severity, const char *message);
extern "C" GIRLCOREDLL void girlErrorIgnore(int code, int severity, const char *message);
extern "C" GIRLCOREDLL void girlErrorPrint(int code, int severity, const char *message);
extern "C" GIRLCOREDLL void girlProgressMsgPrint(int code, int severity, const char *message);

/*
 Error codes

 1 - 10     System and File errors
 11 - 20     Program Limitations
 21 - 40     State Errors
 41 - 60     Parameter and Protocol Errors
 61 - 80     Execution Errors
 */

#define GIRL_NOERROR         0

#define GIRL_NOMEM           1       /* Out of memory */
#define GIRL_SYSTEM          2       /* Miscellaneous system error */
#define GIRL_NOFILE          3       /* File nonexistant */
#define GIRL_BADFILE         4       /* Bad file format */
#define GIRL_BADVERSION      5       /* File version mismatch */
#define GIRL_DISKFULL        6       /* Target disk is full */

#define GIRL_UNIMPLEMENT    12       /* Unimplemented feature */
#define GIRL_LIMIT          13       /* Arbitrary program limit */
#define GIRL_BUG            14       /* Probably a bug in renderer */

#define GIRL_NOTSTARTED     23       /* girlInit() not called */
#define GIRL_NESTING        24       /* Bad begin-end nesting - jromang will be used in API v2 */
#define GIRL_NOTOPTIONS     25       /* Invalid state for options - jromang will be used in API v2 */
#define GIRL_NOTATTRIBS     26       /* Invalid state for attributes - jromang will be used in API v2 */
#define GIRL_NOTPRIMS       27       /* Invalid state for primitives - jromang will be used in API v2 */
#define GIRL_ILLSTATE       28       /* Other invalid state - jromang will be used in API v2 */
#define GIRL_BADMOTION      29       /* Badly formed motion block - jromang will be used in API v2 */
#define GIRL_BADSOLID       30       /* Badly formed solid block - jromang will be used in API v2 */

#define GIRL_BADTOKEN       41       /* Invalid token for request */
#define GIRL_RANGE          42       /* Parameter out of range */
#define GIRL_CONSISTENCY    43       /* Parameters inconsistent */
#define GIRL_BADHANDLE      44       /* Bad object/light handle */
#define GIRL_NOPLUGIN       45       /* Can't load requested plugin */
#define GIRL_MISSINGDATA    46       /* Required parameters not provided */
#define GIRL_SYNTAX         47       /* Declare type syntax error */

#define GIRL_MATH           61       /* Zerodivide, noninvert matrix, etc. */

/* Error severity levels */

#define GIRL_INFO            0       /* Rendering stats & other info */
#define GIRL_WARNING         1       /* Something seems wrong, maybe okay */
#define GIRL_ERROR           2       /* Problem.  Results may be wrong */
#define GIRL_SEVERE          3       /* So bad you should probably abort */

#endif /* GIRL_API_H */
