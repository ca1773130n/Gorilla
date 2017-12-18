/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   realistic.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - realistic.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// realistic.cpp*
#include "camera.h"
#include "dynload.h"

namespace girl
{

struct Lens {
    Lens(const bool ent, const float n, const float ap,
        boost::shared_ptr<Shape> shape) 
        : entering(ent), eta(n), aperture(ap), shape(shape) {}
    bool entering;
    float eta;
    float aperture;
    boost::shared_ptr<Shape> shape;
};

class GIRLCOREDLL  RealisticCamera : public Camera {
public:
  RealisticCamera(const Transform &world2cam, const float Screen[4],
		  float hither, float yon, float sopen,
		  float sclose, float filmdistance, float aperture_diameter, string specfile,
		  float filmdiag, Film *film);
  ~RealisticCamera(void);
  float GenerateRay(const Sample &sample, Ray *) const;

  static Camera *CreateCamera(const Transform &world2cam, const ParamSet &params, Film *film);
  
private:
    float ParseLensData(const string& specfile);

    float filmDistance, filmDist2, filmDiag;
    float apertureDiameter, distToBack, backAperture;
 
    vector<boost::shared_ptr<Lens> > lenses;

    Transform RasterToFilm, RasterToCamera, FilmToCamera;
};

}//namespace girl

