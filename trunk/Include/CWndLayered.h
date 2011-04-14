/*
	CWndLayered.h
	Classe per la trasparenza della finestra (SDK/MFC).
	Luca Piergentili, 26/09/01
	lpiergentili@yahoo.com
*/
#ifndef _CWNDLAYERED_H
#define _CWNDLAYERED_H 1

#include "macro.h"
#include "window.h"

#ifndef WS_EX_LAYERED
  #pragma message("\t\t\t"__FILE__"("STR(__LINE__)"): warning: WS_EX_LAYERED not defined by the current SDK, check the _WIN32_WINNT macro")
  #define WS_EX_LAYERED	0x00080000
  #define LWA_COLORKEY	0x00000001
  #define LWA_ALPHA		0x00000002
#endif

#ifndef WS_EX_TRANSPARENT
  #pragma message("\t\t\t"__FILE__"("STR(__LINE__)"): warning: WS_EX_TRANSPARENT not defined by the current SDK")
  #define WS_EX_TRANSPARENT 0x00000020L
#endif

#define LWA_ALPHA_MIN				0			// invisibile
#define LWA_ALPHA_INVISIBLE			LWA_ALPHA_MIN
#define LWA_ALPHA_MAX				255			// opaca
#define LWA_ALPHA_OPAQUE				LWA_ALPHA_MAX
#define LWA_ALPHA_DEFAULT			80			// trasparenza di default
#define LWA_ALPHA_INVISIBLE_THRESHOLD	20			// soglia (in)visibilita'

// puntatore alla funzione SetLayeredWindowAttributes() presente in USER32.DLL
typedef BOOL (WINAPI* PFNSETLAYEREDWINDOWATTRIBUTES)(HWND hWnd,COLORREF crKey,BYTE bAlpha,DWORD dwFlags);

/*
	CWndLayered
*/
class CWndLayered
{
public:
	CWndLayered(HWND hWnd = NULL);
	virtual ~CWndLayered();

	inline void	SetWindow	(HWND hWnd) {m_hWnd = hWnd;}

	inline BOOL	SetLayer		(BYTE bAlpha = (LWA_ALPHA_MAX * LWA_ALPHA_DEFAULT / 100),COLORREF crKey = 0,DWORD dwFlags = LWA_ALPHA) {return(Layer(bAlpha,crKey,dwFlags));}
	inline BOOL	SetLayer		(HWND hWnd,BYTE bAlpha = (LWA_ALPHA_MAX * LWA_ALPHA_DEFAULT / 100),COLORREF crKey = 0,DWORD dwFlags = LWA_ALPHA) {SetWindow(hWnd); return(Layer(bAlpha,crKey,dwFlags));}

	inline BOOL	SetTransparent	(void) {return(Transparent());}
	inline BOOL	SetTransparent	(HWND hWnd) {SetWindow(hWnd); return(Transparent());}

	BOOL			Reset		(void);

private:
	BOOL			Layer		(BYTE bAlpha = (LWA_ALPHA_MAX * LWA_ALPHA_DEFAULT / 100),COLORREF crKey = 0,DWORD dwFlags = LWA_ALPHA);
	BOOL			Transparent	(void);

	static int	m_nRefCount;

	HWND			m_hWnd;
	LONG			m_lWndStyle;
	
	static HMODULE	m_hUser32Dll;
	static PFNSETLAYEREDWINDOWATTRIBUTES m_pfnSetLayeredWindowAttributes;
};

#endif // _CWNDLAYERED_H
