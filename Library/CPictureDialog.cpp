/*
	CPictureDialog.cpp
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
#include "env.h"
#include "pragma.h"
#include "macro.h"
#include "window.h"
#include "strings.h"
#include <dlgs.h>
#include "CImageFactory.h"
#include "CDibCtrl.h"
#include "CColorStatic.h"
#include "CPictureDialog.h"

#include "traceexpr.h"
//#define _TRACE_FLAG	_TRFLAG_TRACEOUTPUT
//#define _TRACE_FLAG	_TRFLAG_NOTRACE
#define _TRACE_FLAG		_TRFLAG_NOTRACE
#define _TRACE_FLAG_INFO	_TRFLAG_NOTRACE
#define _TRACE_FLAG_WARN	_TRFLAG_NOTRACE
#define _TRACE_FLAG_ERR	_TRFLAG_NOTRACE

#if (defined(_DEBUG) && defined(_WINDOWS)) && (defined(_AFX) || defined(_AFXDLL))
#ifdef PRAGMA_MESSAGE_VERBOSE
  #pragma message("\t\t\t"__FILE__"("STR(__LINE__)"): using DEBUG_NEW macro")
#endif
  #define new DEBUG_NEW
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CPictureDialog,CFileDialog)
	ON_WM_QUERYNEWPALETTE()
	ON_WM_PALETTECHANGED()
	ON_WM_SETFOCUS()
	//ON_BN_CLICKED(IDC_PREVIEW,OnPreview)
END_MESSAGE_MAP()

/*
	CPictureDialog()
*/
CPictureDialog::CPictureDialog(UINT nDialogID,UINT nStaticDibID,UINT nStaticInfo,LPCSTR lpcszLibraryName/*=NULL*/,BOOL bOpenFileDialog/*=TRUE*/,LPCSTR lpszDefExt/*=NULL*/,LPCSTR lpszFileName/*=NULL*/,DWORD dwFlags/*OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT*/,LPCSTR lpszFilter/*=NULL*/,CWnd* pWndParent/*=NULL*/) : CFileDialog(bOpenFileDialog,lpszDefExt,lpszFileName,dwFlags,lpszFilter,pWndParent)
{
	// imposta gli id per i controlli
	m_nDialogID = nDialogID;
	m_nStaticDibID = nStaticDibID;
	m_nStaticInfo = nStaticInfo;

	// libreria di default
	if(lpcszLibraryName)
		strcpyn(m_szLibraryName,lpcszLibraryName,sizeof(m_szLibraryName));
	else
		memset(m_szLibraryName,'\0',sizeof(m_szLibraryName));
	
	// flags per il dialogo
	m_ofn.Flags |= (OFN_EXPLORER | OFN_ENABLETEMPLATE);
	m_ofn.lpTemplateName = MAKEINTRESOURCE(m_nDialogID);
	m_bPreview = TRUE;
	m_bFolderChanged = FALSE;
	
	// font per le info sull'immagine
	m_hFont = NULL;
}

/*
	~CPictureDialog()
*/
CPictureDialog::~CPictureDialog()
{
	if(m_hFont)
		::DeleteObject(m_hFont);
}

/*
	Clear()
*/
void CPictureDialog::Clear(void)
{
	// pulisce l'anteprima (immagine e info)
	m_DibCtrl.Unload();
	memset(m_szImageInfo,'\0',sizeof(m_szImageInfo));
	m_wndStaticInfo.SetWindowText(m_szImageInfo);		// tipo EDIT
	//SetDlgItemText(m_nStaticInfo,m_szImageInfo);		// tipo STATIC
}

/*
	OnInitDialog()
*/
BOOL CPictureDialog::OnInitDialog(void)
{
	// classe base
	CFileDialog::OnInitDialog();

	// subclassa il controllo per l'immagine
	m_DibCtrl.SubclassDlgItem(m_nStaticDibID,this);
	
	// subclassa il controllo per le info sull'immagine (imposta font e colore del testo)
	m_wndStaticInfo.SubclassDlgItem(m_nStaticInfo,this);
	LOGFONT lf = {0};
	lf.lfHeight = -11;
	lf.lfWidth = 0;
	lf.lfEscapement = 0;
	lf.lfOrientation = 0;
	lf.lfWeight = 400;
	lf.lfItalic = 0;
	lf.lfUnderline = 0;
	lf.lfStrikeOut = 0;
	lf.lfCharSet = 0;
	lf.lfOutPrecision = 3;
	lf.lfClipPrecision = 2;
	lf.lfQuality = 1;
	lf.lfPitchAndFamily = 34;
	lstrcpy(lf.lfFaceName,"Tahoma");
	m_hFont = ::CreateFontIndirect(&lf);  
	CFont* pFont = CFont::FromHandle(m_hFont);
	m_wndStaticInfo.SetFont(pFont,FALSE);
	m_wndStaticInfo.SetTextColor(RGB(0,0,0));
	m_wndStaticInfo.SetBkColor(RGB(255,255,255));
	
	// flag per anteprima immagine
	//GetDlgItem(IDC_PREVIEW)->SendMessage(BM_SETCHECK,(m_bPreview) ? 1 : 0);
	m_bPreview = TRUE;

	return(TRUE);
}

/*
	OnPreview()
*/
/*void CPictureDialog::OnPreview(void)
{
	m_bPreview = !m_bPreview;

	if(!m_bPreview)
	{
		m_DibCtrl.Unload();
		SetDlgItemText(IDC_TEXT_INFO,"");
	}
	else
		OnFileNameChange();
}*/

/*
	OnQueryNewPalette()
*/
BOOL CPictureDialog::OnQueryNewPalette(void)
{
	m_DibCtrl.SendMessage(WM_QUERYNEWPALETTE);
	return(CFileDialog::OnQueryNewPalette());
}

/*
	OnPaletteChanged()
*/
void CPictureDialog::OnPaletteChanged(CWnd* pFocusWnd)
{
	CFileDialog::OnPaletteChanged(pFocusWnd);
	m_DibCtrl.SendMessage(WM_PALETTECHANGED,(WPARAM)pFocusWnd->GetSafeHwnd());
}

/*
	OnSetFocus()
*/
void CPictureDialog::OnSetFocus(CWnd* pOldWnd)
{
	CFileDialog::OnSetFocus(pOldWnd);
	m_DibCtrl.SendMessage(WM_QUERYNEWPALETTE);
}

/*
	OnFileNameChange()
*/
void CPictureDialog::OnFileNameChange(void)
{
	// la zoccola di MFC richiama OnFileNameChange() anche quando viene cambiata la directory
	// (quando viene cambiata la directory, il campo del dialogo per il nome file, invece di
	// essere azzerato, rimane impostato con l'ultimo nome file selezionato)
	if(m_bFolderChanged)
	{
		// non deve caricare nessun anteprima (e' stata cambiata la directory corrente)
		m_bFolderChanged = FALSE;
		return;
	}
	
	// se in modalita' anteprima
	if(m_bPreview)
	{
		BOOL bLoaded = FALSE;
		CString strPathName = GetPathName();
		memset(m_szImageInfo,'\0',sizeof(m_szImageInfo));

		// carica l'anteprima dell'immagine sul controllo
		if(!strPathName.IsEmpty())
			if((bLoaded = m_DibCtrl.Load(strPathName,m_szLibraryName))==TRUE)
			{				
				// ricava le info relative all'immagine
				CImage* pImage = (CImage*)m_DibCtrl.GetImage();
				if(pImage)
				{
					char szMemUsed[32];
					strsize(szMemUsed,sizeof(szMemUsed),pImage->GetMemUsed());
					int nColors = pImage->GetNumColors();
					float nXRes = 0.0,nYRes = 0.0;
					pImage->GetDPI(nXRes,nYRes);
					int nResType = pImage->GetURes();
					int nQuality = pImage->GetQuality();
					char szQuality[32] = {""};
					if(nQuality > 0)
						_snprintf(szQuality,sizeof(szQuality)-1,"\r\nquality factor=%d",nQuality);

					_snprintf(m_szImageInfo,
							sizeof(m_szImageInfo)-1
							,
							"%d x %d pixels\r\n"
							"%d%s colors x %d bpp\r\n"
							"%s required\r\n"
							"xRes=%3.2f\r\nyRes=%3.2f\r\n"
							"res type=%s%s"
							,
							pImage->GetWidth(),
							pImage->GetHeight(),
							(nColors > 256 || nColors==0) ? 16 : nColors,
							(nColors > 256 || nColors==0) ? "M" : "",
							pImage->GetBPP(),
							szMemUsed,
							nXRes,nYRes,
							nResType==RESUNITINCH ? "inch" : (nResType==RESUNITCENTIMETER ? "centimeter" : "none"),
							szQuality
							);
				}
			}

		// caricamento fallito (non e' un immagine o e' stata cambiata la directory corrente)
		if(!bLoaded)
		{
			Clear();

			// tipo file non gestito (non e' un immagine riconosciuta dalla libreria)
			// chiama la virtuale (da agganciare con la derivata) con il tipo file
			OnUnknowFileType(strPathName,m_DibCtrl,m_szLibraryName,m_szImageInfo,sizeof(m_szImageInfo));
		}
					
		m_wndStaticInfo.SetWindowText(m_szImageInfo);	// tipo EDIT
		//SetDlgItemText(m_nStaticInfo,m_szImageInfo);	// tipo STATIC
	}
}

/*
	OnFolderChange()
*/
void CPictureDialog::OnFolderChange(void)
{
	// la zoccola di MFC richiama OnFileNameChange() anche quando viene cambiata la directory
	// (quando viene cambiata la directory, il campo del dialogo per il nome file, invece di
	// essere azzerato, rimane impostato con l'ultimo nome file selezionato)
	m_bFolderChanged = TRUE;

	Clear();

	// informa la derivata (agganciata con la virtuale) del cambio di directory
	OnUnknowFileType("",m_DibCtrl,m_szLibraryName,m_szImageInfo,sizeof(m_szImageInfo));
}
