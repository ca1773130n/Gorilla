/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   scene.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - scene.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#ifndef GIRL_SCENE_H
#define GIRL_SCENE_H
// scene.h*
#include "girl.h"
#include "api.h"
#include "primitive.h"
#include "sampling.h"
#include "timer.h"

#include <boost/thread/thread.hpp>
#include <boost/noncopyable.hpp>
#include <boost/thread/mutex.hpp>

namespace girl {

class GIRLCOREDLL  RenderThread : public boost::noncopyable {
public:
    RenderThread( int _n, ThreadSignals _signal, SurfaceIntegrator* _Si, VolumeIntegrator* _Vi,
            Sampler* _Splr, Camera* _Cam, Scene* _Scn)
    : n(_n), signal(_signal), surfaceIntegrator(_Si), volumeIntegrator(_Vi),
            sample(NULL), sampler(_Splr), camera(_Cam), scene(_Scn), thread(NULL) {
        stat_Samples=0;
        stat_blackSamples=0;
        sample = new Sample(surfaceIntegrator, volumeIntegrator, scene);
    }

    ~RenderThread()	{
        delete sample;
        delete thread;
    }

    static void render(RenderThread *r);

    int  n;
	ThreadSignals signal;
    double stat_Samples, stat_blackSamples;
    SurfaceIntegrator *surfaceIntegrator;
    VolumeIntegrator *volumeIntegrator;
    Sample *sample;
    Sampler *sampler;
    Camera *camera;
    Scene *scene;
	TsPack *tspack;
    boost::thread *thread; // keep pointer to delete the thread object
};

// Scene Declarations
class GIRLCOREDLL   Scene {
public:
    // Scene Public Methods
    void Render();
    Scene(Camera *c, SurfaceIntegrator *in,
            VolumeIntegrator *vi, Sampler *s,
            boost::shared_ptr<Primitive> accel, const vector<Light *> &lts,
            VolumeRegion *vr);
    ~Scene();
    bool Intersect(const Ray &ray, Intersection *isect) const {
        return aggregate->Intersect(ray, isect);
    }
    bool IntersectP(const Ray &ray) const {
        return aggregate->IntersectP(ray);
    }
    const BBox &WorldBound() const;
    SWCSpectrum Li(const RayDifferential &ray, const Sample *sample,
            float *alpha = NULL) const;
	// modulates the supplied SWCSpectrum with the transmittance along the ray
    void Transmittance(const TsPack *tspack, const Ray &ray, const Sample *sample, SWCSpectrum *const L) const;

    //Control methods
    void Start();
    void Pause();
    void Exit();
    //controlling number of threads
    int AddThread(); //returns the thread ID
    void RemoveThread();
	int getThreadsStatus(RenderingThreadInfo *info, int maxInfoCount);


    double GetNumberOfSamples();
    double Statistics_SamplesPSec();
    double Statistics_SamplesPTotSec();
    double Statistics_Efficiency();
    double Statistics_SamplesPPx();
    //framebuffer access
    void UpdateFramebuffer();
    unsigned char* GetFramebuffer();
    int DisplayInterval();
    int FilmXres();
    int FilmYres();
	int FilmHaltspp();
    Timer s_Timer;
    double lastSamples, lastTime;
    //statistics
    double Statistics(const string &statName);

    int CreateRenderThread();
    void RemoveRenderThread();
    void SignalThreads(ThreadSignals signal);

    // Scene Data
    boost::shared_ptr<Primitive> aggregate;
    vector<Light *> lights;
    Camera *camera;
    VolumeRegion *volumeRegion;
    SurfaceIntegrator *surfaceIntegrator;
    VolumeIntegrator *volumeIntegrator;
    Sampler *sampler;
    BBox bound;
    int seedBase;

	ContributionPool *contribPool;

    // Dade - number of samples received from network
    double numberOfSamplesFromNetwork;

    // Dade - used to suspend render threads until the preprocessing phase is done
    bool preprocessDone;

	// Dade - tell rendering threads what to do when they have done
	bool suspendThreadsWhenDone;

private:

	// Dade - mutex used for adding/removing threads
	boost::mutex renderThreadsMutex;
    std::vector<RenderThread*> renderThreads;
    //boost::thread_group threadGroup;
    ThreadSignals CurThreadSignal;
	TsPack *tspack;
};

}//namespace girl

#endif // GIRL_SCENE_H
