/*
	WallPaperCrawlerSettingsDlg.cpp
	Dialogo per le opzioni del crawler.
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
#include <stdio.h>
#include "strings.h"
#include "window.h"
#include "win32api.h"
#include "CIconStatic.h"
#include "CFileDialogEx.h"
#include "CImageFactory.h"
#include "CWndLayered.h"
#include "WallPaperConfig.h"
#include "WallPaperMessages.h"
#include "WallPaperCrawlerSettingsDlg.h"
#include "WallPaperCrawlerSettingsConnDlg.h"
#include "WallPaperCrawlerSettingsDownDlg.h"
#include "WallPaperCrawlerSettingsFileTypesDlg.h"
#include "WallPaperCrawlerSettingsDomainDlg.h"
#include "WallPaperCrawlerSettingsParentUrlDlg.h"
#include "WallPaperCrawlerSettingsWildcardsDlg.h"
#include "WallPaperCrawlerSettingsProxyDlg.h"
#include "WallPaperCrawlerSettingsReportDlg.h"
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

BEGIN_MESSAGE_MAP(CWallPaperCrawlerSettingsDlg,CDialog)
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
void CWallPaperCrawlerSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_STATIC_SETTINGS_AREA,m_ctrlSettingsArea);
	DDX_Control(pDX,IDC_TREE_SETTINGS,m_wndSettingsTree);
}

/*
	CWallPaperCrawlerSettingsDlg()
*/
CWallPaperCrawlerSettingsDlg::CWallPaperCrawlerSettingsDlg(CWnd* pParent,CWallPaperConfig* pConfig) : CDialog(IDD_DIALOG_SETTINGS,pParent)
{
	m_pConfig = pConfig;
	m_bCreated = FALSE;
	m_dwRootData = 0L;
	m_nDomainAcceptance = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_DOMAINACCEPTANCE_KEY);
	m_bIncludeSubDomains = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_INCLUDESUBDOMAINS_KEY);
	m_bDomainUseAlways = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_DOMAINUSEALWAYS_KEY);
	m_nParentUrlAcceptance = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_PARENTACCEPTANCE_KEY);
	m_bParentUrlUseAlways = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_PARENTURLUSEALWAYS_KEY);
	m_bSaveList = m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_SAVEINCLEXCLLIST_KEY);
}

/*
	OnInitDialog()
*/
BOOL CWallPaperCrawlerSettingsDlg::OnInitDialog(void)
{
	// classe base
	CDialog::OnInitDialog();

	// titolo
	SetWindowText(IDS_DIALOG_CRAWLER_SETTINGS_TITLE);

	// cornice interna
	m_ctrlSettingsArea.SetIconSize(16);
	m_ctrlSettingsArea.SetIcon(IDI_ICON_SETTINGS);
	m_ctrlSettingsArea.SetText(IDS_DIALOG_CRAWLER_SETTINGS_TITLE);	
	m_ctrlSettingsArea.ShowWindow(SW_SHOW);

	// icona di default
	SetIcon((HICON)::LoadImage(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_ICON_WALLPAPER),IMAGE_ICON,16,16,LR_DEFAULTCOLOR),FALSE);
	SetIcon((HICON)::LoadImage(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_ICON_WALLPAPER),IMAGE_ICON,32,32,LR_DEFAULTCOLOR),TRUE);

	// crea l'albero per le opzioni
	m_hItemRoot = m_wndSettingsTree.InsertItem("Crawler",TVI_ROOT,TVI_LAST);
	m_wndSettingsTree.SetItemData(m_hItemRoot,m_dwRootData);

	// Connection
	m_hItemConnection = m_wndSettingsTree.InsertItem("Connection",m_hItemRoot,TVI_LAST);
	m_wndSettingsTree.SetItemData(m_hItemConnection,(DWORD)&m_dlgTreeConnection);

	// Download
	m_hItemDownload = m_wndSettingsTree.InsertItem("Download",m_hItemRoot,TVI_LAST);
	m_wndSettingsTree.SetItemData(m_hItemDownload,(DWORD)&m_dlgTreeDownload);

	// File Types
	m_hItemFileTypes = m_wndSettingsTree.InsertItem("File Types",m_hItemRoot,TVI_LAST);
	m_wndSettingsTree.SetItemData(m_hItemFileTypes,(DWORD)&m_dlgTreeFileTypes);

	// Domain Acceptance
	m_hItemDomainAcceptance = m_wndSettingsTree.InsertItem("Domain Acceptance",m_hItemRoot,TVI_LAST);
	m_wndSettingsTree.SetItemData(m_hItemDomainAcceptance,m_dwRootData);
	m_hItemDomainInclude = m_wndSettingsTree.InsertItem("Include",m_hItemDomainAcceptance,TVI_LAST);
	m_wndSettingsTree.SetItemData(m_hItemDomainInclude,(DWORD)&m_dlgTreeDomainInclude);
	m_hItemDomainExclude = m_wndSettingsTree.InsertItem("Exclude",m_hItemDomainAcceptance,TVI_LAST);
	m_wndSettingsTree.SetItemData(m_hItemDomainExclude,(DWORD)&m_dlgTreeDomainExclude);

	// Parent Url Acceptance
	m_hItemParentUrlAcceptance = m_wndSettingsTree.InsertItem("Parent Url Acceptance",m_hItemRoot,TVI_LAST);
	m_wndSettingsTree.SetItemData(m_hItemParentUrlAcceptance,m_dwRootData);
	m_hItemParentUrlInclude = m_wndSettingsTree.InsertItem("Include",m_hItemParentUrlAcceptance,TVI_LAST);
	m_wndSettingsTree.SetItemData(m_hItemParentUrlInclude,(DWORD)&m_dlgTreeParentUrlInclude);
	m_hItemParentUrlExclude = m_wndSettingsTree.InsertItem("Exclude",m_hItemParentUrlAcceptance,TVI_LAST);
	m_wndSettingsTree.SetItemData(m_hItemParentUrlExclude,(DWORD)&m_dlgTreeParentUrlExclude);

	// Wildcards
	m_hItemWildcards = m_wndSettingsTree.InsertItem("Wildcards",m_hItemRoot,TVI_LAST);
	m_wndSettingsTree.SetItemData(m_hItemWildcards,(DWORD)&m_dlgTreeWildcards);

	// Proxy
	m_hItemProxy = m_wndSettingsTree.InsertItem("Proxy",m_hItemRoot,TVI_LAST);
	m_wndSettingsTree.SetItemData(m_hItemProxy,(DWORD)&m_dlgTreeProxy);

	// Report
	m_hItemReport = m_wndSettingsTree.InsertItem("Report",m_hItemRoot,TVI_LAST);
	m_wndSettingsTree.SetItemData(m_hItemReport,(DWORD)&m_dlgTreeReport);

	// posiziona sulla radice dell'albero	
	m_wndSettingsTree.Expand(m_hItemRoot,TVE_EXPAND);
	m_wndSettingsTree.SelectItem(m_hItemRoot);

	// crea i dialoghi relativi
	m_dlgTreeConnection.CreateEx(this,IDD_DIALOG_CRAWLER_SETTINGS_CONNECTION,m_pConfig);
	m_dlgTreeDownload.CreateEx(this,IDD_DIALOG_CRAWLER_SETTINGS_DOWNLOAD,m_pConfig);
	m_dlgTreeFileTypes.CreateEx(this,IDD_DIALOG_CRAWLER_SETTINGS_FILETYPES,m_pConfig);
	
	m_dlgTreeDomainInclude.SetFlags(DOMAIN_FLAG_INCLUDE);
	m_dlgTreeDomainInclude.SetDomainAcceptance(m_nDomainAcceptance);
	m_dlgTreeDomainInclude.SetIncludeSubDomains(m_bIncludeSubDomains);
	m_dlgTreeDomainInclude.SetDomainUseAlways(m_bDomainUseAlways);
	m_dlgTreeDomainInclude.SetSaveList(m_bSaveList);
	m_dlgTreeDomainInclude.CreateEx(this,IDD_DIALOG_CRAWLER_SETTINGS_DOMAIN,m_pConfig);
	
	m_dlgTreeDomainExclude.SetFlags(DOMAIN_FLAG_EXCLUDE);
	m_dlgTreeDomainExclude.SetDomainAcceptance(m_nDomainAcceptance);
	m_dlgTreeDomainExclude.SetIncludeSubDomains(m_bIncludeSubDomains);
	m_dlgTreeDomainExclude.SetDomainUseAlways(m_bDomainUseAlways);
	m_dlgTreeDomainExclude.SetSaveList(m_bSaveList);
	m_dlgTreeDomainExclude.CreateEx(this,IDD_DIALOG_CRAWLER_SETTINGS_DOMAIN,m_pConfig);
	
	m_dlgTreeParentUrlInclude.SetFlags(PARENTURL_FLAG_INCLUDE);
	m_dlgTreeParentUrlInclude.SetParentUrlAcceptance(m_nParentUrlAcceptance);
	m_dlgTreeParentUrlInclude.SetParentUrlUseAlways(m_bParentUrlUseAlways);
	m_dlgTreeParentUrlInclude.SetSaveList(m_bSaveList);
	m_dlgTreeParentUrlInclude.CreateEx(this,IDD_DIALOG_CRAWLER_SETTINGS_PARENTURL,m_pConfig);
	
	m_dlgTreeParentUrlExclude.SetFlags(PARENTURL_FLAG_EXCLUDE);
	m_dlgTreeParentUrlExclude.SetParentUrlAcceptance(m_nParentUrlAcceptance);
	m_dlgTreeParentUrlExclude.SetParentUrlUseAlways(m_bParentUrlUseAlways);
	m_dlgTreeParentUrlExclude.SetSaveList(m_bSaveList);
	m_dlgTreeParentUrlExclude.CreateEx(this,IDD_DIALOG_CRAWLER_SETTINGS_PARENTURL,m_pConfig);
	
	m_dlgTreeWildcards.SetSaveList(m_bSaveList);
	m_dlgTreeWildcards.CreateEx(this,IDD_DIALOG_CRAWLER_SETTINGS_WILDCARDS,m_pConfig);
	
	m_dlgTreeProxy.CreateEx(this,IDD_DIALOG_CRAWLER_SETTINGS_PROXY,m_pConfig);

	m_dlgTreeReport.CreateEx(this,IDD_DIALOG_CRAWLER_SETTINGS_REPORT,m_pConfig);

	// posiziona i dialoghi nel controllo
	CRect rcDlg;
	GetDlgItem(IDC_STATIC_SETTINGS_AREA)->GetWindowRect(rcDlg);
	ScreenToClient(rcDlg);
	m_dlgTreeConnection.MoveWindow(rcDlg);
	m_dlgTreeDownload.MoveWindow(rcDlg);
	m_dlgTreeFileTypes.MoveWindow(rcDlg);
	m_dlgTreeDomainInclude.MoveWindow(rcDlg);
	m_dlgTreeDomainExclude.MoveWindow(rcDlg);
	m_dlgTreeParentUrlInclude.MoveWindow(rcDlg);
	m_dlgTreeParentUrlExclude.MoveWindow(rcDlg);
	m_dlgTreeWildcards.MoveWindow(rcDlg);
	m_dlgTreeProxy.MoveWindow(rcDlg);
	m_dlgTreeReport.MoveWindow(rcDlg);

	m_bCreated = TRUE;

	// trasparenza
	if(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAYERED_KEY))
		m_wndLayered.SetLayer(this->m_hWnd,(BYTE)m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_LAYERED_VALUE_KEY));

	return(TRUE);
}

/*
	OnOk()
*/
void CWallPaperCrawlerSettingsDlg::OnOk(void)
{
	int i,nIndex,nTot;
	BOOL bFound;
	char szKey[REGKEY_MAX_KEY_NAME+1];
	char szItem[REGKEY_MAX_KEY_NAME+1];

	// per aggiornare le variabili condivise tra le pagine
	NMHDR nmdr;
	LRESULT lResult = 0L;
	OnSelchangingTree(&nmdr,&lResult);

	// Connection
	if(!m_dlgTreeConnection.UpdateData(TRUE))
	{
		m_wndSettingsTree.SelectItem(m_hItemConnection);
		m_dlgTreeConnection.OnSetActive();
		return;
	}

	if(m_dlgTreeConnection.m_nRetry!=0)
		if(m_dlgTreeConnection.m_nRetry==(UINT)-1 || m_dlgTreeConnection.m_nRetry < MIN_RETRY_VALUE || m_dlgTreeConnection.m_nRetry > MAX_RETRY_VALUE)
		{
			m_wndSettingsTree.SelectItem(m_hItemConnection);
			m_dlgTreeConnection.OnSetActive();
			::MessageBoxResourceEx(this->m_hWnd,MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_INVALID_RETRY,MIN_RETRY_VALUE,MAX_RETRY_VALUE);
			return;
		}

	if(m_dlgTreeConnection.m_nTimeout!=0)
		if(m_dlgTreeConnection.m_nTimeout==(UINT)-1 || m_dlgTreeConnection.m_nTimeout < MIN_TIMEOUT_VALUE || m_dlgTreeConnection.m_nTimeout > MAX_TIMEOUT_VALUE)
		{
			m_wndSettingsTree.SelectItem(m_hItemConnection);
			m_dlgTreeConnection.OnSetActive();
			::MessageBoxResourceEx(this->m_hWnd,MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_INVALID_TIMEOUT,MIN_TIMEOUT_VALUE,MAX_TIMEOUT_VALUE);
			return;
		}

	m_pConfig->UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_CONNECTIONRETRY_KEY,m_dlgTreeConnection.m_nRetry);
	m_pConfig->UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_CONNECTIONTIMEOUT_KEY,m_dlgTreeConnection.m_nTimeout);
	m_pConfig->UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_USEWINSOCKTIMEOUT_KEY,m_dlgTreeConnection.m_bWinsockTimeout);
	m_pConfig->UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_COOKIES_ALLOWED_KEY,m_dlgTreeConnection.m_bAllowCookies);
	m_pConfig->UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_COOKIES_WARNINGONACCEPT_KEY,m_dlgTreeConnection.m_bWarnBeforeAcceptCookie);
	m_pConfig->UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_COOKIES_WARNINGONSEND_KEY,m_dlgTreeConnection.m_bWarnBeforeSendCookie);

	// Download
	if(!m_dlgTreeDownload.UpdateData(TRUE))
	{
		m_wndSettingsTree.SelectItem(m_hItemDownload);
		m_dlgTreeDownload.OnSetActive();
		return;
	}

	m_pConfig->UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_ADDPICTURES_KEY,m_dlgTreeDownload.m_bAddDownloads);
	m_pConfig->UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_ONLYCONTENT_KEY,m_dlgTreeDownload.m_bDownloadPageContent);
	m_pConfig->UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_MINSIZETYPE_KEY,m_dlgTreeDownload.m_nMinSizeType);
	m_pConfig->UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_MINSIZE_KEY,m_dlgTreeDownload.m_nMinSize);
	m_pConfig->UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_MAXSIZETYPE_KEY,m_dlgTreeDownload.m_nMaxSizeType);
	m_pConfig->UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_MAXSIZE_KEY,m_dlgTreeDownload.m_nMaxSize);
	m_pConfig->UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_SKIPEXISTING_KEY,m_dlgTreeDownload.m_bSkipExisting);
	m_pConfig->UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_SKIPEXISTING_FORCE_KEY,m_dlgTreeDownload.m_bForceSkipExisting);
	m_pConfig->UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_SKIPEXISTING_DONT_KEY,m_dlgTreeDownload.m_bDoNotSkipExisting);
	m_pConfig->UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_QUIETINSCRIPTMODE_KEY,m_dlgTreeDownload.m_bBeQuiet);
	m_pConfig->UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_ENABLECGI_KEY,m_dlgTreeDownload.m_bFollowCgi);
	m_pConfig->UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_ENABLEJAVASCRIPT_KEY,m_dlgTreeDownload.m_bFollowJavascript);
	m_pConfig->UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_ENABLEROBOTSTXT_KEY,m_dlgTreeDownload.m_bUseRobotsTxt);

	nTot = 0;
	nIndex = 0;
	bFound = FALSE;

	// se viene specificato il flag per l'analisi dei javascript, controlla se il tipo relativo si trova nella lista
	nTot = m_dlgTreeFileTypes.m_wndFileTypesList.GetItemCount();
	for(i = 0; i < nTot; i++)
	{
		memset(szItem,'\0',sizeof(szItem));
		m_dlgTreeFileTypes.m_wndFileTypesList.GetItemText(i,0,szItem,sizeof(szItem));
		if(stricmp(szItem,".js")==0)
		{
			bFound = TRUE;
			break;
		}
	}

	if(m_pConfig->GetNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_ENABLEJAVASCRIPT_KEY))	
	{
		if(!bFound)
		{
			if(::MessageBoxResourceEx(this->m_hWnd,MB_YESNO|MB_ICONQUESTION,WALLPAPER_PROGRAM_NAME,IDS_QUESTION_ADD_JAVASCRIPT_TYPE,".js",".js")==IDYES)
				m_dlgTreeFileTypes.m_wndFileTypesList.AddItem(".js",0);
		}
	}
	else
	{
		if(bFound)
		{
			if(::MessageBoxResourceEx(this->m_hWnd,MB_YESNO|MB_ICONQUESTION,WALLPAPER_PROGRAM_NAME,IDS_QUESTION_REMOVE_JAVASCRIPT_TYPE,".js",".js")==IDYES)
			{
				nTot = m_dlgTreeFileTypes.m_wndFileTypesList.GetItemCount();
				for(i = 0; i < nTot; i++)
				{
					memset(szItem,'\0',sizeof(szItem));
					m_dlgTreeFileTypes.m_wndFileTypesList.GetItemText(i,0,szItem,sizeof(szItem));
					if(stricmp(szItem,".js")==0)
					{
						m_dlgTreeFileTypes.m_wndFileTypesList.DeleteItem(i);
						break;
					}
				}
			}
		}
	}

	// FileTypes
	if(!m_dlgTreeFileTypes.UpdateData(TRUE))
	{
		m_wndSettingsTree.SelectItem(m_hItemFileTypes);
		m_dlgTreeFileTypes.OnSetActive();
		return;
	}

	m_pConfig->UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_FILETYPESACCEPTANCE_KEY,m_dlgTreeFileTypes.m_bAskFileTypes);
	m_pConfig->UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_MULTITHREAD_KEY,m_dlgTreeFileTypes.m_bMultiThread);

	// azzera la sezione e reinserisce quanto presente nella lista
	m_pConfig->DeleteSection(WALLPAPER_FILETYPELIST_KEY);
	nTot = m_dlgTreeFileTypes.m_wndFileTypesList.GetItemCount();
	for(i = 0; i < nTot; i++)
	{
		memset(szItem,'\0',sizeof(szItem));
		m_dlgTreeFileTypes.m_wndFileTypesList.GetItemText(i,0,szItem,sizeof(szItem));
		_snprintf(szKey,sizeof(szKey)-1,"%s%d",WALLPAPER_FILETYPE_KEY,i);
		m_pConfig->Insert(WALLPAPER_FILETYPELIST_KEY,szKey,szItem);
	}

	// Domain Acceptance
	if(!m_dlgTreeDomainInclude.UpdateData(TRUE))
	{
		m_wndSettingsTree.SelectItem(m_hItemDomainInclude);
		m_dlgTreeDomainInclude.OnSetActive();
		return;
	}

	// azzera la sezione e reinserisce quanto presente nella lista
	m_pConfig->DeleteSection(WALLPAPER_DOMAININCLUDELIST_KEY);
	nTot = m_dlgTreeDomainInclude.m_wndDomainList.GetItemCount();
	for(i = 0; i < nTot; i++)
	{
		memset(szItem,'\0',sizeof(szItem));
		m_dlgTreeDomainInclude.m_wndDomainList.GetItemText(i,0,szItem,sizeof(szItem));
		_snprintf(szKey,sizeof(szKey)-1,"%s%d",WALLPAPER_DOMAININCLUDE_KEY,i);
		m_pConfig->Insert(WALLPAPER_DOMAININCLUDELIST_KEY,szKey,szItem);
	}

	if(!m_dlgTreeDomainExclude.UpdateData(TRUE))
	{
		m_wndSettingsTree.SelectItem(m_hItemDomainExclude);
		m_dlgTreeDomainExclude.OnSetActive();
		return;
	}

	// azzera la sezione e reinserisce quanto presente nella lista
	m_pConfig->DeleteSection(WALLPAPER_DOMAINEXCLUDELIST_KEY);
	nTot = m_dlgTreeDomainExclude.m_wndDomainList.GetItemCount();
	for(i = 0; i < nTot; i++)
	{
		memset(szItem,'\0',sizeof(szItem));
		m_dlgTreeDomainExclude.m_wndDomainList.GetItemText(i,0,szItem,sizeof(szItem));
		_snprintf(szKey,sizeof(szKey)-1,"%s%d",WALLPAPER_DOMAINEXCLUDE_KEY,i);
		m_pConfig->Insert(WALLPAPER_DOMAINEXCLUDELIST_KEY,szKey,szItem);
	}
	
	m_pConfig->UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_DOMAINACCEPTANCE_KEY,m_nDomainAcceptance);
	m_pConfig->UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_INCLUDESUBDOMAINS_KEY,m_bIncludeSubDomains);
	m_pConfig->UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_DOMAINUSEALWAYS_KEY,m_bDomainUseAlways);

	// Parent Url Acceptance
	if(!m_dlgTreeParentUrlInclude.UpdateData(TRUE))
	{
		m_wndSettingsTree.SelectItem(m_hItemParentUrlInclude);
		m_dlgTreeParentUrlInclude.OnSetActive();
		return;
	}

	// azzera la sezione e reinserisce quanto presente nella lista
	m_pConfig->DeleteSection(WALLPAPER_PARENTINCLUDELIST_KEY);
	nTot = m_dlgTreeParentUrlInclude.m_wndParentUrlList.GetItemCount();
	for(i = 0; i < nTot; i++)
	{
		memset(szItem,'\0',sizeof(szItem));
		m_dlgTreeParentUrlInclude.m_wndParentUrlList.GetItemText(i,0,szItem,sizeof(szItem));
		_snprintf(szKey,sizeof(szKey)-1,"%s%d",WALLPAPER_PARENTINCLUDE_KEY,i);
		m_pConfig->Insert(WALLPAPER_PARENTINCLUDELIST_KEY,szKey,szItem);
	}

	if(!m_dlgTreeParentUrlExclude.UpdateData(TRUE))
	{
		m_wndSettingsTree.SelectItem(m_hItemParentUrlExclude);
		m_dlgTreeParentUrlExclude.OnSetActive();
		return;
	}

	// azzera la sezione e reinserisce quanto presente nella lista
	m_pConfig->DeleteSection(WALLPAPER_PARENTEXCLUDELIST_KEY);
	nTot = m_dlgTreeParentUrlExclude.m_wndParentUrlList.GetItemCount();
	for(i = 0; i < nTot; i++)
	{
		memset(szItem,'\0',sizeof(szItem));
		m_dlgTreeParentUrlExclude.m_wndParentUrlList.GetItemText(i,0,szItem,sizeof(szItem));
		_snprintf(szKey,sizeof(szKey)-1,"%s%d",WALLPAPER_PARENTEXCLUDE_KEY,i);
		m_pConfig->Insert(WALLPAPER_PARENTEXCLUDELIST_KEY,szKey,szItem);
	}
	
	m_pConfig->UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_PARENTACCEPTANCE_KEY,m_nParentUrlAcceptance);
	m_pConfig->UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_PARENTURLUSEALWAYS_KEY,m_bParentUrlUseAlways);

	// Wildcards
	if(!m_dlgTreeWildcards.UpdateData(TRUE))
	{
		m_wndSettingsTree.SelectItem(m_hItemWildcards);
		m_dlgTreeWildcards.OnSetActive();
		return;
	}

	// azzera la sezione e reinserisce quanto presente nella lista
	m_pConfig->DeleteSection(WALLPAPER_WILDCARDSEXCLUDELIST_KEY);
	nTot = m_dlgTreeWildcards.m_wndWildcardsList.GetItemCount();
	for(i = 0; i < nTot; i++)
	{
		memset(szItem,'\0',sizeof(szItem));
		m_dlgTreeWildcards.m_wndWildcardsList.GetItemText(i,0,szItem,sizeof(szItem));
		_snprintf(szKey,sizeof(szKey)-1,"%s%d",WALLPAPER_WILDCARDSEXCLUDE_KEY,i);
		m_pConfig->Insert(WALLPAPER_WILDCARDSEXCLUDELIST_KEY,szKey,szItem);
	}

	m_pConfig->UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_SAVEINCLEXCLLIST_KEY,m_bSaveList);

	// Proxy
	if(!m_dlgTreeProxy.UpdateData(TRUE))
	{
		m_wndSettingsTree.SelectItem(m_hItemProxy);
		m_dlgTreeProxy.OnSetActive();
		return;
	}

	m_pConfig->UpdateString(WALLPAPER_OPTIONS_KEY,WALLPAPER_PROXY_ADDRESS_KEY,m_dlgTreeProxy.m_strProxyAddress);
	m_pConfig->UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_PROXY_PORTNUMBER_KEY,m_dlgTreeProxy.m_nProxyPort);
	m_pConfig->UpdateString(WALLPAPER_OPTIONS_KEY,WALLPAPER_PROXY_USER_KEY,m_dlgTreeProxy.m_strProxyUser);
	m_pConfig->UpdateString(WALLPAPER_OPTIONS_KEY,WALLPAPER_PROXY_PASSWORD_KEY,m_dlgTreeProxy.m_strProxyPassword);
	m_pConfig->UpdateString(WALLPAPER_OPTIONS_KEY,WALLPAPER_PROXY_EXCLUDEDOMAINS_KEY,m_dlgTreeProxy.m_strProxyExcludeDomains);
	m_pConfig->UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_USEPROXY_KEY,m_dlgTreeProxy.m_bUseProxy);

	// Report
	if(!m_dlgTreeReport.UpdateData(TRUE))
	{
		m_wndSettingsTree.SelectItem(m_hItemReport);
		m_dlgTreeReport.OnSetActive();
		return;
	}

	m_pConfig->UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_REPORTORDER_KEY,m_dlgTreeReport.m_nSortedBy);
	m_pConfig->UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_GENERATEASCII_KEY,m_dlgTreeReport.m_bGenerateAscii);
	m_pConfig->UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_GENERATEREPORT_KEY,m_dlgTreeReport.m_bGenerateHtml);
	m_pConfig->UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_GENERATETHUMBNAILS_KEY,m_dlgTreeReport.m_bGenerateThumbnails);
	m_pConfig->UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_DUMPDATABASE_KEY,m_dlgTreeReport.m_bDumpDatabase);
	m_pConfig->UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_EXTRACTTEXT_KEY,m_dlgTreeReport.m_bExtractText);
	m_pConfig->UpdateNumber(WALLPAPER_OPTIONS_KEY,WALLPAPER_CRAWLER_EXTRACTJAVASCRIPT_KEY,m_dlgTreeReport.m_bExtractJavascript);

	EndDialog(IDOK);
}

/*
	OnCancel()
*/
void CWallPaperCrawlerSettingsDlg::OnCancel(void)
{
	EndDialog(IDCANCEL);
}

/*
	OnSelchangingTree()
*/
void CWallPaperCrawlerSettingsDlg::OnSelchangingTree(NMHDR* /*pNMHDR*/,LRESULT* pResult)
{
	if(m_bCreated)
	{
		HTREEITEM hCurrent = m_wndSettingsTree.GetSelectedItem();
		if(hCurrent)
		{
			CPropertyPage* pPropertyPage = (CPropertyPage*)m_wndSettingsTree.GetItemData(hCurrent);
			if(pPropertyPage)
			{
				if(pPropertyPage->IsKindOf(RUNTIME_CLASS(CWallPaperCrawlerSettingsDomainDlg)))
				{
					CWallPaperCrawlerSettingsDomainDlg* pPage = (CWallPaperCrawlerSettingsDomainDlg*)pPropertyPage;
					m_nDomainAcceptance = pPage->GetDomainAcceptance();
					m_bIncludeSubDomains = pPage->GetIncludeSubDomains();
					m_bDomainUseAlways = pPage->GetDomainUseAlways();
					m_bSaveList = pPage->GetSaveList();
				}
				else if(pPropertyPage->IsKindOf(RUNTIME_CLASS(CWallPaperCrawlerSettingsParentUrlDlg)))
				{
					CWallPaperCrawlerSettingsParentUrlDlg* pPage = (CWallPaperCrawlerSettingsParentUrlDlg*)pPropertyPage;
					m_nParentUrlAcceptance = pPage->GetParentUrlAcceptance();
					m_bParentUrlUseAlways= pPage->GetParentUrlUseAlways();
					m_bSaveList = pPage->GetSaveList();
				}
				else if(pPropertyPage->IsKindOf(RUNTIME_CLASS(CWallPaperCrawlerSettingsWildcardsDlg)))
				{
					CWallPaperCrawlerSettingsWildcardsDlg* pPage = (CWallPaperCrawlerSettingsWildcardsDlg*)pPropertyPage;
					m_bSaveList = pPage->GetSaveList();
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
void CWallPaperCrawlerSettingsDlg::OnSelchangedTree(NMHDR* /*pNMHDR*/,LRESULT* pResult)
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

				if(pPropertyPage->IsKindOf(RUNTIME_CLASS(CWallPaperCrawlerSettingsDomainDlg)))
				{
					CWallPaperCrawlerSettingsDomainDlg* pPage = (CWallPaperCrawlerSettingsDomainDlg*)pPropertyPage;
					pPage->SetDomainAcceptance(m_nDomainAcceptance);
					pPage->SetIncludeSubDomains(m_bIncludeSubDomains);
					pPage->SetDomainUseAlways(m_bDomainUseAlways);
					pPage->SetSaveList(m_bSaveList);
				}
				else if(pPropertyPage->IsKindOf(RUNTIME_CLASS(CWallPaperCrawlerSettingsParentUrlDlg)))
				{
					CWallPaperCrawlerSettingsParentUrlDlg* pPage = (CWallPaperCrawlerSettingsParentUrlDlg*)pPropertyPage;
					pPage->SetParentUrlAcceptance(m_nParentUrlAcceptance);
					pPage->SetParentUrlUseAlways(m_bParentUrlUseAlways);
					pPage->SetSaveList(m_bSaveList);
				}
				else if(pPropertyPage->IsKindOf(RUNTIME_CLASS(CWallPaperCrawlerSettingsWildcardsDlg)))
				{
					CWallPaperCrawlerSettingsWildcardsDlg* pPage = (CWallPaperCrawlerSettingsWildcardsDlg*)pPropertyPage;
					pPage->SetSaveList(m_bSaveList);
				}

				pPropertyPage->ShowWindow(SW_SHOW);
				pPropertyPage->OnSetActive();
			}
			else
			{
				m_ctrlSettingsArea.SetText(IDS_DIALOG_CRAWLER_SETTINGS_TITLE);	
				m_ctrlSettingsArea.ShowWindow(SW_SHOW);
			}
		}
	}

	*pResult = 0;
}
