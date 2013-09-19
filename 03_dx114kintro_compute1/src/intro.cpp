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

#define USE_PIXEL_SHADER
// #define USE_GEOMETRY_SHADER
#define USE_VERTEX_SHADER
#define USE_COMPUTE_SHADER

/*************************************************************************
* NOTE: The [numthreads(x,y,z)] in CS must match the THREADS_X, THREADS_Y,
* THREADS_Z defines.
*************************************************************************/
#ifdef USE_COMPUTE_SHADER
#define THREAD_GROUPS_X		1280
#define THREAD_GROUPS_Y		1
#define THREAD_GROUPS_Z		1
#define THREADS_X			720
#define THREADS_Y			1
#define THREADS_Z			1
#define THREAD_BUFFER_SIZE	THREAD_GROUPS_X * THREADS_X * THREAD_GROUPS_Y * THREADS_Y * THREAD_GROUPS_Z * THREADS_Z
#endif

/*************************************************************************
*
*************************************************************************/
HWND	                   g_hWnd;

ID3D11Device*              g_pd3dDevice;
ID3D11DeviceContext*       g_pImmediateContext;
IDXGISwapChain*            g_pSwapChain;
ID3D11RenderTargetView*    g_pRenderTargetView;

ID3D11VertexShader*        g_pVertexShader;
ID3D11GeometryShader*      g_pGeometryShader;
ID3D11PixelShader*         g_pPixelShader;
ID3D11ComputeShader*	   g_pComputeShader;

ID3D11ShaderResourceView*  g_pStructuredBufferSRV;
ID3D11UnorderedAccessView* g_pStructuredBufferUAV;
ID3D11Buffer*              g_pStructuredBuffer;
ID3D11Buffer*              g_pConstantBuffer;

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
* 
*************************************************************************/
void CreateBufferResourceAndViews(UINT elementSize, UINT numElements, ID3D11Buffer **ppBuffer, ID3D11ShaderResourceView ** ppBufferSRV, ID3D11UnorderedAccessView **ppBufferUAV)
{
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMem((char*) &bufferDesc, sizeof(bufferDesc));
	bufferDesc.StructureByteStride = elementSize;
	bufferDesc.ByteWidth = elementSize * numElements;
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	// If we are creating any views of the buffer, we will be binding the resource to a shader
	if (ppBufferSRV || ppBufferUAV) {
		bufferDesc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
	}

	// If we are not creating an unordered access view, we can enable optimization
	if (!ppBufferUAV) {
		bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	} else {
		bufferDesc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	}

	// Create the buffer resource
	CHK(g_pd3dDevice->CreateBuffer(&bufferDesc, NULL, ppBuffer));

	// If we wish to create a shader resource view (read-only)
	if (ppBufferSRV) {
		D3D11_SHADER_RESOURCE_VIEW_DESC viewDescSRV;
		ZeroMem((char*) &viewDescSRV, sizeof(viewDescSRV));
		viewDescSRV.Format = DXGI_FORMAT_UNKNOWN;
		viewDescSRV.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		viewDescSRV.Buffer.FirstElement = 0;
		viewDescSRV.Buffer.NumElements = numElements;
		CHK(g_pd3dDevice->CreateShaderResourceView(*ppBuffer, &viewDescSRV, ppBufferSRV));
	}

	// If we weish to create an unordered access view
	if (ppBufferUAV) {
		D3D11_UNORDERED_ACCESS_VIEW_DESC viewDescUAV;
		ZeroMem((char*) &viewDescUAV, sizeof(viewDescUAV));
		viewDescUAV.Format = DXGI_FORMAT_UNKNOWN;
		viewDescUAV.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		viewDescUAV.Buffer.FirstElement = 0;
		viewDescUAV.Buffer.NumElements = numElements;
		CHK(g_pd3dDevice->CreateUnorderedAccessView(*ppBuffer, &viewDescUAV, ppBufferUAV));
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

	CreateBufferResourceAndViews(sizeof(StructuredBuffer), THREAD_BUFFER_SIZE, &g_pStructuredBuffer, &g_pStructuredBufferSRV, &g_pStructuredBufferUAV);

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
	g_pImmediateContext->CSSetUnorderedAccessViews(0, 1, &g_pStructuredBufferUAV, &initCounts);
	g_pImmediateContext->Dispatch(THREAD_GROUPS_X, THREAD_GROUPS_Y, THREAD_GROUPS_Z);

	// Unbind resources
	ID3D11UnorderedAccessView* aUAViewsNULL[ 1 ] = { NULL };
	g_pImmediateContext->CSSetUnorderedAccessViews(0, 1, aUAViewsNULL, &initCounts);

	// Clear
	float ClearColor[4] = { 0.0f, 0.125f, 0.9f, 1.0f };
	g_pImmediateContext->ClearRenderTargetView(g_pRenderTargetView, ClearColor);

	// Draw the result
	g_pImmediateContext->VSSetShaderResources(0, 1, &g_pStructuredBufferSRV);
	g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
	g_pImmediateContext->PSSetShader(g_pPixelShader, NULL, 0);
    g_pImmediateContext->Draw(THREAD_BUFFER_SIZE, 0);
	
	// Unbind resources
	ID3D11ShaderResourceView* aSRViewsNULL[ 1 ] = { NULL };
	g_pImmediateContext->VSSetShaderResources(0, 1, aSRViewsNULL);

	CHK(g_pSwapChain->Present(0, 0));
}
