/*
	CProgressBar.h
	Classe base per il controllo a progresso (SDK).
	Luca Piergentili, 24/08/00
	lpiergentili@yahoo.com
*/
#ifndef _CPROGRESSBAR_H
#define _CPROGRESSBAR_H 1

#include "window.h"

class CProgressBar
{
public:
	CProgressBar()
	{
		m_hWnd = (HWND)NULL;
		m_nMin = m_nMax = 0;
	}

	virtual ~CProgressBar()
	{
		if(m_hWnd)
		{
			if(::IsWindow(m_hWnd))
				::DestroyWindow(m_hWnd);
			m_hWnd = (HWND)NULL;
		}
	}

	inline BOOL Create(HWND hWnd,HINSTANCE hInstance,int x,int y,int nWidth,int nHeigth)
	{
		m_hWnd = ::CreateWindow(	PROGRESS_CLASS,
							"Position",
							WS_CHILD | WS_VISIBLE,
							x,y,nWidth,nHeigth,
							hWnd,
							(HMENU)NULL,
							hInstance,
							NULL
							);

		return(m_hWnd!=NULL);
	}
	
	inline BOOL Attach(HWND hWnd,UINT id)
	{
		m_hWnd = ::GetDlgItem(hWnd,id);
		return(m_hWnd!=NULL);
	}

	inline void SetPos(int nPos)
	{
		if(m_hWnd!=(HWND)NULL)
			::SendMessage(m_hWnd,PBM_SETPOS,nPos,0L);
	}

	inline void SetRange(int iMin = 0,int iMax = 10)
	{
		if(m_hWnd!=(HWND)NULL)
		{
			m_nMin = iMin;
			m_nMax = iMax;
			::SendMessage(m_hWnd,PBM_SETRANGE,0L,MAKELONG(m_nMin,m_nMax));
		}
	}

	inline void SetStep(LONG iStep = 1L)
	{
		if(m_hWnd!=(HWND)NULL)
			::SendMessage(m_hWnd,PBM_SETSTEP,iStep,0L);
	}

	inline void StepIt(void)
	{
		if(m_hWnd!=(HWND)NULL)
			::SendMessage(m_hWnd,PBM_STEPIT,0L,0L);
	}

	inline void Show(void)
	{
		if(m_hWnd!=(HWND)NULL)
			::ShowWindow(m_hWnd,SW_SHOW);
	}

	inline void Hide(void)
	{
		if(m_hWnd!=(HWND)NULL)
			::ShowWindow(m_hWnd,SW_HIDE);
	}

private:
	HWND m_hWnd;
	int m_nMin;
	int m_nMax;
};

#endif // _CPROGRESSBAR_H
