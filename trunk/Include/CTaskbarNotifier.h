/*
	CTaskbarNotifier.h
	Classe per la finestra popup sulla taskbar (MFC).
	Ripresa e modificata dall'originale di John O'Byrne (TaskbarNotifier.cpp, CreateRgnFromBitmap() by Davide Pizzolato).
	Luca Piergentili, 13/06/03
	lpiergentili@yahoo.com
*/
#ifndef _CTASKBARNOTIFIER_H
#define _CTASKBARNOTIFIER_H 1

#include "strings.h"
#include "window.h"
#include "win32api.h"
#include "CWndLayered.h"
#include "CUrl.h"
#include "CToolTipCtrlEx.h"
#include "CTaskbarNotifier.config.h"

#ifdef _LOAD_ALL_IMAGE_FORMATS
  #include "CImageFactory.h"
#endif

// modalita' per il testo
#define TN_TEXT_NORMAL		0x0000
#define TN_TEXT_BOLD		0x0001
#define TN_TEXT_ITALIC		0x0002
#define TN_TEXT_UNDERLINE	0x0004

/*
	TAGS
	struttura per le macro da sostituire nel testo per il tooltip
*/
struct TAGS {
	char		tag[16];
	char*	value;
};

/*
	TASKBARPOPUP
	struttura per i dati relativi al bitmap per il popup
*/
#define TASKBARPOPUP_MAX_NAME			64
#define TASKBARPOPUP_MAX_AUTHOR		64
#define TASKBARPOPUP_MAX_COPYRIGHT		64
#define TASKBARPOPUP_MAX_URL			MAX_URL
#define TASKBARPOPUP_MAX_PICTURENAME	_MAX_FILEPATH
#define TASKBARPOPUP_MAX_FONTNAME		64
#define TASKBARPOPUP_MAX_POPUPTEXT		256
#define TASKBARPOPUP_MIN_POPUPTIME		100
#define TASKBARPOPUP_MAX_POPUPTIME		10000
#define TASKBARPOPUP_DEFAULT_TIMETOSHOW	600
#define TASKBARPOPUP_DEFAULT_TIMETOSTAY	5000
#define TASKBARPOPUP_DEFAULT_TIMETOHIDE	200

struct TASKBARPOPUP {
	// [About]
	char		szName[TASKBARPOPUP_MAX_NAME+1];			// Name
	char		szAuthor[TASKBARPOPUP_MAX_AUTHOR+1];		// Author
	char		szCopyright[TASKBARPOPUP_MAX_COPYRIGHT+1];	// Copyright
	char		szUrl[TASKBARPOPUP_MAX_URL+1];			// Url
	
	// [Bitmap]
	char		szPicture[TASKBARPOPUP_MAX_PICTURENAME+1];	// Picture
	int		nPictureWidth;
	int		nPictureHeight;
	int		nTextAreaLeft;							// TextArea
	int		nTextAreaTop;
	int		nTextAreaRight;
	int		nTextAreaBottom;
	BYTE		R_Transparent;							// TransparentColor
	BYTE		G_Transparent;
	BYTE		B_Transparent;
	
	// [Font]
	char		szFontName[TASKBARPOPUP_MAX_FONTNAME+1];	// Name
	int		nFontSize;							// Size
	BYTE		R_Text;								// Color
	BYTE		G_Text;
	BYTE		B_Text;
	BYTE		R_SelectedText;						// OnMouseOverColor
	BYTE		G_SelectedText;
	BYTE		B_SelectedText;
	int		nFontStyle;							// Style (TN_TEXT_BOLD,TN_TEXT_ITALIC,TN_TEXT_NORMAL)
	DWORD	dwFontAlign;							// Align (DT_CENTER,DT_LEFT,DT_RIGHT,DT_BOTTOM,DT_TOP)

	// [Popup]
	char		szPopupText[TASKBARPOPUP_MAX_POPUPTEXT];	// Text
	int		nPopupTooltip;							// Tooltip=yes			(yes/no)
	int		nPopupScrollText;						// Scroll=no			(yes/no)
	int		nPopupScrollDirection;					// Direction=left		(left/right/none)
	int		nPopupScrollSpeed;						// Speed=200			(100...500)
	int		nPopupTimeToShow;						// TimeToShow=600		(100/5000)
	DWORD	dwPopupTimeToStay;						// TimeToStay=4000		(100/10000 - playtime)
	int		nPopupTimeToHide;						// TimeToHide=200		(100/5000)
	BYTE		nPopupTransparency;						// Transparency=180		(0=invisible/255=opaque)
	int		nPopupAlwaysOnTop;						// AlwaysOnTop=no		(yes/no)
	int		nPopupXPos;							// AbsolutePosition=x,y
	int		nPopupYPos;
	int		nPopupCloseLeft;						// CloseButton
	int		nPopupCloseTop;
	int		nPopupCloseRight;
	int		nPopupCloseBottom;
};

/*
	CTaskbarPopupList
	classe per la lista dei files per i bitmap per i popup
*/
class CTaskbarPopupList : public CNodeList
{
public:
	CTaskbarPopupList() : CNodeList() {}
	virtual ~CTaskbarPopupList() {CNodeList::DeleteAll();}
	void* Create(void)
	{
		return(new TASKBARPOPUP);
	}
	void* Initialize(void* pVoid)
	{
		TASKBARPOPUP* pData = (TASKBARPOPUP*)pVoid;
		if(!pData)
			pData = (TASKBARPOPUP*)Create();
		if(pData)
			memset(pData,'\0',sizeof(TASKBARPOPUP));
		return(pData);
	}
	BOOL PreDelete(ITERATOR iter)
	{
		if((TASKBARPOPUP*)iter->data)
			delete ((TASKBARPOPUP*)iter->data),iter->data = (TASKBARPOPUP*)NULL;
		return(TRUE);
	}
	int Size(void) {return(sizeof(TASKBARPOPUP));}
#ifdef _DEBUG
	const char* Signature(void) {return("CTaskbarPopupList");}
#endif
};

/*
	CTaskbarNotifier
*/
class CTaskbarNotifier : public CWnd, public CWndLayered
{
public:
	CTaskbarNotifier();
	virtual ~CTaskbarNotifier();

	BOOL			Create				(CWnd *pWndParent);
	inline void	SetMessageWindow		(HWND hWnd) {m_hWndParent = hWnd;}
	
	BOOL			Show					(LPCTSTR lpcszText,DWORD dwTimeToShow = 600L,DWORD dwTimeToStay = 4000L,DWORD dwTimeToHide = 200L,int nXPos = -1,int nYPos = -1);
	BOOL			UnShow				(void);
	
	inline void	SetAlwaysOnTop			(BOOL bFlag) {m_bIsTopMost = bFlag;}

	inline void	SetOnMouseClick		(BOOL bHandleMouseClick) {m_bHandleMouseClick = bHandleMouseClick;}
	inline void	SetOnMouseClickMessage	(UINT nMsg) {m_nMsg = nMsg;}
	
	inline void	SetIDTimerAppearing		(UINT nTimerID) {m_nIDT_APPEARING = nTimerID;}
	inline void	SetIDTimerWaiting		(UINT nTimerID) {m_nIDT_WAITING = nTimerID;}
	inline void	SetIDTimerDisappearing	(UINT nTimerID) {m_nIDT_DISAPPEARING = nTimerID;}
	inline void	SetIDTimerScrolling		(UINT nTimerID) {m_nIDT_SCROLLING = nTimerID;}

#ifdef _LOAD_ALL_IMAGE_FORMATS
	void			SetLibrary			(LPCSTR lpcszLibraryName) {strcpyn(m_szLibraryName,lpcszLibraryName,sizeof(m_szLibraryName));}
#endif
	
	BOOL			SetBitmap				(UINT nBitmapID,BYTE R = -1,BYTE G = -1,BYTE B = -1);
	BOOL			SetBitmap				(LPCTSTR lpcszFileName,BYTE R = -1,BYTE G = -1,BYTE B = -1);
	inline int	GetBitmapWidth			(void) const {return(m_nBitmapWidth);}
	inline int	GetBitmapHeight		(void) const {return(m_nBitmapHeight);}
	
	inline void	SetRect				(RECT rcText) {m_rcText = rcText;}
	void			SetFont				(LPCTSTR lpcszFont,int nSize,int nNormalStyle,int nSelectedStyle);
	void			SetDefaultFont			(void);
	void			SetTextColor			(COLORREF crNormalTextColor,COLORREF crSelectedTextColor);
	void			SetTextFormat			(UINT nTextFormat) {m_nDefaultTextFormat = nTextFormat;}
	void			SetTextScroll			(BOOL bScroll,int nDirection = -1,DWORD dwSpeed = 200L);
	inline void	SetTextFiller			(LPCSTR lpcszTextFiller) {m_strTextFiller.Format("%s",lpcszTextFiller);}

	inline void	SetToolTip			(LPCSTR lpcszText = NULL) {if(lpcszText && strcmp(lpcszText,"")!=0) strcpyn(m_szToolTipText,lpcszText,sizeof(m_szToolTipText)); else memset(m_szToolTipText,'\0',sizeof(m_szToolTipText));}
	inline LPCSTR	GetToolTip			(void) const {return(m_szToolTipText);}

protected:
	void			OnMouseMove			(UINT nFlags,CPoint point);
	void			OnLButtonUp			(UINT nFlags,CPoint point);
	LRESULT		OnMouseHover			(WPARAM wParam,LPARAM lParam);
	LRESULT		OnMouseLeave			(WPARAM wParam,LPARAM lParam);
	BOOL			OnEraseBkgnd			(CDC* pDC);
	void			OnPaint				(void);
	BOOL			OnSetCursor			(CWnd* pWnd,UINT nHitTest,UINT nMsg);
	void			OnTimer				(UINT nIDEvent);
	void			OnClose				(void);
	void			OnDestroy				(void);

private:
	void			ResetTimers			(void);
	void			ResetWindow			(void);
	void			ResetBitmap			(void);
	BOOL			SetBitmap				(BYTE R,BYTE G,BYTE B);
	HRGN			CreateRgnFromBitmap		(HBITMAP hBitmap,COLORREF color);

	BOOL			m_bIsValid;
	BOOL			m_bIsTopMost;
	CWnd*		m_pWndParent;
	HWND			m_hWndParent;
	UINT			m_nMsg;
	int			m_nAnimStatus;
	UINT			m_nIDT_APPEARING;
	UINT			m_nIDT_WAITING;
	UINT			m_nIDT_DISAPPEARING;
	UINT			m_nIDT_SCROLLING;
	UINT			m_nTimerAppearing;
	UINT			m_nTimerWaiting;
	UINT			m_nTimerDisappearing;
	UINT			m_nTimerScrolling;
	
	HCURSOR		m_hCursor;

	BOOL			m_bHandleMouseClick;
	BOOL			m_bMouseIsOver;

	CString		m_strText;
	CString		m_strTextFiller;
	CRect		m_rcText;
	UINT			m_nDefaultTextFormat;
	CFont		m_fntNormal;
	CFont		m_fntSelected;
	COLORREF		m_crNormalTextColor;
	COLORREF		m_crSelectedTextColor;
	BOOL			m_bScrollText;
	int			m_nScrollDirection;
	DWORD		m_dwScrollSpeed;
	char			m_szToolTipText[512];
	CToolTipCtrlEx m_wndToolTip;
	
#ifdef _LOAD_ALL_IMAGE_FORMATS
	CImage*		m_pImage;
	CImageFactory	m_ImageFactory;
	char			m_szLibraryName[_MAX_PATH+1];
#endif
	BOOL			m_bBitmapLoaded;
	CBitmap		m_bitmapBackground;
	HRGN			m_hBitmapRegion;
	int			m_nBitmapWidth;
	int			m_nBitmapHeight;

	int			m_nXPos;
	int			m_nYPos;
	int			m_nTaskbarPlacement;
	DWORD		m_dwTimerPrecision;
 	DWORD		m_dwTimeToStay;
	DWORD		m_dwShowEvents;
	DWORD		m_dwHideEvents;
	int			m_nCurrentPosX;
	int			m_nCurrentPosY;
	int			m_nCurrentWidth;
	int			m_nCurrentHeight;
	int			m_nIncrementShow;
	int			m_nIncrementHide;

	UINT			m_nOnLButtonUpFlags;
	CPoint		m_ptOnLButtonUpPoint;
	TRACKMOUSEEVENT m_trackMouseEvent;

	DECLARE_MESSAGE_MAP()
};

#endif // _CTASKBARNOTIFIER_H
