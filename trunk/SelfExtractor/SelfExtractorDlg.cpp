/*
	James Spibey, 04/08/1998
	spib@bigfoot.com
	You are free to use, distribute or modify this code as long as this header is not removed or modified.

	-------------------------------------------------------

	Added support for file compression through the GZW API.
	More changes.
	Luca Piergentili, 24/08/00
	lpiergentili@yahoo.com
*/
#include "env.h"
#include "pragma.h"
#include "window.h"
#include "CSEFileInfo.h"
#include "CSelfExtractor.h"
#include "SelfExtractorDlg.h"

#include "traceexpr.h"
//#define _TRACE_FLAG	_TRFLAG_TRACEOUTPUT
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

BEGIN_MESSAGE_MAP(CSelfExtractorDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()

	ON_BN_CLICKED(IDC_BROWSE_EXTRACTOR,OnBrowseExtractor)
	ON_BN_CLICKED(IDC_READ_EXTRACTOR,OnReadExtractor)

	ON_BN_CLICKED(IDC_BROWSE_EXE,OnBrowseExe)
	ON_BN_CLICKED(IDC_CREATE_EXE,OnCreateExe)

	ON_BN_CLICKED(IDC_ADD_FILE,OnAddFile)
	ON_BN_CLICKED(IDC_ADD_SCRIPT,OnAddScript)
	ON_BN_CLICKED(IDC_CLEAR_LIST,OnClearList)
END_MESSAGE_MAP()

CSelfExtractorDlg::CSelfExtractorDlg(CWnd* pParent) : CDialog(CSelfExtractorDlg::IDD, pParent)
{
	m_strExePath = "";
	m_strExtractorPath = "";
	m_strCurrFile = "";
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSelfExtractorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_PROGRESS, m_Progress);
	DDX_Control(pDX, IDC_LIST, m_List);
	
	DDX_Text(pDX, IDC_EXEPATH, m_strExePath);
	DDX_Text(pDX, IDC_EXTRACTOR_PATH, m_strExtractorPath);
	DDX_Text(pDX, IDC_CURR_FILE, m_strCurrFile);
}

BOOL CSelfExtractorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);
		
	m_strWorkingDir = GetWorkingDir(TRUE);

	ListView_SetExtendedListViewStyle(m_List, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	m_List.InsertColumn(1, "Name", LVCFMT_LEFT, 200);
	m_List.InsertColumn(2, "Size (bytes)", LVCFMT_LEFT, 200);
	
	m_Progress.SetStep(1);
	m_Progress.ShowWindow(SW_HIDE);

	return TRUE;
}

void CSelfExtractorDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

HCURSOR CSelfExtractorDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

CString CSelfExtractorDlg::GetWorkingDir(BOOL withBackSlash)
{
	char FullName[MAX_PATH+1];
	::GetModuleFileName(NULL,FullName,MAX_PATH);
	
	CString Path(FullName);
	int num = Path.ReverseFind('\\');
	if(num != -1)
	{
		if(withBackSlash)
			num +=1;

		Path = Path.Left(num);
		return Path;
	}
	else
		return "";
}

void CSelfExtractorDlg::OnBrowseExtractor() 
{
	CFileDialog dlg(	TRUE,
					NULL,
					NULL, 
					OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, 
					"Exe Files (*.exe)|*.exe|All Files (*.*)|*.*||",
					AfxGetMainWnd()
					);
 
	dlg.m_ofn.lpstrTitle = "Select the Extractor Executable";
	if(dlg.DoModal()==IDOK)
	{
		m_strExtractorPath = dlg.GetPathName();
		UpdateData(FALSE);
	}
}

void CSelfExtractorDlg::OnReadExtractor(void) 
{
	CFileDialog dlg(	TRUE,
					NULL,
					NULL, 
					OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, 
					"Self Extracting Files (*.exe)|*.exe|All Files (*.*)|*.*||",
					AfxGetMainWnd()
					);
	
	dlg.m_ofn.lpstrTitle = "Select the Self Extracting file";
	if(dlg.DoModal()==IDOK)
	{
		int ret = m_Extractor.ReadTOC(dlg.GetPathName());
		if(ret==SFX_SUCCESS)
		{
			UpdateList();
		}
		else if(ret==SFX_INVALID_SIG)
		{
			CString Temp;
			Temp.LoadString(IDS_INVALID_FORMAT);
			MessageBox(Temp);
		}
	}
}

void CSelfExtractorDlg::OnBrowseExe(void) 
{
	CFileDialog dlg(	FALSE,
					"*.exe",
					"untitled.exe", 
					OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, 
					"Self-extracting Executables (*.exe)|*.exe|All Files (*.*)|*.*||",
					AfxGetMainWnd()
					);
	dlg.m_ofn.lpstrTitle = "Save As Self-Extracting Archive";
	if(dlg.DoModal()==IDOK)
	{
		m_strExePath = dlg.GetPathName();
		UpdateData(FALSE);
	}
}

void CSelfExtractorDlg::OnCreateExe(void) 
{
	UpdateData(TRUE);

	if(m_strExePath.IsEmpty())
	{
		MessageBox("You need to select an output Exe before you can create the SFX");
		return;
	}

	m_Progress.SetRange(0, m_Extractor.GetFileCount());
	m_Progress.ShowWindow(SW_SHOW);
	UpdateData(FALSE);
	
	int ret = m_Extractor.Create(m_strExtractorPath, m_strExePath, CSelfExtractorDlg::AddCallBack, (void*)this);
	
	m_Progress.SetPos(0);
	m_Progress.ShowWindow(SW_HIDE);
	m_strCurrFile = "";
	UpdateData(FALSE);
	
	switch(ret)
	{
		case SFX_NOTHING_TO_DO:
			MessageBox("You must add some files to the extractor");
			break;
		case SFX_SUCCESS:
			MessageBox("Archive Created Successfully");
			break;
		default:
			MessageBox("Unknown Error");
			break;
	}
}

void CSelfExtractorDlg::OnAddFile(void) 
{
	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"All Files (*.*)|*.*||",AfxGetMainWnd());
 
	if(dlg.DoModal()==IDOK)
		m_Extractor.Add(dlg.GetPathName());

	UpdateList();
}

void CSelfExtractorDlg::OnAddScript(void) 
{
	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"All Files (*.*)|*.*||",AfxGetMainWnd());

	if(dlg.DoModal()==IDOK)
	{
		char szScript[_MAX_PATH+1];
		_snprintf(szScript,sizeof(szScript)-1,"@%s",dlg.GetPathName());
		m_Extractor.Add(szScript);
	}

	UpdateList();
}

void CSelfExtractorDlg::OnClearList() 
{
	m_Extractor.Reset();
	UpdateList();
}

void CSelfExtractorDlg::UpdateList()
{
	m_List.DeleteAllItems();

	for(int i = 0; i < (int)m_Extractor.GetFileCount(); i++)
	{
		m_List.InsertItem(i, m_Extractor.GetItem(i)->GetFileName());
		CString Temp;
		Temp.Format("%d", m_Extractor.GetItem(i)->GetFileSize());
		m_List.SetItemText(i, 1, Temp);
	}

	UpdateData(FALSE);
}

UINT CSelfExtractorDlg::AddCallBack(LPVOID CallbackData,LPVOID userData)
{
	CSEFileInfo* pData = static_cast<CSEFileInfo*>(CallbackData);
	CSelfExtractorDlg* pDlg = static_cast<CSelfExtractorDlg*>(userData);
	
	pDlg->m_strCurrFile.Format("Adding %s",pData->GetPathName());
	pDlg->m_Progress.StepIt();
	pDlg->UpdateData(FALSE);

	return(0);
}
