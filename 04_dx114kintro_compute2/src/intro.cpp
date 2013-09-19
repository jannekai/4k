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
#define F_PS "PS"
#define F_GS "GS"
#define F_VS "VS"
#define F_CS "CS"
#define LOG(e,...) { logMessage(e,__VA_ARGS__); }
#else
#include "shader.h"
#define F_PS "PS"
#define F_GS "GS"
#define F_VS "VS"
#define F_CS "CS"
#define LOG(e)
#endif

// #define USE_PIXEL_SHADER
// #define USE_GEOMETRY_SHADER
// #define USE_VERTEX_SHADER
#define USE_COMPUTE_SHADER

/*************************************************************************
*
*************************************************************************/
HWND						g_hWnd;
ID3D11Device*				g_pd3dDevice;
ID3D11DeviceContext*		g_pImmediateContext;
IDXGISwapChain*				g_pSwapChain;
ID3D11RenderTargetView*		g_pRenderTargetView;
ID3D11ComputeShader*		g_pComputeShader;
ID3D11UnorderedAccessView*	g_pComputeOutputUAV;

/*************************************************************************
*
*************************************************************************/
struct StructuredBuffer {
	float x, y, z, w;
	float r, g, b, a;
};

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

	#ifdef _DEBUG		
		#ifdef USE_VERTEX_SHADER
		loadShader("../data/shader.hlsl", F_VS, "vs_5_0", &pBlob);	
		CHK(g_pd3dDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &g_pVertexShader));
		RELEASE(pBlob);
		#endif

		#ifdef USE_PIXEL_SHADER
		loadShader("../data/shader.hlsl", F_PS, "ps_5_0", &pBlob);
		CHK(g_pd3dDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &g_pPixelShader));
		RELEASE(pBlob);
		#endif

		#ifdef USE_GEOMETRY_SHADER
		loadShader("../data/shader.hlsl", F_GS, "gs_5_0", &pBlob);
		CHK(g_pd3dDevice->CreateGeometryShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &g_pGeometryShader));
		RELEASE_AND_NULL(pBlob);
		#endif

		#ifdef USE_COMPUTE_SHADER
		loadShader("../data/shader.hlsl", F_CS, "cs_5_0", &pBlob);
		CHK(g_pd3dDevice->CreateComputeShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &g_pComputeShader));
		RELEASE_AND_NULL(pBlob);
		#endif
	#else	
		#ifdef USE_VERTEX_SHADER
		D3DX11CompileFromMemory(shader_hlsl, strlen(shader_hlsl), NULL, NULL, NULL, F_VS, "vs_5_0", 0, 0, NULL, &pBlob, NULL, NULL);
		CHK(g_pd3dDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &g_pVertexShader));
		pBlob->Release();
		#endif

		#ifdef USE_PIXEL_SHADER
		D3DX11CompileFromMemory(shader_hlsl, strlen(shader_hlsl), NULL, NULL, NULL, F_PS, "ps_5_0", 0, 0, NULL, &pBlob, NULL, NULL);		
		CHK(g_pd3dDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &g_pPixelShader));
		pBlob->Release();
		#endif

		#ifdef USE_GEOMETRY_SHADER
		D3DX11CompileFromMemory(shader_hlsl, strlen(shader_hlsl), NULL, NULL, NULL, F_GS, "gs_5_0", 0, 0, NULL, &pBlob, NULL, NULL);		
		CHK(g_pd3dDevice->CreateGeometryShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &g_pGeometryShader));
		RELEASE_AND_NULL(pBlob);
		#endif

		#ifdef USE_COMPUTE_SHADER
		D3DX11CompileFromMemory(shader_hlsl, strlen(shader_hlsl), NULL, NULL, NULL, F_CS, "cs_5_0", 0, 0, NULL, &pBlob, NULL, NULL);
		CHK(g_pd3dDevice->CreateComputeShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &g_pComputeShader));
		RELEASE_AND_NULL(pBlob);
		#endif
	#endif	

	ID3D11Texture2D* pTexture = NULL;
    g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pTexture);
	CHK(g_pd3dDevice->CreateUnorderedAccessView(pTexture, NULL, &g_pComputeOutputUAV));
	pTexture->Release();
		
	g_pImmediateContext->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
	g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
}

/*************************************************************************
* Intro mainloop
*************************************************************************/
#pragma code_seg(".introloop")
void IntroLoop(long sample) 
{	
	// Do compute shader pass
	UINT initCounts = 0;
	g_pImmediateContext->CSSetShader(g_pComputeShader, NULL, 0);
	g_pImmediateContext->CSSetUnorderedAccessViews(0, 1, &g_pComputeOutputUAV, &initCounts);
	g_pImmediateContext->Dispatch(X_RES, Y_RES, 1);
	ID3D11UnorderedAccessView* aUAViewsNULL[ 1 ] = { NULL };
	g_pImmediateContext->CSSetUnorderedAccessViews(0, 1, aUAViewsNULL, &initCounts);

	CHK(g_pSwapChain->Present(0, 0));
}
