/*
	CWndTransparent.h
	Classe per la trasparenza della finestra (SDK/MFC).
	Luca Piergentili, 03/10/03
	lpiergentili@yahoo.com
*/
#ifndef _CWNDTRANSPARENT_H
#define _CWNDTRANSPARENT_H 1

#include "macro.h"
#include "window.h"

#ifndef WS_EX_TRANSPARENT
  #pragma message("\t\t\t"__FILE__"("STR(__LINE__)"): warning: WS_EX_TRANSPARENT not defined by the current SDK")
  #define WS_EX_TRANSPARENT 0x00000020L
#endif

// puntatore alla funzione SetLayeredWindowAttributes() presente in USER32.DLL
typedef BOOL (WINAPI* PFNSETLAYEREDWINDOWATTRIBUTES)(HWND hWnd,COLORREF crKey,BYTE bAlpha,DWORD dwFlags);

/*
	CWndTransparent
*/
class CWndTransparent
{
public:
	CWndTransparent(HWND hWnd = NULL);
	virtual ~CWndTransparent();

	inline void	SetWindow		(HWND hWnd) {m_hWnd = hWnd;}
	inline BOOL	SetTransparent	(void) {return(Transparent());}
	inline BOOL	SetTransparent	(HWND hWnd) {SetWindow(hWnd); return(Transparent());}
	BOOL			Reset		(void);

private:
	BOOL			Transparent	(void);

	static int	m_nRefCount;

	HWND			m_hWnd;
	LONG			m_lWndStyle;
	
	static HMODULE	m_hUser32Dll;
	static PFNSETLAYEREDWINDOWATTRIBUTES m_pfnSetLayeredWindowAttributes;
};

#endif // _CWNDTRANSPARENT_H
