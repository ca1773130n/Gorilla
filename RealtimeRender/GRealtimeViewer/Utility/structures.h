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

#ifndef STRUCTS_INC
#define STRUCTS_INC

#define MAX_MATERIALS		255
#define MAX_MATERIALNAME	255

#define MAX_OBJETS			255
#define	MAX_OBJECTNAME		255

typedef struct
{
	float x,
		  y,
		  z;
} ObjVertex;

typedef struct
{
	float x,
		  y,
		  z;
} ObjNormal;

typedef struct
{
	float u,
		  v;
} ObjTexCoord;

typedef struct
{
	BOOL	valid;
	char	name[MAX_MATERIALNAME];
	char	img_path[256];
	int		opengl_index;
} ObjMaterial;

typedef struct
{
	char			material_name[MAX_MATERIALNAME];
	char			object_name[MAX_OBJECTNAME];
	int				opengl_index;
	int				m_aVertexIndices[3],	//	array of indicies that reference the vertex array in the mesh	
					m_aNormalIndices[3],	//	array of indicies that reference the normal array in the mesh	
					m_aTexCoordIndicies[3];	//	array of indicies that reference the uv coordinate array in the mesh	
} ObjFace;

typedef struct
{
	char	name[MAX_OBJECTNAME];
} ObjObject;

typedef struct _ObjMesh
{
	ObjMaterial		m_aMaterialArray[MAX_MATERIALS];
	ObjObject		m_aObjetArray[MAX_OBJETS];
	ObjVertex		*m_aVertexArray;		//	Array of vertices that make up this mesh	
	ObjNormal		*m_aNormalArray;		//	Array of normals that make up this mesh		
	ObjTexCoord		*m_aTexCoordArray;		//	Array of texturing co-ordinates that make up this mesh 
	ObjFace			*m_aFaces;				//	Array of faces that make up this mesh

	unsigned int	m_iNumberOfMaterials,
					m_iNumberOfObjetcs,
					m_iNumberOfVertices,
					m_iNumberOfNormals,
					m_iNumberOfTexCoords,
					m_iNumberOfFaces;
} ObjMesh;


#endif // STRUCTS_INC
