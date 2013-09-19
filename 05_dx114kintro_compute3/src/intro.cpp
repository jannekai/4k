#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dcompiler.h>
#include "fp.h"
#include "config.h"
#include "4klang.h"
#include "mmsystem.h"
#include "symbols.h"

#ifdef _DEBUG
	#include "debug.h"
	static const char * SHADER_HLSL = "../data/shader.hlsl";
	#define LOAD_SHADER(shader, entrypoint, version, pBlob) { loadShader(shader, entrypoint, version, &pBlob); }
	#define LOG(e,...) { logMessage(e,__VA_ARGS__); }
#else
	#include "shader.h"
	#define LOAD_SHADER(shader, entrypoint, version, pBlob) { D3DX11CompileFromMemory(shader, strlen(shader), NULL, NULL, NULL, entrypoint, version, 0, 0, NULL, &pBlob, NULL, NULL); }
	#define LOG(e)
#endif

/*************************************************************************
*
*************************************************************************/
HWND						g_hWnd;
ID3D11Device*				g_pDevice;
ID3D11DeviceContext*		g_pImmediateContext;
IDXGISwapChain*				g_pSwapChain;
ID3D11RenderTargetView*		g_pRenderTargetView;

ID3D11ComputeShader*		g_pGradientShader;
ID3D11ComputeShader*		g_pCheckersShader;
ID3D11UnorderedAccessView*	g_pOutputUAV;

/*************************************************************************
*
*************************************************************************/
struct Data {
	float r, g, b, a;
};

ID3D11ShaderResourceView*  g_pDataSRV;
ID3D11UnorderedAccessView* g_pDataUAV;
ID3D11Buffer*              g_pData;

/*************************************************************************
*
*************************************************************************/
#pragma code_seg(".zeromem")
void ZeroMem(char *ptr, size_t size) {
	while (size > 0) {
		size--;
		ptr[size] = 0;
	}
}

/*************************************************************************
* Intro initialization
*************************************************************************/
#pragma code_seg(".introinit")
void IntroInit()
{
	ID3DBlob* pBlob = NULL;

	LOAD_SHADER(SHADER_HLSL, "Gradient", "cs_5_0", pBlob);
	CHK(g_pDevice->CreateComputeShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &g_pGradientShader));

	LOAD_SHADER(SHADER_HLSL, "Checkers", "cs_5_0", pBlob);
	CHK(g_pDevice->CreateComputeShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &g_pCheckersShader));
	
	// Create UAV from backbuffer
	ID3D11Texture2D* pTexture = NULL;
    g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pTexture);
	CHK(g_pDevice->CreateUnorderedAccessView(pTexture, NULL, &g_pOutputUAV));
	pTexture->Release();
			
	D3D11_BUFFER_DESC sbDesc;
	ZeroMem((char*) &sbDesc, sizeof(sbDesc));
	sbDesc.StructureByteStride = sizeof(Data);
	sbDesc.ByteWidth = sizeof(Data) * X_RES * Y_RES;
	sbDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	sbDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	sbDesc.Usage = D3D11_USAGE_DEFAULT;
	CHK(g_pDevice->CreateBuffer(&sbDesc, NULL, &g_pData));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMem((char*) &srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.NumElements =  X_RES * Y_RES;
	CHK(g_pDevice->CreateShaderResourceView(g_pData, &srvDesc, &g_pDataSRV));

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	ZeroMem((char*) &uavDesc, sizeof(uavDesc));
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.NumElements = X_RES * Y_RES;
	CHK(g_pDevice->CreateUnorderedAccessView(g_pData, &uavDesc, &g_pDataUAV));

	g_pImmediateContext->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
	g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
}

/*************************************************************************
* Intro mainloop
*************************************************************************/
#pragma code_seg(".introloop")
void IntroLoop(long sample) 
{	
	UINT initCounts = 0;
	ID3D11ShaderResourceView* srvNullView[1] = { NULL };
	ID3D11UnorderedAccessView* uavNullView[2] = { NULL, NULL };
	ID3D11RenderTargetView* rtvNullView[1] = { NULL };

	g_pImmediateContext->OMSetRenderTargets(1, rtvNullView, NULL);
	
	g_pImmediateContext->CSSetShader(g_pGradientShader, NULL, 0);
	g_pImmediateContext->CSSetUnorderedAccessViews(0, 1, &g_pDataUAV, &initCounts);
	g_pImmediateContext->Dispatch(X_RES, Y_RES, 1);	
	g_pImmediateContext->CSSetShader(NULL, NULL, 0);
	g_pImmediateContext->CSSetUnorderedAccessViews(0, 1, uavNullView, &initCounts);
	
	ID3D11UnorderedAccessView* uavViews[2] = { NULL, g_pOutputUAV };
	g_pImmediateContext->CSSetShader(g_pCheckersShader, NULL, 0);
	g_pImmediateContext->CSSetShaderResources(0, 1, &g_pDataSRV);
	g_pImmediateContext->CSSetUnorderedAccessViews(0, 2, uavViews, &initCounts);
	g_pImmediateContext->Dispatch(X_RES, Y_RES, 1);	
	g_pImmediateContext->CSSetUnorderedAccessViews(0, 2, uavNullView, &initCounts);	
	g_pImmediateContext->CSSetShaderResources(0, 1, srvNullView);	

	g_pImmediateContext->OMSetRenderTargets(1, &g_pRenderTargetView, NULL);
	CHK(g_pSwapChain->Present(0, 0));
}
