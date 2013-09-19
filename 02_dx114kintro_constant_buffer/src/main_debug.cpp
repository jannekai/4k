#include "intro.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "bass.h"

#pragma warning(disable : 4996)

extern HWND	                   g_hWnd;
extern D3D_DRIVER_TYPE         g_driverType;
extern D3D_FEATURE_LEVEL       g_featureLevel;
extern ID3D11Device*           g_pd3dDevice;
extern ID3D11DeviceContext*    g_pImmediateContext;
extern IDXGISwapChain*         g_pSwapChain;
extern ID3D11RenderTargetView* g_pRenderTargetView;


/*************************************************************************
* Variables and parameters
*************************************************************************/
HINSTANCE				g_hInstance;
HDC						g_hDC;
HGLRC					g_hRC;
HRESULT				    g_hResult;

int						g_fullScreen;
char                    g_wndClass[] = { 'C','U','B','I','C','L','E' };
HSTREAM	                g_stream;
long		            g_currentFrame = 0;
QWORD	                g_repeatStart = 0;
QWORD	                g_repeatEnd = LONG_MAX;
QWORD	                g_currentPos = 0;
double	                g_currentTime = 0;
double	                g_lastTime = 0;
bool		            g_done = false;
bool		            g_pause = false;

/*************************************************************************
* Update window title
*************************************************************************/
void UpdateTitle() {
	static int	frame = 0;
	static char	title[128];

	if (g_fullScreen || g_currentTime <= 0) {
		return;
	}
	if (g_currentTime > g_lastTime && (g_currentTime - g_lastTime) < 0.01) {
		return;
	}

	sprintf(title, "time: %f pos: %I64d beg: %I64d end: %I64d", g_currentTime, g_currentPos, g_repeatStart, g_repeatEnd);
	SetWindowText(g_hWnd, title);
}

/*************************************************************************
* Initialize bass
*************************************************************************/
void InitSound() 
{
	if (!BASS_Init(-1, 44100, 0, g_hWnd, 0)) {
		OutputDebugString("Failed to initialize bass");
		ExitProcess(0);
	}

	g_stream = BASS_StreamCreateFile(false, "../data/4kintro.mp3", 0, 0, 0);
	if (!g_stream) {
		OutputDebugString("Failed to load mp3");
		ExitProcess(0);
	}
	BASS_Start();
	BASS_ChannelPlay(g_stream, false);
}

/*************************************************************************
* Window proc
*************************************************************************/
static LRESULT CALLBACK WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if (uMsg == WM_SYSCOMMAND && (wParam==SC_SCREENSAVE || wParam==SC_MONITORPOWER)) {
		return 0;
	}
	if (uMsg==WM_CLOSE || uMsg==WM_DESTROY || (uMsg==WM_KEYDOWN && wParam==VK_ESCAPE)) {
		PostQuitMessage(0);
        return 0;
	}
    if ((uMsg==WM_CHAR || uMsg==WM_KEYDOWN) && wParam==VK_ESCAPE) {
		PostQuitMessage(0);
		return 0;
	}
    return (DefWindowProc(hWnd,uMsg,wParam,lParam));
}

/*************************************************************************
* Window Close
*************************************************************************/
void WindowClose()
{
	if (g_hDC) {
		ReleaseDC(g_hWnd, g_hDC );
	}

	if(g_hWnd) {
		DestroyWindow(g_hWnd );
	}

	UnregisterClass(g_wndClass, g_hInstance);

	if (g_fullScreen) {
		ChangeDisplaySettings(0, 0);
		while (ShowCursor(1) < 0) {
		}
	}
}

/*************************************************************************
* Window initialization
*************************************************************************/
bool WindowOpen()
{
    DWORD			dwExStyle, dwStyle;
    DEVMODE			dmScreenSettings;
    RECT			rec;
    WNDCLASS		wc;

    ZeroMemory( &wc, sizeof(WNDCLASS) );
    wc.style         = CS_OWNDC|CS_HREDRAW|CS_VREDRAW;
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = g_hInstance;
    wc.lpszClassName = g_wndClass;
    wc.hbrBackground=(HBRUSH)CreateSolidBrush(0x00785838);
	
    if (!RegisterClass(&wc)) {
        return false;
	}

    if (g_fullScreen) {
        dmScreenSettings.dmSize       = sizeof(DEVMODE);
        dmScreenSettings.dmFields     = DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;
        dmScreenSettings.dmBitsPerPel = 32;
        dmScreenSettings.dmPelsWidth  = X_RES;
        dmScreenSettings.dmPelsHeight = Y_RES;

        if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL) {
            return false;
		}

        dwExStyle = WS_EX_APPWINDOW;
        dwStyle   = WS_VISIBLE | WS_POPUP;
		while (ShowCursor(0) >=0);
    }
    else {
        dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
        dwStyle   = WS_VISIBLE | WS_CAPTION | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_SYSMENU;
    }

    rec.left   = 0;
    rec.top    = 0;
    rec.right  = X_RES;
    rec.bottom = Y_RES;

    AdjustWindowRect( &rec, dwStyle, 0 );

    g_hWnd = CreateWindowEx( 
		dwExStyle, 
		wc.lpszClassName, 
		"CUBICLE", 
		dwStyle,
		(GetSystemMetrics(SM_CXSCREEN)-rec.right+rec.left)>>1,
		(GetSystemMetrics(SM_CYSCREEN)-rec.bottom+rec.top)>>1,
		rec.right-rec.left, 
		rec.bottom-rec.top, 
		0, 
		0, 
		g_hInstance, 0 );

    if (!g_hWnd) {
        return false;
	}

    g_hDC = GetDC(g_hWnd);
	if(!g_hDC) {
        return false;
	}
    
    return true;
}

/*************************************************************************
* Initialize dx11 device
*************************************************************************/
HRESULT InitDevice()
{
	UINT createDeviceFlags = 0;
	#ifdef DEBUG_DEVICE
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	#endif

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
	sd.Windowed = WINDOWED;

	D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_11_0
        // D3D_FEATURE_LEVEL_10_1,        
		// D3D_FEATURE_LEVEL_10_0
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);
	D3D_FEATURE_LEVEL featureLevel;

	g_hResult = D3D11CreateDeviceAndSwapChain(
		NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevels, numFeatureLevels,
		D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pImmediateContext);

	if (FAILED(g_hResult)) {
		return g_hResult;
	}

	// Create render target view
	ID3D11Texture2D* pBackBuffer = NULL;
    g_hResult = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
    if (FAILED(g_hResult )) {
        return g_hResult;
	}

	g_hResult = g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_pRenderTargetView);
    pBackBuffer->Release();
	if (FAILED(g_hResult)) {
		return g_hResult;
	}
	
	g_pImmediateContext->OMSetRenderTargets(1, &g_pRenderTargetView, NULL);

	// Setup the viewport
	D3D11_VIEWPORT vp;
    vp.Width = (float)X_RES;
    vp.Height = (float)Y_RES;
    vp.MinDepth = 0.0;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0.0f;
    vp.TopLeftY = 0.0f;
    g_pImmediateContext->RSSetViewports(1, &vp);

	return S_OK;
}

/*************************************************************************
* Clean device
*************************************************************************/
void CleanupDevice()
{
	if (g_pImmediateContext) g_pImmediateContext->ClearState();
    if (g_pRenderTargetView) g_pRenderTargetView->Release();
    if (g_pSwapChain)        g_pSwapChain->Release();
    if (g_pImmediateContext) g_pImmediateContext->Release();
    if (g_pd3dDevice)        g_pd3dDevice->Release();
}

/*************************************************************************
* Main entrypoint for debug build
*************************************************************************/
int WINAPI WinMain(HINSTANCE instance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    MSG msg;

	g_hInstance = GetModuleHandle(0);
	OutputDebugString("Starting...\n");

    if (!WindowOpen()) {
		OutputDebugString("Failed to open window\n");
		WindowClose();
		return 0;
	}

	if (FAILED(InitDevice())) {
		CleanupDevice();
		return 0;
	}

	IntroInit();
	InitSound();
   
    while (!g_done) {
		g_currentFrame++;
		g_lastTime = g_currentTime;		
		g_currentPos = BASS_ChannelGetPosition(g_stream, BASS_POS_BYTE);
		g_currentTime = BASS_ChannelBytes2Seconds(g_stream, g_currentPos);
		
		if (g_pause) {
			Sleep(100);
		}

	    while (PeekMessage(&msg,0,0,0,PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
				g_done = true;
			}

		    TranslateMessage(&msg);
            DispatchMessage(&msg);

			if (WM_KEYDOWN == msg.message && VK_SPACE == LOWORD(msg.wParam)) {
				if (!g_pause) {
					BASS_ChannelPause(g_stream);
					g_pause = true;
				} else {
					BASS_ChannelPlay(g_stream, false);
					g_pause = false;
				}
			}
			
			if (WM_KEYDOWN == msg.message && VK_LEFT == LOWORD(msg.wParam)) {
				if (g_pause) {
					BASS_ChannelSetPosition(g_stream, g_currentPos-44100, BASS_POS_BYTE);
				} else {
					BASS_ChannelSetPosition(g_stream, g_currentPos-44100*4, BASS_POS_BYTE);
				}
			}

			if (WM_KEYDOWN == msg.message && VK_RIGHT == LOWORD(msg.wParam)) {
				if (g_pause) {
					BASS_ChannelSetPosition(g_stream, g_currentPos+44100, BASS_POS_BYTE);
				} else {
					BASS_ChannelSetPosition(g_stream, g_currentPos+44100*4, BASS_POS_BYTE);
				}
			}

			if (WM_KEYDOWN == msg.message && VK_DOWN == LOWORD(msg.wParam)) {
				BASS_ChannelSetPosition(g_stream, g_currentPos-44100*20, BASS_POS_BYTE);
			}

			if (WM_KEYDOWN == msg.message && VK_UP == LOWORD(msg.wParam)) {
				BASS_ChannelSetPosition(g_stream, g_currentPos+44100*20, BASS_POS_BYTE);
			}

			if (WM_KEYDOWN == msg.message && VK_F1 == LOWORD(msg.wParam)) {
				g_repeatStart = g_currentPos;
				g_repeatEnd = LONG_MAX;
			}

			if (WM_KEYDOWN == msg.message && VK_F2 == LOWORD(msg.wParam)) {
				g_repeatEnd = g_currentPos;
			}

			if (WM_KEYDOWN == msg.message && VK_RETURN == LOWORD(msg.wParam)) {
				g_repeatStart = 0;
				g_repeatEnd = LONG_MAX;
			}
        }

		IntroLoop((long)(g_currentTime*44100.0));
		UpdateTitle();

		if (g_currentPos > g_repeatEnd) {
			BASS_ChannelSetPosition(g_stream, g_repeatStart, BASS_POS_BYTE);
		}
		BASS_Update(0);
    }

	BASS_StreamFree(g_stream);
	BASS_Free();
	
	WindowClose();

    return 0;
}
