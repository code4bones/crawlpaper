/*
	CCaption.h
	Classe per la finestra popup sulla taskbar (MFC).
	Ripresa e modificata dall'originale di Bart Gysens (Iconizer DLL).
	Luca Piergentili, 11/09/03
	lpiergentili@yahoo.com
*/
#include "window.h"
#include "CWindowsVersion.h"
#include "CNodeList.h"

#ifndef _CCAPTION_H
#define _CCAPTION_H 1

/*
	WNDCAPTION
*/
struct WNDCAPTION
{
	HWND		hWnd;
	WNDPROC	lWndProc;
	BOOL		bInNcHandler;
	BOOL		bLButtonDown;	
	BOOL		bIconized;
	BOOL		bRestored;
	int		nHeigth;
	int		nBottom;
	LPVOID	pThis;
};

/*
	CWndCaptionList
*/
class CWndCaptionList : public CNodeList
{
public:
	CWndCaptionList() : CNodeList() {}
	virtual ~CWndCaptionList() {CNodeList::DeleteAll();}
	void* Create(void)
	{
		return(new WNDCAPTION);
	}
	void* Initialize(void* pVoid)
	{
		WNDCAPTION* pData = (WNDCAPTION*)pVoid;
		if(!pData)
			pData = (WNDCAPTION*)Create();
		if(pData)
			memset(pData,'\0',sizeof(WNDCAPTION));
		return(pData);
	}
	BOOL PreDelete(ITERATOR iter)
	{
		if((WNDCAPTION*)iter->data)
			delete ((WNDCAPTION*)iter->data),iter->data = (WNDCAPTION*)NULL;
		return(TRUE);
	}
	int Size(void) {return(sizeof(WNDCAPTION));}
#ifdef _DEBUG
	const char* Signature(void) {return("CWndCaptionList");}
#endif
};

/*
	CCaption
*/
class CCaption
{
public:
	CCaption();
	virtual ~CCaption();
	
	BOOL		Subclass			(HWND hWnd);
	BOOL		UnSubclass		(HWND hWnd);
	BOOL		IsSubclassed		(HWND hWnd);

	// public solo per la callback, non usare direttamente
	LRESULT	OnDefault			(HWND hWnd,UINT nMsg,WPARAM wParam,LPARAM lParam,WNDCAPTION* pWndCaption);
	LRESULT	OnNcActivate		(HWND hWnd,UINT nMsg,WPARAM wParam,LPARAM lParam,WNDCAPTION* pWndCaption);
	LRESULT	OnNcLButtonDown	(HWND hWnd,UINT nMsg,WPARAM wParam,LPARAM lParam,WNDCAPTION* pWndCaption);
	LRESULT	OnNcLButtonDblClk	(HWND hWnd,UINT nMsg,WPARAM wParam,LPARAM lParam,WNDCAPTION* pWndCaption);
	LRESULT	OnNcLButtonUp		(HWND hWnd,UINT nMsg,WPARAM wParam,LPARAM lParam,WNDCAPTION* pWndCaption);
	LRESULT	OnNcMouseMove		(HWND hWnd,UINT nMsg,WPARAM wParam,LPARAM lParam,WNDCAPTION* pWndCaption);	
	LRESULT	OnDrawButton		(HWND hWnd,UINT nMsg,WPARAM wParam,LPARAM lParam,WNDCAPTION* pWndCaption);	
	
protected:
	void		CalcCaptionRect	(HWND hWnd,RECT& rcCaption);
	void		DrawCaptionButton	(HDC hDc,int x,int y,int off);

	HPEN		m_hPen;
	int		m_d1;
	int		m_d2;

private:
	CWindowsVersion m_winVer;
};

#endif // _CCAPTION_H
