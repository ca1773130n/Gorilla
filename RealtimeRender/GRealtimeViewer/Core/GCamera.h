#pragma once

#include "stdafx.h"
#include "DXUTCamera.h"
#include "vector.h"

struct GCameraPacket
{
	FLOAT			fFOV;
	FLOAT	 		fNear;
	FLOAT			fFar;
	FLOAT			fLeft;
	FLOAT			fRight;
	FLOAT			fBottom;
	FLOAT			fTop;
	FLOAT			vEye[3];
	FLOAT			vDir[3];
	FLOAT			vUp[3];
};

class GCamera : public CFirstPersonCamera
{
public:

	CString			m_lpszName;

	GCameraPacket	m_stCameraInfo;
	FLOAT	 		m_fNear;
	FLOAT			m_fFar;
	FLOAT			m_fLeft;
	FLOAT			m_fRight;
	FLOAT			m_fBottom;
	FLOAT			m_fTop;
	FLOAT			m_fFOV;

	D3DXMATRIX		m_matWorldInv;
	D3DXMATRIX		m_matPers;
	D3DXMATRIX		m_matPersInv;
	D3DXMATRIX		m_matView;
	D3DXMATRIX		m_matViewInv;

public:

	GCamera() { m_fFar = 0; }

	FLOAT			CalcFOVFromFrustum( VOID );

	GCameraPacket&	GetCamInfo( VOID ) { return m_stCameraInfo; }

	VOID			Init( char** token );

	VOID			Setup( char** token, UINT iRenderWidth, UINT iRenderHeight );

	VOID			SetWorldInv( char** token );

	VOID			SetName( LPCWSTR szName ) { m_lpszName.SetString(szName); }
};
