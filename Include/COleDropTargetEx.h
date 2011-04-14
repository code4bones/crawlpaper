/*
	COleDropTargetEx.h
	Classe derivata per la gestione del drag & drop via OLE (MFC).
	Luca Piergentili, 07/09/00
	lpiergentili@yahoo.com
*/
#ifndef _COLEDROPTARGETEX_H
#define _COLEDROPTARGETEX_H 1

#include <stdlib.h>
#include "window.h"
#include <afxole.h>
#include <afxdisp.h>
#include "CNodeList.h"

#define MAX_FORMAT	128
#define MAX_OLEDATA	(_MAX_PATH*2)

/*
	CLIPBOARDFORMAT
*/
struct CLIPBOARDFORMAT {
	UINT id;
	char format[MAX_FORMAT+1];
};

/*
	CClipboardFormatList
	Classe per la lista dei formati riconosciuti della ClipBoard.
*/
class CClipboardFormatList : public CNodeList
{
public:
	CClipboardFormatList() : CNodeList() {}
	virtual ~CClipboardFormatList() {CNodeList::DeleteAll();}
	void* Create(void)
	{
		return(new CLIPBOARDFORMAT);
	}
	void* Initialize(void* pVoid)
	{
		CLIPBOARDFORMAT* pData = (CLIPBOARDFORMAT*)pVoid;
		if(!pData)
			pData = (CLIPBOARDFORMAT*)Create();
		if(pData)
			memset(pData,'\0',sizeof(CLIPBOARDFORMAT));
		return(pData);
	}
	BOOL PreDelete(ITERATOR iter)
	{
		if((CLIPBOARDFORMAT*)iter->data)
			delete ((CLIPBOARDFORMAT*)iter->data),iter->data = (CLIPBOARDFORMAT*)NULL;
		return(TRUE);
	}
	int Size(void) {return(sizeof(CLIPBOARDFORMAT));}
#ifdef _DEBUG
	const char* Signature(void) {return("CClipboardFormatList");}
#endif
};

/*
	COleDropTargetEx
*/
class COleDropTargetEx : public COleDropTarget
{
public:
	COleDropTargetEx();
	virtual ~COleDropTargetEx();

	BOOL				Attach				(CWnd* pWnd,UINT nMsg,BOOL bForceInit = FALSE);
	void				Detach				(void);
	inline void		Enable				(void) {m_bEnabled = TRUE;}
	inline void		Disable				(void) {m_bEnabled = FALSE;}
	inline void		Accept				(DROPEFFECT dropEffect) {m_defaultDropEffect = dropEffect;}
	UINT				AddClipboardFormat		(LPCSTR lpcszClipboardFormat);
	DROPEFFECT		CheckForRegisteredFormats(COleDataObject* pDataObject,UINT* id = NULL);
	DROPEFFECT		OnDragEnter			(CWnd* /*pWnd*/,COleDataObject* pDataObject,DWORD/*dwKeyState*/,CPoint/*point*/);
	DROPEFFECT		OnDragOver			(CWnd* /*pWnd*/,COleDataObject* /*pDataObject*/,DWORD/*dwKeyState*/,CPoint/*point*/);
	BOOL				OnDrop				(CWnd* /*pWnd*/,COleDataObject* pDataObject,DROPEFFECT/*dropEffect*/,CPoint/*point*/);

private:
	CWnd*			m_pWnd;
	UINT				m_nMsg;
	BOOL				m_bEnabled;
	DROPEFFECT		m_defaultDropEffect;
	CClipboardFormatList m_listClipboardFormats;
	char				m_szOleData[MAX_OLEDATA+1];
};

/*
	COleDropSourceEx
*/
class COleDropSourceEx : public COleDropSource
{
public:
	COleDropSourceEx();

	virtual SCODE		GiveFeedback	(DROPEFFECT dropEffect);
	DROPEFFECT		GetDropEffect	(void);

private:
	DROPEFFECT		m_dropEffect;
};

#endif // _COLEDROPTARGETEX_H
