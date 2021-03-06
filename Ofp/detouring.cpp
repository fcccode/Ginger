#include "stdafx.h"
#include "trace.h"
#include "Plug.h"
#include "Subcls.h"
#include "llcom.h"

ID2D1RenderTarget* g_ifCurrentDrawTransaction = NULL;
ID2D1RenderTarget* g_currentRtWithGlyph = NULL;


type_GetDC stub_GetDC;
type_ReleaseDC stub_ReleaseDC;
type_BitBlt stub_BitBlt;
type_UpdateLayeredWindow stub_UpdateLayeredWindow;
type_DispatchMessage stub_DispatchMessage;
type_CreateDXGIFactory stub_CreateDXGIFactory;
type_CreateDXGIFactory1 stub_CreateDXGIFactory1;

type_CreateSwapChain stub_CreateSwapChain;
type_CreateSwapChainForHwnd stub_CreateSwapChainForHwnd;
type_Present stub_Present;
type_Present1 stub_Present1;

type_CreateWicBitmapRenderTarget stub_CreateWicBitmapRenderTarget;
type_BeginDraw stub_BeginDraw;
type_EndDraw stub_EndDraw;
type_DrawGlyphRun stub_DrawGlyphRun;
type_CreateBitmapFromWicBitmap stub_CreateBitmapFromWicBitmap;
type_DrawBitmap stub_DrawBitmap;




HDC WINAPI my_GetDC( HWND hwnd )
{
	log_frame( "user", u::info ) << log_var(hwnd) << u::endh;

	HDC hdc = (*stub_GetDC)( hwnd );

	frame << log_ret(hdc);
	return hdc;
}

int WINAPI my_ReleaseDC( HWND hwnd, HDC hdc )
{
	log_frame( "user", u::info ) << log_var(hwnd) << log_var(hdc) << u::endh;

	int result = (*stub_ReleaseDC)( hwnd, hdc );

	frame << log_ret(result);
	return result;
}

int WINAPI my_BitBlt( HDC hdc, int x, int y, int cx, int cy, HDC hdcSrc, int x1, int y1, DWORD rop )
{
	log_frame( "gdi", u::info ) << log_var(hdc) << log_var(x) << log_var(y) << log_var(cx) << log_var(cy) << log_var(rop) << u::endh;

	BOOL result = (*stub_BitBlt)( hdc, x, y, cx, cy, hdcSrc, x1, y1, rop );

	frame << log_ret(result);
	return result;
}


BOOL WINAPI my_UpdateLayeredWindow( HWND hwnd, HDC hdcDst, POINT *pptDst, SIZE *psize, HDC hdcSrc, POINT *pptSrc, COLORREF crKey, BLENDFUNCTION *pblend, DWORD dwFlags )
{
	log_frame( "gdi", u::info ) << log_var(hwnd) << u::endh;
	BOOL result = (*stub_UpdateLayeredWindow)( hwnd, hdcDst, pptDst, psize, hdcSrc, pptSrc, crKey, pblend, dwFlags );
	frame << log_ret(result);
	return result;
}


LRESULT WINAPI my_DispatchMessage( const MSG *pmsg )
{
	log_frame( "user", u::info ) << log_var(pmsg->hwnd) << log_var_f(pmsg->message, "%x") << u::endh;
	LRESULT result = (*stub_DispatchMessage)(pmsg);

	if( pmsg->message == 0x401 ) {
		CPlug* pplug = CPlug::inst();
		if( pplug ) {
			CSubclsWnd* pwnd = pplug->getSubclsWnd();
			if( pwnd ) {
		        //CPlug::inst()->getSubclsWnd()->highlight();
			}
		}
	}

	frame << log_ret(result);
	return result; 
}

HRESULT WINAPI my_CreateDXGIFactory1( REFIID riid, void **ppFactory )
{
	log_frame( "dxgi", u::info ) << u::endh;
	HRESULT result = (*stub_CreateDXGIFactory1)(riid, ppFactory);
	frame << log_ret(result);
	return result;
}

HRESULT WINAPI my_CreateDXGIFactory( REFIID riid, void** ppFactory )
{
	log_frame( "dxgi", u::info ) << u::endh;
	HRESULT result = (*stub_CreateDXGIFactory)(riid, ppFactory);
	frame << log_ret(result);
	return result;
}


HRESULT STDMETHODCALLTYPE my_CreateSwapChain( IDXGIFactory* This, IUnknown* ifDevice, DXGI_SWAP_CHAIN_DESC* pdesc, IDXGISwapChain** ppSwapChain )
{
	log_frame( "dxgi", u::info ) << log_var(ifDevice) << log_var(pdesc->OutputWindow) << log_var(pdesc->BufferCount) <<
		log_var(pdesc->BufferDesc.Width) << log_var(pdesc->BufferDesc.Height) << u::endh;
	HRESULT result = (*stub_CreateSwapChain)( This, ifDevice, pdesc, ppSwapChain );
	frame << log_ret(result);
	return result;
}


HRESULT STDMETHODCALLTYPE my_CreateSwapChainForHwnd( IDXGIFactory2* This, IUnknown* pDevice, HWND hWnd, const DXGI_SWAP_CHAIN_DESC1* pDesc, 
	const DXGI_SWAP_CHAIN_FULLSCREEN_DESC* pFullscreenDesc, IDXGIOutput* pRestrictToOutput, IDXGISwapChain1** ppSwapChain )
{
	log_frame( "dxgi", u::info ) << log_var(pDevice) << log_var(hWnd) << 
		log_var(pDesc->Width) << log_var(pDesc->Height) << log_var(pDesc->BufferCount) << u::endh;
	HRESULT result = (*stub_CreateSwapChainForHwnd)( This, pDevice, hWnd, pDesc, pFullscreenDesc, pRestrictToOutput, ppSwapChain );
	frame << log_var(*ppSwapChain) << log_ret(result);
	return result;
}


HRESULT STDMETHODCALLTYPE my_Present( IDXGISwapChain* This, UINT SyncInterval, UINT Flags )
{
	log_frame( "dxgi", u::info ) << log_var(This) << u::endh;
	HRESULT hr;

	DXGI_SWAP_CHAIN_DESC desc;
	hr = This->GetDesc( &desc );
	frame << log_var(desc.BufferDesc.Width) << log_var(desc.BufferDesc.Height) << log_var(desc.OutputWindow);

	CComPtr<IDXGIDevice> dxgiDevice;
	hr = This->GetDevice( __uuidof(IDXGIDevice), (void**)&dxgiDevice );
	frame << log_var(dxgiDevice);

	HRESULT result = stub_Present(This, SyncInterval, Flags );
	frame << log_ret(result);
	return result;
}

HRESULT STDMETHODCALLTYPE my_Present1( IDXGISwapChain1* This, UINT SyncInterval, UINT PresentFlags, const DXGI_PRESENT_PARAMETERS *pPresentParameters )
{
	log_frame( "dxgi", u::info ) << log_var(This) << u::endh;
	HRESULT hr;

	DXGI_SWAP_CHAIN_DESC1 desc;
	hr = This->GetDesc1( &desc );
	frame << log_var(desc.Width) << log_var(desc.Height);

	HWND hwnd;
	hr = This->GetHwnd( &hwnd );
	frame << log_var(hwnd);

	CComPtr<IDXGIDevice> dxgiDevice;
	hr = This->GetDevice( __uuidof(IDXGIDevice), (void**)&dxgiDevice );
	frame << log_var(dxgiDevice);

	HRESULT result = (*stub_Present1)(This, SyncInterval, PresentFlags, pPresentParameters );
	frame << log_ret(result);
	return result;
}


void STDMETHODCALLTYPE my_BeginDraw( ID2D1RenderTarget* This )
{
	log_frame( "d2d1", u::info ) << log_var(This) << u::endh;
	(*stub_BeginDraw)( This );
	return;
}


HRESULT STDMETHODCALLTYPE my_EndDraw( ID2D1RenderTarget* This, D2D1_TAG *tag1, D2D1_TAG *tag2 )
{
	HRESULT hr;
	log_frame( "d2d1", u::info ) << log_var(This) << u::endh;

    if( g_currentRtWithGlyph == This ) {

	    CComPtr<ID2D1SolidColorBrush> ifBrushBlack;
	    CComPtr<ID2D1SolidColorBrush> ifBrushRed;
	    CComPtr<ID2D1SolidColorBrush> ifBrushGreen;
	    CComPtr<ID2D1SolidColorBrush> ifBrushBlue;
        CComPtr<ID2D1SolidColorBrush> ifBrushOrange;

	    hr = This->CreateSolidColorBrush( D2D1::ColorF( D2D1::ColorF::Black ), &ifBrushBlack );
	    hr = This->CreateSolidColorBrush( D2D1::ColorF( D2D1::ColorF::Red ), &ifBrushRed );
	    hr = This->CreateSolidColorBrush( D2D1::ColorF( D2D1::ColorF::Green ), &ifBrushGreen );
	    hr = This->CreateSolidColorBrush( D2D1::ColorF( D2D1::ColorF::Blue ), &ifBrushBlue );
        hr = This->CreateSolidColorBrush( D2D1::ColorF( D2D1::ColorF::OrangeRed, 0.5f ), &ifBrushOrange );

	    D2D1_RECT_F rect;
	    rect.left = 0;
	    rect.top = 0;
	    rect.right = 1024;
	    rect.bottom = 2014;
	    //This->PushAxisAlignedClip( &rect, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE );
	    //This->DrawLine( D2D1::Point2F(0.0f, 0.0f), D2D1::Point2F(480.0f, 480.0f), ifBrush );

        if( CPlug::inst()->IsDoingGrid() ) {
	        for( int i=0; i<20; ++i ) {

		        for( int j=0; j<20; ++j ) {
			        float cy = i * 50.0f;
			        float cx = j * 50.0f;

			        ID2D1SolidColorBrush* ifBrush = 
				        ( i<10 && j < 10 ) ? ifBrushBlack :
				        ( i<10 ) ? ifBrushRed :
				        ( j<10 ) ? ifBrushGreen :
				        ifBrushBlue;

			        This->DrawLine( D2D1::Point2F( cx - 5.0f, cy - 5.0f ), D2D1::Point2F( cx + 5.0f, cy + 5.0f ), ifBrush );
			        This->DrawLine( D2D1::Point2F( cx + 5.0f, cy - 5.0f ), D2D1::Point2F( cx - 5.0f, cy + 5.0f ), ifBrush );
		        }
	        }
	        //This->PopAxisAlignedClip();
        }

        if( CPlug::inst()->IsDoingRosebud() ) {
            CPlug* pplug = CPlug::inst();
            if( pplug ) {
                int num;
                int i;

                num = pplug->numRosebuds( );

                for( i = 0; i < num; ++i ) {
                    int ileft, itop, iright, ibottom;
                    bool f = pplug->enumRosebud( i, &ileft, &itop, &iright, &ibottom );

                    if( f ) {
                        float left = (float)ileft;
                        float top = (float)itop;
                        float right = (float)iright;
                        float bottom = (float)ibottom;

                        //This->DrawLine( D2D1::Point2F( left, top ), D2D1::Point2F( right, top ), ifBrushBlack );
                        //This->DrawLine( D2D1::Point2F( right, top ), D2D1::Point2F( right, bottom ), ifBrushBlack );
                        //This->DrawLine( D2D1::Point2F( right, bottom ), D2D1::Point2F( left, bottom ), ifBrushBlack );
                        //This->DrawLine( D2D1::Point2F( left, bottom ), D2D1::Point2F( left, top ), ifBrushBlack );

                        D2D1_RECT_F rc = D2D1::RectF( left, top, right, bottom );
                        This->FillRectangle( &rc, ifBrushOrange );

                    }
                }
            }

        }
        g_currentRtWithGlyph = NULL;
    }

	HRESULT result = stub_EndDraw(This, tag1, tag2 );
	frame << log_ret(result);
	return result;
}


HRESULT STDMETHODCALLTYPE my_CreateWicBitmapRenderTarget( ID2D1Factory* This, IWICBitmap* target, const D2D1_RENDER_TARGET_PROPERTIES *renderTargetProperties, ID2D1RenderTarget** renderTarget )
{
	log_frame( "d2d1", u::info ) << log_var(This) << log_var(target) << u::endh;

	// Some data about the bitmap
	UINT width, height;
	double dpix, dpiy;
	target->GetSize( &width, &height );
	target->GetResolution( &dpix, &dpiy );
	frame << log_var(width) << log_var(height);
	frame << log_var(dpix) << log_var(dpiy);

	HRESULT result = (*stub_CreateWicBitmapRenderTarget)( This, target, renderTargetProperties, renderTarget );
	
	frame << log_var(*renderTarget) << log_ret(result);
	return result;
}


void STDMETHODCALLTYPE my_DrawGlyphRun( ID2D1RenderTarget* This, D2D1_POINT_2F baselineOrigin, const DWRITE_GLYPH_RUN* glyphRun, ID2D1Brush *foregroundBrush, DWRITE_MEASURING_MODE measuringMode )
{
	log_frame( "d2d1", u::info ) << log_var(This) << log_var(baselineOrigin.x) << log_var(baselineOrigin.y) <<
		log_var(glyphRun->glyphCount) << u::endh;

    g_currentRtWithGlyph = This;

	(*stub_DrawGlyphRun)( This, baselineOrigin, glyphRun, foregroundBrush, measuringMode );
	
	return;
}

HRESULT STDMETHODCALLTYPE my_CreateBitmapFromWicBitmap( ID2D1RenderTarget* This, IWICBitmapSource* wicBitmapSource, const D2D1_BITMAP_PROPERTIES* bitmapProperties, ID2D1Bitmap** bitmap )
{
	log_frame( "d2d1", u::info ) << log_var(This) << log_var(wicBitmapSource);

	double dpix, dpiy;
	UINT width, height;
	wicBitmapSource->GetResolution( &dpix, &dpiy );
	wicBitmapSource->GetSize( &width, &height );
	frame << log_var(dpix) << log_var(dpiy) << log_var(width) << log_var(height) << u::endh;

	HRESULT result = (*stub_CreateBitmapFromWicBitmap)( This, wicBitmapSource, bitmapProperties, bitmap );

	frame << log_var(*bitmap) << log_ret(result);
	return result;
}

void STDMETHODCALLTYPE my_DrawBitmap( ID2D1RenderTarget* This, ID2D1Bitmap* bitmap, const D2D1_RECT_F* destRc, FLOAT opacity, 
								  D2D1_BITMAP_INTERPOLATION_MODE interpolationMode, const D2D1_RECT_F* srcRc )
{
	log_frame( "d2d1", u::info ) << log_var(This) << log_var(bitmap) << log_var(opacity);

	D2D1_SIZE_F bitmapSize = bitmap->GetSize( );
	frame << log_var(bitmapSize.width) << log_var(bitmapSize.height);

	if( destRc ) {
		frame << log_var(destRc->left) << log_var(destRc->top) << log_var(destRc->right) << log_var(destRc->bottom);
	}

	if( srcRc ) {
		frame << log_var(srcRc->left) << log_var(srcRc->top) << log_var(srcRc->right) << log_var(srcRc->bottom);
	}

	frame << u::endh;

	(*stub_DrawBitmap)(This, bitmap, destRc, opacity, interpolationMode, srcRc );

	return;
}

// ----------------------------------------

struct Hook
{
    bool m_publicAddress;
    void* m_targetAddress;
    const char* m_targetDll;
    const char* m_targetSymbol;
    void** m_stub;
    void* m_myDetour;

public:
    Hook( void* targetAddress, void** stub, void* detour ) 
        :m_publicAddress( true ), m_targetAddress( targetAddress ), m_targetDll( NULL ), m_targetSymbol( NULL ), m_stub( stub ), m_myDetour( detour )
    { }

    Hook( const char* targetDll, const char* targetSymbol, void** stub, void* detour )
        :m_publicAddress( false ), m_targetAddress( NULL ), m_targetDll( targetDll ), m_targetSymbol( targetSymbol ), m_stub( stub ), m_myDetour( detour )
    { }
};

Hook g_hooks[] = {
    Hook( ::GetDC, &(void*&)stub_GetDC, my_GetDC ),
    Hook( ::ReleaseDC, &(void*&)stub_ReleaseDC, my_ReleaseDC ),
    Hook( ::BitBlt, &(void*&)stub_BitBlt, my_BitBlt ),
    Hook( ::UpdateLayeredWindow, &(void*&)stub_UpdateLayeredWindow, my_UpdateLayeredWindow ),
    Hook( ::DispatchMessage, &(void*&)stub_DispatchMessage, my_DispatchMessage ),
    Hook( ::CreateDXGIFactory, &(void*&)stub_CreateDXGIFactory, my_CreateDXGIFactory ),

    Hook( "dxgi.dll", symbol_CreateSwapChain, &(void*&)stub_CreateSwapChain, my_CreateSwapChain ),
    Hook( "dxgi.dll", symbol_CreateSwapChainForHwnd, &(void*&)stub_CreateSwapChainForHwnd, my_CreateSwapChainForHwnd ),
    Hook( "dxgi.dll", symbol_Present, &(void*&)stub_Present, my_Present ),
    Hook( "dxgi.dll", symbol_Present1, &(void*&)stub_Present1, my_Present1 ),

    Hook( "d2d1.dll", symbol_CreateWicBitmapRenderTarget, &(void*&)stub_CreateWicBitmapRenderTarget, my_CreateWicBitmapRenderTarget ),
    Hook( "d2d1.dll", symbol_BeginDraw, &(void*&)stub_BeginDraw, my_BeginDraw ),
    Hook( "d2d1.dll", symbol_EndDraw, &(void*&)stub_EndDraw, my_EndDraw ),
    Hook( "d2d1.dll", symbol_DrawGlyphRun, &(void*&)stub_DrawGlyphRun, my_DrawGlyphRun ),
    Hook( "d2d1.dll", symbol_CreateBitmapFromWicBitmap, &(void*&)stub_CreateBitmapFromWicBitmap, my_CreateBitmapFromWicBitmap ),
    Hook( "d2d1.dll", symbol_DrawBitmap, &(void*&)stub_DrawBitmap, my_DrawBitmap )
};

void resolveAddresses()
{
    for( auto& hook : g_hooks )
    {
        if( !hook.m_publicAddress ) {
            void* addr = DetourFindFunction( hook.m_targetDll, hook.m_targetSymbol );
            hook.m_targetAddress = addr;
        }
    }
}


void attachDetours( )
{
	LONG result, result1;

	resolveAddresses();

    DetourTransactionBegin( );
    DetourUpdateThread( GetCurrentThread() );

    for( auto& hook : g_hooks )
    {
        *hook.m_stub = hook.m_targetAddress;

        if( *hook.m_stub ) {
            result1 = DetourAttach( hook.m_stub, hook.m_myDetour );
        }
    }

    result = DetourTransactionCommit( );
}


void detachDetours( )
{
    DetourTransactionBegin( );
    DetourUpdateThread( GetCurrentThread() );

    for( auto& hook : g_hooks )
    {
        if( *hook.m_stub ) {
            DetourDetach( hook.m_stub, hook.m_myDetour );
        }
    }

    DetourTransactionCommit( );
}
