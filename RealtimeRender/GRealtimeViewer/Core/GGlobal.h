#ifndef __GDEBUG_H__
#define __GDEBUG_H__

#include "MainFrm.h"

#define GDEBUG(x, ...) { if(debugmsg) {sprintf(GGlobal::GetOutputBuffer(), x, __VA_ARGS__); ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetHistoryBar().AddLog( CString(g_szOutputBuffer) );} }

#define DEFAULT_NUMLIGHT			16

namespace GGlobal
{
	static int debugmsg = 1;
	static char g_szOutputBuffer[81920];
	static char* GetOutputBuffer( VOID ) { return g_szOutputBuffer; }
	static void D3DXMatrixToD3DXMATRIX( D3DXMATRIX* pD3DXMat, D3DXMATRIX pGLTMat )
	{
		pGLTMat[0] = pD3DXMat->_11;	
		pGLTMat[1] = pD3DXMat->_12;	
		pGLTMat[2] = pD3DXMat->_13;	
		pGLTMat[3] = pD3DXMat->_14;	
		pGLTMat[4] = pD3DXMat->_21;	
		pGLTMat[5] = pD3DXMat->_22;	
		pGLTMat[6] = pD3DXMat->_23;	
		pGLTMat[7] = pD3DXMat->_24;	
		pGLTMat[8] = pD3DXMat->_31;	
		pGLTMat[9] = pD3DXMat->_32;	
		pGLTMat[10] = pD3DXMat->_33;	
		pGLTMat[11] = pD3DXMat->_34;	
		pGLTMat[12] = pD3DXMat->_41;	
		pGLTMat[13] = pD3DXMat->_42;	
		pGLTMat[14] = pD3DXMat->_43;	
		pGLTMat[15] = pD3DXMat->_44;	
	}
}

using namespace GGlobal;

#endif