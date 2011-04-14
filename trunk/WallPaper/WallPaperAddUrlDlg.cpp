/*
	WallPaperAddUrlDlg.cpp
	Dialogo per l'aggiunta dell'url alla lista delle immagini.
	lpiergentili@yahoo.com

	WallPaper (alias crawlpaper) - the hardcore of Windows desktop
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
#include "env.h"
#include "pragma.h"
#include "macro.h"
#include <stdio.h>
#include "strings.h"
#include "window.h"
#include "win32api.h"
#include "CBrowser.h"
#include "CComboBoxExt.h"
#include "CDialogEx.h"
#include "CRegKey.h"
#include "CUrl.h"
#include "CWndLayered.h"
#include "WallPaperAddUrlDlg.h"
#include "WallPaperConfig.h"
#include "WallPaperMessages.h"
#include "resource.h"

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

BEGIN_MESSAGE_MAP(CWallPaperAddUrlDlg,CDialogEx)
	ON_MESSAGE(WM_DROPOLE,OnDropOle)
	ON_BN_CLICKED(IDOK,OnOK)
	ON_BN_CLICKED(IDCANCEL,OnCancel)
	ON_CBN_SELCHANGE(IDC_COMBO_URL,OnSelChangeComboUrl)
END_MESSAGE_MAP()

IMPLEMENT_DYNCREATE(CWallPaperAddUrlDlg,CDialogEx)

/*
	DoDataExchange()
*/
void CWallPaperAddUrlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_COMBO_URL,m_wndComboUrl);
}

/*
	CWallPaperAddUrlDlg()
*/
CWallPaperAddUrlDlg::CWallPaperAddUrlDlg(HWND /*hWndParent*/,CWallPaperConfig* pConfig)
: CDialogEx(	/*nTemplateID*/		IDD_DIALOG_OPENURL,
			/*nToolbarID*/			0L,
			/*nStatusbarID*/		0L,
			/*nIconID*/			0L,
			/*nMenuID*/			0L,
#ifdef _RESIZABLE_DIALOG_STYLE
			/*bAllowResize*/		FALSE,
#endif
			/*bAllowSnap*/			FALSE,
			/*bAllowDragAndDrop*/	TRUE,
			/*lpcszUniqueName*/		NULL,
			/*bAllowMultipleInstances*/FALSE,
			/*hWndParent*/			NULL)
{
	m_pConfig = pConfig;
}

/*
	OnInitDialog()
*/
BOOL CWallPaperAddUrlDlg::OnInitDialog(void)
{
	// classe base
	if(!CDialogEx::OnInitDialog())
	{
		EndDialog(IDCANCEL);
		return(FALSE);
	}

	// tooltip
	AddToolTip(IDC_COMBO_URL,IDS_TOOLTIP_ADDURL);
	AddToolTip(IDOK,IDS_TOOLTIP_OK);
	AddToolTip(IDCANCEL,IDS_TOOLTIP_CLOSE);

	// abilita il drag and drop via OLE
	// la classe base (CDialogEx) gestisce solo il drag and drop tramite la shell (files)
	if(m_OleDropTarget.Attach(this,WM_DROPOLE))
	{
		m_OleDropTarget.AddClipboardFormat(NETSCAPE_CLIPBOARDFORMAT_NAME);
		m_OleDropTarget.AddClipboardFormat(IEXPLORER_CLIPBOARDFORMAT_NAME);
	}

	// inserisce nel combo le url presenti
	char* p;
	char szKey[REGKEY_MAX_KEY_NAME+1] = {0};
	for(int i = 0; i < MAX_HOST_COMBO_ENTRIES; i++)
	{
		_snprintf(szKey,sizeof(szKey)-1,"%s%d",WALLPAPER_HOST_KEY,i);
		if(strcmp((p = (char*)m_pConfig->GetString(WALLPAPER_HOSTNAMES_KEY,szKey)),"")!=0)
		{
			if(m_wndComboUrl.FindStringExact(-1,p)==CB_ERR)
				m_wndComboUrl.AddString(p);
		}
		else
			break;
	}

	// imposta come corrente l'ultima url che e' stata aggiunta
	char* pHost = (char*)m_pConfig->GetString(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_ADDURL_KEY);
	if(strcmp(pHost,"")==0)
		pHost = LOCAL_HOST;
	m_strUrl.Format("%s",pHost);
	int nIndex = 0;
	if((nIndex = m_wndComboUrl.FindStringExact(-1,m_strUrl))==CB_ERR)
		nIndex = m_wndComboUrl.AddString(m_strUrl);
	m_wndComboUrl.SetCurSel(nIndex);

	UpdateData(FALSE);

	OnInitDialogEx();

	// trasparenza
	if(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAYERED_KEY))
		m_wndLayered.SetLayer(this->m_hWnd,(BYTE)m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAYERED_VALUE_KEY));

	return(TRUE);
}

/*
	OnOK()
*/
void CWallPaperAddUrlDlg::OnOK(void)
{
	BOOL bUpdated = TRUE;

	if(!UpdateData(TRUE))
		bUpdated = FALSE;

	if(bUpdated)
		bUpdated = OnComboUrl();

	if(bUpdated)
		CDialogEx::OnExit(IDOK);
}

/*
	OnCancel()
*/
void CWallPaperAddUrlDlg::OnCancel(void)
{
	CDialogEx::OnExit(IDCANCEL);
}

/*
	OnSelChangeComboUrl()
*/
void CWallPaperAddUrlDlg::OnSelChangeComboUrl(void)
{
	OnComboUrl();
}

/*
	OnComboUrl()
*/
BOOL CWallPaperAddUrlDlg::OnComboUrl(void)
{
	BOOL bUpdated = TRUE;
	char szUrl[(MAX_URL*2)+1];

	if(m_wndComboUrl.GetWindowText(szUrl,sizeof(szUrl)))
	{
		// aggiorna l'url controllandone il contenuto
		m_strUrl.Format("%s",szUrl);

		if(m_strUrl.GetLength() <= 0 || m_strUrl.GetLength() > MAX_URL)
		{
			::MessageBoxResourceEx(this->m_hWnd,MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_INVALID_URL_LEN,1,MAX_URL);
			bUpdated = FALSE;
			goto done;
		}

		CUrl url;
		if(!url.IsUrlType(m_strUrl,HTTP_URL))
		{
			::MessageBoxResourceEx(this->m_hWnd,MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_INVALID_HTTP_URL,m_strUrl);
			bUpdated = FALSE;
			goto done;
		}

		// aggiorna la configurazione
		m_pConfig->UpdateString(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_ADDURL_KEY,m_strUrl);
		m_pConfig->SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_ADDURL_KEY);

		// cerca l'url nel combo, inserendola se gia' non esiste
		char* p;
		char szKey[REGKEY_MAX_KEY_NAME+1] = {0};
		BOOL bFound = FALSE;
		int i = 0;
		for(; i < MAX_HOST_COMBO_ENTRIES; i++)
		{
			_snprintf(szKey,sizeof(szKey)-1,"%s%d",WALLPAPER_HOST_KEY,i);
			if(strcmp((p = (char*)m_pConfig->GetString(WALLPAPER_HOSTNAMES_KEY,szKey)),"")!=0)
			{
				if(strcmp(m_strUrl,p)==0)
				{
					bFound = TRUE;
					break;
				}
			}
			else
				break;
		}
		if(!bFound)
		{
			if(i==MAX_HOST_COMBO_ENTRIES)
				m_wndComboUrl.DeleteString(0);
			if(m_wndComboUrl.FindStringExact(-1,m_strUrl)==CB_ERR)
				m_wndComboUrl.AddString(m_strUrl);
		}

		// aggiorna la configurazione con quanto presente nel combo (elimina e reinserisce)
		for(i = 0; i < MAX_HOST_COMBO_ENTRIES; i++)
		{
			_snprintf(szKey,sizeof(szKey)-1,"%s%d",WALLPAPER_HOST_KEY,i);
			if(strcmp(m_pConfig->GetString(WALLPAPER_HOSTNAMES_KEY,szKey),"")!=0)
				m_pConfig->Delete(WALLPAPER_HOSTNAMES_KEY,szKey);
		}
		m_pConfig->SaveSection(WALLPAPER_HOSTNAMES_KEY);
		
		for(i = 0; i < MAX_HOST_COMBO_ENTRIES; i++)
		{
			if(m_wndComboUrl.GetLBText(i,szUrl) > 0)
			{
				_snprintf(szKey,sizeof(szKey)-1,"%s%d",WALLPAPER_HOST_KEY,i);
				m_pConfig->Insert(WALLPAPER_HOSTNAMES_KEY,szKey,szUrl);
			}
		}
		m_pConfig->SaveSection(WALLPAPER_HOSTNAMES_KEY);
	}

done:
	
	return(bUpdated);
}

/*
	OnDropFiles()
*/
void CWallPaperAddUrlDlg::OnDropFiles(void)
{
	char szFileName[_MAX_PATH+1];
	DWORD dwAttribute;
	int nTotDrops = 0;

	// mette in primo piano quando riceve via drag & drop
	SetForegroundWindowEx(this->m_hWnd);

	// per ogni file ricevuto
	while(CDialogEx::GetDroppedFile(szFileName,sizeof(szFileName),&dwAttribute))
	{
		// distingue tra directory e file
		if(!(dwAttribute & FILE_ATTRIBUTE_DIRECTORY))
		{
			// acceta solo le url (.url)
			if(stristr(szFileName,DEFAULT_URL_EXT))
			{
				char szUrl[MAX_URL+1];
				if(::GetPrivateProfileString("InternetShortcut","URL","",szUrl,sizeof(szUrl),szFileName) > 0)
				{
					// aggiorna l'url
					m_strUrl.Format("%s",szUrl);
					UpdateData(FALSE);

					// aggiorna il combo
					int nIndex;
					if((nIndex = m_wndComboUrl.FindStringExact(-1,m_strUrl))==CB_ERR)
						nIndex = m_wndComboUrl.AddString(m_strUrl);
					m_wndComboUrl.SetCurSel(nIndex);

					// aggiorna la configurazione
					m_pConfig->UpdateString(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_ADDURL_KEY,m_strUrl);
					m_pConfig->SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_ADDURL_KEY);
					
					nTotDrops++;
				}
				else
					::MessageBoxResourceEx(this->m_hWnd,MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_DROPPED_URL,szFileName);
			}
		}
	}
}

/*
	OnDropOle()
*/
LONG CWallPaperAddUrlDlg::OnDropOle(UINT /*wParam*/,LONG lParam)
{
	// mette in primo piano quando riceve via drag & drop
	SetForegroundWindowEx(this->m_hWnd);

	// aggiorna l'url
	m_strUrl.Format("%s",(LPSTR)lParam);
	UpdateData(FALSE);

	// aggiorna il combo
	int nIndex;
	if((nIndex = m_wndComboUrl.FindStringExact(-1,m_strUrl))==CB_ERR)
		nIndex = m_wndComboUrl.AddString(m_strUrl);
	m_wndComboUrl.SetCurSel(nIndex);

	// aggiorna la configurazione
	m_pConfig->UpdateString(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_ADDURL_KEY,m_strUrl);
	m_pConfig->SaveKey(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAST_ADDURL_KEY);

	return(0L);
}
