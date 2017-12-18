/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   context.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - context.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#ifndef GIRL_CONTEXT_H
#define GIRL_CONTEXT_H

#include "girl.h"
#include "renderfarm.h"

#include <boost/thread/mutex.hpp>

//TODO - jromang : convert to enum
#define STATE_UNINITIALIZED  0
#define STATE_OPTIONS_BLOCK  1
#define STATE_WORLD_BLOCK    2

namespace girl {

class GIRLCOREDLL Context {
public:

	Context(std::string n="Girl default context") : name(n) {
            init();
	}

        ~Context() {
            free();
        }

	//static bool checkMode(unsigned char modeMask, std::string callerName, int errorCode); //!< Check the graphics state mode in the active context

	//GState graphicsState;

	//TODO jromang - const & reference
	static Context* getActive() {
		return activeContext;
	}
	static void setActive(Context *c) {
		activeContext=c;
	}

	static map<string, boost::shared_ptr<Texture<float> > > *getActiveFloatTextures() {
		return &(activeContext->graphicsState->floatTextures);
	}
	static map<string, boost::shared_ptr<Texture<SWCSpectrum> > > *getActiveSWCSpectrumTextures() {
		return &(activeContext->graphicsState->colorTextures);
	}

	//'static' API
	//static void girlPixelFilter(const char *, const ParamSet &params) { activeContext->pixelFilter(std::string(name), params); }
	static void girlIdentity() { activeContext->identity(); }
	static void girlTranslate(float dx, float dy, float dz) { activeContext->translate(dx, dy, dz); }
	static void girlRotate(float angle, float ax, float ay, float az) { activeContext->rotate(angle, ax, ay, az); }
	static void girlScale(float sx, float sy, float sz) { activeContext->scale(sx, sy, sz); }
	static void girlLookAt(float ex, float ey, float ez, float lx, float ly, float lz, float ux, float uy, float uz) { activeContext->lookAt(ex, ey, ez, lx, ly, lz, ux, uy, uz) ; }
	static void girlConcatTransform(float transform[16]) { activeContext->concatTransform(transform); }
	static void girlTransform(float transform[16]) { activeContext->transform(transform); }
	static void girlCoordinateSystem(const string &s) { activeContext->coordinateSystem(s) ; }
	static void girlCoordSysTransform(const string &s) { activeContext->coordSysTransform(s); }
	static void girlPixelFilter(const string &name, const ParamSet &params) { activeContext->pixelFilter(name, params); }
	static void girlFilm(const string &type, const ParamSet &params) { activeContext->film(type, params); }
	static void girlSampler(const string &name, const ParamSet &params) { activeContext->sampler(name, params); }
	static void girlAccelerator(const string &name, const ParamSet &params) { activeContext->accelerator(name, params); }
	static void girlSurfaceIntegrator(const string &name, const ParamSet &params) { activeContext->surfaceIntegrator(name, params); }
	static void girlVolumeIntegrator(const string &name, const ParamSet &params) { activeContext->volumeIntegrator(name, params); }
	static void girlCamera(const string &s, const ParamSet &cameraParams) { activeContext->camera(s, cameraParams); }
	static void girlWorldBegin() { activeContext->worldBegin(); }
	static void girlAttributeBegin() { activeContext->attributeBegin(); }
	static void girlAttributeEnd() { activeContext->attributeEnd(); }
	static void girlTransformBegin() { activeContext->transformBegin(); }
	static void girlTransformEnd() { activeContext->transformEnd(); }
	static void girlTexture(const string &name, const string &type, const string &texname, const ParamSet &params) { activeContext->texture(name, type, texname, params); }
	static void girlMaterial(const string &name, const ParamSet &params) { activeContext->material(name, params); }
	static void girlMakeNamedMaterial(const string &name, const ParamSet &params) { activeContext->makenamedmaterial(name, params); }
	static void girlNamedMaterial(const string &name, const ParamSet &params) { activeContext->namedmaterial(name, params); }
	static void girlLightSource(const string &name, const ParamSet &params) { activeContext->lightSource(name, params); }
	static void girlAreaLightSource(const string &name, const ParamSet &params) { activeContext->areaLightSource(name, params); }
	static void girlPortalShape(const string &name, const ParamSet &params) { activeContext->portalShape(name, params); }
	static void girlShape(const string &name, const ParamSet &params) { activeContext->shape(name, params); }
	static void girlReverseOrientation() { activeContext->reverseOrientation(); }
	static void girlVolume(const string &name, const ParamSet &params) { activeContext->volume(name, params); }
	static void girlObjectBegin(const string &name) { activeContext->objectBegin(name); }
	static void girlObjectEnd() { activeContext->objectEnd(); }
	static void girlObjectInstance(const string &name) { activeContext->objectInstance(name); }
	static void girlMotionInstance(const string &name, float startTime, float endTime, const string &toTransform) { activeContext->motionInstance(name, startTime, endTime, toTransform); }
	static void girlWorldEnd() { activeContext->worldEnd(); }

	/**
	 * Creates a material using the given shape parameters, the current
	 * material and the current textures.
	 *
	 * @param shapeparams The parameters of the shape.
	 */
	boost::shared_ptr<Material> makematerial(const ParamSet& shapeparams, bool force);
	void makemixmaterial(const ParamSet& shapeparams, const ParamSet& materialparams, boost::shared_ptr<Material> mtl);

	//TODO - jromang replace by a destructor or remove
	static void girlCleanup() { activeContext->cleanup(); }

	//CORE engine control
	//user interactive thread functions
	static void girlStart() { activeContext->start(); }
	static void girlPause() { activeContext->pause(); }
	static void girlExit() { activeContext->exit(); }
	// Dade - wait for the end of the rendering
	static void girlWait() { activeContext->wait(); }
	static void girlWaitParsing() { activeContext->waitParsing(); }

	static void girlSetHaltSamplePerPixel(int haltspp, bool haveEnoughSamplePerPixel, bool suspendThreadsWhenDone) {
		activeContext->setHaltSamplePerPixel(haltspp, haveEnoughSamplePerPixel,suspendThreadsWhenDone);
	}

	//controlling number of threads
	static int girlAddThread() { return activeContext->addThread(); }
	static void girlRemoveThread() { activeContext->removeThread(); }
	static int girlGetRenderingThreadsStatus(RenderingThreadInfo *info, int maxInfoCount) { return activeContext->getRenderingThreadsStatus(info, maxInfoCount); }

	//framebuffer access
	static void girlUpdateFramebuffer() { activeContext->updateFramebuffer(); }
	static unsigned char* girlFramebuffer() { return activeContext->framebuffer(); }
	//static int girlDisplayInterval() { return activeContext->displayInterval(); }
	//static int girlFilmXres() { return activeContext->filmXres(); }
	//static int girlFilmYres() { return activeContext->filmYres(); }

	//film access (networking)
	static void girlUpdateFilmFromNetwork() { activeContext->updateFilmFromNetwork(); }
	static void girlSetNetworkServerUpdateInterval(int updateInterval) { activeContext->renderFarm->serverUpdateInterval = updateInterval; }
	static int girlGetNetworkServerUpdateInterval() { return activeContext->renderFarm->serverUpdateInterval; }
    static void girlAddServer(const string &name) { activeContext->addServer(name); }
    static void girlRemoveServer(const string &name) { activeContext->removeServer(name); }
    static int girlGetServerCount() { return activeContext->getServerCount(); }
	static int girlGetRenderingServersStatus(RenderingServerInfo *info, int maxInfoCount) { return activeContext->getRenderingServersStatus(info, maxInfoCount); }

	//statistics
	static double girlStatistics(const string &statName) { return activeContext->statistics(statName); }

	//film access (networking)
    static void girlTransmitFilm(std::basic_ostream<char> &stream) { activeContext->transmitFilm(stream); }

    // dade enable debug mode
    static void girlEnableDebugMode() { activeContext->enableDebugMode(); }

private:
	static Context *activeContext;
	string name;
	Scene *girlCurrentScene;

        void init();
        void free();

	// API Function Declarations
	void identity();
	void translate(float dx, float dy, float dz);
	void rotate(float angle, float ax, float ay, float az);
	void scale(float sx, float sy, float sz);
	void lookAt(float ex, float ey, float ez, float lx, float ly, float lz,
			float ux, float uy, float uz);
	void concatTransform(float transform[16]);
	void transform(float transform[16]);
	void coordinateSystem(const string &);
	void coordSysTransform(const string &);
	void pixelFilter(const string &name, const ParamSet &params);
	void film(const string &type, const ParamSet &params);
	void sampler(const string &name, const ParamSet &params);
	void accelerator(const string &name, const ParamSet &params);
	void surfaceIntegrator(const string &name, const ParamSet &params);
	void volumeIntegrator(const string &name, const ParamSet &params);
	void camera(const string &, const ParamSet &cameraParams);
	void worldBegin();
	void attributeBegin();
	void attributeEnd();
	void transformBegin();
	void transformEnd();
	void texture(const string &name, const string &type, const string &texname,
			const ParamSet &params);
	void material(const string &name, const ParamSet &params, const string &type = "");
	void makenamedmaterial(const string &name, const ParamSet &params);
	void namedmaterial(const string &name, const ParamSet &params);
	void lightSource(const string &name, const ParamSet &params);
	void areaLightSource(const string &name, const ParamSet &params);
	void portalShape(const string &name, const ParamSet &params);
	void shape(const string &name, const ParamSet &params);
	void reverseOrientation();
	void volume(const string &name, const ParamSet &params);
	void objectBegin(const string &name);
	void objectEnd();
	void objectInstance(const string &name);
	void motionInstance(const string &name, float startTime, float endTime, const string &toTransform);
	void worldEnd();

	//TODO - jromang replace by a destructor or remove
	void cleanup();

	//CORE engine control
	//user interactive thread functions
	void start();
	void pause();
	void exit();
	void wait();
	void waitParsing();

	void setHaltSamplePerPixel(int haltspp, bool haveEnoughSamplePerPixel,
		bool suspendThreadsWhenDone);

	//controlling number of threads
	int addThread();
	void removeThread();
	int getRenderingThreadsStatus(RenderingThreadInfo *info, int maxInfoCount);

	//framebuffer access
	void updateFramebuffer();
	unsigned char* framebuffer();
	/*
	int displayInterval();
	int filmXres();
	int filmYres();*/

	// Dade - network rendering
	void updateFilmFromNetwork();
    void transmitFilm(std::basic_ostream<char> &stream);

	//statistics
	double statistics(const string &statName);
	void addServer(const string &name);
	void removeServer(const string &name);
	int getServerCount();
	int getRenderingServersStatus(RenderingServerInfo *info, int maxInfoCount);

    void enableDebugMode();

	// API Local Classes
	struct RenderOptions {
		// RenderOptions Public Methods
		RenderOptions() {
			// RenderOptions Constructor Implementation
			FilterName = "mitchell";
			FilmName = "multiimage";
			SamplerName = "random";
			AcceleratorName = "kdtree";
			SurfIntegratorName = "path";
			VolIntegratorName = "emission";
			CameraName = "perspective";
			currentInstance = NULL;
            debugMode = false;
		}

		Scene *MakeScene() const;
		// RenderOptions Public Data
		string FilterName;
		ParamSet FilterParams;
		string FilmName;
		ParamSet FilmParams;
		string SamplerName;
		ParamSet SamplerParams;
		string AcceleratorName;
		ParamSet AcceleratorParams;
		string SurfIntegratorName, VolIntegratorName;
		ParamSet SurfIntegratorParams, VolIntegratorParams;
		string CameraName;
		ParamSet CameraParams;
		Transform WorldToCamera;
		bool gotSearchPath;
		mutable vector<Light *> lights;
		mutable vector<boost::shared_ptr<Primitive> > primitives;
		mutable vector<VolumeRegion *> volumeRegions;
		mutable map<string, vector<boost::shared_ptr<Primitive> > > instances;
		mutable vector<boost::shared_ptr<Primitive> > *currentInstance;
        bool debugMode;
	};

	struct NamedMaterial {
		NamedMaterial() {};

		ParamSet materialParams;
		string material;
	};

	struct GraphicsState {
		// Graphics State Methods
		GraphicsState() {
			// GraphicsState Constructor Implementation
			material = "";
			reverseOrientation = false;
		}
		// Graphics State
		map<string, boost::shared_ptr<Texture<float> > > floatTextures;
		map<string, boost::shared_ptr<Texture<SWCSpectrum> > > colorTextures;
		ParamSet materialParams;
		string material;
		string materialName;
		ParamSet areaLightParams;
		string areaLight;
		string currentLight;
		// Dade - some light source like skysun is composed by 2 lights. So
		// we can have 2 current light sources (i.e. Portal have to be applied
		// to both sources, see bug #297)
 		Light* currentLightPtr0;
 		Light* currentLightPtr1;
		bool reverseOrientation;
	};

	int currentApiState;
	Transform curTransform;
	map<string, Transform> namedCoordinateSystems;
	RenderOptions *renderOptions;
	GraphicsState *graphicsState;
	vector<NamedMaterial> namedmaterials;
	vector<GraphicsState> pushedGraphicsStates;
	vector<Transform> pushedTransforms;
	RenderFarm *renderFarm;

        // Dade - mutex used to wait the end of the rendering
        mutable boost::mutex renderingMutex;
		mutable boost::mutex parsingMutex;
};

}

#endif //GIRL_CONTEXT_H
