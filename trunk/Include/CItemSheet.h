#ifndef _CITEMSHEET_H
#define _CITEMSHEET_H 1

#include "window.h"
#include "CNodeList.h"

struct CTRL {
	CWnd* pCWnd;
	UINT nId;
};

class CCtrlList : public CNodeList
{
public:
	CCtrlList() {}
	virtual ~CCtrlList() {CNodeList::DeleteAll();}
	BOOL PreDelete(ITERATOR iter)
	{
		if((CTRL*)iter->data)
		{
			delete ((CTRL*)iter->data);
			iter->data = (CTRL*)NULL;
		}
		return(TRUE);
	}
	int Size(void) {return(sizeof(CTRL));}
#ifdef _DEBUG
	const char* Signature(void) {return("CCtrlList");}
#endif
};

class CItemSheet  
{
public:
	CItemSheet() {}
	virtual ~CItemSheet() {}

	BOOL	AttachControl	(CWnd* pControl,UINT nId);
	BOOL	ShowWindows	(INT nCmdShow);
	BOOL	EnableWindows	(BOOL bEnable);
	BOOL	UpdateWindows	(UINT nId,BOOL bEnable);

private:
	CCtrlList m_ControlList;
};

#endif // _CITEMSHEET_H
