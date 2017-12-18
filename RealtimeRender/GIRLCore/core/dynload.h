/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   dynload.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - dynload.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#ifndef GIRL_DYNLOAD_H
#define GIRL_DYNLOAD_H

#include "girl.h"

#include <map>
#include <string>
using std::map;
using std::string;

namespace girl
{

// Runtime Loading Declarations
boost::shared_ptr<Shape> MakeShape(const string &name,
	const Transform &object2world, bool reverseOrientation,
	const ParamSet &paramSet);
boost::shared_ptr<Material> MakeMaterial(const string &name,
	const Transform &mtl2world, const TextureParams &mp, const string& matname="");
boost::shared_ptr<Texture<float> > MakeFloatTexture(const string &name,
	const Transform &tex2world, const TextureParams &tp);
boost::shared_ptr<Texture<SWCSpectrum> > MakeSWCSpectrumTexture(const string &name,
	const Transform &tex2world, const TextureParams &tp);
Light *MakeLight(const string &name, const Transform &light2world,
	const ParamSet &paramSet);
AreaLight *MakeAreaLight(const string &name,
	const Transform &light2world, const ParamSet &paramSet, const TextureParams &tp,
	const boost::shared_ptr<Primitive> &prim);
VolumeRegion *MakeVolumeRegion(const string &name,
	const Transform &light2world, const ParamSet &paramSet);
SurfaceIntegrator *MakeSurfaceIntegrator(const string &name,
	const ParamSet &paramSet);
VolumeIntegrator *MakeVolumeIntegrator(const string &name,
	const ParamSet &paramSet);
boost::shared_ptr<Aggregate> MakeAccelerator(const string &name, const vector<boost::shared_ptr<Primitive> > &prims,
	const ParamSet &paramSet);
Camera *MakeCamera(const string &name, const Transform &world2cam,
	const ParamSet &paramSet, Film *film);
Sampler *MakeSampler(const string &name, const ParamSet &paramSet,
	const Film *film);
Filter *MakeFilter(const string &name, const ParamSet &paramSet);
ToneMap *MakeToneMap(const string &name, const ParamSet &paramSet);
Film *MakeFilm(const string &name, const ParamSet &paramSet, Filter *filt);
PixelSampler *MakePixelSampler(const string &name, const ParamSet &paramSet);

class GIRLCOREDLL  DynamicLoader {
	template <class T> class RegisterLoader {
	public:
		RegisterLoader<T>(map<string, T> &store, const string &name, T loader)
		{
			store[name] = loader;
		}
		virtual ~RegisterLoader<T>() {}
	};

public:
	static map<string, boost::shared_ptr<Material> > &registeredMaterialsPointer();
	static map<string, boost::shared_ptr<Shape> > &registeredShapesPointer();
	

	typedef Shape *(*CreateShape)(const Transform&, bool, const ParamSet&);
	static map<string, CreateShape> &registeredShapes();
	template <class T> class RegisterShape : public RegisterLoader<CreateShape> {
	public:
		RegisterShape<T>(const string &name) :
			RegisterLoader<CreateShape>(registeredShapes(), name, &T::CreateShape) {}
		virtual ~RegisterShape<T>() {}
	};

	typedef Material *(*CreateMaterial)(const Transform&,
		const TextureParams&);
	static map<string, CreateMaterial> &registeredMaterials();
	template <class T> class RegisterMaterial : public RegisterLoader<CreateMaterial> {
	public:
		RegisterMaterial<T>(const string &name) :
			RegisterLoader<CreateMaterial>(registeredMaterials(), name, &T::CreateMaterial) {}
		virtual ~RegisterMaterial<T>() {}
	};

	typedef Texture<float> *(*CreateFloatTexture)(const Transform&,
		const TextureParams&);
	static map<string, CreateFloatTexture> &registeredFloatTextures();
	template <class T> class RegisterFloatTexture : public RegisterLoader<CreateFloatTexture> {
	public:
		RegisterFloatTexture<T>(const string &name) :
			RegisterLoader<CreateFloatTexture>(registeredFloatTextures(), name, &T::CreateFloatTexture) {}
		virtual ~RegisterFloatTexture<T>() {}
	};

	typedef Texture<SWCSpectrum> *(*CreateSWCSpectrumTexture)(const Transform&,
		const TextureParams&);
	static map<string, CreateSWCSpectrumTexture> &registeredSWCSpectrumTextures();
	template <class T> class RegisterSWCSpectrumTexture : public RegisterLoader<CreateSWCSpectrumTexture> {
	public:
		RegisterSWCSpectrumTexture<T>(const string &name) :
			RegisterLoader<CreateSWCSpectrumTexture>(registeredSWCSpectrumTextures(), name, &T::CreateSWCSpectrumTexture) {}
		virtual ~RegisterSWCSpectrumTexture<T>() {}
	};

	typedef Light *(*CreateLight)(const Transform&, const ParamSet&);
	static map<string, CreateLight> &registeredLights();
	template <class T> class RegisterLight : public RegisterLoader<CreateLight> {
	public:
		RegisterLight<T>(const string &name) :
			RegisterLoader<CreateLight>(registeredLights(), name, &T::CreateLight) {}
		virtual ~RegisterLight<T>() {}
	};

	typedef AreaLight *(*CreateAreaLight)(const Transform&, const ParamSet&, const TextureParams&,
		const boost::shared_ptr<Primitive>&);
	static map<string, CreateAreaLight> &registeredAreaLights();
	template <class T> class RegisterAreaLight : public RegisterLoader<CreateAreaLight> {
	public:
		RegisterAreaLight<T>(const string &name) :
			RegisterLoader<CreateAreaLight>(registeredAreaLights(), name, &T::CreateAreaLight) {}
		virtual ~RegisterAreaLight<T>() {}
	};

	typedef VolumeRegion *(*CreateVolumeRegion)(const Transform&,
		const ParamSet&);
	static map<string, CreateVolumeRegion> &registeredVolumeRegions();
	template <class T> class RegisterVolumeRegion : public RegisterLoader<CreateVolumeRegion> {
	public:
		RegisterVolumeRegion<T>(const string &name) :
			RegisterLoader<CreateVolumeRegion>(registeredVolumeRegions(), name, &T::CreateVolumeRegion) {}
		virtual ~RegisterVolumeRegion<T>() {}
	};

	typedef SurfaceIntegrator *(*CreateSurfaceIntegrator)(const ParamSet&);
	static map<string, CreateSurfaceIntegrator> &registeredSurfaceIntegrators();
	template <class T> class RegisterSurfaceIntegrator : public RegisterLoader<CreateSurfaceIntegrator> {
	public:
		RegisterSurfaceIntegrator<T>(const string &name) :
			RegisterLoader<CreateSurfaceIntegrator>(registeredSurfaceIntegrators(), name, &T::CreateSurfaceIntegrator) {}
		virtual ~RegisterSurfaceIntegrator<T>() {}
	};

	typedef VolumeIntegrator *(*CreateVolumeIntegrator)(const ParamSet&);
	static map<string, CreateVolumeIntegrator> &registeredVolumeIntegrators();
	template <class T> class RegisterVolumeIntegrator : public RegisterLoader<CreateVolumeIntegrator> {
	public:
		RegisterVolumeIntegrator<T>(const string &name) :
			RegisterLoader<CreateVolumeIntegrator>(registeredVolumeIntegrators(), name, &T::CreateVolumeIntegrator) {}
		virtual ~RegisterVolumeIntegrator<T>() {}
	};

	typedef Aggregate *(*CreateAccelerator)(const vector<boost::shared_ptr<Primitive> >&,
		const ParamSet&);
	static map<string, CreateAccelerator> &registeredAccelerators();
	template <class T> class RegisterAccelerator : public RegisterLoader<CreateAccelerator> {
	public:
		RegisterAccelerator<T>(const string &name) :
			RegisterLoader<CreateAccelerator>(registeredAccelerators(), name, &T::CreateAccelerator) {}
		virtual ~RegisterAccelerator<T>() {}
	};

	typedef Camera *(*CreateCamera)(const Transform&, const ParamSet&,
		Film*);
	static map<string, CreateCamera> &registeredCameras();
	template <class T> class RegisterCamera : public RegisterLoader<CreateCamera> {
	public:
		RegisterCamera<T>(const string &name) :
			RegisterLoader<CreateCamera>(registeredCameras(), name, &T::CreateCamera) {}
		virtual ~RegisterCamera<T>() {}
	};

	typedef Sampler *(*CreateSampler)(const ParamSet&, const Film*);
	static map<string, CreateSampler> &registeredSamplers();
	template <class T> class RegisterSampler : public RegisterLoader<CreateSampler> {
	public:
		RegisterSampler<T>(const string &name) :
			RegisterLoader<CreateSampler>(registeredSamplers(), name, &T::CreateSampler) {}
		virtual ~RegisterSampler<T>() {}
	};

	typedef Filter *(*CreateFilter)(const ParamSet&);
	static map<string, CreateFilter> &registeredFilters();
	template <class T> class RegisterFilter : public RegisterLoader<CreateFilter> {
	public:
		RegisterFilter<T>(const string &name) :
			RegisterLoader<CreateFilter>(registeredFilters(), name, &T::CreateFilter) {}
		virtual ~RegisterFilter<T>() {}
	};

	typedef ToneMap *(*CreateToneMap)(const ParamSet&);
	static map<string, CreateToneMap> &registeredToneMaps();
	template <class T> class RegisterToneMap : public RegisterLoader<CreateToneMap> {
	public:
		RegisterToneMap<T>(const string &name) :
			RegisterLoader<CreateToneMap>(registeredToneMaps(), name, &T::CreateToneMap) {}
		virtual ~RegisterToneMap<T>() {}
	};

	typedef Film *(*CreateFilm)(const ParamSet&, Filter*);
	static map<string, CreateFilm> &registeredFilms();
	template <class T> class RegisterFilm : public RegisterLoader<CreateFilm> {
	public:
		RegisterFilm<T>(const string &name) :
			RegisterLoader<CreateFilm>(registeredFilms(), name, &T::CreateFilm) {}
		virtual ~RegisterFilm<T>() {}
	};

	typedef PixelSampler *(*CreatePixelSampler)(const ParamSet&);
	static map<string, CreatePixelSampler> &registeredPixelSamplers();
	template <class T> class RegisterPixelSampler : public RegisterLoader<CreatePixelSampler> {
	public:
		RegisterPixelSampler<T>(const string &name) :
			RegisterLoader<CreatePixelSampler>(registeredPixelSamplers(), name, &T::CreatePixelSampler) {}
		virtual ~RegisterPixelSampler<T>() {}
	};
};

}//namespace girl

#endif // GIRL_DYNLOAD_H
