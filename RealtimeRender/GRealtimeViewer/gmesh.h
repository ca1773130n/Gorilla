#pragma once

#include "stdafx.h"
#include "GMaterial.h"
#include "GShader.h"
#include "waldtrianglemesh.h"

#include <map>
#include <vector>
using namespace std;

struct GMESHVERTEX
{
	D3DXVECTOR3 pos;	
	D3DXVECTOR3 normal;
	D3DXVECTOR3 tangent;
	D3DXVECTOR2 texcoord;
};
#define D3DFVF_GMESHVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX0)

class CGMesh;
class GMeshSubset
{
public:
	GMeshSubset( Material* pMat, CGMaterial* pGMat );

	VOID						Add( CGMesh* pMesh );

	ID3D10Buffer*				GetVB( VOID ) { return m_pVB; }

	ID3D10Buffer*				GetIB( VOID ) { return m_pIB; }

	VOID						Draw( ID3D10Device* pDev );

	VOID SetBuffers( ID3D10Device* pDev, D3DXVECTOR3& vBoundMin, D3DXVECTOR3& vBoundMax, D3DXVECTOR3& vBoundCtr );

public:	

	Material*					pMaterial;
	CGMaterial*					pGMaterial;
	vector<CGMesh*>				pMeshes;	

	ID3D10Buffer*				m_pVB;
	ID3D10Buffer*				m_pIB;

	UINT						m_numVerts;
	UINT						m_numIndices;
};

class CGMesh
{
public:
	CGMesh::CGMesh( Shape* pShape, map<string, GMeshSubset>& Subsets );

	VOID									Draw( ID3D10Device* pd3dDevice );

	WaldTriangleMesh*						GetGIRLMesh( VOID ) { return m_pShape; }

	UINT									GetNumVerts( VOID ) { return m_dwNumVertices; }

	UINT									GetNumIndices( VOID ) { return m_dwNumIndices; }

	UINT									GetVOffset( VOID ) { return m_iVertStart; }

	UINT									GetIOffset( VOID ) { return m_iIndexStart; }

	D3DXMATRIX*								GetMatrix( VOID ) { return &m_matObjectToWorld; }

	CGMaterial*								GetMaterial( VOID ) { return m_pGMaterial; }
	
	SGTexture&								GetTexture( UINT iPass );

	VOID									SetVOffset( UINT vOffset ) { m_iVertStart = vOffset; }

	VOID									SetIOffset( UINT iOffset ) { m_iIndexStart = iOffset; }

	VOID									SetTRS( LPDIRECT3DDEVICE9 pd3dDevice );

	float									m4x4[16];
protected:
	UINT									m_iMaterialIndex;
	WaldTriangleMesh*						m_pShape;
	CGMaterial*								m_pGMaterial;

	DWORD									m_dwNumVertices;
	DWORD									m_dwNumIndices;

	D3DXMATRIX								m_matObjectToWorld;

	UINT									m_iVertStart;
	UINT									m_iVertSize;
	UINT									m_iIndexStart;
	UINT									m_iIndexSize;
};

class CGGeometry
{
public:
	CGGeometry();
	~CGGeometry();

	D3DXVECTOR3&				GetBoundMax( VOID ) { return m_vBoundMax; }

	D3DXVECTOR3&				GetBoundCtr( VOID ) { return m_vBoundCtr; }

	UINT						GetNumSubsets( VOID ) { return m_Subsets.size(); }

	map<string, GMeshSubset>* 	GetSubsets( VOID ) { return &m_Subsets; }

	HRESULT						LoadFromGIRL( ID3D10Device* pDev, ID3D10EffectTechnique* pTechnique );

	VOID						Draw( ID3D10Device* pd3dDevice, GShader* pShader, ID3D10EffectTechnique* pTechnique, UINT p );

	VOID						DrawSimple( ID3D10Device* pd3dDevice, ID3D10EffectTechnique* pTechnique );

	VOID						DrawDWT( ID3D10Device* pd3dDevice, ID3D10EffectTechnique* pTechnique );

	VOID						Draw( VOID );

	VOID						BuildClusters( VOID );

protected:
	
	map<string, GMeshSubset>	m_Subsets;
	vector<CGMesh>				m_Meshes;
	ID3D10InputLayout*			m_pInputLayout;

	D3DXVECTOR3					m_vBoundMin;
	D3DXVECTOR3					m_vBoundMax;
	D3DXVECTOR3					m_vBoundCtr;

	ULONG						m_iNumVerts;
	ULONG						m_iNumFaces;
};

