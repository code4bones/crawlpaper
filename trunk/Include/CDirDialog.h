/*
	CDirDialog.h
	Classe base per la selezione della directory (SDK/MFC).
	Riadattata e modificata dall'originale di James Spibey per essere usata senza MFC.
	Luca Piergentili, 14/07/00
	lpiergentili@yahoo.com
*/
#ifndef _CDIRDIALOG_H
#define _CDIRDIALOG_H 1

#include <stdlib.h>
#include "strings.h"
#include "window.h"

/*
	CDirDialog
*/
class CDirDialog
{
public:
	CDirDialog(LPCSTR lpcszStartFolder = NULL,LPCSTR lpcszWindowTitle = NULL,LPCSTR lpcszTitle = NULL,BOOL bAlwaysOnTop = FALSE);
	virtual ~CDirDialog() {}

	int			DoModal		(HWND hWnd = NULL);

	inline void	SetWindowTitle	(LPCSTR lpcszTitle)	{strcpyn(m_szWindowTitle,lpcszTitle,sizeof(m_szWindowTitle));}
	inline void	SetTitle		(LPCSTR lpcszTitle)	{strcpyn(m_szTitle,lpcszTitle,sizeof(m_szTitle));}
	inline void	SetSelDir		(LPCSTR lpcszDir)	{strcpyn(m_szSelDir,lpcszDir,sizeof(m_szSelDir));}
	inline void	SetRootDir	(LPCSTR lpcszDir)	{strcpyn(m_szInitDir,lpcszDir,sizeof(m_szInitDir));}

	inline LPCSTR	GetWindowText	(void) const		{return(m_szWindowTitle);}
	inline LPCSTR	GetTitle		(void) const		{return(m_szTitle);}
	inline LPCSTR	GetPathName	(void) const		{return(m_szPathName);}

protected:
	char			m_szWindowTitle[_MAX_PATH+1];
	char			m_szPathName[_MAX_PATH+1];
	char			m_szInitDir[_MAX_PATH+1];
	char			m_szSelDir[_MAX_PATH+1];
	char			m_szTitle[_MAX_PATH+1];
	int			m_nImageIndex;
	BOOL			m_bStatus;

private:
	inline virtual BOOL		SelChanged(LPCSTR /*lpcszSelection*/,LPSTR /*lpszStatusText*/) {return(TRUE);};
	static int __stdcall	BrowseCtrlCallback(HWND hWnd,UINT uMsg,LPARAM lParam,LPARAM lpData);
};

#endif // _CDIRDIALOG_H
