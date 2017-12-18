#include "stdafx.h"
#include "GMesh.h"
#include "dynload.h"
#include "shape.h"
#include "point.h"
#include "coloring.h"
#include "matrix4x4.h"

GMeshSubset::GMeshSubset( Material* pMat, CGMaterial* pGMat )
{
	pMaterial = pMat;
	pGMaterial = pGMat;

	m_numVerts = m_numIndices = 0;
}

VOID
GMeshSubset::Add( CGMesh* pMesh )
{
	m_numVerts += pMesh->GetNumVerts();
	m_numIndices += pMesh->GetNumIndices();
	pMeshes.push_back( pMesh );
}

VOID
GMeshSubset::Draw( ID3D10Device* pDev )
{	
	UINT offset = 0;
	UINT stride = sizeof(GMESHVERTEX);
	pDev->IASetVertexBuffers( 0, 1, &m_pVB, &stride, &offset );	
	pDev->IASetIndexBuffer( m_pIB, DXGI_FORMAT_R32_UINT, 0 );

	/*
	for( int i=0; i < pMeshes.size(); i++ )
	{
		pDev->DrawIndexed( pMeshes[i]->GetNumIndices(), pMeshes[i]->GetIOffset(), 0 );	
	}
	*/
	pDev->DrawIndexed( m_numIndices, 0, 0 );	
}

VOID GMeshSubset::SetBuffers( ID3D10Device* pDev, D3DXVECTOR3& vBoundMin, D3DXVECTOR3& vBoundMax, D3DXVECTOR3& vBoundCtr )
{
	HRESULT hr;

	GMESHVERTEX* pVertices = new GMESHVERTEX[ m_numVerts ];
	UINT *pIndices = new UINT[ m_numIndices ];	
	
	D3DXVECTOR3* tan1 = new D3DXVECTOR3[ m_numVerts ];
	for( int j=0; j < m_numVerts; j++ )
		tan1[j].x = tan1[j].y  = tan1[j].z = 0;

	WaldTriangleMesh* pGIRLMesh = 0;
	Point* pVerts;
	int* pInds;
	Normal* pNormals;
	float* pUVs;
	Point P;
	Normal N;
	

	UINT numV = 0, numF = 0, vOffset = 0, iOffset = 0;
	for( int i=0; i < pMeshes.size(); i++ )
	{
		pGIRLMesh = pMeshes[i]->GetGIRLMesh();
		Point* pVerts = pGIRLMesh->GetPoints();
		int* pInds = pGIRLMesh->GetIndices();
		Normal* pNormals = pGIRLMesh->GetNormals();
		float* pUVs = pGIRLMesh->GetUVs();
		
		numV = pGIRLMesh->GetNumVertices();
		numF = pGIRLMesh->GetNumFaces();
		
		//ZeroMemory( tan1, sizeof(D3DXVECTOR3) * m_numVerts * 2 );

		for( UINT j=0; j < numV; j++ )
		{
			P = pVerts[j];
			N = pNormals[j];
			//P = pGIRLMesh->WorldToObject( P );
			N = Normalize( pGIRLMesh->ObjectToWorld(N) );
			pVertices[vOffset + j].pos = D3DXVECTOR3( -P.x, P.y, P.z ) * 100;
			pVertices[vOffset + j].normal = D3DXVECTOR3( -N.x, N.y, N.z );
			pVertices[vOffset + j].texcoord = pUVs ? D3DXVECTOR2( pUVs[2 * j], 1.f - pUVs[2 * j + 1] ) : D3DXVECTOR2( 0.5, 0.5 );
			while( pVertices[vOffset + j].texcoord.x > 1 )
				pVertices[vOffset + j].texcoord.x -= 1;
			while( pVertices[vOffset + j].texcoord.x < 0 )
				pVertices[vOffset + j].texcoord.x += 1;
			while( pVertices[vOffset + j].texcoord.y > 1 )
				pVertices[vOffset + j].texcoord.y -= 1;
			while( pVertices[vOffset + j].texcoord.y < 0 )
				pVertices[vOffset + j].texcoord.y += 1;


			if( pVertices[vOffset + j].pos > vBoundMax ) 
				vBoundMax = pVertices[vOffset + j].pos;
			if( pVertices[vOffset + j].pos < vBoundMin ) 
				vBoundMin = pVertices[vOffset + j].pos;
			vBoundCtr += pVertices[vOffset + j].pos;
		}

		for( UINT j=0; j < numF * 3; j++ )
			pIndices[iOffset + j] = vOffset + pInds[j];

		for( UINT j=0; j < numF * 3; j += 3 )
		{
			UINT i1 = pIndices[ iOffset + j  ];
			UINT i2 = pIndices[ iOffset + j + 1 ];
			UINT i3 = pIndices[ iOffset + j + 2 ];
                                                                                                                                                                                                                                                                                                                                                      
			const GMESHVERTEX& v1 = pVertices[i1];
			const GMESHVERTEX& v2 = pVertices[i2];
			const GMESHVERTEX& v3 = pVertices[i3];

			float x1 = v2.pos.x - v1.pos.x;
			float x2 = v3.pos.x - v1.pos.x;
			float y1 = v2.pos.y - v1.pos.y;
			float y2 = v3.pos.y - v1.pos.y;
			float z1 = v2.pos.z - v1.pos.z;
			float z2 = v3.pos.z - v1.pos.z;

			float s1 = v2.texcoord.x - v1.texcoord.x;
			float s2 = v3.texcoord.x - v1.texcoord.x;
			float t1 = v2.texcoord.y - v1.texcoord.y;
			float t2 = v3.texcoord.y - v1.texcoord.y;

			float r = 1.0F / (s1 * t2 - s2 * t1);
			D3DXVECTOR3 sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r);
			D3DXVECTOR3 tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r, (s1 * z2 - s2 * z1) * r);

			tan1[i1] += sdir;
	 		tan1[i2] += sdir;
			tan1[i3] += sdir;
		}

		for( UINT j=0; j < numV; j++ )
		{
			const D3DXVECTOR3& n = pVertices[vOffset + j].normal;
			const D3DXVECTOR3& t = tan1[vOffset + j];
			D3DXVec3Normalize( &pVertices[vOffset + j].tangent, &D3DXVECTOR3(t - n * D3DXVec3Dot(&n, &t)) );
		}

		pMeshes[i]->SetVOffset( vOffset );
		pMeshes[i]->SetIOffset( iOffset );

		vOffset += numV;
		iOffset += numF * 3;	

		delete pGIRLMesh;
	}

	D3D10_BUFFER_DESC descBuffer;
	descBuffer.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	descBuffer.Usage = D3D10_USAGE_DEFAULT;
	descBuffer.MiscFlags = 0;
	descBuffer.CPUAccessFlags = 0;
	descBuffer.ByteWidth = sizeof(GMESHVERTEX) * m_numVerts;

	D3D10_SUBRESOURCE_DATA resData;
	resData.pSysMem = pVertices;
	resData.SysMemPitch = 0;
	resData.SysMemSlicePitch = 0;

	V( pDev->CreateBuffer(&descBuffer, &resData, &m_pVB) );

	descBuffer.BindFlags = D3D10_BIND_INDEX_BUFFER;
	descBuffer.ByteWidth = sizeof(UINT) * m_numIndices;
	resData.pSysMem = pIndices;

	V( pDev->CreateBuffer(&descBuffer, &resData, &m_pIB) );

	delete[] pVertices;
	delete[] pIndices;
	delete[] tan1;
}

CGMesh::CGMesh( Shape* pShape, map<string, GMeshSubset>& Subsets )
{
	HRESULT hr;
	ID3D10Device* pd3dDevice = DXUTGetD3D10Device();
	m_pShape = (WaldTriangleMesh*)pShape;

	m_dwNumVertices = m_pShape->GetNumVertices();
	m_dwNumIndices = m_pShape->GetNumFaces() * 3;

	//GMESHVERTEX* pVertices = new GMESHVERTEX[ m_dwNumVertices ];
	//UINT *pIndices = new UINT[ m_dwNumIndices ];	
	
	Point* pVerts = m_pShape->GetPoints();
	int* pInds = m_pShape->GetIndices();
	Normal* pNormals = m_pShape->GetNormals();
	float* pUVs = m_pShape->GetUVs();

	Matrix4x4* pMatObject = pShape->ObjectToWorld.GetMatrix().get();
	Point P;
	Normal N;
	for( int i=0; i < 4; i++ )
		for( int j=0; j < 4; j++ )
			m4x4[i * 4 + j] = pMatObject->m[i][j];
	m_matObjectToWorld = D3DXMATRIX( m4x4 );

	D3DXVECTOR4 vP, vN, vp, vn;
	/*
	BYTE rgb[3];
	for( DWORD i=0; i < m_dwNumVertices; i++ )
	{
		//P = pShape->WorldToObject( pVerts[i] );
		//N = pShape->WorldToObject( pNormals[i] );
		//N = Normalize(N);
		//pVertices[i].pos = D3DXVECTOR3( P.x, P.y, -P.z );
		//pVertices[i].normal = D3DXVECTOR3( N.x, N.y, -N.z );
		pVertices[vOffset + i].pos = D3DXVECTOR3( pVerts[i].x, pVerts[i].y, pVerts[i].z );
		pVertices[vOffset + i].normal = D3DXVECTOR3( pNormals[i].x, pNormals[i].y, pNormals[i].z );
		//pVertices[i].pos = D3DXVECTOR3(vp.x, vp.y, -vp.z);
		//pVertices[i].normal = D3DXVECTOR3(vn.x, vn.y, -vn.z);

		pVertices[vOffset + i].texcoord = D3DXVECTOR2( pUVs[2 * i], pUVs[2 * i + 1] );
	}

	for( DWORD i=0; i < m_dwNumIndices; i++ )
		pIndices[iOffset + i] = pInds[i];

	vOffset += m_dwNumVertices;
	iOffset += m_dwNumIndices;
	*/

	Material* pMaterial = m_pShape->GetMaterial().get();	
	map<string, GMeshSubset>::iterator iter = Subsets.find(pMaterial->matname);
	if( iter == Subsets.end() )
	{
		m_pGMaterial = new CGMaterial( (GIRLMaterial*)pMaterial );
		GMeshSubset newSubset( pMaterial, m_pGMaterial );
		newSubset.Add( this );
		pair<string, GMeshSubset> newPair( pMaterial->matname, newSubset );
		Subsets.insert( newPair );
	}
	else
	{
		GMeshSubset* pSubset = (GMeshSubset*)&iter->second;
		m_pGMaterial = pSubset->pGMaterial;
		pSubset->Add( this );
	}

	switch( m_pGMaterial->GetGIRLMaterial()->GetGIRLMaterialType() )
	{
	case GIRL_MATERIAL_BRDF_LAMBERT:
		m_pGMaterial->SetupLambert();
		break;
	case GIRL_MATERIAL_BRDF_BLINN:
		m_pGMaterial->SetupBlinn();
		break;
	case GIRL_MATERIAL_BRDF_PHONG:
		m_pGMaterial->SetupPhong();
		break;
	}
}

SGTexture&
CGMesh::GetTexture( UINT iPass )
{
	switch( iPass )
	{
	case 0:
		return m_pGMaterial->m_AttrColor.texture;
	case 1:
		return m_pGMaterial->m_AttrAmbientColor.texture;
	case 2:
		return m_pGMaterial->m_AttrIncandescence.texture;
	case 3:
		return m_pGMaterial->m_AttrTransparency.texture;
	case 4:
		return m_pGMaterial->m_AttrSpecularColor.texture;
	case 5:
		return m_pGMaterial->m_AttrSpecularAttr1.texture;
	case 6:
		return m_pGMaterial->m_AttrSpecularAttr2.texture;
	case 7:
		return m_pGMaterial->m_AttrCommon1.texture;
	case 8:
		return m_pGMaterial->m_AttrCommon2.texture;
	case 9:
		return m_pGMaterial->m_AttrCommon3.texture;
	case 10:
		return m_pGMaterial->m_AttrCommon4.texture;
	}
}

VOID
CGMesh::Draw( ID3D10Device* pd3dDevice )
{
	/*
	pd3dDevice->SetTexture( 0, m_pGMaterial->m_AttrAmbientColor.texture.pTex );
	pd3dDevice->SetTexture( 1, m_pGMaterial->m_AttrColor.texture.pTex );
	pd3dDevice->SetTexture( 2, m_pGMaterial->m_AttrSpecularColor.texture.pTex );
	pd3dDevice->SetTexture( 3, m_pGMaterial->m_AttrSpecularAttr1.texture.pTex );
	pd3dDevice->SetTexture( 4, m_pGMaterial->m_AttrSpecularAttr2.texture.pTex );
	pd3dDevice->SetTexture( 5, m_pGMaterial->m_AttrTransparency.texture.pTex );
	pd3dDevice->SetTexture( 6, m_pGMaterial->m_AttrIncandescence.texture.pTex );
	pd3dDevice->SetTexture( 7, m_pGMaterial->m_AttrCommon1.texture.pTex );
	pd3dDevice->SetTexture( 8, m_pGMaterial->m_AttrCommon2.texture.pTex );
	pd3dDevice->SetTexture( 9, m_pGMaterial->m_AttrCommon3.texture.pTex );
	pd3dDevice->SetTexture( 10, m_pGMaterial->m_AttrCommon4.texture.pTex );
	*/

	/*
	pd3dDevice->SetStreamSource( 0, m_pVB, 0, sizeof(GMESHVERTEX) );
	pd3dDevice->SetFVF( D3DFVF_GMESHVERTEX );
	pd3dDevice->SetIndices( m_pIB );
	
	SetTRS( pd3dDevice );
	*/
	UINT offset  = 0;
	UINT stride = sizeof(GMESHVERTEX);
	//pd3dDevice->IASetVertexBuffers( 0, 1, &m_pVB, &stride, &offset );
	//pd3dDevice->IASetIndexBuffer( m_pIB, DXGI_FORMAT_R32_UINT, 0 );
	
	pd3dDevice->DrawIndexed( m_dwNumIndices, m_iIndexStart, 0 );
}

VOID
CGMesh::SetTRS( LPDIRECT3DDEVICE9 pd3dDevice )
{
	Matrix4x4 mat = *m_pShape->WorldToObject.GetMatrix().get();
	D3DXMATRIX matModel, matOld;
	matModel._11 = mat.m[0][0];
	matModel._12 = mat.m[0][1];
	matModel._13 = mat.m[0][2];
	matModel._14 = mat.m[0][3];
	matModel._21 = mat.m[1][0];
	matModel._22 = mat.m[1][1];
	matModel._23 = mat.m[1][2];
	matModel._24 = mat.m[1][3];
	matModel._31 = mat.m[2][0];
	matModel._32 = mat.m[2][1];
	matModel._33 = mat.m[2][2];
	matModel._34 = mat.m[2][3];
	matModel._41 = mat.m[3][0];
	matModel._42 = mat.m[3][1];
	matModel._43 = mat.m[3][2];
	matModel._44 = mat.m[3][3];

	pd3dDevice->GetTransform( D3DTS_WORLD, &matOld );
	matOld *= matModel;

	pd3dDevice->SetTransform( D3DTS_WORLD, &matModel );
}

CGGeometry::CGGeometry()
{
	m_vBoundMin.x = 0;
	m_vBoundMin.y = 0;
	m_vBoundMin.z = 0;
	m_vBoundMax.x = 0;
	m_vBoundMax.y = 0;
	m_vBoundMax.z = 0;
	m_vBoundCtr.x = 0;
	m_vBoundCtr.y = 0;
	m_vBoundCtr.z = 0;

	m_iNumVerts = 0;
	m_iNumFaces = 0;
}

CGGeometry::~CGGeometry()
{

}

HRESULT
CGGeometry::LoadFromGIRL( ID3D10Device* pDev, ID3D10EffectTechnique* pTechnique )
{	
	HRESULT hr = S_OK;

	map<string, boost::shared_ptr<Shape> > mapShapes = DynamicLoader::registeredShapesPointer();
	map<string, boost::shared_ptr<Material> > mapMaterials = DynamicLoader::registeredMaterialsPointer();
	m_Meshes.reserve( mapShapes.size() );

	Material* pMaterial = 0;
	Shape* pShape = 0;
	UINT i = 0;
	map<string, boost::shared_ptr<Shape> >::iterator iter;
	map<Material*, GMeshSubset>::iterator iterSubset;

	/*
	// 총 vertex index 수 카운트
	UINT nVert = 0, nFace = 0;
	for( iter = mapShapes.begin(); iter != mapShapes.end(); iter++, i++ )
	{
		nVert += ((WaldTriangleMesh*)iter->second.get())->GetNumVertices();
		nFace += ((WaldTriangleMesh*)iter->second.get())->GetNumFaces();
	}

	// 버퍼 생성
	GMESHVERTEX* pVertices = new GMESHVERTEX[ nVert ];
	UINT *pIndices = new UINT[ nFace * 3 ];	
	*/

	UINT vOffset = 0, iOffset = 0;
	for( iter = mapShapes.begin(); iter != mapShapes.end(); iter++, i++ )
	{
		pShape = iter->second.get();
		CGMesh* pNewMesh = new CGMesh( pShape, m_Subsets );
		m_Meshes.push_back( *pNewMesh );
	}

	// 데이터 넣음	
	for( map<string, GMeshSubset>::iterator it = m_Subsets.begin(); it != m_Subsets.end(); it++ )
	{
		it->second.SetBuffers( pDev, m_vBoundMin, m_vBoundMax, m_vBoundCtr );
		m_iNumVerts += it->second.m_numVerts;
		m_iNumFaces += it->second.m_numIndices / 3.f;
	}
	m_vBoundCtr /= m_iNumVerts;

	return hr;
}

VOID
CGGeometry::Draw( ID3D10Device* pd3dDevice, GShader* pShader, ID3D10EffectTechnique* pTechnique, UINT p )
{
	UINT nSubset = m_Subsets.size();
	UINT nMeshes = m_Meshes.size();

	D3DXMATRIX mWorldInvTrans;
	D3DXMatrixIdentity( &mWorldInvTrans );
	
	map<string, GMeshSubset>::iterator iter;
	GMeshSubset* pSubset = 0;

	// DWT
	UINT numLevels = 8;

	for( iter = m_Subsets.begin(); iter != m_Subsets.end(); iter++ )
	{		
		pSubset = (GMeshSubset*)&iter->second;
		UINT nSubSetMesh = pSubset->pMeshes.size();

		



		pShader->SetQuadTexSRV( pSubset->pGMaterial );
		pTechnique->GetPassByIndex( p )->Apply( 0 );			

		pSubset->Draw( pd3dDevice );
		/*
		for( UINT j=0; j < nSubSetMesh; j++ )
		{
			pShader->SetWorldMatrix( pSubset->pMeshes[j]->m4x4 );
			D3DXMatrixInverse( &mWorldInvTrans, 0, pSubset->pMeshes[j]->GetMatrix() );
			D3DXMatrixTranspose( &mWorldInvTrans, &mWorldInvTrans );
			pShader->SetWorldInvTransMatrix( &mWorldInvTrans );
			pSubset->pMeshes[j]->Draw( pd3dDevice );
		}		
		*/
	}
}

VOID CGGeometry::DrawSimple( ID3D10Device* pd3dDevice, ID3D10EffectTechnique* pTechnique )
{
	UINT nSubset = m_Subsets.size();
	UINT nMeshes = m_Meshes.size();

	map<string, GMeshSubset>::iterator iter;
	GMeshSubset* pSubset = 0;

	pTechnique->GetPassByIndex( 0 )->Apply( 0 );			

	for( iter = m_Subsets.begin(); iter != m_Subsets.end(); iter++ )
	{		
		pSubset = (GMeshSubset*)&iter->second;
		UINT nSubSetMesh = pSubset->pMeshes.size();
		
		pSubset->Draw( pd3dDevice );
		/*
		for( UINT j=0; j < nSubSetMesh; j++ )
		{
		pShader->SetWorldMatrix( pSubset->pMeshes[j]->m4x4 );
		D3DXMatrixInverse( &mWorldInvTrans, 0, pSubset->pMeshes[j]->GetMatrix() );
		D3DXMatrixTranspose( &mWorldInvTrans, &mWorldInvTrans );
		pShader->SetWorldInvTransMatrix( &mWorldInvTrans );
		pSubset->pMeshes[j]->Draw( pd3dDevice );
		}		
		*/
	}
}

/*
VOID CGGeometry::DrawDWT( ID3D10Device* pd3dDevice, GShader& shader, ID3D10EffectTechnique* pTechnique )
{
	UINT nSubset = m_Subsets.size();
	UINT nMeshes = m_Meshes.size();

	map<string, GMeshSubset>::iterator iter;
	GMeshSubset* pSubset = 0;

	CGAttribute attr;
	ID3D10Texture2D* pTex;
	for( iter = m_Subsets.begin(); iter != m_Subsets.end(); iter++ )
	{		
		pSubset = (GMeshSubset*)&iter->second;
		UINT nSubSetMesh = pSubset->pMeshes.size();

		for( int j=0; j < 8; j++ )
		{
			attr = pSubset->pGMaterial->GetAttribute( j );
			if( !attr.IsConstant() )
			{
				pTex = attr.texture.pTex;
				
				ID3D10EffectTechnique* pTechnique = shader.GetTechnique( "FastWaveletAnalysis" );		
				D3D10_TECHNIQUE_DESC techDesc;
				pTechnique->GetDesc( &techDesc );

				pd3dDevice->OMSetRenderTargets( 2, m_pRTVDWTThin[i], NULL );	
				for( int j = 0; j < 2; j++ )
					m_pd3dDevice->ClearRenderTargetView( m_pRTVDWTThin[i][j], ClearColor );


				pTechnique->GetPassByIndex( 0 )->Apply( 0 );			

				// draw
				

				// thresholding and pack the value and index

				// delete original texture
			}
		}
	}
}
*/

VOID
CGGeometry::BuildClusters( VOID )
{
	const int iNumClusters = 1600;
	
}