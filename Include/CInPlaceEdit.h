/*
	CInPlaceEdit.h
	Classe derivata (CListView) per l'aggiornamento del (sub)elemento della lista (MFC).
	La lista deve essere in modalita' report.
	La classe derivata per la lista deve definire il gestore per il messaggio LVN_ENDLABELEDIT,
	inviato al termine dell'editing del campo.
	Modifiche minori al codice originale: Editable subitems - Zafir Anjum (1998/08/06)
	Luca Piergentili, 12/12/02
	lpiergentili@yahoo.com
*/
#ifndef _CINPLACEEDIT_H
#define _CINPLACEEDIT_H 1

#include "window.h"
#include "strings.h"

/*
	CInPlaceEdit
*/
class CInPlaceEdit : public CEdit
{
public:
	CInPlaceEdit(int nItem,int nSubItem,CString strInitText);
	virtual ~CInPlaceEdit() {}
	
	virtual BOOL	PreTranslateMessage	(MSG* pMsg);

	inline void	SetAutoDelete		(BOOL bFlag) {m_bAutoDelete = bFlag;}
	inline BOOL	GetAutoDelete		(void) const {return(m_bAutoDelete);}
	
	inline LPCSTR	GetPicture		(void) const {return(m_szPicture);}
	inline void	SetPicture		(LPCSTR pPicture) {strcpyn(m_szPicture,pPicture,sizeof(m_szPicture));}

protected:
	int			OnCreate			(LPCREATESTRUCT lpCreateStruct);
	void			OnChar			(UINT nChar,UINT nRepCnt,UINT nFlags);
	void			OnKillFocus		(CWnd* pNewWnd);
	void			OnNcDestroy		(void);

private:
	BOOL			m_bAutoDelete;
	int			m_nItem;
	int			m_nSubItem;
	CString		m_strInitText;
	BOOL			m_bEsc;
	char			m_szPicture[256+1];

	DECLARE_MESSAGE_MAP()
};

#endif // _CINPLACEEDIT_H
