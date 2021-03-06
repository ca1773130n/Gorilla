/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   carpaint.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - carpaint.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// carpaint.cpp*

// Simulate car paint - adopted from Gunther et al, "Effcient Acquisition and Realistic Rendering of Car Paint", 2005

#include "carpaint.h"
#include "bxdf.h"
#include "blinn.h"
#include "fresnelslick.h"
#include "microfacet.h"
#include "constant.h"
#include "fresnelblend.h"
#include "paramset.h"
#include "dynload.h"

using namespace girl;

CarPaint::CarPaint(boost::shared_ptr<Texture<SWCSpectrum> > kd,
                   boost::shared_ptr<Texture<SWCSpectrum> > ks1, boost::shared_ptr<Texture<SWCSpectrum> > ks2, boost::shared_ptr<Texture<SWCSpectrum> > ks3,
                   boost::shared_ptr<Texture<float> > r1, boost::shared_ptr<Texture<float> > r2, boost::shared_ptr<Texture<float> > r3,
                   boost::shared_ptr<Texture<float> > m1, boost::shared_ptr<Texture<float> > m2, boost::shared_ptr<Texture<float> > m3,
                   boost::shared_ptr<Texture<float> > bump) {
  Kd = kd;
  Ks1 = ks1;
  Ks2 = ks2;
  Ks3 = ks3;
  R1 = r1;
  R2 = r2;
  R3 = r3;
  M1 = m1;
  M2 = m2;
  M3 = m3;
  bumpMap = bump;
}

// CarPaint Method Definitions
BSDF *CarPaint::GetBSDF(const TsPack *tspack, const DifferentialGeometry &dgGeom, const DifferentialGeometry &dgShading, float u) const {

  // Allocate _BSDF_, possibly doing bump-mapping with _bumpMap_
  DifferentialGeometry dgs;

  if (bumpMap)
    Bump(bumpMap, dgGeom, dgShading, &dgs);
  else
    dgs = dgShading;

  BSDF *bsdf = BSDF_ALLOC(tspack, BSDF)(dgs, dgGeom.nn);

  // NOTE - lordcrc - changed clamping to 0..1 to avoid >1 reflection
  SWCSpectrum kd = Kd->Evaluate(tspack, dgs).Clamp(0.f, 1.f);
  SWCSpectrum ks1 = Ks1->Evaluate(tspack, dgs).Clamp(0.f, 1.f);
  SWCSpectrum ks2 = Ks2->Evaluate(tspack, dgs).Clamp(0.f, 1.f);
  SWCSpectrum ks3 = Ks3->Evaluate(tspack, dgs).Clamp(0.f, 1.f);

  // NOTE - lordcrc - added clamping to 0..1 to avoid >1 reflection
  float r1 = Clamp(R1->Evaluate(tspack, dgs), 0.f, 1.f);
  float r2 = Clamp(R2->Evaluate(tspack, dgs), 0.f, 1.f);
  float r3 = Clamp(R3->Evaluate(tspack, dgs), 0.f, 1.f);

  float m1 = M1->Evaluate(tspack, dgs);
  float m2 = M2->Evaluate(tspack, dgs);
  float m3 = M3->Evaluate(tspack, dgs);

  MicrofacetDistribution *md1 = BSDF_ALLOC(tspack, Blinn)((2.0 * M_PI / (m1 * m1)) - 1.0);
  MicrofacetDistribution *md2 = BSDF_ALLOC(tspack, Blinn)((2.0 * M_PI / (m2 * m2)) - 1.0);
  MicrofacetDistribution *md3 = BSDF_ALLOC(tspack, Blinn)((2.0 * M_PI / (m3 * m3)) - 1.0);

  /*MicrofacetDistribution *md1 = BSDF_ALLOC(tspack, WardIsotropic)(m1);
  MicrofacetDistribution *md2 = BSDF_ALLOC(tspack, WardIsotropic)(m2);
  MicrofacetDistribution *md3 = BSDF_ALLOC(tspack, WardIsotropic)(m3);*/

  // The Slick approximation is much faster and visually almost the same
  Fresnel *fr1 = BSDF_ALLOC(tspack, FresnelSlick)(r1);
  Fresnel *fr2 = BSDF_ALLOC(tspack, FresnelSlick)(r2);
  Fresnel *fr3 = BSDF_ALLOC(tspack, FresnelSlick)(r3);

  // The Carpaint BRDF is really a Multi-lobe Microfacet model with a Lambertian base

  SWCSpectrum *lobe_ks = (SWCSpectrum *)BSDF::Alloc(tspack, 3 * sizeof(SWCSpectrum));
  lobe_ks[0] = ks1;
  lobe_ks[1] = ks2;
  lobe_ks[2] = ks3;

  MicrofacetDistribution **lobe_dist = (MicrofacetDistribution **)BSDF::Alloc(tspack, 3 * sizeof(MicrofacetDistribution *));
  lobe_dist[0] = md1;
  lobe_dist[1] = md2;
  lobe_dist[2] = md3;

  Fresnel **lobe_fres = (Fresnel **)BSDF::Alloc(tspack, 3 * sizeof(Fresnel *));
  lobe_fres[0] = fr1;
  lobe_fres[1] = fr2;
  lobe_fres[2] = fr3;

  // Broad gloss layers
  for (int i = 0; i < 2; i++) {
    bsdf->Add(BSDF_ALLOC(tspack, Microfacet)(lobe_ks[i], lobe_fres[i], lobe_dist[i]));
  }

  // Clear coat and lambertian base
  bsdf->Add(BSDF_ALLOC(tspack, FresnelBlend)(kd, lobe_ks[2], SWCSpectrum(0.), 0., lobe_dist[2]));

  //bsdf->Add(BSDF_ALLOC(tspack, CookTorrance)(kd, 3, lobe_ks, lobe_dist, lobe_fres));

  return bsdf;
}

void DataFromName(const string name, boost::shared_ptr<Texture<SWCSpectrum> > *Kd,
                                     boost::shared_ptr<Texture<SWCSpectrum> > *Ks1,
                                     boost::shared_ptr<Texture<SWCSpectrum> > *Ks2,
                                     boost::shared_ptr<Texture<SWCSpectrum> > *Ks3,
                                     boost::shared_ptr<Texture<float> > *R1,
                                     boost::shared_ptr<Texture<float> > *R2,
                                     boost::shared_ptr<Texture<float> > *R3,
                                     boost::shared_ptr<Texture<float> > *M1,
                                     boost::shared_ptr<Texture<float> > *M2,
                                     boost::shared_ptr<Texture<float> > *M3) {

  int numPaints = sizeof(carpaintdata) / sizeof(CarPaintData);

  // default (Ford F8)

  int i;

  for (i = 0; i < numPaints; i++) {
    if (name.compare(carpaintdata[i].name) == 0)
      break;
  }

  boost::shared_ptr<Texture<SWCSpectrum> > kd (new ConstantRGBColorTexture<SWCSpectrum>(carpaintdata[i].kd));
  boost::shared_ptr<Texture<SWCSpectrum> > ks1 (new ConstantRGBColorTexture<SWCSpectrum>(carpaintdata[i].ks1));
  boost::shared_ptr<Texture<SWCSpectrum> > ks2 (new ConstantRGBColorTexture<SWCSpectrum>(carpaintdata[i].ks2));
  boost::shared_ptr<Texture<SWCSpectrum> > ks3 (new ConstantRGBColorTexture<SWCSpectrum>(carpaintdata[i].ks3));
  boost::shared_ptr<Texture<float> > r1 (new ConstantFloatTexture<float>(carpaintdata[i].r1));
  boost::shared_ptr<Texture<float> > r2 (new ConstantFloatTexture<float>(carpaintdata[i].r2));
  boost::shared_ptr<Texture<float> > r3 (new ConstantFloatTexture<float>(carpaintdata[i].r3));
  boost::shared_ptr<Texture<float> > m1 (new ConstantFloatTexture<float>(carpaintdata[i].m1));
  boost::shared_ptr<Texture<float> > m2 (new ConstantFloatTexture<float>(carpaintdata[i].m2));
  boost::shared_ptr<Texture<float> > m3 (new ConstantFloatTexture<float>(carpaintdata[i].m3));

  *Kd = kd;
  *Ks1 = ks1;
  *Ks2 = ks2;
  *Ks3 = ks3;
  *R1 = r1;
  *R2 = r2;
  *R3 = r3;
  *M1 = m1;
  *M2 = m2;
  *M3 = m3;
}

Material* CarPaint::CreateMaterial(const Transform &xform, const TextureParams &mp) {

  // Default values for missing parameters is from the Ford F8 dataset
  float def_kd[3], def_ks1[3], def_ks2[3], def_ks3[3], def_r[3], def_m[3];

  for (int i = 0; i < 3; i++) {
    def_kd[i] = carpaintdata[0].kd[i];
    def_ks1[i] = carpaintdata[0].ks1[i];
    def_ks2[i] = carpaintdata[0].ks2[i];
    def_ks3[i] = carpaintdata[0].ks3[i];
  }

  def_r[0] = carpaintdata[0].r1;
  def_r[1] = carpaintdata[0].r2;
  def_r[2] = carpaintdata[0].r3;

  def_m[0] = carpaintdata[0].m1;
  def_m[1] = carpaintdata[0].m2;
  def_m[2] = carpaintdata[0].m3;

  string paintname = mp.FindString("name");

  boost::shared_ptr<Texture<SWCSpectrum> > Kd;

  boost::shared_ptr<Texture<SWCSpectrum> > Ks1;
  boost::shared_ptr<Texture<SWCSpectrum> > Ks2;
  boost::shared_ptr<Texture<SWCSpectrum> > Ks3;

  boost::shared_ptr<Texture<float> > R1;
  boost::shared_ptr<Texture<float> > R2;
  boost::shared_ptr<Texture<float> > R3;

  boost::shared_ptr<Texture<float> > M1;
  boost::shared_ptr<Texture<float> > M2;
  boost::shared_ptr<Texture<float> > M3;

  if (paintname.length() < 1) {
    // we got no name, so try to read material properties directly
    Kd = mp.GetSWCSpectrumTexture("Kd", RGBColor(def_kd));

    Ks1 = mp.GetSWCSpectrumTexture("Ks1", RGBColor(def_ks1));
    Ks2 = mp.GetSWCSpectrumTexture("Ks2", RGBColor(def_ks2));
    Ks3 = mp.GetSWCSpectrumTexture("Ks3", RGBColor(def_ks3));

    R1 = mp.GetFloatTexture("R1", def_r[0]);
    R2 = mp.GetFloatTexture("R2", def_r[1]);
    R3 = mp.GetFloatTexture("R3", def_r[2]);

    M1 = mp.GetFloatTexture("M1", def_m[0]);
    M2 = mp.GetFloatTexture("M2", def_m[1]);
    M3 = mp.GetFloatTexture("M3", def_m[2]);
  }
  else
    // Pick from presets, fall back to the first if name not found
    DataFromName(paintname, &Kd, &Ks1, &Ks2, &Ks3, &R1, &R2, &R3, &M1, &M2, &M3);

  boost::shared_ptr<Texture<float> > bumpMap = mp.GetFloatTexture("bumpmap");

  return new CarPaint(Kd, Ks1, Ks2, Ks3, R1, R2, R3, M1, M2, M3, bumpMap);
}

static DynamicLoader::RegisterMaterial<CarPaint> r("carpaint");
