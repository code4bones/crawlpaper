/*
	WallPaperCrawlerSettingsParentUrlDlg.cpp
	Dialogo per la pagina relativa alle opzioni per il crawler.
	Luca Piergentili, 20/09/01
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
#include "CListCtrlEx.h"
#include "CUrl.h"
#include "CToolTipCtrlEx.h"
#include "WallPaperConfig.h"
#include "WallPaperCrawlerSettingsParentUrlDlg.h"
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

BEGIN_MESSAGE_MAP(CWallPaperCrawlerSettingsParentUrlDlg,CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_ADD,OnButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE,OnButtonRemove)
	ON_CBN_SELCHANGE(IDC_COMBO_PARENTURL,OnSelChangeComboParentUrl)
	ON_BN_CLICKED(IDC_CHECK_PARENTURL_USEALWAYS,OnCheckParentUrlUseAlways)
	ON_BN_CLICKED(IDC_CHECK_SAVELIST,OnCheckSaveList)
	ON_MESSAGE(WM_ONLBUTTONDBLCLK,OnListDoubleClick)
END_MESSAGE_MAP()

IMPLEMENT_DYNCREATE(CWallPaperCrawlerSettingsParentUrlDlg,CPropertyPage)

/*
	DoDataExchange()
*/
void CWallPaperCrawlerSettingsParentUrlDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_COMBO_PARENTURL,m_wndComboParentUrl);
	DDX_Text(pDX,IDC_EDIT_VALUE,m_strValue);
	DDX_Control(pDX,IDC_LIST_PARENTURL,m_wndParentUrlList);
	DDX_Check(pDX,IDC_CHECK_PARENTURL_USEALWAYS,m_bParentUrlUseAlways);
	DDX_Check(pDX,IDC_CHECK_SAVELIST,m_bSaveList);
}

/*
	CreateEx()
*/
BOOL CWallPaperCrawlerSettingsParentUrlDlg::CreateEx(CWnd* pParent,UINT nID,CWallPaperConfig* pConfig)
{
	// configurazione corrente
	m_pConfig = pConfig;

	// crea il dialogo
	return(Create(nID,pParent));
}

/*
	OnInitDialog()
*/
BOOL CWallPaperCrawlerSettingsParentUrlDlg::OnInitDialog(void)
{
	// classe base
	CPropertyPage::OnInitDialog();

	// crea il combo
	m_wndComboParentUrl.AddString(IDS_PARENTACCEPTANCE_ASK);
	m_wndComboParentUrl.AddString(IDS_PARENTACCEPTANCE_EXCLUDE_ALL_OUTSIDE);
	m_wndComboParentUrl.AddString(IDS_PARENTACCEPTANCE_EXCLUDE_ALL_OUTSIDE_EXCEPT_PICTURES);
	m_wndComboParentUrl.AddString(IDS_PARENTACCEPTANCE_EXCLUDE_ALL_OUTSIDE_EXCEPT_REGFILETYPES);
	m_wndComboParentUrl.AddString(IDS_PARENTACCEPTANCE_FOLLOW_ALL_OUTSIDE);
	int nIndex = 0;
	switch(m_nParentUrlAcceptance)
	{
		case PARENTACCEPTANCE_ASK:
			nIndex = 0;
			break;
		case PARENTACCEPTANCE_EXCLUDE_ALL_OUTSIDE:
			nIndex = 1;
			break;
		case PARENTACCEPTANCE_EXCLUDE_ALL_OUTSIDE_EXCEPT_PICTURES:
			nIndex = 2;
			break;
		case PARENTACCEPTANCE_EXCLUDE_ALL_OUTSIDE_EXCEPT_REGFILETYPES:
			nIndex = 3;
			break;
		case PARENTACCEPTANCE_FOLLOW_ALL_OUTSIDE:
			nIndex = 4;
			break;
	}
	m_wndComboParentUrl.SetCurSel(nIndex);

	// crea la lista
	m_wndParentUrlList.Create(this);
	m_wndParentUrlList.SetExStyle(LVS_EX_FULLROWSELECT);
	m_wndParentUrlList.SetIlcColor(ILC_MASK);
	m_wndParentUrlList.SetMultipleSelection(FALSE);
	m_wndParentUrlList.SetLDoubleClickMessage(WM_ONLBUTTONDBLCLK);
	m_wndParentUrlList.RightClickSelects(TRUE);

	m_wndParentUrlList.AddCol("Parent Url",'C');

	// imposta a seconda del tipo di lista
	if(m_dwFlags & PARENTURL_FLAG_INCLUDE)
	{
		SetDlgItemText(IDC_STATIC_DOMAIN,"Include List");
		strcpyn(m_szIncludeExcludeKey,WALLPAPER_PARENTINCLUDE_KEY,sizeof(m_szIncludeExcludeKey));
		strcpyn(m_szIncludeExcludeListKey,WALLPAPER_PARENTINCLUDELIST_KEY,sizeof(m_szIncludeExcludeListKey));
		m_wndParentUrlList.AddIcon(::LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_ICON_URL)));
	}
	else if(m_dwFlags & PARENTURL_FLAG_EXCLUDE)
	{
		SetDlgItemText(IDC_STATIC_DOMAIN,"Exclude List");
		strcpyn(m_szIncludeExcludeKey,WALLPAPER_PARENTEXCLUDE_KEY,sizeof(m_szIncludeExcludeKey));
		strcpyn(m_szIncludeExcludeListKey,WALLPAPER_PARENTEXCLUDELIST_KEY,sizeof(m_szIncludeExcludeListKey));
		m_wndParentUrlList.AddIcon(::LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_ICON_INVALIDURL)));
	}

	// inserisce gli items nella lista
	char* p;
	char szKey[REGKEY_MAX_KEY_NAME+1];
	for(int i = 0;; i++)
	{
		_snprintf(szKey,sizeof(szKey)-1,"%s%d",m_szIncludeExcludeKey,i);
		if(strcmp((p = (char*)m_pConfig->GetString(m_szIncludeExcludeListKey,szKey)),"")!=0)
			m_wndParentUrlList.AddItem(p,0);
		else
			break;
	}

	m_wndParentUrlList.Sort();
	m_wndParentUrlList.SelectItem(0);

	// aggiunge i tooltips
	if(m_Tooltip.Create(this,TTS_ALWAYSTIP))
	{
		m_Tooltip.SetWidth(TOOLTIP_REASONABLE_WIDTH);
		m_Tooltip.SetDelay(TOOLTIP_REASONABLE_DELAYTIME);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_EDIT_VALUE),IDC_EDIT_VALUE,IDS_TOOLTIP_OPTIONS_PARENTURL_VALUE);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_BUTTON_ADD),IDC_BUTTON_ADD,IDS_TOOLTIP_OPTIONS_PARENTURL_ADD);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_BUTTON_REMOVE),IDC_BUTTON_REMOVE,IDS_TOOLTIP_OPTIONS_PARENTURL_REMOVE);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_LIST_PARENTURL),IDC_LIST_PARENTURL,IDS_TOOLTIP_OPTIONS_PARENTURL_LIST);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_COMBO_PARENTURL),IDC_COMBO_PARENTURL,IDS_TOOLTIP_OPTIONS_PARENTURL_ACCEPTANCE);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_CHECK_PARENTURL_USEALWAYS),IDC_CHECK_PARENTURL_USEALWAYS,IDS_TOOLTIP_OPTIONS_PARENTURL_ALWAYS_USE_INCLUDE_LIST);
		m_Tooltip.AddTooltip(GetDlgItem(IDC_CHECK_SAVELIST),IDC_CHECK_SAVELIST,IDS_TOOLTIP_OPTIONS_PARENTURL_ALWAYS_SAVE_LIST);
	}

	UpdateData(FALSE);

	return(TRUE);
}

/*
	OnSetActive()
*/
BOOL CWallPaperCrawlerSettingsParentUrlDlg::OnSetActive(void)
{
	int nIndex = 0;
	switch(m_nParentUrlAcceptance)
	{
		case PARENTACCEPTANCE_ASK:
			nIndex = 0;
			break;
		case PARENTACCEPTANCE_EXCLUDE_ALL_OUTSIDE:
			nIndex = 1;
			break;
		case PARENTACCEPTANCE_EXCLUDE_ALL_OUTSIDE_EXCEPT_PICTURES:
			nIndex = 2;
			break;
		case PARENTACCEPTANCE_EXCLUDE_ALL_OUTSIDE_EXCEPT_REGFILETYPES:
			nIndex = 3;
			break;
		case PARENTACCEPTANCE_FOLLOW_ALL_OUTSIDE:
			nIndex = 4;
			break;
	}
	m_wndComboParentUrl.SetCurSel(nIndex);

	UpdateData(FALSE);

	return(TRUE);
}

/*
	OnButtonAdd()
*/
void CWallPaperCrawlerSettingsParentUrlDlg::OnButtonAdd(void)
{
	char szValue[MAX_URL+1];

	UpdateData(TRUE);
	strcpyn(szValue,m_strValue,sizeof(szValue));

	CUrl url;
	URL Url;

	// controlla che non contenga caratteri jolly
	if(strchr(szValue,'?') || strchr(szValue,'*'))
	{
		::MessageBoxResourceEx(this->m_hWnd,MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_WILDCARDS,szValue);
		return;
	}

	url.SplitUrl(szValue,&Url);
	BOOL bHaveHost = FALSE;
	if(strlen(Url.host) > 0)
		bHaveHost = TRUE;
	if(bHaveHost)
	{
		::MessageBoxResourceEx(this->m_hWnd,MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_IS_HTTP_DOMAIN,szValue);
		return;
	}

	int nLen = strlen(szValue)-1;
	if(szValue[nLen]!='/')
		strcat(szValue,"/");
	if(szValue[0]!='/')
	{
		nLen = strlen(szValue);
		memmove(szValue+1,szValue,nLen);
		szValue[0] = '/';
		szValue[nLen] = '\0';
	}

	if(strlen(szValue) > 0)
	{
		if(m_wndParentUrlList.FindItem(szValue) < 0)
			m_wndParentUrlList.SelectItem(m_wndParentUrlList.AddItem(szValue,0));
		else
			::MessageBoxResourceEx(this->m_hWnd,MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_VALUE_EXISTS,szValue);
	}
	else
		::MessageBoxResource(this->m_hWnd,MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_INVALID_VALUE);
}

/*
	OnButtonRemove()
*/
void CWallPaperCrawlerSettingsParentUrlDlg::OnButtonRemove(void)
{
	int nCurSel = m_wndParentUrlList.GetCurrentItem();

	if(nCurSel >= 0)
	{
		m_wndParentUrlList.DeleteItem(nCurSel);

		if(m_wndParentUrlList.GetItemCount() > 0)
			m_wndParentUrlList.SelectItem(0);
	}
	else
		::MessageBoxResource(this->m_hWnd,MB_OK|MB_ICONERROR,WALLPAPER_PROGRAM_NAME,IDS_ERROR_SELECTITEM);
}

/*
	OnListDoubleClick()
*/
LONG CWallPaperCrawlerSettingsParentUrlDlg::OnListDoubleClick(UINT /*wParam*/,LONG /*lParam*/)
{
	int nItem;
	
	if((nItem = m_wndParentUrlList.GetCurrentItem()) >= 0)
	{
		char szItem[MAX_ITEM_SIZE+1];
		memset(szItem,'\0',sizeof(szItem));
		m_wndParentUrlList.GetItemText(nItem,0,szItem,sizeof(szItem));
		m_strValue.Format("%s",szItem);
		UpdateData(FALSE);
	}
	
	return(0L);
}

/*
	OnSelChangeComboParentUrl()
*/
void CWallPaperCrawlerSettingsParentUrlDlg::OnSelChangeComboParentUrl(void)
{
	// ricava la selezione corrente
	CString strValue;
	m_wndComboParentUrl.GetLBText(m_wndComboParentUrl.GetCurSel(),strValue);
	
	if(strcmp(strValue,IDS_PARENTACCEPTANCE_ASK)==0)
		m_nParentUrlAcceptance = PARENTACCEPTANCE_ASK;
	else if(strcmp(strValue,IDS_PARENTACCEPTANCE_EXCLUDE_ALL_OUTSIDE)==0)
		m_nParentUrlAcceptance = PARENTACCEPTANCE_EXCLUDE_ALL_OUTSIDE;
	else if(strcmp(strValue,IDS_PARENTACCEPTANCE_EXCLUDE_ALL_OUTSIDE_EXCEPT_PICTURES)==0)
		m_nParentUrlAcceptance = PARENTACCEPTANCE_EXCLUDE_ALL_OUTSIDE_EXCEPT_PICTURES;
	else if(strcmp(strValue,IDS_PARENTACCEPTANCE_EXCLUDE_ALL_OUTSIDE_EXCEPT_REGFILETYPES)==0)
		m_nParentUrlAcceptance = PARENTACCEPTANCE_EXCLUDE_ALL_OUTSIDE_EXCEPT_REGFILETYPES;
	else if(strcmp(strValue,IDS_PARENTACCEPTANCE_FOLLOW_ALL_OUTSIDE)==0)
		m_nParentUrlAcceptance = PARENTACCEPTANCE_FOLLOW_ALL_OUTSIDE;
	else
		m_nParentUrlAcceptance = DOMAINACCEPTANCE_ASK;
}

/*
	OnCheckParentUrlUseAlways()
*/
void CWallPaperCrawlerSettingsParentUrlDlg::OnCheckParentUrlUseAlways(void)
{
	m_bParentUrlUseAlways = !m_bParentUrlUseAlways;
}

/*
	OnCheckSaveList()
*/
void CWallPaperCrawlerSettingsParentUrlDlg::OnCheckSaveList(void)
{
	m_bSaveList = !m_bSaveList;
}
