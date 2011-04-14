/*
	CWindowsXPTheme.h
	Classe di interfaccia verso i temi di Windows XP.
	Il codice relativo all'interfaccia con l'API vera e propria e'
	stato ripreso e modificato dall'originale di David Yuheng Zhao.
	Luca Piergentili, 14/09/03
	lpiergentili@yahoo.com
*/
#ifndef _CWINDOWSXPTHEME_H
#define _CWINDOWSXPTHEME_H 1

#define _HAS_UXTHEME 1

#ifdef _HAS_UXTHEME
  #include <uxtheme.h>
#endif
#include "window.h"

// tipi specifici dell'API
#ifndef _HAS_UXTHEME
#ifndef _XPTHEME_TYPES_DEFINED
#define _XPTHEME_TYPES_DEFINED 1
  typedef HANDLE HTHEME;
  typedef RECT MARGINS;
  typedef int INTLIST;
#endif // _XPTHEME_TYPES_DEFINED
#endif // _HAS_UXTHEME

// messaggio inviato dall'os alle applicazioni per cambio tema
#ifndef WM_THEMECHANGED
  #define WM_THEMECHANGED 0x031A
#endif // WM_THEMECHANGED

// ricava il tipo di cambio dai parametri del messaggio di cui sopra
#define WINDOWSCLASSICSTYLETHEMEAPPLIED(wParam,lParam)	((WPARAM)wParam==(WPARAM)-1 && (LPARAM)lParam==0)
#define WINDOWSXPSTYLETHEMEAPPLIED(wParam,lParam)		((WPARAM)wParam!=(WPARAM)-1 && (LPARAM)lParam==1)

/*
	CWindowsXPTheme
*/
class CWindowsXPTheme
{
public:
	CWindowsXPTheme();
	virtual ~CWindowsXPTheme();

	// helpers
	BOOL		IsThemingSupported			(void);
	BOOL		IsWindowsClassicStyle		(void);

	// API
	HTHEME	OpenThemeData				(HWND hwnd,LPCWSTR pszClassList) {return((*((PFNOPENTHEMEDATA)GetProc("OpenThemeData",(void*)OpenThemeDataFail)))(hwnd,pszClassList));}
	HRESULT	CloseThemeData				(HTHEME hTheme) {return((*((PFNCLOSETHEMEDATA)GetProc("CloseThemeData",(void*)CloseThemeDataFail)))(hTheme));}
	HRESULT	DrawThemeBackground			(HTHEME hTheme,HDC hdc,int iPartId,int iStateId,const RECT* pRect,const RECT* pClipRect) {return((*((PFNDRAWTHEMEBACKGROUND)GetProc("DrawThemeBackground",(void*)DrawThemeBackgroundFail)))(hTheme,hdc,iPartId,iStateId,pRect,pClipRect));}
	HRESULT	DrawThemeText				(HTHEME hTheme,HDC hdc,int iPartId,int iStateId,LPCWSTR pszText,int iCharCount,DWORD dwTextFlags,DWORD dwTextFlags2,const RECT* pRect) {return((*((PFNDRAWTHEMETEXT)GetProc("DrawThemeText",(void*)DrawThemeTextFail)))(hTheme,hdc,iPartId,iStateId,pszText,iCharCount,dwTextFlags,dwTextFlags2,pRect));}
	HRESULT	GetThemeBackgroundContentRect	(HTHEME hTheme,HDC hdc,int iPartId,int iStateId,const RECT* pBoundingRect,RECT* pContentRect) {return((*((PFNGETTHEMEBACKGROUNDCONTENTRECT)GetProc("GetThemeBackgroundContentRect",(void*)GetThemeBackgroundContentRectFail)))(hTheme,hdc,iPartId,iStateId,pBoundingRect,pContentRect));}
	HRESULT	GetThemeBackgroundExtent		(HTHEME hTheme,HDC hdc,int iPartId,int iStateId,const RECT* pContentRect,RECT* pExtentRect) {return((*((PFNGETTHEMEBACKGROUNDEXTENT)GetProc("GetThemeBackgroundExtent",(void*)GetThemeBackgroundExtentFail)))(hTheme,hdc,iPartId,iStateId,pContentRect,pExtentRect));}
	HRESULT	GetThemePartSize			(HTHEME hTheme,HDC hdc,int iPartId,int iStateId,RECT* pRect,enum THEMESIZE eSize,SIZE* psz) {return((*((PFNGETTHEMEPARTSIZE)GetProc("GetThemePartSize",(void*)GetThemePartSizeFail)))(hTheme,hdc,iPartId,iStateId,pRect,eSize,psz));}
	HRESULT	GetThemeTextExtent			(HTHEME hTheme,HDC hdc,int iPartId,int iStateId,LPCWSTR pszText,int iCharCount,DWORD dwTextFlags,const RECT* pBoundingRect,RECT* pExtentRect) {return((*((PFNGETTHEMETEXTEXTENT)GetProc("GetThemeTextExtent",(void*)GetThemeTextExtentFail)))(hTheme,hdc,iPartId,iStateId,pszText,iCharCount,dwTextFlags,pBoundingRect,pExtentRect));}
	HRESULT	GetThemeTextMetrics			(HTHEME hTheme,HDC hdc,int iPartId,int iStateId,TEXTMETRIC* ptm) {return((*((PFNGETTHEMETEXTMETRICS)GetProc("GetThemeTextMetrics",(void*)GetThemeTextMetricsFail)))(hTheme,hdc,iPartId,iStateId,ptm));}
	HRESULT	GetThemeBackgroundRegion		(HTHEME hTheme,HDC hdc,int iPartId,int iStateId,const RECT* pRect,HRGN* pRegion) {return((*((PFNGETTHEMEBACKGROUNDREGION)GetProc("GetThemeBackgroundRegion",(void*)GetThemeBackgroundRegionFail)))(hTheme,hdc,iPartId,iStateId,pRect,pRegion));}
	HRESULT	HitTestThemeBackground		(HTHEME hTheme,HDC hdc,int iPartId,int iStateId,DWORD dwOptions,const RECT* pRect,HRGN hrgn,POINT ptTest,WORD* pwHitTestCode) {return((*((PFNHITTESTTHEMEBACKGROUND)GetProc("HitTestThemeBackground",(void*)HitTestThemeBackgroundFail)))(hTheme,hdc,iPartId,iStateId,dwOptions,pRect,hrgn,ptTest,pwHitTestCode));}
	HRESULT	DrawThemeEdge				(HTHEME hTheme,HDC hdc,int iPartId,int iStateId,const RECT* pDestRect,UINT uEdge,UINT uFlags,RECT* pContentRect) {return((*((PFNDRAWTHEMEEDGE)GetProc("DrawThemeEdge",(void*)DrawThemeEdgeFail)))(hTheme,hdc,iPartId,iStateId,pDestRect,uEdge,uFlags,pContentRect));}
	HRESULT	DrawThemeIcon				(HTHEME hTheme,HDC hdc,int iPartId,int iStateId,const RECT* pRect,HIMAGELIST himl,int iImageIndex) {return((*((PFNDRAWTHEMEICON)GetProc("DrawThemeIcon",(void*)DrawThemeIconFail)))(hTheme,hdc,iPartId,iStateId,pRect,himl,iImageIndex));}
	BOOL		IsThemePartDefined			(HTHEME hTheme,int iPartId,int iStateId) {return((*((PFNISTHEMEPARTDEFINED)GetProc("IsThemePartDefined",(void*)IsThemePartDefinedFail)))(hTheme,iPartId,iStateId));}
	BOOL		IsThemeBackgroundPartiallyTransparent(HTHEME hTheme,int iPartId,int iStateId) {return((*((PFNISTHEMEBACKGROUNDPARTIALLYTRANSPARENT)GetProc("IsThemeBackgroundPartiallyTransparent",(void*)IsThemeBackgroundPartiallyTransparentFail)))(hTheme,iPartId,iStateId));}
	HRESULT	GetThemeColor				(HTHEME hTheme,int iPartId,int iStateId,int iPropId,COLORREF* pColor) {return((*((PFNGETTHEMECOLOR)GetProc("GetThemeColor",(void*)GetThemeColorFail)))(hTheme,iPartId,iStateId,iPropId,pColor));}
	HRESULT	GetThemeMetric				(HTHEME hTheme,HDC hdc,int iPartId,int iStateId,int iPropId,int* piVal) {return((*((PFNGETTHEMEMETRIC)GetProc("GetThemeMetric",(void*)GetThemeMetricFail)))(hTheme,hdc,iPartId,iStateId,iPropId,piVal));}
	HRESULT	GetThemeString				(HTHEME hTheme,int iPartId,int iStateId,int iPropId,LPWSTR pszBuff,int cchMaxBuffChars) {return((*((PFNGETTHEMESTRING)GetProc("GetThemeString",(void*)GetThemeStringFail)))(hTheme,iPartId,iStateId,iPropId,pszBuff,cchMaxBuffChars));}
	HRESULT	GetThemeBool				(HTHEME hTheme,int iPartId,int iStateId,int iPropId,BOOL* pfVal) {return((*((PFNGETTHEMEBOOL)GetProc("GetThemeBool",(void*)GetThemeBoolFail)))(hTheme,iPartId,iStateId,iPropId,pfVal));}
	HRESULT	GetThemeInt				(HTHEME hTheme,int iPartId,int iStateId,int iPropId,int* piVal) {return((*((PFNGETTHEMEINT)GetProc("GetThemeInt",(void*)GetThemeIntFail)))(hTheme,iPartId,iStateId,iPropId,piVal));}
	HRESULT	GetThemeEnumValue			(HTHEME hTheme,int iPartId,int iStateId,int iPropId,int* piVal) {return((*((PFNGETTHEMEENUMVALUE)GetProc("GetThemeEnumValue",(void*)GetThemeEnumValueFail)))(hTheme,iPartId,iStateId,iPropId,piVal));}
	HRESULT	GetThemePosition			(HTHEME hTheme,int iPartId,int iStateId,int iPropId,POINT* pPoint) {return((*((PFNGETTHEMEPOSITION)GetProc("GetThemePosition",(void*)GetThemePositionFail)))(hTheme,iPartId,iStateId,iPropId,pPoint));}
	HRESULT	GetThemeFont				(HTHEME hTheme,HDC hdc,int iPartId,int iStateId,int iPropId,LOGFONT* pFont) {return((*((PFNGETTHEMEFONT)GetProc("GetThemeFont",(void*)GetThemeFontFail)))(hTheme,hdc,iPartId,iStateId,iPropId,pFont));}
	HRESULT	GetThemeRect				(HTHEME hTheme,int iPartId,int iStateId,int iPropId,RECT* pRect) {return((*((PFNGETTHEMERECT)GetProc("GetThemeRect",(void*)GetThemeRectFail)))(hTheme,iPartId,iStateId,iPropId,pRect));}
	HRESULT	GetThemeMargins			(HTHEME hTheme,HDC hdc,int iPartId,int iStateId,int iPropId,RECT* prc,MARGINS* pMargins) {return((*((PFNGETTHEMEMARGINS)GetProc("GetThemeMargins",(void*)GetThemeMarginsFail)))(hTheme,hdc,iPartId,iStateId,iPropId,prc,pMargins));}
	HRESULT	GetThemeIntList			(HTHEME hTheme,int iPartId,int iStateId,int iPropId,INTLIST* pIntList) {return((*((PFNGETTHEMEINTLIST)GetProc("GetThemeIntList",(void*)GetThemeIntListFail)))(hTheme,iPartId,iStateId,iPropId,pIntList));}
	HRESULT	GetThemePropertyOrigin		(HTHEME hTheme,int iPartId,int iStateId,int iPropId,enum PROPERTYORIGIN* pOrigin) {return((*((PFNGETTHEMEPROPERTYORIGIN)GetProc("GetThemePropertyOrigin",(void*)GetThemePropertyOriginFail)))(hTheme,iPartId,iStateId,iPropId,pOrigin));}
	HRESULT	SetWindowTheme				(HWND hwnd,LPCWSTR pszSubAppName,LPCWSTR pszSubIdList) {return((*((PFNSETWINDOWTHEME)GetProc("SetWindowTheme",(void*)SetWindowThemeFail)))(hwnd,pszSubAppName,pszSubIdList));}
	HRESULT	GetThemeFilename			(HTHEME hTheme,int iPartId,int iStateId,int iPropId,LPWSTR pszThemeFileName,int cchMaxBuffChars) {return((*((PFNGETTHEMEFILENAME)GetProc("GetThemeFilename",(void*)GetThemeFilenameFail)))(hTheme,iPartId,iStateId,iPropId,pszThemeFileName,cchMaxBuffChars));}
	COLORREF	GetThemeSysColor			(HTHEME hTheme,int iColorId) {return((*((PFNGETTHEMESYSCOLOR)GetProc("GetThemeSysColor",(void*)GetThemeSysColorFail)))(hTheme,iColorId));}
	HBRUSH	GetThemeSysColorBrush		(HTHEME hTheme,int iColorId) {return((*((PFNGETTHEMESYSCOLORBRUSH)GetProc("GetThemeSysColorBrush",(void*)GetThemeSysColorBrushFail)))(hTheme,iColorId));}
	BOOL		GetThemeSysBool			(HTHEME hTheme,int iBoolId) {return((*((PFNGETTHEMESYSBOOL)GetProc("GetThemeSysBool",(void*)GetThemeSysBoolFail)))(hTheme,iBoolId));}
	int		GetThemeSysSize			(HTHEME hTheme,int iSizeId) {return((*((PFNGETTHEMESYSSIZE)GetProc("GetThemeSysSize",(void*)GetThemeSysSizeFail)))(hTheme,iSizeId));}
	HRESULT	GetThemeSysFont			(HTHEME hTheme,int iFontId,LOGFONT* plf) {return((*((PFNGETTHEMESYSFONT)GetProc("GetThemeSysFont",(void*)GetThemeSysFontFail)))(hTheme,iFontId,plf));}
	HRESULT	GetThemeSysString			(HTHEME hTheme,int iStringId,LPWSTR pszStringBuff,int cchMaxStringChars) {return((*((PFNGETTHEMESYSSTRING)GetProc("GetThemeSysString",(void*)GetThemeSysStringFail)))(hTheme,iStringId,pszStringBuff,cchMaxStringChars));}
	HRESULT	GetThemeSysInt				(HTHEME hTheme,int iIntId,int* piValue) {return((*((PFNGETTHEMESYSINT)GetProc("GetThemeSysInt",(void*)GetThemeSysIntFail)))(hTheme,iIntId,piValue));}
	BOOL		IsThemeActive				(void) {return((*((PFNISTHEMEACTIVE)GetProc("IsThemeActive",(void*)IsThemeActiveFail)))());}
	BOOL		IsAppThemed				(void) {return((*((PFNISAPPTHEMED)GetProc("IsAppThemed",(void*)IsAppThemedFail)))());}	
	HTHEME	GetWindowTheme				(HWND hwnd) {return((*((PFNGETWINDOWTHEME)GetProc("GetWindowTheme",(void*)GetWindowThemeFail)))(hwnd));}
	HRESULT	EnableThemeDialogTexture		(HWND hwnd,DWORD dwFlags) {return((*((PFNENABLETHEMEDIALOGTEXTURE)GetProc("EnableThemeDialogTexture",(void*)EnableThemeDialogTextureFail)))(hwnd,dwFlags));}
	BOOL		IsThemeDialogTextureEnabled	(HWND hwnd) {return((*((PFNISTHEMEDIALOGTEXTUREENABLED)GetProc("IsThemeDialogTextureEnabled",(void*)IsThemeDialogTextureEnabledFail)))(hwnd));}
	DWORD	GetThemeAppProperties		(void) {return((*((PFNGETTHEMEAPPPROPERTIES)GetProc("GetThemeAppProperties",(void*)GetThemeAppPropertiesFail)))());}
	void		SetThemeAppProperties		(DWORD dwFlags) {((*((PFNSETTHEMEAPPPROPERTIES)GetProc("SetThemeAppProperties",(void*)SetThemeAppPropertiesFail)))(dwFlags));}
	HRESULT	GetCurrentThemeName			(LPWSTR pszThemeFileName,int cchMaxNameChars,LPWSTR pszColorBuff,int cchMaxColorChars,LPWSTR pszSizeBuff,int cchMaxSizeChars) {return((*((PFNGETCURRENTTHEMENAME)GetProc("GetCurrentThemeName",(void*)GetCurrentThemeNameFail)))(pszThemeFileName,cchMaxNameChars,pszColorBuff,cchMaxColorChars,pszSizeBuff,cchMaxSizeChars));}
	HRESULT	GetThemeDocumentationProperty	(LPCWSTR pszThemeName,LPCWSTR pszPropertyName,LPWSTR pszValueBuff,int cchMaxValChars) {return((*((PFNGETTHEMEDOCUMENTATIONPROPERTY)GetProc("GetThemeDocumentationProperty",(void*)GetThemeDocumentationPropertyFail)))(pszThemeName,pszPropertyName,pszValueBuff,cchMaxValChars));}
	HRESULT	DrawThemeParentBackground	(HWND hwnd,HDC hdc,RECT* prc) {return((*((PFNDRAWTHEMEPARENTBACKGROUND)GetProc("DrawThemeParentBackground",(void*)DrawThemeParentBackgroundFail)))(hwnd,hdc,prc));}
	HRESULT	EnableTheming				(BOOL fEnable) {return((*((PFNENABLETHEMING)GetProc("EnableTheming",(void*)EnableThemingFail)))(fEnable));}

private:
	void*	GetProc					(LPCSTR szProc,void* pfnFail);

	typedef	HTHEME (__stdcall *PFNOPENTHEMEDATA)(HWND hwnd,LPCWSTR pszClassList);
	static	HTHEME OpenThemeDataFail(HWND,LPCWSTR) {return NULL;}

	typedef	HRESULT (__stdcall *PFNCLOSETHEMEDATA)(HTHEME hTheme);
	static	HRESULT CloseThemeDataFail(HTHEME) {return E_FAIL;}

	typedef	HRESULT (__stdcall *PFNDRAWTHEMEBACKGROUND)(HTHEME hTheme,HDC hdc,int iPartId,int iStateId,const RECT* pRect,const RECT* pClipRect);
	static	HRESULT DrawThemeBackgroundFail(HTHEME,HDC,int,int,const RECT*,const RECT*) {return E_FAIL;}

	typedef	HRESULT (__stdcall *PFNDRAWTHEMETEXT)(HTHEME hTheme,HDC hdc,int iPartId,int iStateId,LPCWSTR pszText,int iCharCount,DWORD dwTextFlags,DWORD dwTextFlags2,const RECT* pRect);
	static	HRESULT DrawThemeTextFail(HTHEME,HDC,int,int,LPCWSTR,int,DWORD,DWORD,const RECT*) {return E_FAIL;}

	typedef	HRESULT (__stdcall *PFNGETTHEMEBACKGROUNDCONTENTRECT)(HTHEME hTheme,HDC hdc,int iPartId,int iStateId,const RECT* pBoundingRect,RECT* pContentRect);
	static	HRESULT GetThemeBackgroundContentRectFail(HTHEME,HDC,int,int,const RECT*,RECT*) {return E_FAIL;}
	
	typedef	HRESULT (__stdcall *PFNGETTHEMEBACKGROUNDEXTENT)(HTHEME hTheme,HDC hdc,int iPartId,int iStateId,const RECT* pContentRect,RECT* pExtentRect);
	static	HRESULT GetThemeBackgroundExtentFail(HTHEME,HDC,int,int,const RECT*,RECT*) {return E_FAIL;}

	typedef	HRESULT (__stdcall *PFNGETTHEMEPARTSIZE)(HTHEME hTheme,HDC hdc,int iPartId,int iStateId,RECT* pRect,enum THEMESIZE eSize,SIZE* psz);
	static	HRESULT GetThemePartSizeFail(HTHEME,HDC,int,int,RECT*,enum THEMESIZE,SIZE*) {return E_FAIL;}

	typedef	HRESULT (__stdcall *PFNGETTHEMETEXTEXTENT)(HTHEME hTheme,HDC hdc,int iPartId,int iStateId,LPCWSTR pszText,int iCharCount,DWORD dwTextFlags,const RECT* pBoundingRect,RECT* pExtentRect);
	static	HRESULT GetThemeTextExtentFail(HTHEME,HDC,int,int,LPCWSTR,int,DWORD,const RECT*,RECT*) {return E_FAIL;}

	typedef	HRESULT (__stdcall *PFNGETTHEMETEXTMETRICS)(HTHEME hTheme,HDC hdc,int iPartId,int iStateId,TEXTMETRIC* ptm);
	static	HRESULT GetThemeTextMetricsFail(HTHEME,HDC,int,int,TEXTMETRIC*) {return E_FAIL;}

	typedef	HRESULT (__stdcall *PFNGETTHEMEBACKGROUNDREGION)(HTHEME hTheme,HDC hdc,int iPartId,int iStateId,const RECT* pRect,HRGN* pRegion);
	static	HRESULT GetThemeBackgroundRegionFail(HTHEME,HDC,int,int,const RECT*,HRGN*) {return E_FAIL;}

	typedef	HRESULT (__stdcall *PFNHITTESTTHEMEBACKGROUND)(HTHEME hTheme,HDC hdc,int iPartId,int iStateId,DWORD dwOptions,const RECT* pRect,HRGN hrgn,POINT ptTest,WORD* pwHitTestCode);
	static	HRESULT HitTestThemeBackgroundFail(HTHEME,HDC,int,int,DWORD,const RECT*,HRGN,POINT,WORD*) {return E_FAIL;}

	typedef	HRESULT (__stdcall *PFNDRAWTHEMEEDGE)(HTHEME hTheme,HDC hdc,int iPartId,int iStateId,const RECT* pDestRect,UINT uEdge,UINT uFlags,RECT* pContentRect);
	static	HRESULT DrawThemeEdgeFail(HTHEME,HDC,int,int,const RECT*,UINT,UINT,RECT*) {return E_FAIL;}

	typedef	HRESULT (__stdcall *PFNDRAWTHEMEICON)(HTHEME hTheme,HDC hdc,int iPartId,int iStateId,const RECT* pRect,HIMAGELIST himl,int iImageIndex);
	static	HRESULT DrawThemeIconFail(HTHEME,HDC,int,int,const RECT*,HIMAGELIST,int) {return E_FAIL;}

	typedef	BOOL (__stdcall *PFNISTHEMEPARTDEFINED)(HTHEME hTheme,int iPartId,int iStateId);
	static	BOOL IsThemePartDefinedFail(HTHEME,int,int) {return FALSE;}

	typedef	BOOL (__stdcall *PFNISTHEMEBACKGROUNDPARTIALLYTRANSPARENT)(HTHEME hTheme,int iPartId,int iStateId);
	static	BOOL IsThemeBackgroundPartiallyTransparentFail(HTHEME,int,int) {return FALSE;}

	typedef	HRESULT (__stdcall *PFNGETTHEMECOLOR)(HTHEME hTheme,int iPartId,int iStateId,int iPropId,COLORREF* pColor);
	static	HRESULT GetThemeColorFail(HTHEME,int,int,int,COLORREF*) {return E_FAIL;}

	typedef	HRESULT (__stdcall *PFNGETTHEMEMETRIC)(HTHEME hTheme,HDC hdc,int iPartId,int iStateId,int iPropId,int* piVal);
	static	HRESULT GetThemeMetricFail(HTHEME,HDC,int,int,int,int*) {return E_FAIL;}

	typedef	HRESULT (__stdcall *PFNGETTHEMESTRING)(HTHEME hTheme,int iPartId,int iStateId,int iPropId,LPWSTR pszBuff,int cchMaxBuffChars);
	static	HRESULT GetThemeStringFail(HTHEME,int,int,int,LPWSTR,int) {return E_FAIL;}

	typedef	HRESULT (__stdcall *PFNGETTHEMEBOOL)(HTHEME hTheme,int iPartId,int iStateId,int iPropId, BOOL* pfVal);
	static	HRESULT GetThemeBoolFail(HTHEME,int,int,int,BOOL*) {return E_FAIL;}

	typedef	HRESULT (__stdcall *PFNGETTHEMEINT)(HTHEME hTheme,int iPartId,int iStateId,int iPropId,int* piVal);
	static	HRESULT GetThemeIntFail(HTHEME,int,int,int,int*) {return E_FAIL;}

	typedef	HRESULT (__stdcall *PFNGETTHEMEENUMVALUE)(HTHEME hTheme,int iPartId,int iStateId,int iPropId,int* piVal);
	static	HRESULT GetThemeEnumValueFail(HTHEME,int,int,int,int*) {return E_FAIL;}

	typedef	HRESULT (__stdcall *PFNGETTHEMEPOSITION)(HTHEME hTheme,int iPartId,int iStateId,int iPropId,POINT* pPoint);
	static	HRESULT GetThemePositionFail(HTHEME,int,int,int,POINT*) {return E_FAIL;}

	typedef	HRESULT (__stdcall *PFNGETTHEMEFONT)(HTHEME hTheme,HDC hdc,int iPartId,int iStateId,int iPropId,LOGFONT* pFont);
	static	HRESULT GetThemeFontFail(HTHEME,HDC,int,int,int,LOGFONT*) {return E_FAIL;}

	typedef	HRESULT (__stdcall *PFNGETTHEMERECT)(HTHEME hTheme,int iPartId,int iStateId,int iPropId,RECT* pRect);
	static	HRESULT GetThemeRectFail(HTHEME,int,int,int,RECT*) {return E_FAIL;}

	typedef	HRESULT (__stdcall *PFNGETTHEMEMARGINS)(HTHEME hTheme,HDC hdc,int iPartId,int iStateId,int iPropId,RECT* prc,MARGINS* pMargins);
	static	HRESULT GetThemeMarginsFail(HTHEME,HDC,int,int,int,RECT*,MARGINS*) {return E_FAIL;}

	typedef	HRESULT (__stdcall *PFNGETTHEMEINTLIST)(HTHEME hTheme,int iPartId,int iStateId,int iPropId,INTLIST* pIntList);
	static	HRESULT GetThemeIntListFail(HTHEME,int,int,int,INTLIST*) {return E_FAIL;}

	typedef	HRESULT (__stdcall *PFNGETTHEMEPROPERTYORIGIN)(HTHEME hTheme,int iPartId,int iStateId,int iPropId,enum PROPERTYORIGIN* pOrigin);
	static	HRESULT GetThemePropertyOriginFail(HTHEME,int,int,int,enum PROPERTYORIGIN*) {return E_FAIL;}

	typedef	HRESULT (__stdcall *PFNSETWINDOWTHEME)(HWND hwnd,LPCWSTR pszSubAppName,LPCWSTR pszSubIdList);
	static	HRESULT SetWindowThemeFail(HWND,LPCWSTR,LPCWSTR) {return E_FAIL;}

	typedef	HRESULT (__stdcall *PFNGETTHEMEFILENAME)(HTHEME hTheme,int iPartId,int iStateId,int iPropId,LPWSTR pszThemeFileName,int cchMaxBuffChars);
	static	HRESULT GetThemeFilenameFail(HTHEME,int,int,int,LPWSTR,int) {return E_FAIL;}

	typedef	COLORREF (__stdcall *PFNGETTHEMESYSCOLOR)(HTHEME hTheme,int iColorId);
	static	COLORREF GetThemeSysColorFail(HTHEME,int) {return RGB(255,255,255);}

	typedef	HBRUSH (__stdcall *PFNGETTHEMESYSCOLORBRUSH)(HTHEME hTheme,int iColorId);
	static	HBRUSH GetThemeSysColorBrushFail(HTHEME,int) {return NULL;}

	typedef	BOOL (__stdcall *PFNGETTHEMESYSBOOL)(HTHEME hTheme,int iBoolId);
	static	BOOL GetThemeSysBoolFail(HTHEME,int) {return FALSE;}

	typedef	int (__stdcall *PFNGETTHEMESYSSIZE)(HTHEME hTheme,int iSizeId);
	static	int GetThemeSysSizeFail(HTHEME,int) {return 0;}

	typedef	HRESULT (__stdcall *PFNGETTHEMESYSFONT)(HTHEME hTheme,int iFontId,LOGFONT* plf);
	static	HRESULT GetThemeSysFontFail(HTHEME,int,LOGFONT*) {return E_FAIL;}

	typedef	HRESULT (__stdcall *PFNGETTHEMESYSSTRING)(HTHEME hTheme,int iStringId,LPWSTR pszStringBuff,int cchMaxStringChars);
	static	HRESULT GetThemeSysStringFail(HTHEME,int,LPWSTR,int) {return E_FAIL;}

	typedef	HRESULT (__stdcall *PFNGETTHEMESYSINT)(HTHEME hTheme,int iIntId,int *piValue);
	static	HRESULT GetThemeSysIntFail(HTHEME,int,int*) {return E_FAIL;}

	typedef	BOOL (__stdcall *PFNISTHEMEACTIVE)(void);
	static	BOOL IsThemeActiveFail(void) {return FALSE;}

	typedef	BOOL(__stdcall *PFNISAPPTHEMED)(void);
	static	BOOL IsAppThemedFail(void) {return FALSE;}

	typedef	HTHEME (__stdcall *PFNGETWINDOWTHEME)(HWND hwnd);
	static	HTHEME GetWindowThemeFail(HWND) {return NULL;}

	typedef	HRESULT (__stdcall *PFNENABLETHEMEDIALOGTEXTURE)(HWND hwnd,DWORD dwFlags);
	static	HRESULT EnableThemeDialogTextureFail(HWND,DWORD) {return E_FAIL;}

	typedef	BOOL (__stdcall *PFNISTHEMEDIALOGTEXTUREENABLED)(HWND hwnd);
	static	BOOL IsThemeDialogTextureEnabledFail(HWND) {return FALSE;}

	typedef	DWORD (__stdcall *PFNGETTHEMEAPPPROPERTIES)(void);
	static	DWORD GetThemeAppPropertiesFail(void) {return 0L;}

	typedef	void (__stdcall *PFNSETTHEMEAPPPROPERTIES)(DWORD dwFlags);
	static	void SetThemeAppPropertiesFail(DWORD) {}

	typedef	HRESULT (__stdcall *PFNGETCURRENTTHEMENAME)(LPWSTR pszThemeFileName,int cchMaxNameChars,LPWSTR pszColorBuff,int cchMaxColorChars,LPWSTR pszSizeBuff,int cchMaxSizeChars);
	static	HRESULT GetCurrentThemeNameFail(LPWSTR,int,LPWSTR,int,LPWSTR,int) {return E_FAIL;}

	typedef	HRESULT (__stdcall *PFNGETTHEMEDOCUMENTATIONPROPERTY)(LPCWSTR pszThemeName,LPCWSTR pszPropertyName,LPWSTR pszValueBuff,int cchMaxValChars);
	static	HRESULT GetThemeDocumentationPropertyFail(LPCWSTR,LPCWSTR,LPWSTR,int) {return E_FAIL;}

	typedef	HRESULT (__stdcall *PFNDRAWTHEMEPARENTBACKGROUND)(HWND hwnd,HDC hdc,RECT* prc);
	static	HRESULT DrawThemeParentBackgroundFail(HWND,HDC,RECT*) {return E_FAIL;}

	typedef	HRESULT (__stdcall *PFNENABLETHEMING)(BOOL fEnable);
	static	HRESULT EnableThemingFail(BOOL) {return E_FAIL;}

	static int	m_nRefCount;
	
	static HMODULE	m_hThemeDll;
	static int	m_nIsThemingSupported;
};

#endif // _CWINDOWSXPTHEME_H
