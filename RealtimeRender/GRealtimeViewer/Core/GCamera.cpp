#include "stdafx.h"
#include "GCamera.h"

FLOAT
GCamera::CalcFOVFromFrustum( VOID )
{
	float b = m_fTop / 2.f;
	float a = m_fNear;
	float c = sqrtf( a * a + b * b);
	return acosf( m_fNear / c ) * 2.f;
}

VOID
GCamera::Init( char** token )
{
	*token = strtok( NULL, " " );
	sscanf( *token, "%f", &m_fNear );
	*token = strtok( NULL, " " );
	sscanf( *token, "%f", &m_fFar );
	*token = strtok( NULL, " " );
	sscanf( *token, "%f", &m_fLeft );
	*token = strtok( NULL, " " );
	sscanf( *token, "%f", &m_fRight );
	*token = strtok( NULL, " " );
	sscanf( *token, "%f", &m_fBottom );
	*token = strtok( NULL, " " );	
	sscanf( *token, "%f", &m_fTop );
	*token = strtok( NULL, " " );
	sscanf( *token, "%f", &m_fFOV );
	*token = strtok( NULL, " " );
}

VOID GCamera::Setup( char** token, UINT iRenderWidth, UINT iRenderHeight )
{
	D3DXVECTOR3 vEye, vDir, vUp;
	
	*token = strtok( NULL, " " );
	sscanf( *token, "%f", &m_stCameraInfo.fFOV );

	*token = strtok( NULL, " " );
	sscanf( *token, "%f", &m_stCameraInfo.fLeft );

	*token = strtok( NULL, " " );
	sscanf( *token, "%f", &m_stCameraInfo.fRight );

	*token = strtok( NULL, " " );
	sscanf( *token, "%f", &m_stCameraInfo.fBottom );

	*token = strtok( NULL, " " );
	sscanf( *token, "%f", &m_stCameraInfo.fTop );

	*token = strtok( NULL, " " );
	sscanf( *token, "%f", &m_stCameraInfo.fNear );

	*token = strtok( NULL, " " );
	sscanf( *token, "%f", &m_stCameraInfo.fFar );
	
	for( int i=0; i < 3; i++ )
	{
		*token = strtok( NULL, " " );
		sscanf( *token, "%f", &m_stCameraInfo.vEye[i] );
	}
	for( int i=0; i < 3; i++ )
	{
		*token = strtok( NULL, " " );
		sscanf( *token, "%f", &m_stCameraInfo.vDir[i] );
	}
	for( int i=0; i < 3; i++ )
	{
		*token = strtok( NULL, " " );
		sscanf( *token, "%f", &m_stCameraInfo.vUp[i] );
	}

	/*
	D3DXVECTOR3 vEyePos, vDirection, vUpVector;
	vEyePos = D3DXVECTOR3( vEye[0], vEye[2], vEye[1] );
	vDirection = D3DXVECTOR3( vDir[0],vDir[2],vDir[1] );
	vUpVector = D3DXVECTOR3( vUp[0],vUp[2],vUp[1] );

	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );
	SetWorldMatrix( matWorld );

	D3DXMATRIX matView;
	D3DXMatrixLookAtLH( &matView, &vEyePos, &D3DXVECTOR3(vDirection), &vUpVector );
	SetViewMatrix( matView );

	SetProjParams( fov, (FLOAT)iRenderWidth / (FLOAT)iRenderHeight, m_fNear, m_fFar );		
	*/

	D3DXVECTOR3 vEyePos, vDirection, vUpVector;
	vEyePos = D3DXVECTOR3( -m_stCameraInfo.vEye[0], -m_stCameraInfo.vEye[2], m_stCameraInfo.vEye[1] );
	vDirection = D3DXVECTOR3( -m_stCameraInfo.vDir[0], -m_stCameraInfo.vDir[2], m_stCameraInfo.vDir[1] );
	vUpVector = D3DXVECTOR3( -m_stCameraInfo.vUp[0], -m_stCameraInfo.vUp[2], m_stCameraInfo.vUp[1] );

	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );
	SetWorldMatrix( matWorld );

	D3DXMATRIX matView, matProj;
	D3DXMatrixLookAtLH( &matView, &vEyePos, &D3DXVECTOR3(vDirection), &vUpVector );
	SetViewMatrix( matView );

	D3DXMatrixPerspectiveOffCenterLH( &matProj, m_stCameraInfo.fLeft, m_stCameraInfo.fRight, m_stCameraInfo.fBottom, m_stCameraInfo.fTop, m_stCameraInfo.fNear, m_stCameraInfo.fFar );
	SetProjMatrix( matProj );
	
	SetRotateButtons(TRUE,TRUE,TRUE,TRUE);
	SetEnablePositionMovement(TRUE);
	SetEnableYAxisMovement(TRUE);
}

VOID
GCamera::SetWorldInv( char** token )
{
	*token = strtok( NULL, " " );
	for(int i = 0; i < 16; i++)
	{
		sscanf( *token, "%f", m_matWorldInv[i] );
		*token = strtok( NULL, " " );
	}			
}










