/*
	WallBrowserDoc.h
	Classe per il documento.
	Luca Piergentili, 30/07/01
	lpiergentili@yahoo.com

	WallBrowser - the smart picture browser
	http://www.crawlpaper.com/
	copyright © 1998-2004 Luca Piergentili, all rights reserved
	crawlpaper is a registered name, all rights reserved
	
	This is a free software, released under the terms of the BSD license. Do not
	attempt to use it in any form which violates the license or you will be persecuted
	and charged for this.

	Redistribution and use in source and binary forms, with or without modification,
	are permitted provided that the following conditions are met:

	- Redistributions of source code must retain the above copyright notice, this
	list of conditions and the following disclaimer. 

	- Redistributions in binary form must reproduce the above copyright notice, this
	list of conditions and the following disclaimer in the documentation and/or other
	materials provided with the distribution. 

	- Neither the name of "crawlpaper" nor the names of its contributors may be used
	to endorse or promote products derived from this software without specific prior
	written permission. 

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
	ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
	WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
	IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
	INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
	BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
	LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
	OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
	OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef _WALLBROWSERDOC_H
#define _WALLBROWSERDOC_H 1

#include <stdio.h>
#include <stdlib.h>
#include "strings.h"
#include "window.h"
#include <afxdisp.h>
#include "CImageFactory.h"
#include "WallBrowserMessages.h"

// numero delle viste
#define TOT_VIEWS				3

// modalita' di visualizzazione per lo sfondo
#define WINDOWS_DRAWMODE_NORMAL	0
#define WINDOWS_DRAWMODE_TILE		1
#define WINDOWS_DRAWMODE_STRETCH	2

/*
	CWallBrowserDoc
*/
class CWallBrowserDoc : public CDocument
{
	DECLARE_DYNCREATE(CWallBrowserDoc)

public:
	CWallBrowserDoc();
	virtual ~CWallBrowserDoc() {}

	void			SetTitle			(LPCTSTR lpszTitle);
	BOOL			SaveModified		(void);
	BOOL			SaveModified		(LPCSTR lpcszFileName);
	void			SetPictureFlag		(BOOL bFlag);
	inline BOOL	GetPictureFlag		(void) const				{return(m_bPictureFlag);}
	void			SetLibrary		(LPCSTR lpcszLibraryName);
	inline CImage*	GetImage			(void) const				{return(m_pImage);}
	
	inline void	SetFileName		(LPCSTR lpcszFileName)		{strcpyn(m_szFileName,lpcszFileName,sizeof(m_szFileName));}
	void			SetPathName		(LPCSTR lpcszPathName);
	inline LPCSTR	GetFileName		(void) const				{return(m_szFileName);}
	inline LPCSTR	GetPathName		(void) const				{return(m_szPathName);}
	inline void	ResetFileName		(void)					{memset(m_szFileName,'\0',sizeof(m_szFileName));}
	inline void	ResetPathName		(void)					{memset(m_szPathName,'\0',sizeof(m_szPathName));}
	inline BOOL	HaveFileName		(void) const				{return(m_szFileName[0]!='\0');}
	inline BOOL	HavePathName		(void) const				{return(m_szPathName[0]!='\0');}

	BOOL			OnSaveDocument		(LPCTSTR lpszPathName);
	void			OnFileSave		(void);
	void			OnFileSaveAs		(void);
	void			OnNotImplementedYet	(void);

	void			OnSetWallPaper		(void);
	void			OnResetWallPaper	(void);

	void			SetWallPaperMode	(UINT nMode);
	void			SetWallPaper		(LPCSTR lpcszFileName,UINT nMode = WINDOWS_DRAWMODE_NORMAL);
	void			ResetWallPaper		(void);

	void			GotoNextView		(void);
	BOOL			RouteCmdToAllViews	(CView* pView,UINT nID,int nCode,void* pExtra,AFX_CMDHANDLERINFO* pHandlerInfo);

private:
	BOOL			m_bPictureFlag;
	char			m_szLibraryName[_MAX_PATH+1];
	CImageFactory	m_ImageFactory;
	CImage*		m_pImage;
	char			m_szFileName[_MAX_PATH+1];
	char			m_szPathName[_MAX_PATH+1];
	char			m_szModified[_MAX_PATH+1];
	CView*		m_ViewArray[TOT_VIEWS];
	int			m_nCurrentView;

	DECLARE_MESSAGE_MAP()
};

#endif // _WALLBROWSERDOC_H
