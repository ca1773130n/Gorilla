/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   blender_noiselib.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - blender_noiselib.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// blender_noiselib.cpp*
#include "girl.h"

namespace blender
{

// Dade - sources imported from Blender with authors permission

float BLI_hnoise(float noisesize, float x, float y, float z);
float BLI_hnoisep(float noisesize, float x, float y, float z);
float BLI_turbulence(float noisesize, float x, float y, float z, int nr);
float BLI_turbulence1(float noisesize, float x, float y, float z, int nr);
float BLI_gNoise(float noisesize, float x, float y, float z, int hard, int noisebasis);
float BLI_gTurbulence(float noisesize, float x, float y, float z, int oct, int hard, int noisebasis);
/* newnoise: musgrave functions */
float mg_fBm(float x, float y, float z, float H, float lacunarity, float octaves, int noisebasis);
float mg_MultiFractal(float x, float y, float z, float H, float lacunarity, float octaves, int noisebasis);
float mg_VLNoise(float x, float y, float z, float distortion, int nbas1, int nbas2);
float mg_HeteroTerrain(float x, float y, float z, float H, float lacunarity, float octaves, float offset, int noisebasis);
float mg_HybridMultiFractal(float x, float y, float z, float H, float lacunarity, float octaves, float offset, float gain, int noisebasis);
float mg_RidgedMultiFractal(float x, float y, float z, float H, float lacunarity, float octaves, float offset, float gain, int noisebasis);
/* newnoise: voronoi */
void voronoi(float x, float y, float z, float* da, float* pa, float me, int dtype);
/* newnoise: cellNoise & cellNoiseV (for vector/point/color) */
float cellNoise(float x, float y, float z);
void cellNoiseV(float x, float y, float z, float *ca);

} // namespace blender
