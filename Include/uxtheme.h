// borrowed from http://www.mingw.org/download.shtml as a part of w32api-2.5-src.tar
// the following is the copyright found in README.w32api license file

/*
        Free headers and libraries for the Win32 API

        Originally written by Anders Norlander 
	Last known and not working email: <anorland@hem2.passagen.se>

	Now maintained by MinGW Developers
        Send bug reports and questions to MinGW-users@lists.sourceforge.net
	URL: http://www.mingw.org

* License 2.0

  You are free to use, modify and copy this package as long as this
  README.w32api file is included unmodified with any distribution, source or
  binary, of this package.  No restrictions are imposed on any package or 
  product using or incorporating this package.  You are free to license your 
  package as you see fit.
  
  You may not restrict others freedoms as set forth in the above paragraph.
  You may distribute this library as part of another package or as a
  modified package if and only if you do *not* restrict others freedoms as
  set forth in the above paragraph as it concerns this package.  You do have
  the right to restrict uses of any package using this package.

  This package is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

* What is it?

  This is a free set of headers and import libraries for the Win32
  API.  The library differs from the GNU Windows32 library in that I
  have tried to use a file structure that mirrors Microsoft's.  I
  don't like having *all* definitions in one single header as in the
  GNU Windows32 library, I want a clean separation between different
  parts of the API.

  Daniel Guerrero Miralles contributed the DirectX 6.1 import
  libraries and DirectX GUID definitions.

  See the files NOTES and TODO for what needs to be done.

* Size does matter

  Since the WIN32 API is severely bloated (as most MS products seem to
  be) the headers are designed to be as compact as possible, while
  still being readable, in order to minimize parsing time.

  The convention is to omit parameter names for function prototypes,
  no excessive white space. Struct/union members are indented with tab
  characters to make them readable. Comment only when necessary.

  If you are contributing a patch please follow the above mentioned
  convention. Make sure your editor does not convert tabs to spaces.

* What do I need to use it?

  The library is intended for use with egcs 1.1 or later but it is
  possible to use with some other tools as well (although it is not
  very useful). LCC-Win32, MSVC and Borland C++ 5.01 or higher may
  work as well. The import libraries are for GNU tools only.

  The library requires egcs 1.1 or later, since the `#pragma pack'
  feature is used. Mumit Khan provides egcs patches and binaries for
  win32 at `http://www.xraylith.wisc.edu/~khan/software/gnu-win32/'.

  If you are going to use C++ COM objects, you will need a version of
  egcs that recognizes the `comobject' attribute and then define
  HAVE_COMOBJECT when compiling your program. Antonio Mendes de
  Oliveira Neto has a prebuilt version at
  `http://li.facens.br/EGCS-WIN32/english/index.html'. Note that this
  is very experimental. If you want to use COM objects in C++ but with
  C interfaces you must define CINTERFACE.

  Objective-C programs cannot use COM functionality because of
  conflicts between the interface define and the Objective-C
  @interface directive.  There is also a conflict between the windows
  Obj-C BOOL types. To avoid this conflict you should use WINBOOL in
  all places where you would use BOOL in a C/C++ windows program. If
  you include any windows headers *after* `windows.h' you must use the
  method outlined below:

  // non-windows includes
  #include <objc/objc.h>
  ...
  // windows specific headers
  #include <windows.h>
  #define BOOL WINBOOL
  #include <commctrl.h>
  ...
  #undef BOOL
  ...
  // include other headers
*/
#ifndef _UXTHEME_H
#define _UXTHEME_H
#if __GNUC__ >= 3
#pragma GCC system_header
#endif

#ifdef __cplusplus
extern "C" {
#endif

//LPI #if (_WIN32_WINNT >= 0x0501)
#define DTBG_CLIPRECT 0x00000001
#define DTBG_DRAWSOLID 0x00000002
#define DTBG_OMITBORDER 0x00000004
#define DTBG_OMITCONTENT 0x00000008
#define DTBG_COMPUTINGREGION 0x00000010
#define DTBG_MIRRORDC 0x00000020
#define DTT_GRAYED 0x00000001
#define ETDT_DISABLE 0x00000001
#define ETDT_ENABLE 0x00000002
#define ETDT_USETABTEXTURE 0x00000004
#define ETDT_ENABLETAB (ETDT_ENABLE|ETDT_USETABTEXTURE)
#define STAP_ALLOW_NONCLIENT 0x00000001
#define STAP_ALLOW_CONTROLS 0x00000002
#define STAP_ALLOW_WEBCONTENT 0x00000004
#define HTTB_BACKGROUNDSEG 0x0000  
#define HTTB_FIXEDBORDER 0x0002
#define HTTB_CAPTION 0x0004
#define HTTB_RESIZINGBORDER_LEFT 0x0010
#define HTTB_RESIZINGBORDER_TOP 0x0020
#define HTTB_RESIZINGBORDER_RIGHT 0x0040
#define HTTB_RESIZINGBORDER_BOTTOM 0x0080
#define HTTB_RESIZINGBORDER (HTTB_RESIZINGBORDER_LEFT|HTTB_RESIZINGBORDER_TOP|HTTB_RESIZINGBORDER_RIGHT|HTTB_RESIZINGBORDER_BOTTOM)
#define HTTB_SIZINGTEMPLATE 0x0100
#define HTTB_SYSTEMSIZINGMARGINS 0x0200

typedef enum PROPERTYORIGIN {
	PO_STATE = 0,
	PO_PART = 1,
	PO_CLASS = 2,
	PO_GLOBAL = 3,
	PO_NOTFOUND = 4
} PROPERTYORIGIN;
typedef enum THEMESIZE {
	TS_MIN,
	TS_TRUE,
	TS_DRAW
} THEME_SIZE;
typedef struct _DTBGOPTS {
	DWORD dwSize;
	DWORD dwFlags;
	RECT rcClip;
} DTBGOPTS, *PDTBGOPTS;
#define MAX_INTLIST_COUNT 10
typedef struct _INTLIST {
	int iValueCount;
	int iValues[MAX_INTLIST_COUNT];
} INTLIST, *PINTLIST;
typedef struct _MARGINS {
	int cxLeftWidth;
	int cxRightWidth;
	int cyTopHeight;
	int cyBottomHeight;
} MARGINS, *PMARGINS;
typedef HANDLE HTHEME;

HRESULT WINAPI CloseThemeData(HTHEME);
HRESULT WINAPI DrawThemeBackground(HTHEME,HDC,int,int,const RECT*,const RECT*);
HRESULT WINAPI DrawThemeBackgroundEx(HTHEME,HDC,int,int,const RECT*,const DTBGOPTS*);
HRESULT WINAPI DrawThemeEdge(HTHEME,HDC,int,int,const RECT*,UINT,UINT,RECT*);
HRESULT WINAPI DrawThemeIcon(HTHEME,HDC,int,int,const RECT*,HIMAGELIST,int);
HRESULT WINAPI DrawThemeParentBackground(HWND,HDC,RECT*);
HRESULT WINAPI DrawThemeText(HTHEME,HDC,int,int,LPCWSTR,int,DWORD,DWORD,const RECT*);
HRESULT WINAPI EnableThemeDialogTexture(HWND,DWORD);
HRESULT WINAPI EnableTheming(BOOL);
HRESULT WINAPI GetCurrentThemeName(LPWSTR,int,LPWSTR,int,LPWSTR,int);
DWORD WINAPI GetThemeAppProperties();
HRESULT WINAPI GetThemeBackgroundContentRect(HTHEME,HDC,int,int,const RECT*,RECT*);
HRESULT WINAPI GetThemeBackgroundExtent(HTHEME,HDC,int,int,const RECT*,RECT*);
HRESULT WINAPI GetThemeBackgroundRegion(HTHEME,HDC,int,int,const RECT*,HRGN*);
HRESULT WINAPI GetThemeBool(HTHEME,int,int,int,BOOL*);
HRESULT WINAPI GetThemeColor(HTHEME,int,int,int,COLORREF*);
HRESULT WINAPI GetThemeDocumentationProperty(LPCWSTR,LPCWSTR,LPWSTR,int);
HRESULT WINAPI GetThemeEnumValue(HTHEME,int,int,int,int*);
HRESULT WINAPI GetThemeFilename(HTHEME,int,int,int,LPWSTR,int);
HRESULT WINAPI GetThemeFont(HTHEME,HDC,int,int,int,LOGFONT*);
HRESULT WINAPI GetThemeInt(HTHEME,int,int,int,int*);
HRESULT WINAPI GetThemeIntList(HTHEME,int,int,int,INTLIST*);
HRESULT WINAPI GetThemeMargins(HTHEME,HDC,int,int,int,RECT*,MARGINS*);
HRESULT WINAPI GetThemeMetric(HTHEME,HDC,int,int,int,int*);
HRESULT WINAPI GetThemePartSize(HTHEME,HDC,int,int,RECT*,THEME_SIZE,SIZE*);
HRESULT WINAPI GetThemePosition(HTHEME,int,int,int,POINT*);
HRESULT WINAPI GetThemePropertyOrigin(HTHEME,int,int,int,PROPERTYORIGIN*);
HRESULT WINAPI GetThemeRect(HTHEME,int,int,int,RECT*);
HRESULT WINAPI GetThemeString(HTHEME,int,int,int,LPWSTR,int);
BOOL WINAPI GetThemeSysBool(HTHEME,int);
COLORREF WINAPI GetThemeSysColor(HTHEME,int);
HBRUSH WINAPI GetThemeSysColorBrush(HTHEME,int);
HRESULT WINAPI GetThemeSysFont(HTHEME,int,LOGFONT*);
HRESULT WINAPI GetThemeSysInt(HTHEME,int,int*);
int WINAPI GetThemeSysSize(HTHEME,int);
HRESULT WINAPI GetThemeSysString(HTHEME,int,LPWSTR,int);
HRESULT WINAPI GetThemeTextExtent(HTHEME,HDC,int,int,LPCWSTR,int,DWORD,const RECT*,RECT*);
HRESULT WINAPI GetThemeTextMetrics(HTHEME,HDC,int,int,TEXTMETRIC*);
HTHEME WINAPI GetWindowTheme(HWND);
HRESULT WINAPI HitTestThemeBackground(HTHEME,HDC,int,int,DWORD,const RECT*,HRGN,POINT,WORD*);
BOOL WINAPI IsAppThemed();
BOOL WINAPI IsThemeActive();
BOOL WINAPI IsThemeBackgroundPartiallyTransparent(HTHEME,int,int);
BOOL WINAPI IsThemeDialogTextureEnabled(HWND);
BOOL WINAPI IsThemePartDefined(HTHEME,int,int);
HTHEME WINAPI OpenThemeData(HWND,LPCWSTR);
void WINAPI SetThemeAppProperties(DWORD);
HRESULT WINAPI SetWindowTheme(HWND,LPCWSTR,LPCWSTR);
//LPI #endif

#ifdef __cplusplus
}
#endif
#endif
