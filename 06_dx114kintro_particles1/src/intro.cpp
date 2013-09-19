#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dcompiler.h>
#include <xnamath.h>
#include "fp.h"
#include "config.h"
#include "4klang.h"
#include "mmsystem.h"
#include "defines.h"

#ifdef _DEBUG
	#include "debug.h"
	const char * SHADER_HLSL = "../data/shader.hlsl";
#else
	#include "shader.h"
	#define LOAD_CS(shader, entrypoint, pBlob, pPointer) { \
		D3DX11CompileFromMemory(shader, strlen(shader), NULL, NULL, NULL, entrypoint, "cs_5_0", 0, 0, NULL, &pBlob, NULL, NULL); \
		g_pDevice->CreateComputeShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &pPointer); \
	}
	#define LOAD_VS(shader, entrypoint, pBlob, pPointer) { \
		D3DX11CompileFromMemory(shader, strlen(shader), NULL, NULL, NULL, entrypoint, "vs_5_0", 0, 0, NULL, &pBlob, NULL, NULL); \
		g_pDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &pPointer); \
	}
	#define LOAD_GS(shader, entrypoint, pBlob, pPointer) { \
		D3DX11CompileFromMemory(shader, strlen(shader), NULL, NULL, NULL, entrypoint, "gs_5_0", 0, 0, NULL, &pBlob, NULL, NULL); \
		g_pDevice->CreateGeometryShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &pPointer); \
	}
	#define LOAD_PS(shader, entrypoint, pBlob, pPointer) { \
		D3DX11CompileFromMemory(shader, strlen(shader), NULL, NULL, NULL, entrypoint, "ps_5_0", 0, 0, NULL, &pBlob, NULL, NULL); \
		g_pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &pPointer); \
	}
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
ID3D11RasterizerState*		g_pRasterizerState;

ID3D11ComputeShader*		g_pComputeShader;
ID3D11VertexShader*			g_pVertexShader;
ID3D11PixelShader*			g_pPixelShader;

/*************************************************************************
*
*************************************************************************/
struct Data {
	XMFLOAT4 position;
	XMFLOAT4 color;
	XMFLOAT4 time;
};
ID3D11ShaderResourceView*  g_pDataSRV;
ID3D11UnorderedAccessView* g_pDataUAV;
ID3D11Buffer*              g_pData;

struct StaticConstants {	
	XMFLOAT4	quadData[6];
	XMFLOAT4	randomData[64];
};
struct StaticConstants g_staticConstants = {
	XMFLOAT4(-1.0f,  1.0f, 0.0f, 1.0f),
	XMFLOAT4( 1.0f,	1.0f, 0.0f, 1.0f),
	XMFLOAT4(-1.0f, -1.0f, 0.0f, 1.0f),
	XMFLOAT4(-1.0f, -1.0f, 0.0f, 1.0f),
	XMFLOAT4( 1.0f,	1.0f, 0.0f, 1.0f),
	XMFLOAT4( 1.0f, -1.0f, 0.0f, 1.0f)
};
ID3D11Buffer* g_pStaticConstantBuffer;

struct FrameConstants {
	XMMATRIX	world;
	XMMATRIX	view;
	XMMATRIX	projection;
	UINT		sample;
	float		time;
};
struct FrameConstants g_frameConstants;
ID3D11Buffer* g_pFrameConstantBuffer;

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
* Init random table
*************************************************************************/
#pragma code_seg(".initrandomtable")
void InitializeRandomTable() {
	srand(12345);
	for (int i = 0; i < 64; i++) {
		g_staticConstants.randomData[i] = XMFLOAT4(
			float(rand()) / (RAND_MAX + 1.0f),
			float(rand()) / (RAND_MAX + 1.0f),
			float(rand()) / (RAND_MAX + 1.0f),
			float(rand()) / (RAND_MAX + 1.0f)
		);
	}
}

/*************************************************************************
* Intro initialization
*************************************************************************/
#pragma code_seg(".introinit")
void IntroInit()
{
	InitializeRandomTable();

	ID3DBlob* pBlob = NULL;

	LOAD_CS(SHADER_HLSL, "CS", pBlob, g_pComputeShader);
	LOAD_VS(SHADER_HLSL, "VS", pBlob, g_pVertexShader);
	LOAD_PS(SHADER_HLSL, "PS", pBlob, g_pPixelShader);
			
	D3D11_BUFFER_DESC sbDesc;
	ZeroMem((char*) &sbDesc, sizeof(sbDesc));
	sbDesc.StructureByteStride = sizeof(Data);
	sbDesc.ByteWidth = sizeof(Data) * BUFFER_SIZE;
	sbDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	sbDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	sbDesc.Usage = D3D11_USAGE_DEFAULT;
	CHK(g_pDevice->CreateBuffer(&sbDesc, NULL, &g_pData));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMem((char*) &srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.NumElements = BUFFER_SIZE;
	CHK(g_pDevice->CreateShaderResourceView(g_pData, &srvDesc, &g_pDataSRV));

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	ZeroMem((char*) &uavDesc, sizeof(uavDesc));
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.NumElements = BUFFER_SIZE;
	CHK(g_pDevice->CreateUnorderedAccessView(g_pData, &uavDesc, &g_pDataUAV));

	D3D11_BUFFER_DESC cbDesc;

	ZeroMem((char*) &cbDesc, sizeof(cbDesc));
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.Usage = D3D11_USAGE_DEFAULT;
	cbDesc.ByteWidth = sizeof(StaticConstants);
	CHK(g_pDevice->CreateBuffer(&cbDesc, NULL, &g_pStaticConstantBuffer));

	ZeroMem((char*) &cbDesc, sizeof(cbDesc));
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.Usage = D3D11_USAGE_DEFAULT;
	cbDesc.ByteWidth = sizeof(FrameConstants);
	CHK(g_pDevice->CreateBuffer(&cbDesc, NULL, &g_pFrameConstantBuffer));
		
	g_pImmediateContext->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
	g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	
}

/*************************************************************************
* Intro mainloop
*************************************************************************/
#pragma code_seg(".updateconstantbuffer")
void UpdateConstantBuffer(long sample)
{
	XMVECTOR eye				= XMVectorSet(0.0f, 0.0f, -5.0f, 0.0f);
	XMVECTOR at					= XMVectorSet(0.0f, 0.0f,  0.0f, 0.0f);
	XMVECTOR up					= XMVectorSet(0.0f, 1.0f,  0.0f, 0.0f);

	g_frameConstants.world		= XMMatrixIdentity();
	g_frameConstants.world		= XMMatrixTranspose(g_frameConstants.world);
	g_frameConstants.view		= XMMatrixLookAtLH(eye, at, up);
	g_frameConstants.view		= XMMatrixTranspose(g_frameConstants.view);
	g_frameConstants.projection	= XMMatrixPerspectiveFovLH(XM_PIDIV2, float(X_RES) / float(Y_RES), 0.01f, 100.0f);
	g_frameConstants.projection	= XMMatrixTranspose(g_frameConstants.projection);
	g_frameConstants.sample		= sample;
	g_frameConstants.time		= double(sample)/44100.0f;

	g_pImmediateContext->UpdateSubresource(g_pFrameConstantBuffer, 0, NULL, &g_frameConstants, 0, 0);	
	g_pImmediateContext->UpdateSubresource(g_pStaticConstantBuffer, 0, NULL, &g_staticConstants, 0, 0);	
}

/*************************************************************************
* Intro mainloop
*************************************************************************/
#pragma code_seg(".introloop")
void IntroLoop(long sample) 
{	
	#ifdef _DEBUG
		static long lastCheck = 0;
		if (sample - lastCheck > 22100) {			
			lastCheck = sample;
			if (IsShaderModified(SHADER_HLSL)) {			
				ID3DBlob* pBlob = NULL;
				RELOAD_CS(SHADER_HLSL, "CS", pBlob, g_pComputeShader);			
				RELOAD_VS(SHADER_HLSL, "VS", pBlob, g_pVertexShader);
				RELOAD_PS(SHADER_HLSL, "PS", pBlob, g_pPixelShader);			
			}
		}		
	#endif

	UINT initCounts = 0;
	ID3D11ShaderResourceView*	srvNullView[2] = { NULL, NULL };
	ID3D11UnorderedAccessView*	uavNullView[2] = { NULL, NULL };
	ID3D11RenderTargetView*		rtvNullView[2] = { NULL, NULL };

	UpdateConstantBuffer(sample);	

	// Compute	
	g_pImmediateContext->CSSetShader(g_pComputeShader, NULL, 0);	
	g_pImmediateContext->CSSetConstantBuffers(0, 1, &g_pStaticConstantBuffer);
	g_pImmediateContext->CSSetConstantBuffers(1, 1, &g_pFrameConstantBuffer);	
	g_pImmediateContext->CSSetUnorderedAccessViews(0, 1, &g_pDataUAV, &initCounts);
	g_pImmediateContext->Dispatch(GROUPS_X, GROUPS_Y, GROUPS_Z);	
	g_pImmediateContext->CSSetUnorderedAccessViews(0, 1, uavNullView, &initCounts);	

	// Clear
	float clearColor[4] = { 0.1f, 0.2f, 0.3f, 1.0f };
	g_pImmediateContext->ClearRenderTargetView(g_pRenderTargetView, clearColor);

	// Draw
	g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
	g_pImmediateContext->PSSetShader(g_pPixelShader, NULL, 0);
	g_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pStaticConstantBuffer);	
	g_pImmediateContext->VSSetConstantBuffers(1, 1, &g_pFrameConstantBuffer);
	g_pImmediateContext->VSSetShaderResources(0, 1, &g_pDataSRV);	
	g_pImmediateContext->Draw(BUFFER_SIZE*6, 0);	
	g_pImmediateContext->VSSetShaderResources(0, 1, srvNullView);	

	CHK(g_pSwapChain->Present(0, 0));
}
