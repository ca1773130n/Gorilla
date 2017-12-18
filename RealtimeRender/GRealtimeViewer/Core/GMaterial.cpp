#include "stdafx.h"
#include "GMaterial.h"
#include "primitive.h"
#include "GIRLBlinn.h"
#include "GIRLPhong.h"
#include "GIRLLambert.h"
#include "ltga.h"
#include "corona.h"
#include "io.h"

#ifdef _WIN32
#define F_CHECK_FLAG 00
#define F_CHECK(file) _access(file, F_CHECK_FLAG)
#define F_EXIST  -1
#endif

#ifdef LINUX
#define F_CHECK_FLAG F_OK
#define F_CHECK(file) access(file, F_CHECK_FLAG)
#define F_EXIST 0
#endif

BYTE 
PackFloatInByte(float in)
{
	return (BYTE) ((in+1.0f) / 2.0f * 255.0f);
}

CGAttribute&
CGMaterial::GetAttribute( int iAttr )
{
	switch( iAttr )
	{
	case 0:
		return m_AttrAmbientColor;
	case 1:
		return m_AttrColor;
	case 2:
		return m_AttrSpecularColor;
	case 3:
		return m_AttrSpecularAttr1;
	case 4:
		return m_AttrTransparency;
	case 5:
		return m_AttrCommon1;		
	case 6:
		return m_AttrCommon2;		
	case 7:
		return m_AttrNormal;		
	}
}

VOID
CGMaterial::SetupAttribute( CGAttribute& attr, Texture<SWCSpectrum>* pTex1, Texture<FLOAT>* pTex2 /* = NULL */, bool bBumpMap /* = FALSE */ )
{
	ID3D10Device* pDev = DXUTGetD3D10Device();

	TsPack tspack;
	tspack.swl = new SpectrumWavelengths();
	tspack.swl->Sample( 0.5f, 0.5f );
	DifferentialGeometry dg;
	dg.p = Point( 0, 0, 0 );
	dg.nn = Normal( 0, 0, 1 );

	if( pTex1->GetType() == TEXTURE_CONSTANT )
	{
		attr.constant = pTex1->GetRGB();
		if( pTex2 && pTex2->GetType() == TEXTURE_CONSTANT ) 
			attr.constant.SetAlpha( pTex2->GetV() );
		else
			attr.constant.SetAlpha( 1 );
		attr.texture.pTex = NULL;
	}
	else
	{
		attr.constant = RGBColor( -9999.f );

		char normalMapPath[MAX_PATH];
		sprintf(normalMapPath, "%s.png", pTex1->filepath.c_str());

		D3DX10_IMAGE_LOAD_INFO loadInfo;
		ZeroMemory( &loadInfo, sizeof(D3DX10_IMAGE_LOAD_INFO) );
		loadInfo.BindFlags = D3D10_BIND_SHADER_RESOURCE;
		loadInfo.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

		//loadInfo.Format = DXGI_FORMAT_BC3_UNORM;

		if( bBumpMap )
		{
			//loadInfo.Format = DXGI_FORMAT_BC5_UNORM;

			// 범프맵을 노멀맵으로 변환
			
			if( F_CHECK(normalMapPath) == F_EXIST )
			{
				corona::Image* pOrgImage = corona::OpenImage( pTex1->filepath.c_str() );
				int width = pOrgImage->getWidth();
				corona::PixelFormat pf = pOrgImage->getFormat();
				int pixelDepth = 3;
				if( pf == corona::PixelFormat::PF_R8G8B8A8 )
					pixelDepth = 4;
				BYTE* pPixels = (BYTE*)pOrgImage->getPixels();
				BYTE* pNewPixels = new BYTE[ width * width * pixelDepth ];

				float dX, dY, nX, nY, nZ, oolen;

				for( int i=0; i < width; i++ )
					for( int j=0; j < width; j++ )
					{
						// Do Y sobel filter
						dY = pPixels[ ((i+1) % width) * width * pixelDepth + ((j-1+width) % width) * pixelDepth ] / 255.f * -1.f;

						dY += pPixels[ ((i+1) % width) * width * pixelDepth + (j % width) * pixelDepth ] / 255.f * -2.f;

						dY += pPixels[ ((i+1) % width) * width * pixelDepth + ((j+1) % width) * pixelDepth ] / 255.f * -1.f;

						dY += pPixels[ ((i-1+width) % width) * width * pixelDepth + ((j-1+width) % width) * pixelDepth ] / 255.f * 1.f;

						dY += pPixels[ ((i-1+width) % width) * width * pixelDepth + (j % width) * pixelDepth ] / 255.f * 2.f;

						dY += pPixels[ ((i-1+width) % width) * width * pixelDepth + ((j+1) % width) * pixelDepth ] / 255.f * 1.f;

						// Do X sobel filter
						dX = pPixels[ ((i-1+width) % width) * width * pixelDepth + ((j-1+width) % width) * pixelDepth ] / 255.f * -1.f;

						dX += pPixels[ (i % width) * width * pixelDepth + ((j-1+width) % width) * pixelDepth ] / 255.f * -2.f;

						dX += pPixels[ ((i+1) % width) * width * pixelDepth + ((j-1+width) % width) * pixelDepth ] / 255.f * -1.f;

						dX += pPixels[ ((i-1+width) % width) * width * pixelDepth + ((j+1) % width) * pixelDepth ] / 255.f * 1.f;

						dX += pPixels[ (i % width) * width * pixelDepth + ((j+1) % width) * pixelDepth ] / 255.f * 2.f;

						dX += pPixels[ ((i+1) % width) * width * pixelDepth + ((j+1) % width) * pixelDepth ] / 255.f * 1.f;

						nX = -dX;
						nY = -dY;
						nZ = 0.2;

						// Normalize
						oolen = 1.0f/((float) sqrt(nX*nX + nY*nY + nZ*nZ));
						nX *= oolen;
						nY *= oolen;
						nZ *= oolen;

						pNewPixels[ i * width * pixelDepth + j * pixelDepth ] = PackFloatInByte( nX );
						pNewPixels[ i * width * pixelDepth + j * pixelDepth + 1 ] = PackFloatInByte( nY );
						pNewPixels[ i * width * pixelDepth + j * pixelDepth + 2 ] = PackFloatInByte( nZ );
					}

				corona::Image* pImage = corona::CreateImage( width, width, pf, pNewPixels );

				corona::SaveImage( normalMapPath, corona::FileFormat::FF_PNG, pImage );
				TRACE(normalMapPath);	
				D3DX10CreateShaderResourceViewFromFile( pDev, CString(normalMapPath), &loadInfo, NULL, &attr.texture.pTexSRV, NULL );

				delete pOrgImage;	
				delete pNewPixels;
			}
			else
				D3DX10CreateShaderResourceViewFromFile( pDev, CString(normalMapPath), &loadInfo, NULL, &attr.texture.pTexSRV, NULL );
				
		}
		else
		{			
			D3DX10CreateShaderResourceViewFromFile( pDev, CString(pTex1->filepath.c_str()), &loadInfo, NULL, &attr.texture.pTexSRV, NULL );			
		}

		ID3D10Resource* pRes;
		ID3D10ShaderResourceView* pSRV;
		attr.texture.pTexSRV->GetResource( &pRes);
		pRes->QueryInterface( __uuidof(ID3D10Texture2D), (LPVOID*)&attr.texture.pTex );

		/*
		if( pTex2 )
		{
			Texture<float>* pTex[2] = { pTex1, pTex2 };
			int flag = 0, ch = 0;

			for( int i=0; i < 2; i++ )
			{
				if( !pTex[i] ) continue;
				if( pTex[i]->GetType() == TEXTURE_IMAGEMAP )
					flag |= 1 << i;
			}

			if( flag == 0 )
			{
				attr.constant = pTex1->Evaluate( &tspack, dg ).Clamp(0, 1).ToXYZ().ToRGB();	
				attr.constant.SetAlpha( pTex2->Evaluate( &tspack, dg ) );
				attr.texture.pTexSRV = NULL;
				return;
			}

			ID3D10Texture2D* pTexture = NULL;
			ID3D10Texture2D* pTextureTemp = NULL;
			ID3D10Texture2D* pTextureStaging = NULL;
			ID3D10Texture2D* pTextureStaging2 = NULL;
			ID3D10Resource* pTexRes = NULL;		
			ID3D10Resource* pTexResTemp = NULL;		
			ID3D10Resource* pTexResStaging = NULL;		

			D3D10_TEXTURE2D_DESC texDesc;
			texDesc.Width = 256;
			texDesc.Height = 256;
			texDesc.MipLevels = 1;
			texDesc.ArraySize = 1;
			texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			texDesc.SampleDesc.Count = 1;
			texDesc.SampleDesc.Quality = 0;
			texDesc.Usage = D3D10_USAGE_DEFAULT;
			texDesc.BindFlags = D3D10_BIND_SHADER_RESOURCE;
			texDesc.CPUAccessFlags = 0;
			texDesc.MiscFlags = 0;
			pDev->CreateTexture2D( &texDesc, NULL, &pTexture );

			ID3D10ShaderResourceView* pSRV;
			D3D10_SHADER_RESOURCE_VIEW_DESC srDesc;
			ZeroMemory( &srDesc, sizeof(D3D10_SHADER_RESOURCE_VIEW_DESC) );
			srDesc.Format = texDesc.Format;
			srDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
			srDesc.Texture2D.MostDetailedMip = 0;
			srDesc.Texture2D.MipLevels = texDesc.MipLevels;
			pDev->CreateShaderResourceView( pTexture, &srDesc, &pSRV );
			attr.texture.pTexSRV = pSRV;

			texDesc.BindFlags = 0;
			texDesc.Usage = D3D10_USAGE_STAGING;
			texDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
			pDev->CreateTexture2D( &texDesc, NULL, &pTextureStaging );

			texDesc.MipLevels = 9;
			texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			texDesc.CPUAccessFlags = D3D10_CPU_ACCESS_READ;
			pDev->CreateTexture2D( &texDesc, NULL, &pTextureStaging2 );

			D3D10_MAPPED_TEXTURE2D mappedTex, mappedTexTemp;
			pTextureStaging->Map( 0, D3D10_MAP_WRITE, 0, &mappedTex );

			FLOAT* pFloatArray = (FLOAT*)mappedTex.pData;

			if( pTex1->GetType() == TEXTURE_IMAGEMAP )
			for( int i=0; i < 2; i++ )
			{
				if( flag & (1 << i) )
				{			
					D3DX10CreateTextureFromFile( pDev, CString(pTex[i]->filepath.c_str()), NULL, NULL, &pTexResTemp, NULL );
					pDev->CopyResource( pTextureStaging2, pTexResTemp );

					pTextureStaging2->Map( 0, D3D10_MAP_READ, 0, &mappedTexTemp );
					BYTE* pByteArray2 = (BYTE*)mappedTexTemp.pData;

					for( int height=0; height < 256; height++ )
						for( int width=0; width < 256; width++ )
							pFloatArray[256 * height * 4 + width * 4 + i] = pByteArray2[256 * height * 4 + width * 4] / 255.f;

					pTextureStaging2->Unmap( 0 );
					SAFE_RELEASE( pTexResTemp );
				}
				else if( pTex[i] )
				{
					FLOAT fConstant = pTex[i]->Evaluate( &tspack, dg );
					for( int height=0; height < 256; height++ )
						for( int width=0; width < 256; width++ )
							pFloatArray[256 * height * 4 + width * 4 + i] = fConstant;
				}
			}
			pTextureStaging->Unmap( 0 );	

			pDev->CopyResource( pTexture, pTextureStaging );


			SAFE_RELEASE( pTextureStaging );
			SAFE_RELEASE( pTextureStaging2 );
		}
		*/
	}
	delete tspack.swl;
}

VOID
CGMaterial::SetupAttribute( CGAttribute& attr, Texture<float>* pTex1, Texture<float>* pTex2, Texture<float>* pTex3, Texture<float>* pTex4 )
{
	ID3D10Device* pDev = DXUTGetD3D10Device();
	
	TsPack tspack;
	tspack.swl = new SpectrumWavelengths();
	tspack.swl->Sample( 0.5f, 0.5f );
	DifferentialGeometry dg;
	dg.p = Point( 0, 0, 0 );
	dg.nn = Normal( 0, 0, 1 );

	Texture<float>* pTex[4] = { pTex1, pTex2, pTex3, pTex4 };
	int flag = 0, ch = 0;
	
	int textureWidth = 0;

	for( int i=0; i < 4; i++ )
	{
		if( !pTex[i] ) continue;
		if( pTex[i]->GetType() == TEXTURE_IMAGEMAP )
		{
			flag |= 1 << i;
			// 텍스쳐 사이즈 얻어옴
			CImage cimage;
			WCHAR tmpFilePath[MAX_PATH];
			MultiByteToWideChar( CP_ACP, 0, pTex[i]->filepath.c_str(), -1, tmpFilePath, MAX_PATH );
			cimage.Load( tmpFilePath );
			textureWidth = cimage.GetWidth();
			cimage.Destroy();
		}
	}

	if( flag == 0 )
	{
		FLOAT fRGB[4] = 
		{ 
			pTex1 ? pTex1->GetV() : 0,
			pTex2 ? pTex2->GetV() : 0,
			pTex3 ? pTex3->GetV() : 0,
			pTex4 ? pTex4->GetV() : 1
		};
		for( int i=0; i < 4; i++ )
			if( fRGB[i] > 1 ) fRGB[i] /= 100.f;
		attr.constant = RGBColor( fRGB );	
		attr.constant.SetAlpha( fRGB[3] );
		attr.texture.pTexSRV = NULL;
	}
	else
	{	
		attr.constant = RGBColor( -9999.f );

		ID3D10Texture2D* pTexture = NULL;
		ID3D10Texture2D* pTextureTemp = NULL;
		ID3D10Texture2D* pTextureStaging = NULL;
		ID3D10Texture2D* pTextureStaging2 = NULL;
		ID3D10Resource* pTexRes = NULL;		
		ID3D10Resource* pTexResTemp = NULL;		
		ID3D10Resource* pTexResStaging = NULL;			

		D3D10_TEXTURE2D_DESC texDesc;
		texDesc.Width = textureWidth;
		texDesc.Height = textureWidth;
		texDesc.MipLevels = 1;
		texDesc.ArraySize = 1;
		texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Usage = D3D10_USAGE_DEFAULT;
		texDesc.BindFlags = D3D10_BIND_SHADER_RESOURCE;
		texDesc.CPUAccessFlags = 0;
		texDesc.MiscFlags = 0;
		pDev->CreateTexture2D( &texDesc, NULL, &pTexture );

		ID3D10ShaderResourceView* pSRV;
		D3D10_SHADER_RESOURCE_VIEW_DESC srDesc;
		ZeroMemory( &srDesc, sizeof(D3D10_SHADER_RESOURCE_VIEW_DESC) );
		srDesc.Format = texDesc.Format;
		srDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
		srDesc.Texture2D.MostDetailedMip = 0;
		srDesc.Texture2D.MipLevels = texDesc.MipLevels;
		pDev->CreateShaderResourceView( pTexture, &srDesc, &pSRV );
		attr.texture.pTexSRV = pSRV;		

		texDesc.BindFlags = 0;
		texDesc.Usage = D3D10_USAGE_STAGING;
		texDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		pDev->CreateTexture2D( &texDesc, NULL, &pTextureStaging );

		texDesc.MipLevels = 0;
		texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		texDesc.CPUAccessFlags = D3D10_CPU_ACCESS_READ;
		pDev->CreateTexture2D( &texDesc, NULL, &pTextureStaging2 );

		D3D10_MAPPED_TEXTURE2D mappedTex, mappedTexTemp;
		pTextureStaging->Map( 0, D3D10_MAP_WRITE, 0, &mappedTex );

		FLOAT* pFloatArray = (FLOAT*)mappedTex.pData;

		for( int i=0; i < 4; i++ )
		{
			if( flag & (1 << i) )
			{			
				D3DX10_IMAGE_LOAD_INFO dl;				
				D3DX10CreateTextureFromFile( pDev, CString(pTex[i]->filepath.c_str()), NULL, NULL, &pTexResTemp, NULL );
				pDev->CopyResource( pTextureStaging2, pTexResTemp );
								
				pTextureStaging2->Map( 0, D3D10_MAP_READ, 0, &mappedTexTemp );
				BYTE* pByteArray2 = (BYTE*)mappedTexTemp.pData;

				for( int height=0; height < textureWidth; height++ )
					for( int width=0; width < textureWidth; width++ )
						pFloatArray[textureWidth * height * 4 + width * 4 + i] = pByteArray2[textureWidth * height * 4 + width * 4] / 255.f;

				pTextureStaging2->Unmap( 0 );
				SAFE_RELEASE( pTexResTemp );
			}
			else if( pTex[i] )
			{
				FLOAT fConstant = pTex[i]->Evaluate( &tspack, dg );
				if( fConstant > 1 )
					fConstant /= 100.f;
				for( int height=0; height < textureWidth; height++ )
					for( int width=0; width < textureWidth; width++ )
						pFloatArray[textureWidth * height * 4 + width * 4 + i] = fConstant;
			}
		}
		pTextureStaging->Unmap( 0 );	

		pDev->CopyResource( pTexture, pTextureStaging );

		attr.texture.pTex = pTexture;

		SAFE_RELEASE( pTextureStaging );
		SAFE_RELEASE( pTextureStaging2 );
	}	

	delete tspack.swl;
}

VOID
CGMaterial::SetupLambert( VOID )
{
	GIRLLambert* pLambert = (GIRLLambert*)m_pGIRLMaterial;
	SetupAttribute( m_AttrSpecularColor, 0.f );
	SetupAttribute( m_AttrSpecularAttr1, 0.f );
}

VOID
CGMaterial::SetupBlinn( VOID )
{
	GIRLBlinn* pBlinn = (GIRLBlinn*)m_pGIRLMaterial;
	SetupAttribute( m_AttrSpecularColor, pBlinn->specularColor.get() );
	SetupAttribute( m_AttrSpecularAttr1, pBlinn->eccentricity.get(), pBlinn->specularRollOff.get(), 0, 0 );
}

VOID
CGMaterial::SetupPhong( VOID )
{
	GIRLPhong* pPhong = (GIRLPhong*)m_pGIRLMaterial;
	SetupAttribute( m_AttrSpecularColor, pPhong->specularColor.get() );
	SetupAttribute( m_AttrSpecularAttr1, pPhong->cosinePower.get(), pPhong->reflectivity.get(), 0, 0 );
}

