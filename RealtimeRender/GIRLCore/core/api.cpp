/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   api.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - api.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// api.cpp*

#include "api.h"
#include "context.h"
#include "paramset.h"
#include "error.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <vector>
#include <cstdarg>

using namespace girl;

#define	EXTRACT_PARAMETERS(_start) \
	va_list	pArgs; \
	va_start( pArgs, _start ); \
\
	std::vector<GirlToken> aTokens; \
	std::vector<GirlPointer> aValues; \
	int count = buildParameterList( pArgs, aTokens, aValues );

#define PASS_PARAMETERS \
	count, aTokens.size()>0?&aTokens[0]:0, aValues.size()>0?&aValues[0]:0

namespace girl
{

//----------------------------------------------------------------------
// BuildParameterList
// Helper function to build a parameter list to pass on to the V style functions.
// returns a parameter count.

int buildParameterList( va_list pArgs, std::vector<GirlToken>& aTokens, std::vector<GirlPointer>& aValues )
{
    int count = 0;
    GirlToken pToken = va_arg( pArgs, GirlToken );
    GirlPointer pValue;
    aTokens.clear();
    aValues.clear();
    while ( pToken != 0 && pToken != GIRL_NULL )          	// While not GIRL_NULL
    {
        aTokens.push_back( pToken );
        pValue = va_arg( pArgs, GirlPointer );
        aValues.push_back( pValue );
        pToken = va_arg( pArgs, GirlToken );
        count++;
    }
    return ( count );
}

}

static bool initialized = false;


// API Function Definitions

extern "C" GIRLCOREDLL void girlAddServer(const char * name)
{
	Context::girlAddServer(std::string(name));
}

extern "C" GIRLCOREDLL void girlRemoveServer(const char * name)
{
	Context::girlRemoveServer(std::string(name));
}

extern "C" GIRLCOREDLL int girlGetServerCount()
{
	return Context::girlGetServerCount();
}

extern "C" GIRLCOREDLL int girlGetRenderingServersStatus(RenderingServerInfo *info, int maxInfoCount) {
	return Context::girlGetRenderingServersStatus(info, maxInfoCount);
}

extern "C" GIRLCOREDLL void girlCleanup()
{
	Context::girlCleanup();
}

extern "C" GIRLCOREDLL void girlIdentity()
{
	Context::girlIdentity();
}
extern "C" GIRLCOREDLL void girlTranslate(float dx, float dy, float dz)
{
	Context::girlTranslate(dx,dy,dz);
}
extern "C" GIRLCOREDLL void girlTransform(float tr[16])
{
	Context::girlTransform(tr);
}
extern "C" GIRLCOREDLL void girlConcatTransform(float tr[16]) {
	Context::girlConcatTransform(tr);
}
extern "C" GIRLCOREDLL void girlRotate(float angle, float dx, float dy, float dz)
{
	Context::girlRotate(angle,dx,dy,dz);
}
extern "C" GIRLCOREDLL void girlScale(float sx, float sy, float sz)
{
	Context::girlScale(sx,sy,sz);
}
extern "C" GIRLCOREDLL void girlLookAt(float ex, float ey, float ez, float lx, float ly, float lz,
		float ux, float uy, float uz)
{
	Context::girlLookAt(ex, ey, ez, lx, ly, lz, ux, uy, uz);
}
extern "C" GIRLCOREDLL void girlCoordinateSystem(const char *name)
{
	Context::girlCoordinateSystem(std::string(name));
}
extern "C" GIRLCOREDLL void girlCoordSysTransform(const char *name)
{
	Context::girlCoordSysTransform(std::string(name));
}
extern "C" GIRLCOREDLL void girlPixelFilter(const char *name, ...) //
{
	EXTRACT_PARAMETERS( name )
	girlPixelFilterV( name, PASS_PARAMETERS );
}

extern "C" GIRLCOREDLL void girlPixelFilterV (const char *name, int n, GirlToken tokens[], GirlPointer params[])
{
	//TODO - jromang : construct the paramset
	//Context::girlpixelFilter(name,params);
	Context::girlPixelFilter(name,ParamSet(n,name,tokens,params));
}

/*
void girlFilm(const char *type, const ParamSet &params)
{
	Context::girlFilm(std::string(type),params);
}*/
extern "C" GIRLCOREDLL void girlFilm(const char *name, ...)
{
	EXTRACT_PARAMETERS( name )
	girlFilmV( name, PASS_PARAMETERS );
}

extern "C" GIRLCOREDLL void girlFilmV(const char *name, int n, GirlToken tokens[], GirlPointer params[])
{
	Context::girlFilm(name,ParamSet(n,name,tokens,params));
}


/*
void girlSampler(const char *name, const ParamSet &params)
{
	Context::girlSampler(std::string(name),params);
}*/

extern "C" GIRLCOREDLL void girlSampler(const char *name, ...)
{
	EXTRACT_PARAMETERS( name )
	girlSamplerV( name, PASS_PARAMETERS );
}

extern "C" GIRLCOREDLL void girlSamplerV(const char *name, int n, GirlToken tokens[], GirlPointer params[])
{
	Context::girlSampler(name,ParamSet(n,name,tokens,params));
}

/*
void girlAccelerator(const char *name, const ParamSet &params)
{
	Context::girlAccelerator(std::string(name),params);
}*/
extern "C" GIRLCOREDLL void girlAccelerator(const char *name, ...)
{
	EXTRACT_PARAMETERS( name )
	girlAcceleratorV( name, PASS_PARAMETERS );
}

extern "C" GIRLCOREDLL void girlAcceleratorV(const char *name, int n, GirlToken tokens[], GirlPointer params[])
{
	Context::girlAccelerator(name,ParamSet(n,name,tokens,params));
}
/*
void girlSurfaceIntegrator(const char *name, const ParamSet &params)
{
	Context::girlSurfaceIntegrator(std::string(name),params);
}*/
extern "C" GIRLCOREDLL void girlSurfaceIntegrator(const char *name, ...)
{
	EXTRACT_PARAMETERS( name )
	girlSurfaceIntegratorV( name, PASS_PARAMETERS );
}

extern "C" GIRLCOREDLL void girlSurfaceIntegratorV(const char *name, int n, GirlToken tokens[], GirlPointer params[])
{
	Context::girlSurfaceIntegrator(name,ParamSet(n,name,tokens,params));
}

/*
void girlVolumeIntegrator(const char *name, const ParamSet &params)
{
	Context::girlVolumeIntegrator(std::string(name),params);
}*/

extern "C" GIRLCOREDLL void girlVolumeIntegrator(const char *name, ...)
{
	EXTRACT_PARAMETERS( name )
	girlVolumeIntegratorV( name, PASS_PARAMETERS );
}

extern "C" GIRLCOREDLL void girlVolumeIntegratorV(const char *name, int n, GirlToken tokens[], GirlPointer params[])
{
	Context::girlVolumeIntegrator(name,ParamSet(n,name,tokens,params));
}

/*
void girlCamera(const char *name, const ParamSet &params)
{
	Context::girlCamera(std::string(name),params);
}*/

extern "C" GIRLCOREDLL void girlCamera(const char *name, ...)
{
	EXTRACT_PARAMETERS( name )
	girlCameraV( name, PASS_PARAMETERS );
}

extern "C" GIRLCOREDLL void girlCameraV(const char *name, int n, GirlToken tokens[], GirlPointer params[])
{
	Context::girlCamera(name,ParamSet(n,name,tokens,params));
}

extern "C" GIRLCOREDLL void girlWorldBegin()
{
	Context::girlWorldBegin();
}
extern "C" GIRLCOREDLL void girlAttributeBegin()
{
	Context::girlAttributeBegin();
}
extern "C" GIRLCOREDLL void girlAttributeEnd()
{
	Context::girlAttributeEnd();
}
extern "C" GIRLCOREDLL void girlTransformBegin()
{
	Context::girlTransformBegin();
}
extern "C" GIRLCOREDLL void girlTransformEnd()
{
	Context::girlTransformEnd();
}
/*
void girlTexture(const char *name, const char *type, const char *texname,
		const ParamSet &params)
{
	Context::girlTexture(std::string(name), std::string(type), std::string(texname), params);
}*/
extern "C" GIRLCOREDLL void girlTexture(const char *name, const char *type, const char *texname, ...)
{
	EXTRACT_PARAMETERS( texname )
	girlTextureV( name, type, texname, PASS_PARAMETERS );
}

extern "C" GIRLCOREDLL void girlTextureV(const char *name, const char *type, const char *texname, int n, GirlToken tokens[], GirlPointer params[])
{
	Context::girlTexture(name,type,texname,ParamSet(n,name,tokens,params));
}
/*
void girlMaterial(const char *name, const ParamSet &params)
{
	Context::girlMaterial(name,params);
}*/
extern "C" GIRLCOREDLL void girlMaterial(const char *name, ...)
{
	EXTRACT_PARAMETERS( name )
	girlMaterialV( name, PASS_PARAMETERS );
}

extern "C" GIRLCOREDLL void girlMaterialV(const char *name, int n, GirlToken tokens[], GirlPointer params[])
{
	Context::girlMaterial(name,ParamSet(n,name,tokens,params));
}

extern "C" GIRLCOREDLL void girlMakeNamedMaterial(const char *name, ...)
{
	EXTRACT_PARAMETERS( name )
	girlMakeNamedMaterialV( name, PASS_PARAMETERS );
}

extern "C" GIRLCOREDLL void girlMakeNamedMaterialV(const char *name, int n, GirlToken tokens[], GirlPointer params[])
{
	Context::girlMakeNamedMaterial(name,ParamSet(n,name,tokens,params));
}

extern "C" GIRLCOREDLL void girlNamedMaterial(const char *name, ...)
{
	EXTRACT_PARAMETERS( name )
	girlNamedMaterialV( name, PASS_PARAMETERS );
}

extern "C" GIRLCOREDLL void girlNamedMaterialV(const char *name, int n, GirlToken tokens[], GirlPointer params[])
{
	Context::girlNamedMaterial(name,ParamSet(n,name,tokens,params));
}
/*
void girlLightSource(const char *name, const ParamSet &params)
{
	Context::girlLightSource(std::string(name),params);
}*/
extern "C" GIRLCOREDLL void girlLightSource(const char *name, ...)
{
	EXTRACT_PARAMETERS( name )
	girlLightSourceV( name, PASS_PARAMETERS );
}

extern "C" GIRLCOREDLL void girlLightSourceV(const char *name, int n, GirlToken tokens[], GirlPointer params[])
{
	Context::girlLightSource(name,ParamSet(n,name,tokens,params));
}

/*
void girlAreaLightSource(const char *name, const ParamSet &params) {
	Context::girlAreaLightSource(std::string(name),params);
}*/
extern "C" GIRLCOREDLL void girlAreaLightSource(const char *name, ...)
{
	EXTRACT_PARAMETERS( name )
	girlAreaLightSourceV( name, PASS_PARAMETERS );
}

extern "C" GIRLCOREDLL void girlAreaLightSourceV(const char *name, int n, GirlToken tokens[], GirlPointer params[])
{
	Context::girlAreaLightSource(name,ParamSet(n,name,tokens,params));
}

/*
void girlPortalShape(const char *name, const ParamSet &params) {
	Context::girlPortalShape(std::string(name),params);
}*/
extern "C" GIRLCOREDLL void girlPortalShape(const char *name, ...)
{
	EXTRACT_PARAMETERS( name )
	girlPortalShapeV( name, PASS_PARAMETERS );
}

extern "C" GIRLCOREDLL void girlPortalShapeV(const char *name, int n, GirlToken tokens[], GirlPointer params[])
{
	Context::girlPortalShape(name,ParamSet(n,name,tokens,params));
}

/*
void girlShape(const char *name, const ParamSet &params) {
	Context::girlShape(std::string(name),params);
}*/
extern "C" GIRLCOREDLL void girlShape(const char *name, ...)
{
	EXTRACT_PARAMETERS( name )
	girlShapeV( name, PASS_PARAMETERS );
}

extern "C" GIRLCOREDLL void girlShapeV(const char *name, int n, GirlToken tokens[], GirlPointer params[])
{
	Context::girlShape(name,ParamSet(n,name,tokens,params));
}

extern "C" GIRLCOREDLL void girlReverseOrientation() {
	Context::girlReverseOrientation();
}
/*
void girlVolume(const char *name, const ParamSet &params) {
	Context::girlVolume(std::string(name),params);
}*/
extern "C" GIRLCOREDLL void girlVolume(const char *name, ...)
{
	EXTRACT_PARAMETERS( name )
	girlVolumeV( name, PASS_PARAMETERS );
}

extern "C" GIRLCOREDLL void girlVolumeV(const char *name, int n, GirlToken tokens[], GirlPointer params[])
{
	Context::girlVolume(name,ParamSet(n,name,tokens,params));
}

extern "C" GIRLCOREDLL void girlObjectBegin(const char *name) {
	Context::girlObjectBegin(std::string(name));
}
extern "C" GIRLCOREDLL void girlObjectEnd() {
	Context::girlObjectEnd();
}
extern "C" GIRLCOREDLL void girlObjectInstance(const char *name) {
	Context::girlObjectInstance(std::string(name));
}
extern "C" GIRLCOREDLL void girlMotionInstance(const char *name, float startTime, float endTime, const char *toTransform) {
	Context::girlMotionInstance(std::string(name), startTime, endTime, std::string(toTransform));
}
extern "C" GIRLCOREDLL void girlWorldEnd() {
	Context::girlWorldEnd();
}


extern "C" GIRLCOREDLL void girlInit() {

	// System-wide initialization

	// Make sure floating point unit's rounding stuff is set
	// as is expected by the fast FP-conversion routines.  In particular,
	// we want double precision on Linux, not extended precision!
#ifdef FAST_INT
#if defined(__linux__) && defined(__i386__)
	int cword = _FPU_MASK_DM | _FPU_MASK_ZM | _FPU_MASK_OM | _FPU_MASK_PM |
	_FPU_MASK_UM | _FPU_MASK_IM | _FPU_DOUBLE | _FPU_RC_NEAREST;
	_FPU_SETCW(cword);
#endif
#if defined(WIN32)
	_control87(_PC_53, MCW_PC);
#endif
#endif // FAST_INT


	// API Initialization
	if (initialized)
		girlError(GIRL_ILLSTATE,GIRL_ERROR,"girlInit() has already been called.");
	else
		Context::setActive(new Context());

	initialized=true;
}

//interactive control

//CORE engine control


//user interactive thread functions
extern "C" GIRLCOREDLL void girlStart() {
	Context::girlStart();
}

extern "C" GIRLCOREDLL void girlPause() {
	Context::girlPause();
}

extern "C" GIRLCOREDLL void girlExit() {
	Context::girlExit();
}

extern "C" GIRLCOREDLL void girlWait() {
	Context::girlWait();
}

extern "C" GIRLCOREDLL void girlSetHaltSamplePerPixel(int haltspp, bool haveEnoughSamplePerPixel,
		bool suspendThreadsWhenDone) {
	Context::girlSetHaltSamplePerPixel(haltspp, haveEnoughSamplePerPixel, suspendThreadsWhenDone);
}
//controlling number of threads
extern "C" GIRLCOREDLL int girlAddThread() {
	return Context::girlAddThread();
	//return girlCurrentScene->AddThread();
}

extern "C" GIRLCOREDLL void girlRemoveThread() {
	Context::girlRemoveThread();
	//girlCurrentScene->RemoveThread();
}

extern "C" GIRLCOREDLL int girlGetRenderingThreadsStatus(RenderingThreadInfo *info, int maxInfoCount) {
	return Context::girlGetRenderingThreadsStatus(info, maxInfoCount);
}

//framebuffer access
extern "C" GIRLCOREDLL void girlUpdateFramebuffer() {
	Context::girlUpdateFramebuffer();
	//girlCurrentScene->UpdateFramebuffer();
}

extern "C" GIRLCOREDLL unsigned char* girlFramebuffer() {
	return Context::girlFramebuffer();
	//return girlCurrentScene->GetFramebuffer();
}

/*
//TODO - jromang : remove & replace by girlstatistics
extern "C" int girlDisplayInterval() {
	return Context::girlDisplayInterval();
	//return girlCurrentScene->DisplayInterval();
}


//TODO - jromang : remove & replace by girlstatistics
extern "C" int girlFilmXres() {
	return Context::girlFilmXres();
	//return girlCurrentScene->FilmXres();
}

//TODO - jromang : remove & replace by girlstatistics
extern "C" int girlFilmYres() {
	return Context::girlFilmYres();
	//return girlCurrentScene->FilmYres();
}*/

extern "C" GIRLCOREDLL double girlStatistics(const char *statName) {
	if(initialized) return Context::girlStatistics(statName);
	else
		{
			girlError(GIRL_NOTSTARTED,GIRL_SEVERE,"girlInit() must be called before calling  'girlStatistics'. Ignoring.");
			return 0;
		}
}

extern "C" GIRLCOREDLL void girlEnableDebugMode() {
	Context::girlEnableDebugMode();
}

extern "C" GIRLCOREDLL void girlUpdateFilmFromNetwork() {

	Context::girlUpdateFilmFromNetwork();
}

extern "C" GIRLCOREDLL void girlSetNetworkServerUpdateInterval(int updateInterval) {

	Context::girlSetNetworkServerUpdateInterval(updateInterval);
}

extern "C" GIRLCOREDLL int girlGetNetworkServerUpdateInterval() {

	return Context::girlGetNetworkServerUpdateInterval();
}

//error handling
GirlErrorHandler girlError=girlErrorPrint;
int girlLastError=GIRL_NOERROR;

extern "C" GIRLCOREDLL void girlErrorHandler(GirlErrorHandler handler) {
	girlError=handler;
}

extern "C" GIRLCOREDLL void girlErrorAbort(int code, int severity, const char *message) {
	girlErrorPrint(code, severity, message);
	if (severity!=GIRL_INFO)
		exit(code);
}

extern "C" GIRLCOREDLL void girlErrorIgnore(int code, int severity, const char *message) {
	girlLastError=code;
}

extern "C" GIRLCOREDLL void girlErrorPrint(int code, int severity, const char *message) {
	girlLastError=code;
	std::cerr<<"[";
#ifndef WIN32 //windows does not support ANSI escape codes
	//set the color
	switch (severity) {
	case GIRL_INFO:
		std::cerr<<"\033[0;32m";
		break;
	case GIRL_WARNING:
		std::cerr<<"\033[0;33m";
		break;
	case GIRL_ERROR:
		std::cerr<<"\033[0;31m";
		break;
	case GIRL_SEVERE:
		std::cerr<<"\033[0;31m";
		break;
	}
#endif
	std::cerr<<"Girl ";
	std::cerr<<boost::posix_time::second_clock::local_time()<<' ';
	switch (severity) {
	case GIRL_INFO:
		std::cerr<<"INFO";
		break;
	case GIRL_WARNING:
		std::cerr<<"WARNING";
		break;
	case GIRL_ERROR:
		std::cerr<<"ERROR";
		break;
	case GIRL_SEVERE:
		std::cerr<<"SEVERE ERROR";
		break;
	}
	std::cerr<<" : "<<code;
#ifndef WIN32 //windows does not support ANSI escape codes
	std::cerr<<"\033[0m";
#endif
	std::cerr<<"] "<<message<<std::endl;
}

extern "C" GIRLCOREDLL  void girlProgressMsgPrint(int code, int severity, const char *message) {
	girlLastError=code;
	std::cerr<<"\r[";
	std::cerr<<boost::posix_time::second_clock::local_time()<<"] ";
	std::cerr<<message;
}

bool girlPerformRender = TRUE;
extern "C" GIRLCOREDLL void girlSetNotToRender()
{
	girlPerformRender = FALSE;
}