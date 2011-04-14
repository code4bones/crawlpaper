#ifndef _COPTIONSTREE_H
#define _COPTIONSTREE_H 1

#include "window.h"

class COptionsTree : public CTreeCtrl
{
public:
	COptionsTree();
	virtual ~COptionsTree() {}

	BOOL	AddControl(HTREEITEM hItem,CWnd* pWnd,UINT nId = (UINT)-1);
	BOOL	UpdateControl(HTREEITEM hItem,UINT nId,BOOL bFlag);
	BOOL	SetItemData(HTREEITEM hItem,DWORD dwData);
	DWORD GetItemData(HTREEITEM hItem) const;
	void SetCallback(HWND hWnd,UINT nMessage);

protected:
	void OnSelchanged(NMHDR* pNMHDR,LRESULT* pResult);
	void OnDestroy(void);
	void OnDeleteItem(NMHDR* pNMHDR,LRESULT* pResult);

private:
	HWND m_hWnd;
	UINT m_nMessage;

	DECLARE_MESSAGE_MAP()
};

#endif // _COPTIONSTREE_H
