#include "stdafx.h"
#include "GLight.h"
#include "coloring.h"
#include "corona.h"
#include "ou_thread.h"
#include "girl.h"

using namespace std;
using namespace openutils;

extern CRITICAL_SECTION g_cs;

GLight::GLight( VOID )
{

}

GLight::GLight( char** token, int type ) 
{
	bNeedDepthRendering			= TRUE;
	bNeedLinkUpdate				= FALSE;
	bLinkInfoInclusive			= TRUE;
	bLinkInfoAll				= FALSE;

	bDirty						= FALSE;
	bInBaseTex					= TRUE;

	iType = type;
	//enShadowMaskTex = 0;
	//enLinkMaskTex = 0;

	// 라이트 이름
	szName.assign( *token );

	// 키프레임 갖는지 여부
	int bKey = 0, selected = 0;
	*token = strtok( NULL," " );
	sscanf( *token,"%d", &bKey );					
	bHasKey = (bKey == 1) ? true : FALSE;

	// 선택되었는지 여부
	*token = strtok( NULL," " );
	sscanf( *token,"%d", &selected );					
	bSelected = (selected == 1) ? TRUE : FALSE;

	// 라이트 컬러
	*token = strtok( NULL," " );
	for(int i = 0; i < 3; i++)
	{
		sscanf( *token, "%f", &vColor[i] );
		*token = strtok( NULL, " " );
	}

	// intensity
	sscanf( *token, "%f", &fIntensity );
	*token = strtok( NULL, " " );	

	// shadow color
	vShadowColor = D3DXVECTOR3( 0, 0, 0 );

	pShadowMaskTex = NULL;
	pLinkMaskTex = NULL;

	pSoftShadowMap = NULL;
}

GLight::~GLight( VOID )
{
	//if(glIsTexture(enLinkMaskTex))		glDeleteTextures(1, &enLinkMaskTex);	
	//if(glIsTexture(enShadowMaskTex))	glDeleteTextures(1, &enShadowMaskTex);	
}

/*
VOID
GLight::SetLinkMaskTex( GLenum tex )
{
	enLinkMaskTex = tex;
}
*/

VOID GLight::Setup( const D3DXMATRIX& matViewInv, UINT& iLightFlag )
{
	switch( iType )
	{
	case MAYA_AMBIENT:
		((GAmbientLight*)this)->Setup( matViewInv );
		break;
	case MAYA_POINT:
		((GPointLight*)this)->Setup( matViewInv );
		break;
	case MAYA_DIRECTIONAL:
		((GDirectionalLight*)this)->Setup( matViewInv );
		break;
	case MAYA_SPOT:
		((GSpotLight*)this)->Setup( matViewInv );
		break;
	case MAYA_AREA :
		break;
	case MAYA_VOLUME :
		break;
	}

	iLightFlag = iType;

	if( bHasShadow ) SetNeedDepthRendering( TRUE );
}

VOID
GLight::Modify( char** token )
{

	// 키프레임 갖는지 여부
	*token = strtok( NULL, " " );
	sscanf( *token, "%d", &bHasKey );

	// 선택되었는지 여부
	*token = strtok( NULL," " );
	sscanf( *token,"%d", &bSelected );					

	*token = strtok( NULL, " " );
	sscanf( *token, "%d", &iType );

	*token = strtok( NULL, " " );
	for(int i = 0; i < 3; i++)
	{
		sscanf( *token, "%f", &vColor[i] );
		*token = strtok( NULL, " " );
	}

	sscanf( *token, "%f", &fIntensity );

	*token = strtok(NULL," ");
	switch( iType )
	{
	case MAYA_AMBIENT : 
		((GAmbientLight*)this)->Modify( token );		
		break;
	case MAYA_DIRECTIONAL :
		((GDirectionalLight*)this)->Modify( token );		
		break;
	case MAYA_POINT : 
		((GPointLight*)this)->Modify( token );
		break;
	case MAYA_SPOT :
		((GSpotLight*)this)->Modify( token );
		break;
	case MAYA_AREA :
		//emit diffuse
		//emit specular
		// decay
		break;
	case MAYA_VOLUME :
		//emit diffuse
		//emit specular
		//light shape
		break;
	default :

		break;
	}	

	bDirty = TRUE;
	//bInBaseTex = FALSE;

	if( bHasShadow ) SetNeedDepthRendering( TRUE );
}

HRESULT GLight::SetToDepthRendering( ID3D10Device* pd3dDevice, FLOAT fOrtho, FLOAT fAspect, D3DXVECTOR3 vCenter, D3DXMATRIX* pMatView, D3DXMATRIX* pMatProj )
{
	HRESULT hr = S_OK;

	switch( iType )
	{
	case MAYA_POINT:
		break;
	case MAYA_DIRECTIONAL:
		{
			GDirectionalLight* pL = (GDirectionalLight*)this;
			pL->SetToDepthRendering( fOrtho, fAspect, vCenter );
			break;
		}
	case MAYA_SPOT:
		{
			GSpotLight* pL = (GSpotLight*)this;
			V( pL->SetToDepthRendering(pd3dDevice, fOrtho, fAspect, vCenter, pMatView, pMatProj) );
			break;
		}
	case MAYA_AMBIENT:
		break;
	default:
		break;
	}	

	return hr;
}

VOID
GLight::ParseLinkData( Thread* pThread, char** token )
{
	// total object num
	//*token = strtok( NULL, " " );
	int numobject = atoi(*token);
	iNumTotalObject = numobject;

	// link object num
	*token = strtok( NULL, " " );
	int numlink = atoi(*token);
	*token = strtok( NULL, " " );

	// alloc link bitmask	
	if( LinkBitmask.GetSize() == 0 )
	{
		EnterCriticalSection(&g_cs);
		LinkBitmask.Allocate(numobject);
		LeaveCriticalSection(&g_cs);
	}


	//EnterCriticalSection(&g_CS);
	//LinkBitmask.Reallocate(numobject, 0);
	//LeaveCriticalSection(&g_CS);

	int* pMask = LinkBitmask.GetMask();

	int item;
	for( int i=0; i < LinkBitmask.GetSize(); i++ )
	{
		sscanf( *token, "%d", &item );
		pMask[i] = item;		
		*token = strtok( NULL, " " );
	}

	if( numlink > numobject / 2 )
		bLinkInfoInclusive = FALSE;

	bNeedLinkUpdate = true;

	/*
	*token = strtok( NULL, " " );

	setLinkedObjectIDs.clear();

	int id;
	while( strcmp(*token, "\\") )
	{
	sscanf( *token, "%d", &id );
	setLinkedObjectIDs.insert( id );
	*token = strtok( NULL, " " );
	}

	bNeedLinkUpdate = true;
	*/
}	

/*
VOID			
GLight::GenerateLinkMap( UINT iRenderWidth, UINT iRenderHeight, GLenum link, LTGA* pObjectMap )
{
	if( !enLinkMaskTex ) 
		glGenTextures(1, &enLinkMaskTex);

	if( bLinkInfoInclusive )
		iLinkMapSize = LinkBitmask.CountSetBit();
	else
		iLinkMapSize = LinkBitmask.CountZeroBit() - (LinkBitmask.GetTotalBit() - iNumTotalObject);

	if( iLinkMapSize <= 0 ) 
	{
		bLinkInfoAll = TRUE;
		return;
	}
	bLinkInfoAll = FALSE;

	GLubyte* colorArray = new GLubyte[iLinkMapSize * 4];

	int id = 0;
	int x = 0;
	GLubyte rgb[4];
	for( int i=0; i < iLinkMapSize; i++ )
	{
		rgb[0] = rgb[1] = rgb[2] = 0;
		id = ( bLinkInfoInclusive ) ? LinkBitmask.FindFirst1Bit(id + 1) : LinkBitmask.FindFirst0Bit(id + 1);

		getColorFTag( rgb, id );
		colorArray[x++] = rgb[0];
		colorArray[x++] = rgb[1];
		colorArray[x++] = rgb[2];
		colorArray[x++] = 255;
	}

	glActiveTextureARB(GL_TEXTURE9_ARB);	
	glBindTexture( GL_TEXTURE_1D, link );
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);  
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);  
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); 

	glTexImage1D( GL_TEXTURE_1D, 0, 4, iLinkMapSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, colorArray );

	//glActiveTextureARB(GL_TEXTURE9_ARB);	
	//glBindTexture( GL_TEXTURE_1D, link );
	//glReadPixels(0, 0, iLinkMapSize, 1, GL_RGBA, GL_UNSIGNED_BYTE, colorArray);
	
	bNeedLinkUpdate = false;
	delete[] colorArray;
}
*/

GAmbientLight::GAmbientLight( char** token, int type ) : GPointLight( token, type )
{
	//Ambient Shade 

	sscanf( *token, "%f", &fAmbientShade );
	*token = strtok( NULL, " " );
	//위치
	for(int i = 0; i < 3; i++)
	{
		sscanf( *token, "%f", &vPosition[i] );
		*token = strtok( NULL, " " );
	}
}

VOID GAmbientLight::Setup( const D3DXMATRIX& matViewInv )
{
	D3DXVECTOR4 w_lightPos, transPos; 
	w_lightPos = D3DXVECTOR4( vPosition, 1 );

	D3DXVECTOR4 trans_light; 
	D3DXVec4Transform( &trans_light, &w_lightPos, &matViewInv );

	vTransPosition.x = trans_light.x;
	vTransPosition.y = trans_light.y;
	vTransPosition.z = trans_light.z;
}

VOID
GAmbientLight::Modify( char** token )
{
	//Ambient Shade 
	sscanf( *token, "%f", &fAmbientShade );
	*token = strtok( NULL, " " );
	//위치
	for(int i = 0; i < 3; i++)
	{
		sscanf( *token, "%f", &vPosition[i] );
		*token = strtok( NULL, " " );
	}
}

GPointLight::GPointLight( char** token, int type ) : GLight( token, type )
{
	if( type != MAYA_POINT ) return;

	int iHasShadow;
	sscanf( *token, "%d", &iHasShadow );
	bHasShadow = ( iHasShadow == 1 ) ? TRUE : FALSE;

	*token = strtok( NULL, " " );
	//emit diffuse
	//emit specular
	//decay
	sscanf( *token, "%d", &iDecay );
	*token = strtok( NULL, " " );
	//위치

	sscanf( *token, "%f", &vPosition[0] );
	*token = strtok( NULL, " " );
	sscanf( *token, "%f", &vPosition[2] );
	*token = strtok( NULL, " " );
	sscanf( *token, "%f", &vPosition[1] );
	*token = strtok( NULL, " " );
	vPosition[0] = -vPosition[0];
	vPosition[1] = -vPosition[1];
}

VOID GPointLight::Setup( const D3DXMATRIX& matViewInv )
{
	D3DXVECTOR4 light, trans_light, transPos; 

	light = D3DXVECTOR4( vPosition, 1 );

	D3DXVec4Transform( &trans_light, &light, &matViewInv );

	vTransPosition.x = trans_light.x;
	vTransPosition.y = trans_light.y;
	vTransPosition.z = trans_light.z;
}

VOID
GPointLight::Modify( char** token )
{
	int iHasShadow;
	sscanf( *token, "%d", &iHasShadow );
	bHasShadow = ( iHasShadow == 1 ) ? TRUE : FALSE;

	*token = strtok( NULL, " " );
	//emit diffuse
	//emit specular
	//decay
	sscanf( *token, "%d", &iDecay );
	*token = strtok( NULL, " " );
	//위치
	sscanf( *token, "%f", &vPosition[0] );
	*token = strtok( NULL, " " );
	sscanf( *token, "%f", &vPosition[2] );
	*token = strtok( NULL, " " );
	sscanf( *token, "%f", &vPosition[1] );
	*token = strtok( NULL, " " );

	vPosition[0] = -vPosition[0];
	vPosition[1] = -vPosition[1];
}

GDirectionalLight::GDirectionalLight( char** token, int type ) : GLight( token, type )
{
	int iHasShadow;
	sscanf( *token, "%d", &iHasShadow );
	bHasShadow = ( iHasShadow == 1 ) ? TRUE : FALSE;

	*token = strtok(NULL," ");
	//emit diffuse
	//emit specular
	//회전
	for(int i = 0; i < 16; i++)
	{
		sscanf( *token,"%f", &matRotate[i] );
		*token = strtok( NULL, " " );
	}

	/*
	sscanf( *token,"%f", &matRotate[0] );
	*token = strtok( NULL, " " );
	sscanf( *token,"%f", &matRotate[1] );
	*token = strtok( NULL, " " );
	sscanf( *token,"%f", &matRotate[2] );
	*token = strtok( NULL, " " );
	sscanf( *token,"%f", &matRotate[3] );
	*token = strtok( NULL, " " );
	matRotate[0] = -matRotate[0];
	matRotate[1] = -matRotate[1];
	matRotate[2] = -matRotate[2];

	sscanf( *token,"%f", &matRotate[8] );
	*token = strtok( NULL, " " );
	sscanf( *token,"%f", &matRotate[9] );
	*token = strtok( NULL, " " );
	sscanf( *token,"%f", &matRotate[10] );
	*token = strtok( NULL, " " );
	sscanf( *token,"%f", &matRotate[11] );
	*token = strtok( NULL, " " );

	sscanf( *token,"%f", &matRotate[4] );
	*token = strtok( NULL, " " );
	sscanf( *token,"%f", &matRotate[5] );
	*token = strtok( NULL, " " );
	sscanf( *token,"%f", &matRotate[6] );
	*token = strtok( NULL, " " );
	sscanf( *token,"%f", &matRotate[7] );
	*token = strtok( NULL, " " );
	matRotate[4] = -matRotate[4];
	matRotate[5] = -matRotate[5];
	matRotate[6] = -matRotate[6];

	sscanf( *token,"%f", &matRotate[12] );
	*token = strtok( NULL, " " );
	sscanf( *token,"%f", &matRotate[13] );
	*token = strtok( NULL, " " );
	sscanf( *token,"%f", &matRotate[14] );
	*token = strtok( NULL, " " );
	sscanf( *token,"%f", &matRotate[15] );
	*token = strtok( NULL, " " );
	*/
}

VOID GDirectionalLight::Setup( const D3DXMATRIX& matViewInv )
{
	D3DXMATRIX light_m = matRotate;

	D3DXMatrixTranspose( &light_m, &light_m );
	D3DXMatrixInverse( &light_m, NULL, &light_m );


	D3DXMATRIX view_light;
	D3DXMatrixMultiply( &view_light, &matViewInv, &light_m );

	D3DXVECTOR4 light_s( 0, 0, 0, 1 );
	D3DXVECTOR4 light_e( 0, 0, 1, 1 );
	D3DXVECTOR3 w_lightDir, transDir;

	D3DXVECTOR4 trans_light_s,trans_light_e; 
	D3DXVec4Transform( &trans_light_s, &light_s, &view_light );
	D3DXVec4Transform( &trans_light_e, &light_e, &view_light );

	D3DXVECTOR4 dir = trans_light_e - trans_light_s;
	D3DXVec3Normalize( &vTransDirection, &D3DXVECTOR3(dir.z, -dir.y, dir.x) );

	// world direction : for shadow
	D3DXVec4Transform( &trans_light_s, &light_s, &light_m );
	D3DXVec4Transform( &trans_light_e, &light_e, &light_m );

	dir = trans_light_e - trans_light_s;
	D3DXVec3Normalize( &vWorldDirection, &D3DXVECTOR3(-dir.x, -dir.z, dir.y) );
}

VOID
GDirectionalLight::Modify( char** token )
{
	int iHasShadow;
	sscanf( *token, "%d", &iHasShadow );
	bHasShadow = ( iHasShadow == 1 ) ? TRUE : FALSE;
	*token = strtok( NULL, " " );
	//emit diffuse
	//emit specular
	//회전
	for(int i = 0; i < 16; i++)
	{
		sscanf( *token,"%f", &matRotate[i] );
		*token = strtok( NULL, " " );
	}

	/*
	sscanf( *token,"%f", &matRotate[0] );
	*token = strtok( NULL, " " );
	sscanf( *token,"%f", &matRotate[1] );
	*token = strtok( NULL, " " );
	sscanf( *token,"%f", &matRotate[2] );
	*token = strtok( NULL, " " );
	sscanf( *token,"%f", &matRotate[3] );
	*token = strtok( NULL, " " );
	matRotate[0] = -matRotate[0];
	matRotate[1] = -matRotate[1];
	matRotate[2] = -matRotate[2];

	sscanf( *token,"%f", &matRotate[8] );
	*token = strtok( NULL, " " );
	sscanf( *token,"%f", &matRotate[9] );
	*token = strtok( NULL, " " );
	sscanf( *token,"%f", &matRotate[10] );
	*token = strtok( NULL, " " );
	sscanf( *token,"%f", &matRotate[11] );
	*token = strtok( NULL, " " );

	sscanf( *token,"%f", &matRotate[4] );
	*token = strtok( NULL, " " );
	sscanf( *token,"%f", &matRotate[5] );
	*token = strtok( NULL, " " );
	sscanf( *token,"%f", &matRotate[6] );
	*token = strtok( NULL, " " );
	sscanf( *token,"%f", &matRotate[7] );
	*token = strtok( NULL, " " );
	matRotate[4] = -matRotate[4];
	matRotate[5] = -matRotate[5];
	matRotate[6] = -matRotate[6];

	sscanf( *token,"%f", &matRotate[12] );
	*token = strtok( NULL, " " );
	sscanf( *token,"%f", &matRotate[13] );
	*token = strtok( NULL, " " );
	sscanf( *token,"%f", &matRotate[14] );
	*token = strtok( NULL, " " );
	sscanf( *token,"%f", &matRotate[15] );
	*token = strtok( NULL, " " );
	*/
}

VOID
GDirectionalLight::SetToDepthRendering( FLOAT fOrtho, FLOAT fAspect, D3DXVECTOR3 vCenter )
{
	/*
	glOrtho( -fOrtho * fAspect , fOrtho * fAspect , -fOrtho , fOrtho , 0.01f , fOrtho * 2.0f );
	gluLookAt( vCenter[0] + (vWorldDirection[0] * fOrtho), vCenter[1] + (vWorldDirection[1] * fOrtho), vCenter[2] + (vWorldDirection[2] * fOrtho), vCenter[0], vCenter[1], vCenter[2], 0, 1, 0 );

	if( !glIsTexture(enShadowMaskTex) )
		glGenTextures(1, &enShadowMaskTex);
		*/
}

GSpotLight::GSpotLight( char** token, int type ) 
{
	new (this) GDirectionalLight::GLight( token, type );	

	int iHasShadow;
	sscanf( *token, "%d", &iHasShadow );
	bHasShadow = ( iHasShadow == 1 ) ? TRUE : FALSE;

	*token = strtok( NULL, " " );
	//emit diffuse
	//emit specular
	//decay
	sscanf( *token, "%d", &iDecay );
	*token = strtok( NULL, " " );
	//cone angle
	sscanf( *token, "%f", &fAngleTheta );
	*token = strtok( NULL, " " );
	//penumbra angle
	sscanf( *token, "%f", &fAnglePhi );
	*token = strtok( NULL, " " );
	//dropoff
	sscanf( *token, "%f", &fFalloff );
	*token = strtok( NULL, " " );
	//위치
	/*
	for(int i = 0; i < 3; i++)
	{
		sscanf( *token, "%f",&vPosition[i] );
		*token = strtok( NULL, " " );
	}
	*/
	sscanf( *token, "%f",&vPosition[0] );
	*token = strtok( NULL, " " );
	sscanf( *token, "%f",&vPosition[2] );
	*token = strtok( NULL, " " );
	sscanf( *token, "%f",&vPosition[1] );
	*token = strtok( NULL, " " );
	vPosition[0] = -vPosition[0];
	vPosition[1] = -vPosition[1];
	//회전
	
	for(int i = 0; i < 16; i++)
	{
		sscanf( *token,"%f", &matRotate[i] );
		*token = strtok( NULL, " " );
	}
	
	/*
	sscanf( *token,"%f", &matRotate[0] );
	*token = strtok( NULL, " " );
	sscanf( *token,"%f", &matRotate[1] );
	*token = strtok( NULL, " " );
	sscanf( *token,"%f", &matRotate[2] );
	*token = strtok( NULL, " " );
	sscanf( *token,"%f", &matRotate[3] );
	*token = strtok( NULL, " " );
	matRotate[0] = -matRotate[0];
	matRotate[1] = -matRotate[1];
	matRotate[2] = -matRotate[2];

	sscanf( *token,"%f", &matRotate[8] );
	*token = strtok( NULL, " " );
	sscanf( *token,"%f", &matRotate[9] );
	*token = strtok( NULL, " " );
	sscanf( *token,"%f", &matRotate[10] );
	*token = strtok( NULL, " " );
	sscanf( *token,"%f", &matRotate[11] );
	*token = strtok( NULL, " " );

	sscanf( *token,"%f", &matRotate[4] );
	*token = strtok( NULL, " " );
	sscanf( *token,"%f", &matRotate[5] );
	*token = strtok( NULL, " " );
	sscanf( *token,"%f", &matRotate[6] );
	*token = strtok( NULL, " " );
	sscanf( *token,"%f", &matRotate[7] );
	*token = strtok( NULL, " " );
	matRotate[4] = -matRotate[4];
	matRotate[5] = -matRotate[5];
	matRotate[6] = -matRotate[6];

	sscanf( *token,"%f", &matRotate[12] );
	*token = strtok( NULL, " " );
	sscanf( *token,"%f", &matRotate[13] );
	*token = strtok( NULL, " " );
	sscanf( *token,"%f", &matRotate[14] );
	*token = strtok( NULL, " " );
	sscanf( *token,"%f", &matRotate[15] );
	*token = strtok( NULL, " " );
	*/

	HRESULT hr;

	
}

VOID GSpotLight::Setup( const D3DXMATRIX& matViewInv )
{
	D3DXVECTOR4 light( vPosition, 1 );
	
	D3DXVECTOR4 trans_light; 
	D3DXVec4Transform( &trans_light, &light, &matViewInv );

	vTransPosition.x = trans_light.x;
	vTransPosition.y = trans_light.y;
	vTransPosition.z = trans_light.z;

	// direction

	D3DXMATRIX light_m = matRotate;

	D3DXMatrixTranspose( &light_m, &light_m );
	D3DXMatrixInverse( &light_m, NULL, &light_m );
	

	D3DXMATRIX view_light;
	D3DXMatrixMultiply( &view_light, &matViewInv, &light_m );

	D3DXVECTOR4 light_s( 0, 0, 0, 1 );
	D3DXVECTOR4 light_e( 0, 0, 1, 1 );
	D3DXVECTOR3 w_lightDir, transDir;

	D3DXVECTOR4 trans_light_s,trans_light_e; 
	D3DXVec4Transform( &trans_light_s, &light_s, &view_light );
	D3DXVec4Transform( &trans_light_e, &light_e, &view_light );

	D3DXVECTOR4 dir = trans_light_e - trans_light_s;
	D3DXVec3Normalize( &vTransDirection, &D3DXVECTOR3(dir.z, -dir.y, dir.x) );

	// world direction : for shadow
	D3DXVec4Transform( &trans_light_s, &light_s, &light_m );
	D3DXVec4Transform( &trans_light_e, &light_e, &light_m );

	dir = trans_light_e - trans_light_s;
	D3DXVec3Normalize( &vWorldDirection, &D3DXVECTOR3(-dir.x, -dir.z, dir.y) );

	double p_angle, t_angle;
	double d_off;

	t_angle = fAngleTheta;
	p_angle = fAnglePhi;
	d_off = fFalloff;

	double half_angle = t_angle/2.0;
	if(p_angle > 0.0)
	{
		fCosPenumbra = cosf(Radians(half_angle+p_angle));
		fCosUmbra = cosf(Radians(half_angle));
	}
	else
	{
		fCosPenumbra = cosf((FLOAT)Radians(half_angle));
		if(half_angle > -(p_angle))
		{
			fCosUmbra = cosf((FLOAT)Radians(half_angle+p_angle));
		}
		else
		{
			fCosUmbra = 1.0f;
		}
	}

	if(fCosUmbra <0.0f)
	{
		fCosUmbra = 0.0f;
	}
	if(fCosPenumbra <0.0f)
	{
		fCosPenumbra = 0.0f;
	}
	fFalloff = (FLOAT)d_off;
	t_angle = max(t_angle+(2*p_angle),t_angle);
}

VOID
GSpotLight::Modify( char** token )
{
	int iHasShadow;
	sscanf( *token, "%d", &iHasShadow );
	bHasShadow = ( iHasShadow == 1 ) ? TRUE : FALSE;

	*token = strtok( NULL, " " );
	//emit diffuse
	//emit specular
	//decay
	sscanf( *token, "%d", &iDecay );
	*token = strtok( NULL, " " );
	//cone angle
	sscanf( *token, "%f", &fAngleTheta );
	*token = strtok( NULL," " );
	//penumbra angle
	sscanf( *token, "%f", &fAnglePhi );
	*token = strtok( NULL, " " );
	//dropoff
	sscanf( *token, "%f", &fFalloff );
	*token = strtok( NULL, " " );
	//위치
	sscanf( *token, "%f",&vPosition[0] );
	*token = strtok( NULL, " " );
	sscanf( *token, "%f",&vPosition[2] );
	*token = strtok( NULL, " " );
	sscanf( *token, "%f",&vPosition[1] );
	*token = strtok( NULL, " " );
	vPosition[0] = -vPosition[0];
	vPosition[1] = -vPosition[1];
	//회전
	
	for(int i = 0; i < 16; i++)
	{
		sscanf( *token,"%f", &matRotate[i] );
		*token = strtok( NULL, " " );
	}
	/*

	sscanf( *token,"%f", &matRotate[0] );
	*token = strtok( NULL, " " );
	sscanf( *token,"%f", &matRotate[1] );
	*token = strtok( NULL, " " );
	sscanf( *token,"%f", &matRotate[2] );
	*token = strtok( NULL, " " );
	sscanf( *token,"%f", &matRotate[3] );
	*token = strtok( NULL, " " );
	matRotate[0] = -matRotate[0];
	matRotate[1] = -matRotate[1];
	matRotate[2] = -matRotate[2];

	sscanf( *token,"%f", &matRotate[8] );
	*token = strtok( NULL, " " );
	sscanf( *token,"%f", &matRotate[9] );
	*token = strtok( NULL, " " );
	sscanf( *token,"%f", &matRotate[10] );
	*token = strtok( NULL, " " );
	sscanf( *token,"%f", &matRotate[11] );
	*token = strtok( NULL, " " );

	sscanf( *token,"%f", &matRotate[4] );
	*token = strtok( NULL, " " );
	sscanf( *token,"%f", &matRotate[5] );
	*token = strtok( NULL, " " );
	sscanf( *token,"%f", &matRotate[6] );
	*token = strtok( NULL, " " );
	sscanf( *token,"%f", &matRotate[7] );
	*token = strtok( NULL, " " );
	matRotate[4] = -matRotate[4];
	matRotate[5] = -matRotate[5];
	matRotate[6] = -matRotate[6];

	sscanf( *token,"%f", &matRotate[12] );
	*token = strtok( NULL, " " );
	sscanf( *token,"%f", &matRotate[13] );
	*token = strtok( NULL, " " );
	sscanf( *token,"%f", &matRotate[14] );
	*token = strtok( NULL, " " );
	sscanf( *token,"%f", &matRotate[15] );
	*token = strtok( NULL, " " );
	*/
}

HRESULT GSpotLight::SetToDepthRendering( ID3D10Device* pd3dDevice, FLOAT fOrtho, FLOAT fAspect, D3DXVECTOR3 vCenter, D3DXMATRIX* pMatView, D3DXMATRIX* pMatProj )
{
	HRESULT hr = S_OK;

	D3DXVECTOR3 dis_tmp;
	dis_tmp = vPosition - vCenter;

	float lc_dis;
	lc_dis = D3DXVec3Length( &dis_tmp );

	float l_near,l_far;

	l_near = lc_dis - fOrtho;
	fFar = lc_dis + fOrtho;

	if( l_near < 0.01f )
	{
		l_near = 0.01f;
	}

	fNear = l_near;
	D3DXMatrixPerspectiveFovLH( pMatProj, fAngleTheta, 1, fNear, fFar );
	D3DXMatrixLookAtLH( pMatView, &vPosition, &D3DXVECTOR3(vPosition - vWorldDirection), &D3DXVECTOR3(0, -1, 0) );

	D3DXMATRIX mView, mProj, mViewProj;
	matView = *pMatView;
	matProj = *pMatProj;
	mView = *pMatView;
	mProj = *pMatProj;
	D3DXMatrixMultiply( &mViewProj, &mView, &mProj );
	D3DXMatrixIdentity( &matShadowInv );
	D3DXMatrixInverse( &matShadowInv, NULL, &mViewProj );

	if( bHasShadow && !pSoftShadowMap )
	{
		pSoftShadowMap = new SSMap();
		/*
		D3D10_TEXTURE2D_DESC d;
		d.CPUAccessFlags	= 0;
		d.Usage				= D3D10_USAGE_DEFAULT;
		d.Width				= SHADOWMAP_SIZE;
		d.Height			= SHADOWMAP_SIZE;
		d.MipLevels			= 1;
		d.Format			= DXGI_FORMAT_R32_TYPELESS;
		d.ArraySize			= 1;
		d.BindFlags			= D3D10_BIND_SHADER_RESOURCE | D3D10_BIND_DEPTH_STENCIL;
		d.MiscFlags			= 0;
		d.SampleDesc.Count	= 1;
		d.SampleDesc.Quality= 0;

		ID3D10Device* pd3dDevice = DXUTGetD3D10Device();
		V( pd3dDevice->CreateTexture2D(&d, 0, &pShadowMaskTex) );

		D3D10_DEPTH_STENCIL_VIEW_DESC descDSV;

		descDSV.Format = DXGI_FORMAT_D32_FLOAT;
		descDSV.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
		descDSV.Texture2D.MipSlice = 0;		

		V( pd3dDevice->CreateDepthStencilView(pShadowMaskTex, &descDSV, &pDSVShadowMask) );

		D3D10_SHADER_RESOURCE_VIEW_DESC descSRV;
		memset( &descSRV, 0, sizeof(D3D10_SHADER_RESOURCE_VIEW_DESC) );
		descSRV.Format = DXGI_FORMAT_R32_FLOAT;
		descSRV.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
		descSRV.Texture2D.MostDetailedMip = 0;
		descSRV.Texture2D.MipLevels = 1;

		V( pd3dDevice->CreateShaderResourceView(pShadowMaskTex, &descSRV, &pSRVShadowMask) );	
		*/
	}
	

	return hr;
}