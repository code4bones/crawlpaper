/*
	CPictureDialog.h
	Classe base per il dialogo File|Apri con visualizzazione dell'anteprima (MFC).
	Luca Piergentili, 07/08/00
	lpiergentili@yahoo.com

	Riadattata e modificata (aggiunta interfaccia per CImage, modificata e corretta l'anteprima,
	aggiunte le virtuali per l'anteprima dei formati non gestiti, modifiche varie) a partire da:
	PreView.cpp - Copyright (C) 1998 by Jorge Lodos
	All rights reserved
	Distribute and use freely, except:
	1. Don't alter or remove this notice.
	2. Mark the changes you made
	Send bug reports, bug fixes, enhancements, requests, etc. to: lodos@cigb.edu.cu
*/
#ifndef _CPICTUREDIALOG_H
#define _CPICTUREDIALOG_H 1

#include "window.h"
#include "CColorStatic.h"
#include "CDibCtrl.h"
#include "CFileDialogEx.h"

/*
	CPictureDialog
*/
class CPictureDialog : public CFileDialog
{
public:
	CPictureDialog(
				UINT		nDialogID,
				UINT		nStaticDibID,
				UINT		nStaticInfo,
				LPCSTR	lpcszLibraryName = NULL,
				BOOL		bOpenFileDialog = TRUE,
				LPCSTR	lpszDefExt = NULL,
				LPCSTR	lpszFileName = NULL,
				DWORD	dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
				LPCSTR	lpszFilter = NULL,
				CWnd*	pWndParent = NULL
				);

	virtual ~CPictureDialog();

	void			Clear			(void);

protected:
	virtual BOOL	OnInitDialog		(void);			
	//void		OnPreview			(void);
	BOOL			OnQueryNewPalette	(void);
	void			OnPaletteChanged	(CWnd*);
	void			OnSetFocus		(CWnd*);
	virtual void	OnFileNameChange	(void);
	virtual void	OnFolderChange		(void);
	virtual void	OnUnknowFileType	(LPCSTR	/*lpcszFileName*/,
								CDibCtrl&	/*pDibCtrl*/,
								LPCSTR	/*lpcszLibraryName*/,
								LPSTR	/*lpszInfo*/,
								int		/*nInfoSize*/) {}

private:
	UINT			m_nDialogID;					// id del dialogo
	UINT			m_nStaticDibID;				// id del controllo per l'immagine (in anteprima)
	UINT			m_nStaticInfo;					// id del controllo per le info (in anteprima)
	CColorStatic	m_wndStaticInfo;				// oggetto per il subclassing del controllo per le info
	HFONT		m_hFont;						// font per l'oggetto di cui sopra
	char			m_szLibraryName[_MAX_PATH+1];		// nome libreria
	BOOL			m_bPreview;					// flag per anteprima
	BOOL			m_bFolderChanged;				// flag per cambio directory
	CDibCtrl		m_DibCtrl;					// controllo per l'immagine (in anteprima)
	char			m_szImageInfo[1024];			// buffer per le info

	DECLARE_MESSAGE_MAP()
};

#endif // _CPICTUREDIALOG_H
