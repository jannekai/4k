#include "intro.h"
#include <string.h>
#include <stdio.h>

extern HWND	                   g_hWnd;
extern HRESULT				   g_hResult;
extern D3D_DRIVER_TYPE         g_driverType;
extern D3D_FEATURE_LEVEL       g_featureLevel;
extern ID3D11Device*           g_pd3dDevice;
extern ID3D11DeviceContext*    g_pImmediateContext;
extern IDXGISwapChain*         g_pSwapChain;
extern ID3D11RenderTargetView* g_pRenderTargetView;

void loadShader(char* fileName, char* entryPoint, char* shaderModel, ID3DBlob** ppBlobOut) 
{
	DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG;
	ID3DBlob* pErrorBlob;

	g_hResult = D3DX11CompileFromFile(fileName, NULL, NULL, entryPoint, shaderModel, shaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL);

	if (FAILED(g_hResult)) {
		OutputDebugString("!!!!!!!! Failed to compile shader\n");
		OutputDebugString(fileName);
		OutputDebugString("\n");
		OutputDebugString(entryPoint);
		OutputDebugString("\n");
		OutputDebugString(shaderModel);
		OutputDebugString("\n");
		if (pErrorBlob != NULL) {
			OutputDebugString((char*) pErrorBlob->GetBufferPointer());
			pErrorBlob->Release();			
		}	
		ExitProcess(0);
	}
	if (pErrorBlob) {
		pErrorBlob->Release();
	}		
}

void debugRS() {
	D3D11_RASTERIZER_DESC rsDesc;
	ID3D11RasterizerState* rsState;
	g_pImmediateContext->RSGetState(&rsState);

	if (rsState != NULL) {
		rsState->GetDesc(&rsDesc);
	} else {
		OutputDebugString("No RS state set, defaults are in use");
	}	
}

void debugTime(float time, float beat, float measure, float hit)
{
	char tmp[4096];
	sprintf(tmp, "Time %f beat %f measure %f hit %f\n", time, beat, measure, hit);
	OutputDebugString(tmp);
}
