/*
	CNotify.h
	Classe per l'invio del messaggio (SDK/MFC).
	Luca Piergentili, 12/12/02
	lpiergentili@yahoo.com
*/
#ifndef _CNOTIFY_H
#define _CNOTIFY_H 1

#include "window.h"

/*
	CNotify
*/
class CNotify
{
public:
	CNotify(HWND hWnd,UINT nMsg);
	virtual ~CNotify() {}
	
	virtual void Send(WPARAM wParam = 0,LPARAM lParam = 0L);
	virtual void Post(WPARAM wParam = 0,LPARAM lParam = 0L);

private:
	HWND	m_hWnd;
	UINT	m_nMsg;
};

#endif // _CNOTIFY_H
