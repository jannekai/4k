#include "intro.h"
#include "debug.h"
#include <string.h>
#include <stdio.h>
#include <wtypes.h>

#pragma warning( disable : 4996 )

/*************************************************************************
* Globals defined in main_debug.cpp
*************************************************************************/
extern HWND	                   g_hWnd;
extern ID3D11Device*           g_pDevice;
extern ID3D11DeviceContext*    g_pImmediateContext;
extern IDXGISwapChain*         g_pSwapChain;
extern ID3D11RenderTargetView* g_pRenderTargetView;

/*************************************************************************
*
*************************************************************************/
bool LoadShader(const char* fileName, const char* entryPoint, const char* shaderModel, ID3DBlob** ppBlobOut, bool reload) {
	bool success = true;
	ID3DBlob* pErrorBlob;	
	DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
	HRESULT hr = D3DX11CompileFromFile(fileName, NULL, NULL, entryPoint, shaderModel, shaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL);

	if (FAILED(hr)) {
		LOG("!!!!!!!! Failed to compile shader %s %s %s\n", fileName, entryPoint, shaderModel);
		success = false;
	}
	if (pErrorBlob != NULL) {
		LOG("%s", (char*) pErrorBlob->GetBufferPointer());
		pErrorBlob->Release();
		success = false;
	}		
	if (!reload && !success) {
		ExitProcess(0);
	}
	return success;
}

/*************************************************************************
*
*************************************************************************/
bool IsShaderModified(const char* fileName) {
	static DWORD previousTime = 0;

	HANDLE hFile;
	FILETIME ftWrite;
	
	hFile = CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	GetFileTime(hFile, NULL, NULL, &ftWrite);
	CloseHandle(hFile);

	if (previousTime == 0) {
		previousTime = ftWrite.dwLowDateTime;
		return false;
	} else if (ftWrite.dwLowDateTime != previousTime) {
		return true;
	}
	return false;
}

/*************************************************************************
*
*************************************************************************/
void LoadPixelShader(const char* fileName, const char* entryPoint, const char* shaderModel, ID3DBlob** ppBlobOut, ID3D11PixelShader** ppShader, bool reload) {	
	bool success = LoadShader(fileName, entryPoint, shaderModel, ppBlobOut, reload);
	if (success) {
		ID3D11PixelShader* pTmpShader;
		HRESULT hr = g_pDevice->CreatePixelShader((*ppBlobOut)->GetBufferPointer(), (*ppBlobOut)->GetBufferSize(), NULL, &pTmpShader);
		if (SUCCEEDED(hr)) {
			*ppShader = pTmpShader;
		}
	}
}

/*************************************************************************
*
*************************************************************************/
void LoadGeometryShader(const char* fileName, const char* entryPoint, const char* shaderModel, ID3DBlob** ppBlobOut, ID3D11GeometryShader** ppShader, bool reload) {
	bool success = LoadShader(fileName, entryPoint, shaderModel, ppBlobOut, reload);
	if (success) {
		ID3D11GeometryShader* pTmpShader;
		HRESULT hr = g_pDevice->CreateGeometryShader((*ppBlobOut)->GetBufferPointer(), (*ppBlobOut)->GetBufferSize(), NULL, &pTmpShader);
		if (SUCCEEDED(hr)) {
			*ppShader = pTmpShader;
		}
	}
}

/*************************************************************************
*
*************************************************************************/
void LoadVertexShader(const char* fileName, const char* entryPoint, const char* shaderModel, ID3DBlob** ppBlobOut, ID3D11VertexShader** ppShader, bool reload) {
	bool success = LoadShader(fileName, entryPoint, shaderModel, ppBlobOut, reload);
	if (success) {
		ID3D11VertexShader* pTmpShader;
		HRESULT hr = g_pDevice->CreateVertexShader((*ppBlobOut)->GetBufferPointer(), (*ppBlobOut)->GetBufferSize(), NULL, &pTmpShader);
		if (SUCCEEDED(hr)) {
			*ppShader = pTmpShader;
		}
	}
}

/*************************************************************************
*
*************************************************************************/
void LoadComputeShader(const char* fileName, const char* entryPoint, const char* shaderModel, ID3DBlob** ppBlobOut, ID3D11ComputeShader** ppShader, bool reload) {
	bool success = LoadShader(fileName, entryPoint, shaderModel, ppBlobOut, reload);
	if (success) {
		ID3D11ComputeShader* pTmpShader;
		HRESULT hr = g_pDevice->CreateComputeShader((*ppBlobOut)->GetBufferPointer(), (*ppBlobOut)->GetBufferSize(), NULL, &pTmpShader);
		if (SUCCEEDED(hr)) {
			*ppShader = pTmpShader;
		}
	}
}

/*************************************************************************
*
*************************************************************************/
void LogMessage(const char* format, ...) {
	va_list args;
    va_start(args, format);    
	TCHAR szBuffer[4096]; // get rid of this hard-coded buffer
    vsnprintf(szBuffer, 4095, format, args);
    OutputDebugString(szBuffer);
    va_end(args);
}

/*************************************************************************
*
*************************************************************************/
ID3D11Buffer* CreateAndCopyToDebugBuf(ID3D11Buffer* pBuffer) {
    ID3D11Buffer* debugBuffer = NULL;

    D3D11_BUFFER_DESC sbDesc;
    pBuffer->GetDesc(&sbDesc);
    sbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    sbDesc.Usage = D3D11_USAGE_STAGING;
    sbDesc.BindFlags = 0;
    sbDesc.MiscFlags = 0;
	CHK(g_pDevice->CreateBuffer(&sbDesc, NULL, &debugBuffer));
	g_pImmediateContext->CopyResource(debugBuffer, pBuffer);

    return debugBuffer;
}
