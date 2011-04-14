/*
	CFileNameOpenDialog.h
	Classe base per il dialogo File|Apri per i formati diversi dalle immagini (MFC).
	Luca Piergentili, 13/06/03
	lpiergentili@yahoo.com
*/
#ifndef _CFILENAMEOPENDIALOG_H
#define _CFILENAMEOPENDIALOG_H 1

#include "window.h"
#include "CImageDialog.h"

/*
	CFileNameOpenDialog
*/
class CFileNameOpenDialog : public CImageOpenDialog
{
public:
	CFileNameOpenDialog(UINT		nDialogID,
					UINT		nStaticDibID,
					UINT		nStaticInfo,
					CWnd*	pParent = NULL,
					LPCSTR	lpcszLibraryName = NULL,
					LPCSTR	lpcszInitialDir = ".\\",
					LPCSTR	lpcszDialogTitle = NULL,
					BOOL		bAlwaysOnTop = FALSE
					)
					:
					CImageOpenDialog(	nDialogID,
									nStaticDibID,
									nStaticInfo,
									pParent,
									lpcszLibraryName,
									lpcszInitialDir,
									lpcszDialogTitle)
					{
						memset(m_szBitmapFileName,'\0',sizeof(m_szBitmapFileName));
						m_bAlwaysOnTop = bAlwaysOnTop;
					}

	virtual BOOL OnInitDialog(void)
	{
		BOOL bRet = CImageOpenDialog::OnInitDialog();
		if(m_bAlwaysOnTop)
			this->SetWindowPos(&wndTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
		return(bRet);
	}

	virtual void OnUnknowFileType(LPCSTR lpcszFileName,CDibCtrl& pDibCtrl,LPCSTR lpcszLibraryName,LPSTR lpszInfo,int nInfoSize);

	virtual void SetBitmap(LPCSTR lpcszBitmapFileName)
	{
		strcpyn(m_szBitmapFileName,lpcszBitmapFileName,sizeof(m_szBitmapFileName));
	}

private:
	char m_szBitmapFileName[_MAX_PATH+1];
	BOOL m_bAlwaysOnTop;
};

#endif // _CFILENAMEOPENDIALOG_H
