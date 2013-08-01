// Plug.cpp : Implementation of CPlug

#include "stdafx.h"
#include "Plug.h"
#include "wndtools.h"

_ATL_FUNC_INFO FuncInfo_DocumentOpen = { CC_STDCALL, VT_EMPTY, 1, { VT_BYREF|VT_USERDEFINED }   };
_ATL_FUNC_INFO FuncInfo_NewDocument = { CC_STDCALL, VT_EMPTY, 1, { VT_BYREF|VT_USERDEFINED }   };


CPlug::CPlug() 
{
}


HRESULT
CPlug::FinalConstruct()
{
	return S_OK;
}


void
CPlug::FinalRelease()
{
}


STDMETHODIMP CPlug::OnConnection( IDispatch *dispApplication, addin::ext_ConnectMode connectMode, IDispatch* dispAddIn, SAFEARRAY** custom )
{
	HRESULT hr = S_OK;
	CComPtr<word::_Application> ifWord;

	if( SUCCEEDED(hr) ) {
	    hr = dispApplication->QueryInterface( &ifWord );
	}

	m_dispApplication = dispApplication;

	if( SUCCEEDED(hr) ) {
		hr = EventImpl_Word::DispEventAdvise( dispApplication );
	}

	return hr;
}


STDMETHODIMP CPlug::OnDisconnection( addin::ext_DisconnectMode removeMode, SAFEARRAY** custom )
{
    return S_OK;
}


STDMETHODIMP CPlug::OnAddInsUpdate( SAFEARRAY** custom )
{
    UNREFERENCED_PARAMETER( custom );
    return S_OK;
}

STDMETHODIMP CPlug::OnStartupComplete( SAFEARRAY** custom )
{
    UNREFERENCED_PARAMETER( custom );
    return S_OK;
}

STDMETHODIMP CPlug::OnBeginShutdown( SAFEARRAY** custom )
{
    UNREFERENCED_PARAMETER( custom );
    return S_OK;
}

STDMETHODIMP CPlug::GetCustomUI( BSTR RibbonID, BSTR* pRibbonXml )
{
    HINSTANCE hinst = _AtlBaseModule.GetResourceInstance();
    HRSRC hrsrc = FindResource( hinst, MAKEINTRESOURCE(IDR_RIBBON), L"XML" );
    HGLOBAL hres = LoadResource( hinst, hrsrc );
    void* resource = LockResource( hres );
    char* CustomUIXml = (char*)resource;
    HRESULT hr = S_OK;
    
    if( !pRibbonXml ) {
        hr = E_POINTER;
    }
    else {
        CComBSTR CustomUiStr( CustomUIXml );
        *pRibbonXml = CustomUiStr.Detach( );
    }
    return hr;
}


STDMETHODIMP CPlug::OnDocumentOpen( word::_Document* ifDoc )
{
	MessageBox( NULL, L"hello", L"world", MB_OK );
    return S_OK;
}

STDMETHODIMP CPlug::OnNewDocument ( word::_Document* ifDoc )
{
	return S_OK;
}



STDMETHODIMP CPlug::Smile( IDispatch* dispRibbonCtrl )
{
    HRESULT hr;
    CComPtr<mso::IRibbonControl> ifRibbonCtrl;
    hr = dispRibbonCtrl->QueryInterface( &ifRibbonCtrl );
    highlight();
    return hr;
}


void CPlug::highlight( )
{
	HRESULT hr = S_OK;
	CComPtr<word::_Application> ifWord;
	CComPtr<word::_Document> ifDoc;
	CComPtr<word::Windows> ifWindows;
	CComPtr<word::Window> ifWindow;
	long numWindows;
	CComBSTR name;
	HWND hwndTop;
	HWND hwnd;

	hr = m_dispApplication.QueryInterface( &ifWord );
	hr = ifWord->get_ActiveDocument( &ifDoc );
	hr = ifDoc->get_Windows( &ifWindows );

	hr = ifDoc->get_Name( &name );

	hr = ifWindows->get_Count( &numWindows );
	ATLTRACE( "There are currently %d windows for current document" );

	CComVariant ndxVar((long)0);
	hr = ifWindows->Item( &ndxVar, &ifWindow );

	// Find top window with title
	std::wstring title(name);
	title += L" - Microsoft Word";
	hwndTop = findWindow( [title](HWND hwnd) -> bool {
		wchar_t ttl[200];
		GetWindowText( hwnd, ttl, _countof(ttl) );
		ATLTRACE( "window: %ls\n", ttl );
		return title == ttl;
	});


	// Find the child window
	hwnd = findChildWindow( hwndTop, [](HWND hwnd) -> bool {
		wchar_t ttl[200];
		GetWindowText( hwnd, ttl, _countof(ttl) );
		ATLTRACE( "child window: %ls\n", ttl );
		return !wcscmp( ttl, L"Microsoft Word Document" );
	});

	doHighlight(hwnd);
}


void CPlug::doHighlight( HWND hwnd )
{
	HDC hdc;
	RECT rect;
	PAINTSTRUCT ps;

	GetClientRect( hwnd, &rect );

	hdc = BeginPaint( hwnd, &ps );
	MoveToEx( hdc, 0, 0, NULL );
	LineTo( hdc, 50, 50 );
	EndPaint( hwnd, &ps );
}

