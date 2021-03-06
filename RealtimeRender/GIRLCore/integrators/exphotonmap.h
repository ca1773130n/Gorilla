/***************************************************************************
 *   Copyright (C) 1998-2008 by authors (see AUTHORS.txt )                 *
 *                                                                         *
 *   This file is part of Girl Renderer.                                    *
 *                                                                         *
 *   Girl Renderer is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   Girl Renderer is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 *                                                                         *
 *   This project is based on PBRT ; see http://www.pbrt.org               *
 *   Girl Renderer website : http://www.girlrender.org                       *
 ***************************************************************************/

// exphotonmap.cpp*
#ifndef GIRL_EXPHOTONMAP_H
#define GIRL_EXPHOTONMAP_H


#include "girl.h"
#include "transport.h"
#include "scene.h"
#include "kdtree.h"
#include "mc.h"
#include "sampling.h"
#include "photonmap.h"

namespace girl
{

class ExPhotonIntegrator : public SurfaceIntegrator {
public:
	// ExPhotonIntegrator types
	enum RenderingMode { RM_DIRECTLIGHTING, RM_PATH };
	enum LightStrategy {
		SAMPLE_ALL_UNIFORM, SAMPLE_ONE_UNIFORM,
		SAMPLE_AUTOMATIC
	};

	// ExPhotonIntegrator Public Methods
	ExPhotonIntegrator(
			RenderingMode rm,
			LightStrategy st,
			int ndir, int ncaus, int nindir, int nrad,
			int nLookup, int mdepth,
			float maxdist, bool finalGather, int gatherSamples, float ga,
			PhotonMapRRStrategy rrstrategy, float rrcontprob,
			float distThreshold,
			string *mapsFileName,
			bool dbgEnableDirect, bool dbgEnableDirectMap, bool dbgEnableCaustic,
			bool dbgEnableIndirect, bool dbgEnableSpecular);
	~ExPhotonIntegrator();

	SWCSpectrum Li(const TsPack *tspack, const Scene *scene, const RayDifferential &ray, 
		const Sample *sample, float *alpha) const;
	void RequestSamples(Sample *sample, const Scene *scene);
	void Preprocess(const TsPack *tspack, const Scene *scene);

	static SurfaceIntegrator *CreateSurfaceIntegrator(const ParamSet &params);
private:
    SWCSpectrum LiDirectLightingMode(const TsPack *tspack, const Scene *scene, 
		const RayDifferential &ray, const Sample *sample, float *alpha,
		const int reflectionDepth, const bool specularBounce) const;
    SWCSpectrum LiPathMode(const TsPack *tspack, const Scene *scene,
		const RayDifferential &ray, const Sample *sample, float *alpha) const;

	// ExPhotonIntegrator Private Data
	RenderingMode renderingMode;
	LightStrategy lightStrategy;
	u_int nDirectPhotons, nCausticPhotons, nIndirectPhotons, nRadiancePhotons;
	u_int nLookup;
	int maxDepth;
	float maxDistSquared;

	bool finalGather;
	float cosGatherAngle;
	int gatherSamples;
	PhotonMapRRStrategy rrStrategy;
	float rrContinueProbability;
	float distanceThreshold;

	// Dade - != NULL if I have to read/write photon maps on file
	string *mapsFileName;

	// Dade - debug flags
	bool debugEnableDirect, debugUseRadianceMap, debugEnableCaustic,
			debugEnableIndirect, debugEnableSpecular;

	int bufferId;

	// Declare sample parameters for light source sampling
	int sampleOffset;
	int sampleFinalGather1Offset;
	int sampleFinalGather2Offset;

	LightPhotonMap *causticMap;
	LightPhotonMap *indirectMap;
	RadiancePhotonMap *radianceMap;
};

}//namespace girl

#endif
