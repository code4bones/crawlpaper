/*
	WallPaperThumbSettingsDlg.cpp
	Dialogo per le opzioni relative alla miniature.
	Luca Piergentili, 21/10/00
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
#include "window.h"
#include "win32api.h"
#include "CIconStatic.h"
#include "CWndLayered.h"
#include "WallPaperConfig.h"
#include "WallPaperMessages.h"
#include "WallPaperThumbSettingsDlg.h"
#include "WallPaperThumbSettingsFormatDlg.h"
#include "WallPaperThumbSettingsHtmlOutputDlg.h"
#include "WallPaperThumbSettingsHtmlTableDlg.h"
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

BEGIN_MESSAGE_MAP(CWallPaperThumbSettingsDlg,CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK,OnOk)
	ON_BN_CLICKED(IDCANCEL,OnCancel)
	ON_NOTIFY(TVN_SELCHANGING,IDC_TREE_SETTINGS,OnSelchangingTree)
	ON_NOTIFY(TVN_SELCHANGED,IDC_TREE_SETTINGS,OnSelchangedTree)
END_MESSAGE_MAP()

/*
	DoDataExchange()
*/
void CWallPaperThumbSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_STATIC_SETTINGS_AREA,m_ctrlSettingsArea);
	DDX_Control(pDX,IDC_TREE_SETTINGS,m_wndSettingsTree);
}

/*
	CWallPaperThumbSettingsDlg()
*/
CWallPaperThumbSettingsDlg::CWallPaperThumbSettingsDlg(CWnd* pParent,CWallPaperConfig* pConfig) : CDialog(IDD_DIALOG_SETTINGS,pParent)
{
	m_pConfig = pConfig;
	m_bCreated = FALSE;
	m_dwRootData = 0L;
	m_bGenerateHtml = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_GENERATETABLE_KEY);
}

/*
	OnInitDialog()
*/
BOOL CWallPaperThumbSettingsDlg::OnInitDialog(void)
{
	// classe base
	CDialog::OnInitDialog();

	// titolo
	SetWindowText(IDS_DIALOG_THUMBNAILS_SETTINGS_TITLE);

	// cornice interna
	m_ctrlSettingsArea.SetIconSize(16);
	m_ctrlSettingsArea.SetIcon(IDI_ICON_SETTINGS);
	m_ctrlSettingsArea.SetText(IDS_DIALOG_THUMBNAILS_SETTINGS_TITLE);	
	m_ctrlSettingsArea.ShowWindow(SW_SHOW);

	// icona di default
	SetIcon((HICON)::LoadImage(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_ICON_WALLPAPER),IMAGE_ICON,16,16,LR_DEFAULTCOLOR),FALSE);
	SetIcon((HICON)::LoadImage(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_ICON_WALLPAPER),IMAGE_ICON,32,32,LR_DEFAULTCOLOR),TRUE);

	// crea l'albero per le opzioni
	m_hItemRoot = m_wndSettingsTree.InsertItem("Thumbnails",TVI_ROOT,TVI_LAST);
	m_wndSettingsTree.SetItemData(m_hItemRoot,m_dwRootData);

	// Format
	m_hItemFormat = m_wndSettingsTree.InsertItem("Format",m_hItemRoot,TVI_LAST);
	m_wndSettingsTree.SetItemData(m_hItemFormat,(DWORD)&m_dlgTreeFormat);

	// HTML Output
	m_hItemHtmlOutput = m_wndSettingsTree.InsertItem("HTML Output",m_hItemRoot,TVI_LAST);
	m_wndSettingsTree.SetItemData(m_hItemHtmlOutput,(DWORD)&m_dlgTreeHtmlOutput);

	// HTML Table
	m_hItemHtmlTable = m_wndSettingsTree.InsertItem("HTML Table",m_hItemRoot,TVI_LAST);
	m_wndSettingsTree.SetItemData(m_hItemHtmlTable,(DWORD)&m_dlgTreeHtmlTable);

	// posiziona sulla radice dell'albero	
	m_wndSettingsTree.Expand(m_hItemRoot,TVE_EXPAND);
	m_wndSettingsTree.SelectItem(m_hItemRoot);

	// crea i dialoghi relativi
	m_dlgTreeFormat.CreateEx(this,IDD_DIALOG_THUMBNAILS_SETTINGS_FORMAT,m_pConfig);
	m_dlgTreeHtmlOutput.SetGenerateHtmlFlag(m_bGenerateHtml);
	m_dlgTreeHtmlOutput.CreateEx(this,IDD_DIALOG_THUMBNAILS_SETTINGS_HTMLOUTPUT,m_pConfig);
	m_dlgTreeHtmlTable.SetGenerateHtmlFlag(m_bGenerateHtml);
	m_dlgTreeHtmlTable.CreateEx(this,IDD_DIALOG_THUMBNAILS_SETTINGS_HTMLTABLE,m_pConfig);

	// posiziona i dialoghi nel controllo
	CRect rcDlg;
	GetDlgItem(IDC_STATIC_SETTINGS_AREA)->GetWindowRect(rcDlg);
	ScreenToClient(rcDlg);
	m_dlgTreeFormat.MoveWindow(rcDlg);  
	m_dlgTreeHtmlOutput.MoveWindow(rcDlg);  
	m_dlgTreeHtmlTable.MoveWindow(rcDlg);  

	m_bCreated = TRUE;

	// trasparenza
	if(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAYERED_KEY))
		m_wndLayered.SetLayer(this->m_hWnd,(BYTE)m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAYERED_VALUE_KEY));

	return(TRUE);
}

/*
	OnOk()
*/
void CWallPaperThumbSettingsDlg::OnOk(void)
{
	// per aggiornare le variabili condivise tra le pagine
	NMHDR nmdr;
	LRESULT lResult = 0L;
	OnSelchangingTree(&nmdr,&lResult);

	// Format --------------------------------------------------------------------------------------------------------------------------------
	if(!m_dlgTreeFormat.UpdateData(TRUE))
	{
		m_wndSettingsTree.SelectItem(m_hItemFormat);
		m_dlgTreeFormat.OnSetActive();
		return;
	}

	// dimensione thumbnails (width/height)
	if(m_dlgTreeFormat.m_nWidth < 0/*DEFAULT_DRAWRECTX_MIN*/ || m_dlgTreeFormat.m_nWidth > (UINT)::GetSystemMetrics(SM_CXSCREEN))
	{
		m_wndSettingsTree.SelectItem(m_hItemFormat);
		m_dlgTreeFormat.OnSetActive();
		::MessageBoxResourceEx(this->m_hWnd,MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_INVALID_WIDTH,DEFAULT_DRAWRECTX_MIN,::GetSystemMetrics(SM_CXSCREEN));
		return;
	}
	if(m_dlgTreeFormat.m_nHeight < 0/*DEFAULT_DRAWRECTY_MIN*/ || m_dlgTreeFormat.m_nHeight > (UINT)::GetSystemMetrics(SM_CYSCREEN))
	{
		m_wndSettingsTree.SelectItem(m_hItemFormat);
		m_dlgTreeFormat.OnSetActive();
		::MessageBoxResourceEx(this->m_hWnd,MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_INVALID_HEIGHT,DEFAULT_DRAWRECTX_MIN,::GetSystemMetrics(SM_CXSCREEN));
		return;
	}

	// regola per la generazione del nome
	if(m_dlgTreeFormat.m_nNumeric)
	{
		if(m_dlgTreeFormat.m_strName.IsEmpty())
		{
			m_wndSettingsTree.SelectItem(m_hItemFormat);
			m_dlgTreeFormat.OnSetActive();
			::MessageBoxResource(this->m_hWnd,MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_INVALID_PREFIXSUFFIX);
			return;
		}
		
		if(!m_dlgTreeFormat.m_nPrefix || !m_dlgTreeFormat.m_nSuffix)
		{
			if(strchr(m_dlgTreeFormat.m_strName,'*') || strchr(m_dlgTreeFormat.m_strName,'?'))
			{
				m_wndSettingsTree.SelectItem(m_hItemFormat);
				m_dlgTreeFormat.OnSetActive();
				::MessageBoxResource(this->m_hWnd,MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_INVALID_PREFIXSUFFIX);
				return;
			}
		}
	}

	m_pConfig->UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_X_KEY,m_dlgTreeFormat.m_nWidth);
	m_pConfig->UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_Y_KEY,m_dlgTreeFormat.m_nHeight);
	m_pConfig->UpdateString(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_FORMAT_KEY,m_dlgTreeFormat.m_strFormat);
	int nNamingRule = 0;
	switch(m_dlgTreeFormat.m_NamingRule)
	{
		case NUMERIC_NAMINGRULE:
			nNamingRule = 1;
			m_pConfig->UpdateString(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_NUMBER_KEY,m_dlgTreeFormat.m_strName);
			break;
		case	PREFIX_NAMINGRULE:
			nNamingRule = 2;
			m_pConfig->UpdateString(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_NAME_KEY,m_dlgTreeFormat.m_strName);
			break;
		case SUFFIX_NAMINGRULE:
			nNamingRule = 3;
			m_pConfig->UpdateString(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_NAME_KEY,m_dlgTreeFormat.m_strName);
			break;
		case DEFAULT_NAMINGRULE:
		case UNDEFINED_NAMINGRULE:
		default:
			nNamingRule = 0;
			m_pConfig->UpdateString(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_NAME_KEY,DEFAULT_THUMBNAILS_NAME);
			break;
	}
	m_pConfig->UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_NAMING_KEY,nNamingRule);

	// HTML Output ---------------------------------------------------------------------------------------------------------------------------
	if(!m_dlgTreeHtmlOutput.UpdateData(TRUE))
	{
		m_wndSettingsTree.SelectItem(m_hItemHtmlOutput);
		m_dlgTreeHtmlOutput.OnSetActive();
		return;
	}

	// nome file di output
	if(m_bGenerateHtml)
		if(m_dlgTreeHtmlOutput.m_strHtmlOutput.IsEmpty())
		{
			m_wndSettingsTree.SelectItem(m_hItemHtmlOutput);
			m_dlgTreeHtmlOutput.OnSetActive();
			::MessageBoxResource(this->m_hWnd,MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_INVALID_HTML_NAME);
			return;
		}

	m_pConfig->UpdateString(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_HTMLFILE_KEY,m_dlgTreeHtmlOutput.m_strHtmlOutput);
	m_pConfig->UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_GENERATETABLE_KEY,m_bGenerateHtml);

	// HTML Table ----------------------------------------------------------------------------------------------------------------------------
	if(!m_dlgTreeHtmlTable.UpdateData(TRUE))
	{
		m_wndSettingsTree.SelectItem(m_hItemHtmlTable);
		m_dlgTreeHtmlTable.OnSetActive();
		return;
	}

	// dimensione della tabella (% width)
	if(m_dlgTreeHtmlTable.m_nWidth < MIN_THUMBNAILS_TABLE_WIDTH || m_dlgTreeHtmlTable.m_nWidth > MAX_THUMBNAILS_TABLE_WIDTH)
	{
		m_wndSettingsTree.SelectItem(m_hItemHtmlTable);
		m_dlgTreeHtmlTable.OnSetActive();
		::MessageBoxResourceEx(this->m_hWnd,MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_INVALID_TABLE_WIDTH,MIN_THUMBNAILS_TABLE_WIDTH,MAX_THUMBNAILS_TABLE_WIDTH);
		return;
	}
	
	// numero di colonne della tabella
	if(m_dlgTreeHtmlTable.m_nCols < MIN_THUMBNAILS_COLS || m_dlgTreeHtmlTable.m_nCols > MAX_THUMBNAILS_COLS)
	{
		m_wndSettingsTree.SelectItem(m_hItemHtmlTable);
		m_dlgTreeHtmlTable.OnSetActive();
		::MessageBoxResourceEx(this->m_hWnd,MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_INVALID_COLUMN_NUMBER,MIN_THUMBNAILS_COLS,MAX_THUMBNAILS_COLS);
		return;
	}

	// bordo della tabella
	if(m_dlgTreeHtmlTable.m_nBorder < 0)
	{
		m_wndSettingsTree.SelectItem(m_hItemHtmlTable);
		m_dlgTreeHtmlTable.OnSetActive();
		::MessageBoxResource(this->m_hWnd,MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_INVALID_BORDER_VALUE);
		return;
	}

	// spacing per le celle della tabella
	if(m_dlgTreeHtmlTable.m_nSpacing < 0)
	{
		m_wndSettingsTree.SelectItem(m_hItemHtmlTable);
		m_dlgTreeHtmlTable.OnSetActive();
		::MessageBoxResource(this->m_hWnd,MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_INVALID_SPACING_VALUE);
		return;
	}

	// padding per le celle della tabella
	if(m_dlgTreeHtmlTable.m_nPadding < 0)
	{
		m_wndSettingsTree.SelectItem(m_hItemHtmlTable);
		m_dlgTreeHtmlTable.OnSetActive();
		::MessageBoxResource(this->m_hWnd,MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_INVALID_PADDING_VALUE);
		return;
	}

	m_pConfig->UpdateString(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_TABLETITLE_KEY,m_dlgTreeHtmlTable.m_strTitle);
	m_pConfig->UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_TABLEWIDTH_KEY,m_dlgTreeHtmlTable.m_nWidth);
	m_pConfig->UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_TABLECOLS_KEY,m_dlgTreeHtmlTable.m_nCols);
	m_pConfig->UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_TABLEBORDER_KEY,m_dlgTreeHtmlTable.m_nBorder);
	m_pConfig->UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_TABLESPACING_KEY,m_dlgTreeHtmlTable.m_nSpacing);
	m_pConfig->UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_THUMBNAILS_TABLEPADDING_KEY,m_dlgTreeHtmlTable.m_nPadding);

	EndDialog(IDOK);
}

/*
	OnCancel()
*/
void CWallPaperThumbSettingsDlg::OnCancel(void)
{
	EndDialog(IDCANCEL);
}

/*
	OnSelchangingTree()
*/
void CWallPaperThumbSettingsDlg::OnSelchangingTree(NMHDR* /*pNMHDR*/,LRESULT* pResult)
{
	if(m_bCreated)
	{
		HTREEITEM hCurrent = m_wndSettingsTree.GetSelectedItem();
		if(hCurrent)
		{
			CPropertyPage* pPropertyPage = (CPropertyPage*)m_wndSettingsTree.GetItemData(hCurrent);
			if(pPropertyPage)
			{
				if(pPropertyPage->IsKindOf(RUNTIME_CLASS(CWallPaperThumbSettingsFormatDlg)))
					;
				else if(pPropertyPage->IsKindOf(RUNTIME_CLASS(CWallPaperThumbSettingsHtmlOutputDlg)))
				{
					CWallPaperThumbSettingsHtmlOutputDlg* pPage = (CWallPaperThumbSettingsHtmlOutputDlg*)pPropertyPage;
					m_bGenerateHtml = pPage->GetGenerateHtmlFlag();
				}
				else if(pPropertyPage->IsKindOf(RUNTIME_CLASS(CWallPaperThumbSettingsHtmlTableDlg)))
				{
					CWallPaperThumbSettingsHtmlTableDlg* pPage = (CWallPaperThumbSettingsHtmlTableDlg*)pPropertyPage;
					m_bGenerateHtml = pPage->GetGenerateHtmlFlag();
				}

				pPropertyPage->ShowWindow(SW_HIDE);
			}
		}
	}

	*pResult = 0;
}

/*
	OnSelchangedTree()
*/
void CWallPaperThumbSettingsDlg::OnSelchangedTree(NMHDR* /*pNMHDR*/,LRESULT* pResult)
{
	if(m_bCreated)
	{
		HTREEITEM hCurrent = m_wndSettingsTree.GetSelectedItem();
		if(hCurrent)
		{
			CPropertyPage* pPropertyPage = (CPropertyPage*)m_wndSettingsTree.GetItemData(hCurrent);
			if(pPropertyPage)
			{
				m_ctrlSettingsArea.SetText("");	
				m_ctrlSettingsArea.ShowWindow(SW_HIDE);

				if(pPropertyPage->IsKindOf(RUNTIME_CLASS(CWallPaperThumbSettingsFormatDlg)))
					;
				else if(pPropertyPage->IsKindOf(RUNTIME_CLASS(CWallPaperThumbSettingsHtmlOutputDlg)))
				{
					CWallPaperThumbSettingsHtmlOutputDlg* pPage = (CWallPaperThumbSettingsHtmlOutputDlg*)pPropertyPage;
					pPage->SetGenerateHtmlFlag(m_bGenerateHtml);
				}
				else if(pPropertyPage->IsKindOf(RUNTIME_CLASS(CWallPaperThumbSettingsHtmlTableDlg)))
				{
					CWallPaperThumbSettingsHtmlTableDlg* pPage = (CWallPaperThumbSettingsHtmlTableDlg*)pPropertyPage;
					pPage->SetGenerateHtmlFlag(m_bGenerateHtml);
				}

				pPropertyPage->ShowWindow(SW_SHOW);
				pPropertyPage->OnSetActive();
			}
			else
			{
				m_ctrlSettingsArea.SetText(IDS_DIALOG_THUMBNAILS_SETTINGS_TITLE);	
				m_ctrlSettingsArea.ShowWindow(SW_SHOW);
			}
		}
	}

	*pResult = 0;
}
