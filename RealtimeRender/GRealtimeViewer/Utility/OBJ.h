/*
  OBJ ( LighWave *.obj ) Mesh loader
  
  Copyright (c) 2007 By JadeWare - http://jadeware.net
  Based on the C loader, Copyright (c) Robert Bateman, www.robthebloke.org, 2004

  Features:  
  ---------
  - C++ Oriented Loader
  - Materials loading
  - Multi Objects
  - Works in almost all cases and perfeclty with Blender OBJ exports.
*/

#ifndef _OBJ_LOADER_H_
#define _OBJ_LOADER_H_

#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE
#endif

#ifdef WIN32
#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <malloc.h>

#include "structures.h"


class OBJ
{
	public:

		ObjMesh		Mesh;
		ObjVertex	pos;
		ObjVertex	rot;
		ObjVertex	siz;

		ObjVertex	min;
		ObjVertex	max;
		ObjVertex	center;

	public:

		OBJ();
		~OBJ();

	private:

		unsigned int ResizeTriangleArray(ObjMesh *pMesh,unsigned int iExtraTriangleCount);
		void		GetIndices(char *pString, int *pVert, int *pNorm, int *pUV);
		bool		LoadMaterials(ObjMesh *mesh, char *name);
		void		CenterVertice();

		void		Calbound();

	public:

		bool		Load(const char *filename, bool center=false);
		void		Unload();
		void		SetPos(ObjVertex p);
};

#endif
