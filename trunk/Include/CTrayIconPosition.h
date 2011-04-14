/*
	CTrayIconPosition.h
	Basata sul codice originale di Irek Zielinski
	Luca Piergentili, 10/04/04 (modificato il codice originale)
*/
#ifndef _CTRAYICONPOSITION_H
#define _CTRAYICONPOSITION_H 1

#include "window.h"

/*
	CTrayIconPosition
*/
class CTrayIconPosition
{
public:
	typedef enum enumPrecision {Default=0,Low,Medium,High};
	
	CTrayIconPosition();
	virtual ~CTrayIconPosition() {}

	inline void			Invalidate		(void) {m_tLastUpdate = CTime(2000,1,1,1,1,1);}
	
	void					SetPrecisionTimeout	(int nLowSec,int nMedSec,int nHighSec);
	inline void			SetDefaultPrecision	(enumPrecision ePrecision) {m_eDefaultPrecision = ePrecision;};
	inline enumPrecision	GetDefaultPrecision	(void) const {return(m_eDefaultPrecision);};

	void					InitializeTracking	(HWND hWndParent,int nIconID,int nBlankIconID);
	BOOL					GetPosition		(CPoint& point,enumPrecision prPrec = Default);

protected:
	HWND					m_hWndParent;
	int					m_nTrayIconID;
	UINT					m_nBlankIconID;
	CRect				m_rcTrayRect;
	enumPrecision			m_eDefaultPrecision;
	CTime				m_tLastUpdate;
	CPoint				m_ptPosition;
	int					m_nPrecisionArray[3];

	int					GetTrayRect		(void);
	
	static BOOL CALLBACK FindTrayRectCallback(HWND hWnd,LPARAM lParam);
};

#endif // _CTRAYICONPOSITION_H
