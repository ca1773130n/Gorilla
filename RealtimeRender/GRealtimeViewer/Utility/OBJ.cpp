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

#include "OBJ.h"


OBJ::OBJ()
{
	Mesh.m_aFaces			= NULL;
	Mesh.m_aNormalArray		= NULL;
	Mesh.m_aTexCoordArray	= NULL;
	Mesh.m_aVertexArray		= NULL;
	Mesh.m_iNumberOfMaterials	= 0;
	Mesh.m_iNumberOfFaces		= 0;
	Mesh.m_iNumberOfNormals		= 0;
	Mesh.m_iNumberOfTexCoords	= 0;
	Mesh.m_iNumberOfVertices	= 0;
	Mesh.m_iNumberOfObjetcs		= 0;
	for(int i=0 ; i<MAX_OBJETS ; i++)
	{
		memset(Mesh.m_aObjetArray[i].name, 0, MAX_OBJECTNAME);
	}
	for(int m=0 ; m<MAX_MATERIALS ; m++)
	{
		Mesh.m_aMaterialArray[m].opengl_index = -1;
	}

	pos.x = 0;
	pos.y = 0;
	pos.z = 0;

	rot.x = 0;
	rot.y = 0;
	rot.z = 0;

	siz.x = 1;
	siz.y = 1;
	siz.z = 1;

}

OBJ::~OBJ()
{
	Unload();
}

bool OBJ::LoadMaterials(ObjMesh *mesh, char *name)
{
	mesh->m_iNumberOfMaterials = 0;

	FILE *f = fopen(name, "r+b");
	if(!f)
		return false;
	while(!feof(f))
	{
		char buffer[200];
		memset(buffer,0,200);

		//	Grab a line at a time
		fgets(buffer,256,f);

		//	look for the prefix
		if( strncmp("newmtl",buffer,6) == 0 )
		{	
			mesh->m_aMaterialArray[ mesh->m_iNumberOfMaterials ].valid = false;
			memset(mesh->m_aMaterialArray[ mesh->m_iNumberOfMaterials ].name, 0, 256);
			memset(mesh->m_aMaterialArray[ mesh->m_iNumberOfMaterials ].img_path, 0, 256);
			sscanf(buffer, "newmtl %s", mesh->m_aMaterialArray[ mesh->m_iNumberOfMaterials ].name);
		}
		else if( strncmp("map_Kd",buffer,6) == 0 )
		{
			sscanf(buffer, "map_Kd %s", mesh->m_aMaterialArray[ mesh->m_iNumberOfMaterials ].img_path);
			mesh->m_aMaterialArray[ mesh->m_iNumberOfMaterials ].valid = true;
			mesh->m_iNumberOfMaterials ++;
		}
	}
	fclose(f);
	return true;
}

void OBJ::CenterVertice()
{
	printf("centering vertice..\n");
	ObjMesh *pMesh = &Mesh;
	if(pMesh == NULL)
		return;
	if(pMesh->m_iNumberOfVertices == 0)
		return;
	/*ObjVertex vMin = pMesh->m_aVertexArray[0];	
	ObjVertex vMax = pMesh->m_aVertexArray[0]; 
	for(unsigned int i=0 ; i<pMesh->m_iNumberOfVertices ; i++)
	{
		if(pMesh->m_aVertexArray[i].x > vMax.x)	vMax.x = pMesh->m_aVertexArray[i].x;
		if(pMesh->m_aVertexArray[i].y > vMax.y)	vMax.y = pMesh->m_aVertexArray[i].y;
		if(pMesh->m_aVertexArray[i].z > vMax.z)	vMax.z = pMesh->m_aVertexArray[i].z;

		if(pMesh->m_aVertexArray[i].x < vMin.x)	vMin.x = pMesh->m_aVertexArray[i].x;
		if(pMesh->m_aVertexArray[i].y < vMin.y)	vMin.y = pMesh->m_aVertexArray[i].y;
		if(pMesh->m_aVertexArray[i].z < vMin.z)	vMin.z = pMesh->m_aVertexArray[i].z;
	}
	printf("boundaries are: (%f,%f,%f) -> (%f,%f,%f)\n", vMin.x, vMin.y, vMin.z, vMax.x, vMax.y, vMax.z);

	ObjVertex CenterOffset;
	CenterOffset.x = -(vMin.x + vMax.x)/2;
	CenterOffset.y = -(vMin.y + vMax.y)/2;
	CenterOffset.z = -(vMin.z + vMax.z)/2;
	printf("center offset is: (%f,%f,%f)\n", CenterOffset.x, CenterOffset.y, CenterOffset.z);

	for(unsigned int i=0 ; i<pMesh->m_iNumberOfVertices ; i++)
	{
		pMesh->m_aVertexArray[i].x += CenterOffset.x;
		pMesh->m_aVertexArray[i].y += CenterOffset.y;
		pMesh->m_aVertexArray[i].z += CenterOffset.z;
	}*/
	for(unsigned int i=0 ; i<pMesh->m_iNumberOfVertices ; i++)
	{
		pMesh->m_aVertexArray[i].x -= center.x;
		pMesh->m_aVertexArray[i].y -= center.y;
		pMesh->m_aVertexArray[i].z -= center.z;
	}
}

void OBJ::Calbound()
{
	
	ObjMesh *pMesh = &Mesh;
	if(pMesh == NULL)
		return;
	if(pMesh->m_iNumberOfVertices == 0)
		return;
	min = pMesh->m_aVertexArray[0];	
	max = pMesh->m_aVertexArray[0]; 
	for(unsigned int i=0 ; i<pMesh->m_iNumberOfVertices ; i++)
	{
		if(pMesh->m_aVertexArray[i].x > max.x)	max.x = pMesh->m_aVertexArray[i].x;
		if(pMesh->m_aVertexArray[i].y > max.y)	max.y = pMesh->m_aVertexArray[i].y;
		if(pMesh->m_aVertexArray[i].z > max.z)	max.z = pMesh->m_aVertexArray[i].z;

		if(pMesh->m_aVertexArray[i].x < min.x)	min.x = pMesh->m_aVertexArray[i].x;
		if(pMesh->m_aVertexArray[i].y < min.y)	min.y = pMesh->m_aVertexArray[i].y;
		if(pMesh->m_aVertexArray[i].z < min.z)	min.z = pMesh->m_aVertexArray[i].z;
	}
#ifdef GORDEBUG
	printf("boundaries are: (%f,%f,%f) -> (%f,%f,%f)\n", min.x, min.y, min.z, max.x, max.y, max.z);
#endif

	center.x = (min.x + max.x)/2;
	center.y = (min.y + max.y)/2;
	center.z = (min.z + max.z)/2;
	printf("center is: (%f,%f,%f)\n", center.x, center.y, center.z);

	
}

/*
===========================================================
This method will take a pointer to the mesh and the number 
of extra triangles that are required. It will then resize 
the polygon face array within the mesh and return the index
of the first new face that was allocated.
===========================================================
*/
unsigned int OBJ::ResizeTriangleArray(ObjMesh *pMesh, unsigned int iExtraTriangleCount)
{
	ObjFace *pTempFaces = pMesh->m_aFaces;
	unsigned int iTemp  = pMesh->m_iNumberOfFaces;
	pMesh->m_iNumberOfFaces += iExtraTriangleCount;
	pMesh->m_aFaces = (ObjFace*)malloc(pMesh->m_iNumberOfFaces*sizeof(ObjFace));
	memcpy(pMesh->m_aFaces,pTempFaces,iTemp*sizeof(ObjFace));
	free(pTempFaces);
	memset(&pMesh->m_aFaces[ iTemp ],0,iExtraTriangleCount*sizeof(ObjFace) );
	return iTemp;
}

/*
===========================================================
Walk through the string a character at a time looking for the '/' character. The first one
we find will may have the uv - coord index behind it. The '/' character will be removed by
setting it to NULL, If for example the texturing co-ord is missing, then you'd expect "//" to
appear. This while loop would set both of those to NULL, that would give you a NULL string
for the uv coord. This allows us to test the string later to see if there is or isn't a
value there.
===========================================================
*/
void OBJ::GetIndices(char *pString, int *pVert, int *pNorm, int *pUV)
{
	//	Use 3 pointers to hold references to the bits of the string where the indices start
	//	for the vertex, normal and texturing co-ordinate respectivly
	char *pV = pString, *pN = NULL, *pT = NULL;

	while( *pString != '\0' )
	{
		if(*pString == '/')
		{
			*pString = '\0';
			if(!pT)
			{
				pT = pString+1;
			}
			else
			{
				pN = pString+1;
			}
		}
		pString++;
	}

	//	Read the vertex index
	sscanf(pV,"%d",pVert);

	//	If a normal is there, then read it's value, otherwise set the default of 1
	//	(1 is used because we will decrease the value by 1 later thus giving us 0)
	if( pN == NULL || *pN == '\0' )
	{
		*pNorm = 1;
	}
	else
	{
		sscanf(pN,"%d",pNorm);
	}

	//	If a texture co-ordinate is there, then read it's value, otherwise set the default of 1
	//	(1 is used because we will decrease the value by 1 later thus giving us 0)
	if( pT == NULL || *pT == '\0' )
	{
		*pUV = 1;
	}
	else
	{
		sscanf(pT,"%d",pUV);
	}

	if(*pVert < 0)
		*pVert = -*pVert;
	if(*pNorm < 0)
		*pNorm = -*pNorm;
	if(*pUV < 0)
		*pUV = -*pUV;
}

/*
=============================================================
1- Run through the whole file looking for the various  flags
so that we can count up how many data elements  there are.
2- Load the datas as they are available or not : normals/uv

Convert all polys with vertices above 3 to triangles. The number of triangles will be (ii-2).
For example, with a poly that has vertices a,b,c,d,e; there will be 3 triangles comprised
of verts (a,b,c), (a,c,d) and (a,d,e) respectively. We need to check the amount of triangles
that will be needed for this polygon and resize the array accordingly.

tokenise the string using strtok(). Basically this splits the string up
and removes the spaces from each chunk. This way we only have to deal with
one set of indices at a time for each of the poly's vertices.

In order to start constructing our triangles that are created after ww split the poly,
we need to store som of the indices that are going to be used for the extra additional
triangles after this. Basically we store the first and last sets of indices from this
first triangle because they will be used as the first & second sets for the next triangle.

=============================================================
*/
bool OBJ::Load(const char *filename, bool center)
{
	printf("loading.. %s\n", filename);
	ObjMesh *pMesh=NULL;
	unsigned int vc=0,nc=0,tc=0,fc=0;
	char buffer[256];
	FILE *fp = NULL;
	fp = fopen(filename,"r");
	if(!fp)
	{
		return false;
	}

	pMesh = &Mesh;
	if(pMesh)
	{
		Unload();
	}

	while(!feof(fp))
	{
		memset(buffer,0,200);
		fgets(buffer,256,fp);		
		if( strncmp("mtllib",buffer,6) == 0 )		//	'mtllib ' materials file
		{
			char tplfile[128];
			sscanf(buffer, "mtllib %s", tplfile);
			LoadMaterials(pMesh, tplfile);
		}
		else if( strncmp("vn ",buffer,3) == 0 )		//	'vn' vertex normal
		{
			pMesh->m_iNumberOfNormals ++;
		}
		else if( strncmp("o ",buffer,2) == 0 )		//	'o' object
		{
			char obj[MAX_OBJECTNAME];
			memset(obj,0,255);
			sscanf((buffer+2),"%s", obj);
			strcpy(pMesh->m_aObjetArray[pMesh->m_iNumberOfObjetcs].name, obj);
			pMesh->m_iNumberOfObjetcs ++;
		}
		else if( strncmp("vt ",buffer,3) == 0 )		// 'vt' texturing co-ordinate
		{
			pMesh->m_iNumberOfTexCoords ++;
		}
		else if( strncmp("v ",buffer,2) == 0 )		// 'v ' vertex co-ordinate
		{
			pMesh->m_iNumberOfVertices ++;
		}
		else if( strncmp("f ",buffer,2) == 0 )		// 'f ' face
		{
			pMesh->m_iNumberOfFaces ++;
		}
	}
	fclose(fp);

	printf("%d vertice\n",		pMesh->m_iNumberOfVertices);
	printf("%d uvs\n",			pMesh->m_iNumberOfTexCoords);
	printf("%d normals\n",		pMesh->m_iNumberOfNormals);
	printf("%d faces\n",		pMesh->m_iNumberOfFaces);
	printf("%d materials\n",	pMesh->m_iNumberOfMaterials);
	printf("%d objects\n",		pMesh->m_iNumberOfObjetcs);
	

	pMesh->m_aVertexArray = (ObjVertex*)malloc( pMesh->m_iNumberOfVertices	* sizeof(ObjVertex)	  );
	if( pMesh->m_iNumberOfNormals > 0 )
	{
		pMesh->m_aNormalArray = (ObjNormal*)malloc( pMesh->m_iNumberOfNormals	* sizeof(ObjNormal)	  );
	}
	if( pMesh->m_iNumberOfTexCoords > 0 )
	{
		pMesh->m_aTexCoordArray = (ObjTexCoord*)malloc( pMesh->m_iNumberOfTexCoords	* sizeof(ObjTexCoord) );
	}
	pMesh->m_aFaces = (ObjFace*)malloc( pMesh->m_iNumberOfFaces	* sizeof(ObjFace) );


	char curmat[MAX_MATERIALNAME];
	memset(curmat,0,255);

	char curobj[MAX_OBJECTNAME];
	memset(curobj,0,255);

	fp = fopen(filename,"r");
	while(!feof(fp))
	{
		memset(buffer,0,255);
		fgets(buffer,256,fp);

		if( strncmp("o ",buffer,2) == 0 )		//	'o' object
		{
			sscanf((buffer+2),"%s", curobj);
		}
		else if( strncmp("vn ",buffer,3) == 0 )
		{
			sscanf((buffer+2),"%f%f%f",
							&pMesh->m_aNormalArray[ nc ].x,
							&pMesh->m_aNormalArray[ nc ].y,
							&pMesh->m_aNormalArray[ nc ].z);
			nc ++;
		}
		else if( strncmp("vt ",buffer,3) == 0 )
		{
			sscanf((buffer+2),"%f%f",
							&pMesh->m_aTexCoordArray[ tc ].u,
							&pMesh->m_aTexCoordArray[ tc ].v);
			tc ++;
		}
		else if( strncmp("v ",buffer,2) == 0 )
		{
			sscanf((buffer+1),"%f%f%f",
							&pMesh->m_aVertexArray[ vc ].x,
							&pMesh->m_aVertexArray[ vc ].y,
							&pMesh->m_aVertexArray[ vc ].z);
			vc ++;
		}
		else if( strncmp("usemtl ",buffer,7) == 0 )
		{
			sscanf((buffer+7),"%s", curmat);
		}
		else if( strncmp("f ",buffer,2) == 0 )
		{
			char *pSplitString = NULL;
			unsigned int iNewTrianglesIndex = 0,
						 iNumberOfExtraTriangles = 0;
			unsigned int aFirstIndices[3],
						 aSecondIndices[3];

			ObjFace *pf = NULL;

			// To count how many vertice per faces : 3,4 or more.
			int numberOfVertice = 0;
			unsigned int i=2;
			for(;i<strlen(buffer);i++)
			{
				if(buffer[i] == '\n')
					break;
				if(buffer[i-1] == ' ' && buffer[i] == '\n')
					break;
				if(buffer[i] == ' ')
					numberOfVertice++;
			}
			numberOfVertice ++;
			if(numberOfVertice < 3 || numberOfVertice > 4)
			{
				printf("warning: face with _%d_ vertice!!\n", numberOfVertice);
			}

			if( (numberOfVertice-2) > 1 )
			{
				iNumberOfExtraTriangles = numberOfVertice-3;
				iNewTrianglesIndex = ResizeTriangleArray(pMesh,iNumberOfExtraTriangles);
			
				for(unsigned int h=0; h<iNumberOfExtraTriangles ; h++)
				{
					pf = &pMesh->m_aFaces[ iNewTrianglesIndex+h ];
					strcpy(pf->material_name, curmat);
					strcpy(pf->object_name, curobj);
					pf->opengl_index = -1;
				}
			}

			pf = &pMesh->m_aFaces[ fc ];
			strcpy(pf->material_name, curmat);
			strcpy(pf->object_name, curobj);
			pf->opengl_index = -1;

			//ÀÓ½Ã·Îprintf(".");
			//printf("face%d has material: %s, object: %s\n", fc+1, pf->material_name, pf->object_name);

			pSplitString = strtok((buffer+2)," \t\n");
			i=0;

			//	Get the vertex structures that we know WILL exists ( 3 at least )
			for(i=0 ; i<3 ; i++)
			{
				GetIndices( pSplitString,
							&pf->m_aVertexIndices[i],
							&pf->m_aNormalIndices[i],
							&pf->m_aTexCoordIndicies[i]);

				// reduce the indices by 1 because array indices start at 0, obj starts at 1
				pf->m_aTexCoordIndicies[i] --;
				pf->m_aNormalIndices[i] --;
				pf->m_aVertexIndices[i] --;

				switch(i)
				{
					case 0:
					aFirstIndices[0] = pf->m_aVertexIndices[i];
					aFirstIndices[1] = pf->m_aNormalIndices[i];
					aFirstIndices[2] = pf->m_aTexCoordIndicies[i];
					break;

					case 2:
					aSecondIndices[0] = pf->m_aVertexIndices[i];
					aSecondIndices[1] = pf->m_aNormalIndices[i];
					aSecondIndices[2] = pf->m_aTexCoordIndicies[i];
					break;
					
					default:
					break;
				}

				pSplitString = strtok(NULL," \t\n");
			}
			fc ++;

			//	Get the additional triangles that may be there
			for(i=0 ; i<iNumberOfExtraTriangles ; i++)
			{
				pf = &pMesh->m_aFaces[ iNewTrianglesIndex + i ];

				GetIndices( pSplitString,
							&pf->m_aVertexIndices[2],
							&pf->m_aNormalIndices[2],
							&pf->m_aTexCoordIndicies[2]);

				// need to reduce the indices by 1 because array indices start at 0, obj starts at 1
				pf->m_aTexCoordIndicies[2] --;
				pf->m_aNormalIndices[2] --;
				pf->m_aVertexIndices[2] --;

				//	Now assign the other indices for the triangle
				pf->m_aVertexIndices[0]		= aFirstIndices[0];
				pf->m_aNormalIndices[0]		= aFirstIndices[1];
				pf->m_aTexCoordIndicies[0]	= aFirstIndices[2];

				pf->m_aVertexIndices[1]		= aSecondIndices[0];
				pf->m_aNormalIndices[1]		= aSecondIndices[1];
				pf->m_aTexCoordIndicies[1]	= aSecondIndices[2];

				//	Copy over the indices for the next triangle.
				aSecondIndices[0] = pf->m_aVertexIndices[2];
				aSecondIndices[1] = pf->m_aNormalIndices[2];
				aSecondIndices[2] = pf->m_aTexCoordIndicies[2];

				pSplitString = strtok(NULL," \t\n");
			}
		}
	}
	fclose(fp);
	printf("\n");

	for(unsigned int i=0 ; i<pMesh->m_iNumberOfObjetcs ; i++)
	{
		printf("<%s>\n", pMesh->m_aObjetArray[i].name);
		int nbfaces = 0;
		for(unsigned int j=0 ; j<pMesh->m_iNumberOfFaces ; j++)
		{
			if(!strcmp(pMesh->m_aFaces[j].object_name, pMesh->m_aObjetArray[i].name))
			{
				nbfaces++;
			}
		}
		printf("  %d faces\n", nbfaces);
	}

	Calbound();

	if(center == true)
	{
		CenterVertice();
	}
	return true;
}

void OBJ::Unload()
{
	//	delete the face array 
	if(Mesh.m_aFaces)
	{
		free(Mesh.m_aFaces);
		Mesh.m_aFaces = NULL;
	}

	//	delete the vertex array 
	if(Mesh.m_aVertexArray)
	{
		free(Mesh.m_aVertexArray);
		Mesh.m_aVertexArray = NULL;
	}

	//	delete the normal array
	if(Mesh.m_aNormalArray)
	{
		free(Mesh.m_aNormalArray);
		Mesh.m_aNormalArray = NULL;
	}

	//	delete the texturing co-ordinate array
	if(Mesh.m_aTexCoordArray)
	{
		free(Mesh.m_aTexCoordArray);
		Mesh.m_aTexCoordArray = NULL;
	}
}

//------------------------------------------------------------------------

void OBJ::SetPos(ObjVertex p)
{
	pos = p;
}
