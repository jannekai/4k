#include "intro.h"
#include <stdio.h>

/*************************************************************************
* External function and variable declarations
*************************************************************************/
extern HRESULT IntroInit();
extern void IntroLoop(long sample);

extern HWND	                   g_hWnd;
extern D3D_DRIVER_TYPE         g_driverType;
extern D3D_FEATURE_LEVEL       g_featureLevel;
extern ID3D11Device*           g_pd3dDevice;
extern ID3D11DeviceContext*    g_pImmediateContext;
extern IDXGISwapChain*         g_pSwapChain;
extern ID3D11RenderTargetView* g_pRenderTargetView;


/*************************************************************************
* Initialization parameters not needed in intro side
*************************************************************************/
HWAVEOUT				g_hWaveOut;
SAMPLE_TYPE				g_lpSoundBuffer[MAX_SAMPLES*2];

#pragma data_seg(".wavefmt")
WAVEFORMATEX waveFMT =
{
	#ifdef FLOAT_32BIT
		WAVE_FORMAT_IEEE_FLOAT,
	#else
		WAVE_FORMAT_PCM,
	#endif		
	2,									// channels
	SAMPLE_RATE,						// samples per sec
	SAMPLE_RATE*sizeof(SAMPLE_TYPE)*2,	// bytes per sec
	sizeof(SAMPLE_TYPE)*2,				// block alignment;
	sizeof(SAMPLE_TYPE)*8,				// bits per sample
	0									// extension not needed
};

#pragma data_seg(".wavehdr")
WAVEHDR waveHDR = 
{
	(LPSTR) g_lpSoundBuffer, 
	MAX_SAMPLES * sizeof(SAMPLE_TYPE) * 2,	// MAX_SAMPLES*sizeof(float)*2(stereo)
	0, 
	0, 
	0, 
	0, 
	0, 
	0
};

#pragma data_seg(".mmtime")
MMTIME mmTime = 
{ 
	TIME_SAMPLES,
	0
};

/*************************************************************************
* Initialize sound system
*************************************************************************/
#pragma code_seg(".initsound")
void  InitSound()
{
#ifdef USE_SOUND_THREAD
	// thx to xTr1m/blu-flame for providing a smarter and smaller way to create the thread :)
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)_4klang_render, g_lpSoundBuffer, 0, 0);
#else
	_4klang_render(g_lpSoundBuffer);
#endif
	waveOutOpen(&g_hWaveOut, WAVE_MAPPER, &waveFMT, NULL, 0, CALLBACK_NULL);
	waveOutPrepareHeader(g_hWaveOut, &waveHDR, sizeof(waveHDR));
	waveOutWrite(g_hWaveOut, &waveHDR, sizeof(waveHDR));	
}

/*************************************************************************
* Main entrypoint for release version 
*************************************************************************/
#pragma code_seg(".mainrelease")
void MainRelease( void )
{    
	// g_hWnd = CreateWindow("static",0,WS_POPUP|WS_VISIBLE,0,0,X_RES,Y_RES,0,0,0,0);
	g_hWnd = CreateWindow("static",0,WS_POPUP|WS_VISIBLE,0,0,GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN),0,0,0,0);
	
	if (!g_hWnd) {
		return;
	}

	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferCount = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.Height = Y_RES;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;	
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.Width = X_RES;	
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.Flags = 0;
	sd.OutputWindow = g_hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Windowed = true;

	D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_11_0
        // D3D_FEATURE_LEVEL_10_1,        
		// D3D_FEATURE_LEVEL_10_0
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	D3D11CreateDeviceAndSwapChain(
		NULL, g_driverType, NULL, 0, featureLevels, numFeatureLevels,
		D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &g_featureLevel, &g_pImmediateContext);

	// Create render target view
	ID3D11Texture2D* pBackBuffer = NULL;
    g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_pRenderTargetView);
    pBackBuffer->Release();
	g_pImmediateContext->OMSetRenderTargets(1, &g_pRenderTargetView, NULL);
	
	// Setup the viewport
	D3D11_VIEWPORT vp;
    vp.Width = (float)X_RES;
    vp.Height = (float)Y_RES;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0.0f;
    vp.TopLeftY = 0.0f;
    g_pImmediateContext->RSSetViewports(1, &vp);	
	
	ShowCursor(0);  

	IntroInit();
	InitSound();

    do {
		waveOutGetPosition(g_hWaveOut, &mmTime, sizeof(MMTIME));				
		IntroLoop(mmTime.u.sample);		        
    } while (mmTime.u.sample < 153*SAMPLE_RATE && !GetAsyncKeyState(VK_ESCAPE));

    ExitProcess(0);
}
