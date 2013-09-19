#include "intro.h"
#include "debug.h"
#include <string.h>
#include <stdio.h>
#include <wtypes.h>

extern HWND	                   g_hWnd;
extern HRESULT				   g_hResult;
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

void logMessage(const char* format, ...) {
	va_list args;
    va_start(args, format);    
	TCHAR szBuffer[4096]; // get rid of this hard-coded buffer
    vsnprintf(szBuffer, 4095, format, args);
    OutputDebugString(szBuffer);
    va_end(args);
}