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
#else
#include "shader.h"
#endif

#define USE_PIXEL_SHADER
#define USE_GEOMETRY_SHADER
#define USE_VERTEX_SHADER
// #define USE_COMPUTE_SHADER

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

struct ConstantBuffer {
	float time;
	float beat;
	float measure;
	float hit;
};

#ifdef _DEBUG
	#define CHK(e) { if ((e) < 0) { ExitProcess(0); } }	
#else
	#define CHK(e) { (e) }
#endif

#define RELEASE_AND_NULL(p) { p->Release(); p = NULL; }
#define RELEASE(p) { p->Release(); }

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
		D3DX11CompileFromMemory(shader_hlsl, strlen(shader_hlsl), NULL, NULL, NULL, F_VS, "vs_5_0", 0, 0, NULL, &pBlob, NULL, NULL);
		CHK(g_pd3dDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &g_pVertexShader));
		pBlob->Release();

		D3DX11CompileFromMemory(shader_hlsl, strlen(shader_hlsl), NULL, NULL, NULL, F_PS, "ps_5_0", 0, 0, NULL, &pBlob, NULL, NULL);		
		CHK(g_pd3dDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &g_pPixelShader));
		pBlob->Release();

		D3DX11CompileFromMemory(shader_hlsl, strlen(shader_hlsl), NULL, NULL, NULL, F_GS, "gs_5_0", 0, 0, NULL, &pBlob, NULL, NULL);		
		CHK(g_pd3dDevice->CreateGeometryShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &g_pGeometryShader));
		RELEASE_AND_NULL(pBlob);

		D3DX11CompileFromMemory(shader_hlsl, strlen(shader_hlsl), NULL, NULL, NULL, F_CS, "cs_5_0", 0, 0, NULL, &pBlob, NULL, NULL);
		CHK(g_pd3dDevice->CreateComputeShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &g_pComputeShader));
		RELEASE_AND_NULL(pBlob);
	#endif	

	D3D11_BUFFER_DESC cbDesc;
	ZeroMem((char*) &cbDesc, sizeof(D3D11_BUFFER_DESC));
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.ByteWidth = sizeof(ConstantBuffer);
	CHK(g_pd3dDevice->CreateBuffer(&cbDesc, NULL, &g_pConstantBuffer));

	g_pImmediateContext->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
	g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
}

/*************************************************************************
* Intro mainloop
*************************************************************************/
#pragma code_seg(".introloop")
void IntroLoop(long sample) 
{	
	// Update shader constants information
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	CHK(g_pImmediateContext->Map(g_pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));
	ConstantBuffer* data = (ConstantBuffer*)(mappedResource.pData);
	data->time = float(sample)/44100.0f;
	data->beat = (float(BPM) / 60.0f * float(sample)) / 44100.0f;
	data->measure = float(int(data->beat / 4.0f));
	data->hit = data->beat - data->measure * 4.0f;
	debugTime(data->time, data->beat, data->measure, data->hit);
	g_pImmediateContext->Unmap(g_pConstantBuffer, 0);

	float ClearColor[4] = { 0.0f, 0.125f, 0.9f, 1.0f };
	g_pImmediateContext->ClearRenderTargetView(g_pRenderTargetView, ClearColor);

	g_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pConstantBuffer);
	g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
	g_pImmediateContext->GSSetShader(g_pGeometryShader, NULL, 0);	
	g_pImmediateContext->PSSetShader(g_pPixelShader, NULL, 0);
    g_pImmediateContext->Draw(2, 0);

	CHK(g_pSwapChain->Present(0, 0));
}
