/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   texture.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - texture.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#ifndef GIRL_TEXTURE_H
#define GIRL_TEXTURE_H
// texture.h*
#include "girl.h"
#include "spectrum.h"
#include "geometry.h"

namespace girl
{
// Texture Declarations
class GIRLCOREDLL   TextureMapping2D {
public:
	// TextureMapping2D Interface
	virtual ~TextureMapping2D() { }
	virtual void Map(const DifferentialGeometry &dg,
		float *s, float *t, float *dsdx, float *dtdx,
		float *dsdy, float *dtdy) const = 0;
};
class GIRLCOREDLL   UVMapping2D : public TextureMapping2D {
public:
	// UVMapping2D Public Methods
	UVMapping2D(float su = 1, float sv = 1,
		float du = 0, float dv = 0);
	void Map(const DifferentialGeometry &dg, float *s, float *t,
		float *dsdx, float *dtdx,
		float *dsdy, float *dtdy) const;
private:
	float su, sv, du, dv;
};
class GIRLCOREDLL   SphericalMapping2D : public TextureMapping2D {
public:
	// SphericalMapping2D Public Methods
	SphericalMapping2D(const Transform &toSph)
		: WorldToTexture(toSph) {
	}
	void Map(const DifferentialGeometry &dg, float *s, float *t,
		float *dsdx, float *dtdx,
		float *dsdy, float *dtdy) const;
private:
	void sphere(const Point &P, float *s, float *t) const;
	Transform WorldToTexture;
};
class GIRLCOREDLL 
 CylindricalMapping2D : public TextureMapping2D {
public:
	// CylindricalMapping2D Public Methods
	CylindricalMapping2D(const Transform &toCyl)
		: WorldToTexture(toCyl) {
	}
	void Map(const DifferentialGeometry &dg, float *s, float *t,
		float *dsdx, float *dtdx,
		float *dsdy, float *dtdy) const;
private:
	void cylinder(const Point &P, float *s, float *t) const;
	Transform WorldToTexture;
};
class GIRLCOREDLL   PlanarMapping2D : public TextureMapping2D {
public:
	// PlanarMapping2D Public Methods
	PlanarMapping2D(const Vector &v1, const Vector &v2,
		float du = 0, float dv = 0);
	void Map(const DifferentialGeometry &dg, float *s, float *t,
		float *dsdx, float *dtdx,
		float *dsdy, float *dtdy) const;
private:
	Vector vs, vt;
	float ds, dt;
};
class GIRLCOREDLL   TextureMapping3D {
public:
	// TextureMapping3D Interface
	virtual ~TextureMapping3D() { }
	virtual Point Map(const DifferentialGeometry &dg,
		Vector *dpdx, Vector *dpdy) const = 0;
};
class GIRLCOREDLL   IdentityMapping3D : public TextureMapping3D {
public:
	IdentityMapping3D(const Transform &x)
		: WorldToTexture(x) { }
	Point Map(const DifferentialGeometry &dg, Vector *dpdx,
		Vector *dpdy) const;
		void Apply3DTextureMappingOptions(const TextureParams &tp);
//private:
	Transform WorldToTexture;
};
class GIRLCOREDLL   EnvironmentMapping {
public:
	// EnvironmentMapping Interface
	virtual ~EnvironmentMapping() { }
	virtual void Map(const Vector &wh, float *s, float *t) const = 0;
};
class GIRLCOREDLL   LatLongMapping : public EnvironmentMapping {
public:
	// LatLongMapping Public Methods
	LatLongMapping() {}
	virtual void Map(const Vector &wh, float *s, float *t) const;
};
class GIRLCOREDLL   AngularMapping : public EnvironmentMapping {
public:
	// AngularMapping Public Methods
	AngularMapping() {}
	virtual void Map(const Vector &wh, float *s, float *t) const;
};
class GIRLCOREDLL   VerticalCrossMapping : public EnvironmentMapping {
public:
	// VerticalCross Public Methods
	VerticalCrossMapping() {}
	virtual void Map(const Vector &wh, float *s, float *t) const;
};

enum enTextureType
{
	TEXTURE_CONSTANT,
	TEXTURE_IMAGEMAP
};
template <class T> class GIRLCOREDLL  Texture {
public:
	//typedef boost::shared_ptr<Texture> TexturePtr; <<! Not working with GCC
	// Texture Interface
	virtual T Evaluate(const TsPack *tspack, const DifferentialGeometry &) const = 0;
	virtual void SetPower(float power, float area) { }
	virtual void SetIlluminant() { }
	virtual ~Texture() { }

	virtual enTextureType GetType() { return textureType; }
	virtual string& GetFilePath() { return filepath; }
	virtual T GetV() { return value; }
	virtual RGBColor GetRGB() { return rgb; }
public:
	enTextureType textureType;
	string filepath;
	T value;
	RGBColor rgb;
};
	


  float Noise(float x, float y = .5f, float z = .5f);
  float Noise(const Point &P);
  float FBm(const Point &P, const Vector &dpdx, const Vector &dpdy,
	float omega, int octaves);
  float Turbulence(const Point &P, const Vector &dpdx, const Vector &dpdy,
	float omega, int octaves);
  float Lanczos(float, float tau=2);
}//namespace girl

#endif // GIRL_TEXTURE_H
